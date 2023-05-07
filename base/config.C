/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/pwd.H"
#include "x/config.H"
#include "x/appid.H"
#include "x/pidinfo.H"
#include "x/fileattr.H"
#include "x/sysexception.H"
#include "x/property_value.H"
#include "x/strtok.H"
#include "x/fileattr.H"
#include <limits>
#include <charconv>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include "gettext_in.h"
#include <filesystem>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

static property::value<unsigned>
configupdate_interval(LIBCXX_NAMESPACE_STR
		      "::configdir::update_interval", 60);

std::string appid() noexcept __attribute__((weak));

std::string appid() noexcept
{
	std::vector<std::string> parts;

	strtok_str("localdomain/localhost/"+exename()+ "/exe", "/", parts);

	std::string s;
	size_t l=0;

	for (const auto &p:parts)
	{
		l = l+1+p.size();
	}

	s.reserve(l);

	for (auto b=parts.begin(), e=parts.end(); b != e; )
	{
		--e;
		if (!s.empty())
			s.push_back('.');

		s += *e;
	}

	return s;
}

std::string appver() noexcept __attribute__((weak));

std::string appver() noexcept
{
	auto s=fileattr::create(exename())->stat().st_mtim;

	char buffer[std::numeric_limits<decltype(s.tv_sec)>::digits10 +
		    std::numeric_limits<decltype(s.tv_nsec)>::digits10+4];

	auto ret=std::to_chars(std::begin(buffer),
			       std::end(buffer)-1,
			       s.tv_sec);

	*ret.ptr++='.';

	ret=std::to_chars(ret.ptr, std::end(buffer), s.tv_nsec);

	return {&buffer[0], ret.ptr};
}

std::string configdir()
{
	return configdir(appid());
}

std::string configdir(const std::string_view &appid)
{
	if (appid.empty())
		throw EXCEPTION(_("Unset application identifier (appid)"));

	// Auto create ~/.libcxx/$appid

	std::string basedir=std::string{passwd{geteuid()}->pw_dir} + "/.libcxx";

	mkdir(basedir.c_str(), 0700);

	auto dir=basedir + "/";

	dir.reserve(dir.size()+appid.size());

	dir.insert(dir.end(), appid.begin(), appid.end());

	mkdir(dir.c_str(), 0700);

	auto me=exename();

	auto my_dotexe = dir+"/.exe";

	// Make sure that .exe exists, and if it's wrong replace it.

	auto target=fileattr::create(my_dotexe, true)->try_readlink();

	if (!target || *target != me)
	{
		unlink(my_dotexe.c_str());
		if (symlink(me.c_str(), my_dotexe.c_str()) < 0)
			throw SYSEXCEPTION("symlink(" << my_dotexe << ")");
	}

	// Check ~/.libcxx/$appid/.exe's timestamp; if it too old, time
	// to do some cleanup.

	time_t now=time(NULL);

	time_t when=now-configupdate_interval.get() * (60 * 60 * 24 / 2);
	time_t cutoff=now-configupdate_interval.get() * 60 * 60 * 24;

	auto timestamp=fileattr::create(my_dotexe)->stat().st_mtime;

	// When will then be now?
	if (timestamp < now || timestamp < when)
	{
		utimensat(AT_FDCWD, my_dotexe.c_str(), NULL,
			  AT_SYMLINK_NOFOLLOW);

		for (auto &d:std::filesystem::directory_iterator{basedir})
		{
			std::string fullname=d.path();

			auto dotexe = fullname + "/.exe";

			auto st=fileattr::create(dotexe, true)->try_stat();

			if (st && st->st_mtime >= cutoff)
				continue;

			// If the symlink is still good, bump its timestamp.

			auto symlink=fileattr::create(dotexe, false);

			st=symlink->try_stat();

			auto rl=symlink->try_readlink();

			if (rl && *rl == me &&
			    dotexe != my_dotexe)
			{
				// .exe symlink points to me, but it's not
				// our dotexe. This application's ID must've
				// changed, so we'll remove the old appid's
				// configuration.
				st.reset();
			}

			if (st)
			{
				utimensat(AT_FDCWD, dotexe.c_str(), NULL,
					  AT_SYMLINK_NOFOLLOW);
				continue;
			}

			std::filesystem::remove_all(fullname);
		}
	}

	return dir;
}
#if 0
{
#endif
}
