/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/forkexec.H"
#include "x/messages.H"
#include "x/sysexception.H"
#include "x/to_string.H"
#include "x/globlock.H"
#include "x/fditer.H"

#include "gettext_in.h"

#include <unistd.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

forkexec::forkexec(const std::vector<std::string> &argvArg) : argv(argvArg)
{
	if (argv.empty())
		throw EXCEPTION(_("Argument list cannot be empty"));
}

forkexec::~forkexec()
{
}

forkexec &forkexec::set_fd(int filedesc, const fd &fdArg)
{
	if (filedesc < 0)
		throw EXCEPTION(_("Invalid file descriptor"));

	filedescs.erase(filedesc);
	filedescs.insert(std::make_pair(filedesc, fdArg));
	return *this;
}


fd forkexec::pipe_from(int filedesc)
{
	auto pipe=fd::base::pipe();

	set_fd(filedesc, pipe.second);

	return pipe.first;
}

fd forkexec::pipe_to(int filedesc)
{
	auto pipe=fd::base::pipe();

	set_fd(filedesc, pipe.first);

	return pipe.second;
}

fd forkexec::socket_fd(int filedesc)
{
	auto pipe=fd::base::socketpair();

	set_fd(filedesc, pipe.first);

	return pipe.second;
}

void forkexec::after_fork(const fd &errArg) noexcept
{
	try {
		exec();
	} catch (const exception &e)
	{
		(*errArg->getostream()) << e << std::flush;
		_exit(1);
	}
}

void forkexec::exec()
{
	// Sanity check

	for (auto &fd:filedescs)
	{
		int n=fd.second->get_fd();

		if (n < 0)
		{
			std::ostringstream o;

			o << "File descriptor " << fd.first << " is not open";

			throw EXCEPTION(o.str());
		}

		if (filedescs.find(n) != filedescs.end())
		{
			std::ostringstream o;

			o << "File descriptor " << n << " already in use";

			throw EXCEPTION(o.str());
		}
	}

	for (auto &fd:filedescs)
	{
		fd.second->closeonexec(false);

		if (dup2(fd.second->get_fd(), fd.first) < 0)
			throw EXCEPTION("dup2");
		fd.second->close();
	}

	std::vector<char *> argvstr;

	argvstr.reserve(argv.size()+1);

	for (auto &s:argv)
	{
		argvstr.push_back(const_cast<char *>(s.c_str()));
	}
	argvstr.push_back(0);

	const char *prog=exe.empty() ? argvstr[0]:exe.c_str();

	execvp(prog, &argvstr[0]);

	throw SYSEXCEPTION(prog);
}

void forkexec::before_fork(const fd &errArg)
{
	std::string error=std::string(fdinputiter(errArg), fdinputiter());

	if (!error.empty())
		throw EXCEPTION(error);
}

pid_t forkexec::spawn()
{
	auto pair=fd::base::pipe();

	pid_t p=fork();

	if (p == 0)
	{
		pair.first->close();
		after_fork(pair.second);
	}
	pair.second->close();
	before_fork(pair.first);
	return (p);
}

void forkexec::spawn_detached()
{
	auto pair=fd::base::pipe();

	pid_t p=fork();

	if (p == 0)
	{
		pair.first->close();

		p=::fork();

		if (p < 0)
		{
			(*pair.second->getostream()) << "fork failed"
						     << std::flush;
			_exit(0);
		}

		if (p == 0)
		{
			after_fork(pair.second);
		}
		_exit(0);
	}
	pair.second->close();
	before_fork(pair.first);
	wait4(p);
}

int forkexec::system()
{
	return wait4(spawn());
}

int forkexec::wait4(pid_t p)
{
	int waitstat;

	if (::wait4(p, &waitstat, 0, 0) != p)
		throw SYSEXCEPTION("wait");

	return WIFEXITED(waitstat) ? WEXITSTATUS(waitstat)
		: WIFSIGNALED(waitstat) ? WTERMSIG(waitstat):256;
}

#if 0
{
#endif
}
