/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/dir.H"
#include "x/dirobj.H"
#include "x/opendirobj.H"
#include "x/sysexception.H"

#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void dirBase::rmrf(const std::string &dirname, bool removeit)
{
	std::list<std::string> dirs, notdirs;

	// Removing directory entries while scanning the directory at the
	// same time is undefined behavior.
	{
		dir thisDir=dir::create(dirname);

		iterator b, e;

		try {
			b=thisDir->begin();
			e=thisDir->end();
		} catch (const exception &e)
		{
			return;
		}

		while (b != e)
		{
			std::string n=b->fullpath();

			if (b->filetype() == DT_DIR)
				dirs.push_back(n);
			else
				notdirs.push_back(n);
			++b;
		}
	}

	while (!dirs.empty())
	{
		rmrf(dirs.front());
		dirs.pop_front();
	}

	while (!notdirs.empty())
	{
		if (unlink(notdirs.front().c_str()) < 0 && errno != ENOENT)
		{
			throw SYSEXCEPTION(notdirs.front());
		}
		notdirs.pop_front();
	}
	if (removeit && rmdir(dirname.c_str()) < 0 && errno != ENOENT)
		throw SYSEXCEPTION(dirname);
}

void dirBase::mkdir(const char *dirName, mode_t m)
{
	char buf[strlen(dirName)+1];

	strcpy(buf, dirName);

	char *b=buf, *e=buf+strlen(buf), *p=e;

	while (::mkdir(b, m) < 0)
	{
		if (errno != ENOENT)
			throw SYSEXCEPTION("mkdir");

		while (p > b)
		{
			if (*--p == '/')
			{
				*p=0;
				break;
			}
		}

		if (!*b)
			throw SYSEXCEPTION("mkdir");
	}

	while (p < e)
	{
		*p++ = '/';

		while (p < e)
		{
			if (!*p)
				break;
			++p;
		}

		if (::mkdir(b, m) < 0)
		{
			if (errno != EEXIST)
				throw SYSEXCEPTION("mkdir");
		}
	}
}

void dirBase::mkdir_parent(const std::string &f, mode_t m)
{
	size_t p=f.rfind('/');

	if (p == f.npos)
		return;

	mkdir(f.substr(0, p));
}

#if 0
{
#endif
}
