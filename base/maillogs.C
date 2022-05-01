/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"

#include "x/options.H"
#include "x/locale.H"
#include "x/messages.H"
#include "x/join.H"
#include "x/fd.H"
#include "x/pcre.H"
#include "x/forkexec.H"
#include "gettext_in.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <map>

#include <sys/wait.h>

class logstate {

public:
	std::string statefile;

	std::map<std::string, off64_t> state;

	logstate(const std::string &statefileArg) : statefile(statefileArg)
	{
		std::ifstream i(statefile);

		if (!i.is_open())
			return;

		std::string line;

		while (!std::getline(i, line).eof())
		{
			size_t p=line.rfind(':');

			if (p == std::string::npos)
				continue;

			off64_t n=0;

			std::istringstream(line.substr(p+1)) >> n;

			state[line.substr(0, p)] = n;
		}
	}

	void save()
	{
		auto fd=LIBCXX_NAMESPACE::fd::create(statefile, 0666);

		auto ostream=fd->getostream();

		for (auto &file : state)
		{
			(*ostream) << file.first << ':'
				   << file.second << std::endl;
		}

		(*ostream) << std::flush;
	}
};

class outputbase {

public:

	virtual void operator()(const std::string &)=0;
};

class outputtostdout : public outputbase {

public:

	void operator()(const std::string &str) override
	{
		std::cout << str << std::endl;
	}
};

#include "maillogs.h"

class mailoutput : public outputbase {

	std::string sender;
	std::list<std::string> recipients;
	std::string subject;
	std::string charset;

	std::string sendmail;

	LIBCXX_NAMESPACE::fdptr fd;
	LIBCXX_NAMESPACE::ostreamptr o;
	pid_t p;

public:
	mailoutput(const maillogs &options)
		: sender(options.sender->value),
		  recipients(options.recipient->values),
		  subject(options.subject->value),
		  charset(options.charset->value)
	{
		if (recipients.empty())
			throw EXCEPTION("Must specify at least one recipient");
	}

	void operator()(const std::string &str) override
	{
		if (fd.null())
		{
			std::vector<std::string> argv;

			argv.reserve(recipients.size()+4);

			{
				const char *p=getenv("SENDMAIL");

				if (!p) p="/usr/bin/sendmail";
				sendmail=p;
				argv.push_back(sendmail);
			}

			if (!sender.empty())
			{
				argv.push_back("-f");
				argv.push_back(sender);
				argv.insert(argv.end(), recipients.begin(),
					    recipients.end());
			}

			LIBCXX_NAMESPACE::forkexec sendmail(argv);

			auto pipe=sendmail.pipe_to();

			p=sendmail.spawn();

			fd=pipe;
			o=fd->getostream();

			auto &os= *o;

			if (!sender.empty())
				os << "From: " << sender << std::endl;

			os << "To: "
			   << LIBCXX_NAMESPACE::join(recipients, ",\n    ")
			   << std::endl
			   << "Subject: " << subject << std::endl
			   << "Mime-Version: 1.0" << std::endl
			   << "Content-Type: text/plain; charset=\""
			   << charset << "\"" << std::endl << std::endl;
		}
		(*o) << str << std::endl;
	}

	void done()
	{
		if (fd.null())
			return;

		o->flush();
		fd->close();

		int status=LIBCXX_NAMESPACE::forkexec::wait4(p);

		if (status)
			throw EXCEPTION(sendmail + " exited with " +
					LIBCXX_NAMESPACE::to_string(status));
	}
};

static void searchlogs(outputbase &output,
		       std::list<std::string> &files,
		       logstate &state,
		       maillogs &args)
{
	// Remove from the state file any log files that were not listed.

	{
		std::set<std::string> lookup(files.begin(), files.end());

		for (auto b=state.state.begin(), e=state.state.end(), p=b;
		     b != e; )
		{
			p=b;
			++b;

			if (lookup.find(p->first) == lookup.end())
				state.state.erase(p);
		}
	}

	for (auto &logfile : files )
	{
		bool opened=false;

		try
		{
			auto fd=LIBCXX_NAMESPACE::fd::base::open(logfile,
								 O_RDONLY);

			opened=true;

			// Seek to where we were last time.
			{
				auto p=state.state.find(logfile);

				if (p != state.state.end())
					fd->seek(p->second, SEEK_SET);
			}

			auto i=fd->getistream();

			std::string logline;

			while (!std::getline(*i, logline).eof())
			{
				output(logline);
			}

			state.state[logfile]=fd->seek(0, SEEK_CUR);

		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			if (opened)
				throw EXCEPTION(logfile + ": "
						+ (std::string)*e);
		}
	}
}

class filter_pattern : public outputbase {

public:

	LIBCXX_NAMESPACE::pcre regex;

	outputbase &out;

	filter_pattern(const LIBCXX_NAMESPACE::pcre &regexArg,
		       outputbase &outArg)
		: regex(regexArg),
		  out(outArg)
	{
	}

	void operator()(const std::string &s) override
	{
		try {
			if (!regex->match(s).empty())
				out(s);
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::ostringstream o;

			o << e;

			out(o.str());
		}
	}
};

static void do_searchlogs_pattern(outputbase &output,
				  std::list<std::string> &files,
				  logstate &state,
				  maillogs &args,
				  const std::string &pattern,
				  int options)
{
	filter_pattern filter(LIBCXX_NAMESPACE::pcre::create(pattern, options),
			      output);

	searchlogs(filter, files, state, args);
}

static void searchlogs_pattern(outputbase &output,
			       std::list<std::string> &files,
			       logstate &state,
			       maillogs &args)
{
	if (args.pattern->is_set())
		do_searchlogs_pattern(output, files, state, args,
				      args.pattern->value, 0);
	else if (args.utf8pattern->is_set())
		do_searchlogs_pattern(output, files, state, args,
		 		      args.utf8pattern->value, PCRE2_UTF);
	else searchlogs(output, files, state, args);
}

int main2(int argc, char **argv)
{
	auto locale=LIBCXX_NAMESPACE::locale::base::environment();
	auto msgs=LIBCXX_NAMESPACE::messages::create(LIBCXX_DOMAIN, locale);

	maillogs args{msgs};

	std::list<std::string> files(args.parse(argc, argv, locale)->args);

	if (files.empty())
		throw EXCEPTION(_("state file must be specified"));

	logstate state(files.front());

	files.pop_front();

	if (args.tostdout->is_set())
	{
		outputtostdout out;
		searchlogs_pattern(out, files, state, args);
	}
	else
	{
		mailoutput out(args);

		searchlogs_pattern(out, files, state, args);
		out.done();
	}

	state.save();
	return 0;
}

int main(int argc, char **argv)
{
	int rc;

	try {
		rc=main2(argc, argv);
	} catch (const x::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}

	exit(rc);
	return (0);
}
