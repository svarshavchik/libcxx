/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/dhparams.H"
#include "x/pwd.H"
#include "x/grp.H"
#include "tlsparamsgen.h"

#include <iomanip>

static void create_dsa(tlsparamsgen &gen, LIBCXX_NAMESPACE::fd &newfile)
{
	LIBCXX_NAMESPACE::gnutls::dhparams
		dhp(LIBCXX_NAMESPACE::gnutls::dhparams
		    ::create());

	dhp->generate(gen.bits->value);
	dhp->export_pk(GNUTLS_X509_FMT_PEM)->save(newfile);
}

static void create(tlsparamsgen &gen, const std::string &filename,
		   void (*create_func)(tlsparamsgen &, LIBCXX_NAMESPACE::fd &))
{
	mode_t mode;

	{
		std::istringstream i(gen.mode->value);

		i >> std::oct >> mode;

		if (i.fail())
			throw EXCEPTION("Invalid mode: " +
					gen.mode->value);
	}

	if (gen.definit->value)
	{
		LIBCXX_NAMESPACE::fdptr fd;

		try {
			fd=LIBCXX_NAMESPACE::fd::base::open(filename, O_RDONLY);
		} catch (...) {
		}

		if (!fd.null())
		{
			LIBCXX_NAMESPACE::istream i=fd->getistream();
			std::string line;

			std::getline(*i, line);

			if (line.substr(0, 21) != ">>>DEFAULT PARAMETERS")
				return;
		}
	}

	LIBCXX_NAMESPACE::fdptr
		lockfile(LIBCXX_NAMESPACE::fdptr::base
			 ::lockf(filename + ".lock", F_TLOCK, 0600));

	if (lockfile.null())
		return;

	LIBCXX_NAMESPACE::fd newfile(LIBCXX_NAMESPACE::fd::create(filename, 0600));

	try {
		uid_t uid;
		gid_t gid;

		{
			auto st=newfile->stat();

			uid=st.st_uid;
			gid=st.st_gid;
		}

		std::string user_given=gen.user->value;
		std::string group_given=gen.group->value;

		if (user_given.size() || group_given.size())
		{
			if (user_given.size())
				uid=LIBCXX_NAMESPACE::passwd(user_given)
					->pw_uid;

			if (group_given.size())
				gid=LIBCXX_NAMESPACE::group(group_given)
					->gr_gid;


			newfile->chown(uid, gid);
		}

		(*create_func)(gen, newfile);

		newfile->chmod(mode);
		newfile->close();
	} catch (...)
	{
		newfile->cancel();
		throw;
	}
}

int main(int argc, char **argv)
{
	try {
		tlsparamsgen gen;

		std::list<std::string> filenames=gen.parse(argc, argv)->args;

		void (*create_func)(tlsparamsgen &, LIBCXX_NAMESPACE::fd &);

		if (gen.type->value == "dsa")
			create_func=&create_dsa;
		else throw EXCEPTION("Unknown parameter type: "
				     + gen.type->value);

		for (std::list<std::string>::const_iterator
			     b=filenames.begin(),
			     e=filenames.end(); b != e; ++b)
		{
			create(gen, *b, create_func);
		}

	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}
