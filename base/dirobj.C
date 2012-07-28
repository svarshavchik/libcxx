/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "dirobj.H"
#include "opendirobj.H"
#include "sysexception.H"
#include <sys/types.h>
#include <sys/stat.h>
#include <cstddef>
#include <cstring>
#include <unistd.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

dirObj::dirObj(const std::string &dirnameArg, bool showimplicitArg) noexcept
	: dirname(dirnameArg), showimplicit(showimplicitArg)
{
}

dirObj::~dirObj() noexcept
{
}


dirObj::iterator::iterator() noexcept
{
}

dirObj::iterator::iterator(const dirObj::iterator &o) noexcept
	: nameref(o.nameref), val(o.val)
{
}


dirObj::iterator &dirObj::iterator::operator=(const dirObj::iterator &o) noexcept
{
	nameref=o.nameref;
	val=o.val;

	return *this;
}

dirObj::iterator dirObj::begin() const
{
	dirObj::iterator b(ptr<opendirObj>::create(),
			   const_dirptr(this));
	
	if ((b.val.d->dirp=opendir((b.val.d->dirname=getDirname()).c_str()))
	    == NULL)
		throw SYSEXCEPTION(b.val.d->dirname);
	b.val.d->readdir_size=
		offsetof(struct dirent, d_name)+
		fpathconf(dirfd(b.val.d->dirp), _PC_NAME_MAX)+1;

	return ++b;
}

dirObj::iterator dirObj::end() const noexcept
{
	return dirObj::iterator();
}


dirObj::iterator::iterator(const ptr<opendirObj> &dArg,
			   const const_dirptr &namerefArg) noexcept
: nameref(namerefArg)
{
	val.d=dArg;
}

dirObj::iterator::~iterator()
{
}

dirObj::iterator &dirObj::iterator::operator++()
{
	if (!val.d.null())
	{
		opendirObj &o= *val.d;

		struct dirent *de;

		char readdir_buf[o.readdir_size];

		do
		{
			int err=readdir_r(o.dirp, (struct dirent *)readdir_buf,
					  &de);
			if (err)
			{
				errno= -err;
				throw SYSEXCEPTION("readdir");
			}
		} while (!nameref->getImplicitFlag() && de &&
			 (strcmp(de->d_name, ".") == 0 ||
			  strcmp(de->d_name, "..") == 0));

		if (de == NULL)
		{
			val.d=ptr<opendirObj>();
			nameref=ptr<dirObj>();
		}
		else
		{
			val.first=de->d_name;
			val.second=de->d_type;
		}
	}

	return *this;
}

dirObj::iterator dirObj::iterator::operator++(int)
{
	dirObj::iterator cpy= *this;

	operator++();
	return cpy;
}

bool dirObj::iterator::operator==(const iterator &o) noexcept
{
	return (val.d.null() ? o.val.d.null():
		!o.val.d.null() && val.first == o.val.first &&
		nameref->getDirname() == o.nameref->getDirname());
}

std::string dirObj::entry::fullpath() const noexcept
{
	char buf[first.size() + d->dirname.size() + 2];

	strcat(strcat(strcpy(buf, d->dirname.c_str()), "/"),
	       first.c_str());

	return buf;
}

unsigned char dirObj::entry::filetype() const
{
	if (second != DT_UNKNOWN)
		return second;

	char buf[first.size() + d->dirname.size() + 2];

	strcat(strcat(strcpy(buf, d->dirname.c_str()), "/"),
	       first.c_str());

	struct stat stat_buf;

	if (lstat(buf, &stat_buf) == 0)
	{
		if (S_ISREG(stat_buf.st_mode))
			return DT_REG;
		else if (S_ISDIR(stat_buf.st_mode))
			return DT_DIR;
		else if (S_ISLNK(stat_buf.st_mode))
			return DT_LNK;
		else if (S_ISFIFO(stat_buf.st_mode))
			return DT_FIFO;
		else if (S_ISSOCK(stat_buf.st_mode))
			return DT_SOCK;
		else if (S_ISBLK(stat_buf.st_mode))
			return DT_BLK;
		else if (S_ISCHR(stat_buf.st_mode))
			return DT_CHR;
	}

	return DT_UNKNOWN;
}

#if 0
{
#endif
}
