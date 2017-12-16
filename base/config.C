/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/pwd.H"
#include "x/dir.H"
#include "x/config.H"
#include "x/pidinfo.H"
#include "x/fileattr.H"
#include "x/sysexception.H"
#include "x/property_value.H"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

static property::value<unsigned>
configupdate_interval(LIBCXX_NAMESPACE_STR
		      "::configdir::update_interval", 60);

std::string configdir(const std::string &appid)
{
	// Auto create ~/.libcxx/$appid

	std::string basedir=std::string{passwd{geteuid()}->pw_dir} + "/.libcxx";

	mkdir(basedir.c_str(), 0700);

	auto dir=basedir + "/" + appid;

	mkdir(dir.c_str(), 0700);

	auto me=exename();

	auto dotexe = dir+"/.exe";

	// Make sure that .exe exists, and if it's wrong replace it.

	auto target=fileattr::create(dotexe, true)->try_readlink();

	if (!target || *target != me)
	{
		unlink(dotexe.c_str());
		if (symlink(me.c_str(), dotexe.c_str()) < 0)
			throw SYSEXCEPTION("symlink(" << dotexe << ")");
	}

	// Check ~/.libcxx/$appid/.exe's timestamp; if it too old, time
	// to do some cleanup.

	time_t now=time(NULL);

	time_t when=now-configupdate_interval.get() * (60 * 60 * 24 / 2);
	time_t cutoff=now-configupdate_interval.get() * 60 * 60 * 24;

	auto timestamp=fileattr::create(dotexe)->stat().st_mtime;

	// When will then be now?
	if (timestamp < now || timestamp < when)
	{
		utimensat(AT_FDCWD, dotexe.c_str(), NULL,
			  AT_SYMLINK_NOFOLLOW);

		auto d=dir::create(basedir);

		std::vector<std::string> contents{d->begin(), d->end()};

		for (const auto &n:contents)
		{
			auto fullname = basedir + "/" + n;

			auto dotexe = fullname + "/.exe";

			auto st=fileattr::create(dotexe, true)->try_stat();

			if (st && st->st_mtime >= cutoff)
				continue;

			// If the symlink is still good, bump its timestamp.

			st=fileattr::create(dotexe, false)->try_stat();

			if (st)
			{
				utimensat(AT_FDCWD, dotexe.c_str(), NULL,
					  AT_SYMLINK_NOFOLLOW);
				continue;
			}

			dir::base::rmrf(fullname);
		}
	}

	return dir;
}
#if 0
{
#endif
}
