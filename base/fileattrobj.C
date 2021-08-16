/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fileattr.H"

#if HAVE_XATTR
#include <sys/xattr.h>
#endif
#if HAVE_EXTATTR
#include <sys/param.h>
#include <sys/extattr.h>
#endif
#include <unistd.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

fileattrObj::fileattrObj(const char *filenameArg, bool readsymlinksArg) noexcept
	: filename(filenameArg), readsymlinks(readsymlinksArg)
{
}

fileattrObj::fileattrObj(const std::string &filenameArg,
			 bool readsymlinksArg) noexcept
	: filename(filenameArg), readsymlinks(readsymlinksArg)
{
}


fileattrObj::~fileattrObj()
{
}

#if HAVE_XATTR

ssize_t fileattrObj::getattr_internal(const char *name,
				      void *value,
				      size_t size) noexcept
{
	return readsymlinks
		? lgetxattr(filename.c_str(), name, value, size)
		: getxattr(filename.c_str(), name, value, size);
}

int fileattrObj::setattr_internal(const char *name,
				  const void *value,
				  size_t size,
				  int flags) noexcept
{
	return readsymlinks
		? lsetxattr(filename.c_str(), name, value, size, flags)
		: setxattr(filename.c_str(), name, value, size, flags);
}

int fileattrObj::removeattr_internal(const char *name) noexcept
{
	return readsymlinks
		? lremovexattr(filename.c_str(), name)
		: removexattr(filename.c_str(), name);
}

#else
#if HAVE_EXTATTR

extern int extattr_parsens(int &ns, const char * &name) noexcept;

ssize_t fileattrObj::getattr_internal(const char *name,
				      void *value,
				      size_t size) noexcept
{
	int ns;

	if (extattr_parsens(ns, name) < 0)
		return -1;

	return readsymlinks
		? extattr_get_link(filename.c_str(), ns, name, value, size)
		: extattr_get_file(filename.c_str(), ns, name, value, size);
}

int fileattrObj::setattr_internal(const char *name,
				  const void *value,
				  size_t size,
				  int flags) noexcept
{
	int ns;

	if (extattr_parsens(ns, name) < 0)
		return -1;

	if (flags)
	{
		if ((readsymlinks
		     ? extattr_get_link(filename.c_str(), ns, name, NULL, 0)
		     : extattr_get_file(filename.c_str(), ns, name, NULL, 0))
		     < 0)
		{
			if (flags & XATTR_REPLACE)
			{
				errno=ENOATTR;
				return -1;
			}
		}
		else
		{
			if (flags & XATTR_CREATE)
			{
				errno=EEXIST;
				return -1;
			}
		}
	}

	return (readsymlinks
		? extattr_set_link(filename.c_str(), ns, name, value, size)
		: extattr_set_file(filename.c_str(), ns, name, value, size))
		< 0 ? -1:0;
}

int fileattrObj::removeattr_internal(const char *name) noexcept
{
	int ns;

	if (extattr_parsens(ns, name) < 0)
		return -1;

	return readsymlinks
		? extattr_delete_link(filename.c_str(), ns, name)
		: extattr_delete_file(filename.c_str(), ns, name);
}

#else

ssize_t fileattrObj::getattr_internal(const char *name,
				      void *value,
				      size_t size) noexcept
{
	errno=ENODEV;
	return -1;
}

int fileattrObj::setattr_internal(const char *name,
				  const void *value,
				  size_t size,
				  int flags) noexcept
{
	errno=ENODEV;
	return -1;
}

int fileattrObj::removeattr_internal(const char *name) noexcept
{
	errno=ENODEV;
	return -1;
}
#endif
#endif

int fileattrObj::stat_internal(struct ::stat *buf) noexcept
{
	return readsymlinks
		? lstat(filename.c_str(), buf)
		: ::stat(filename.c_str(), buf);
}

ssize_t fileattrObj::readlink_internal(char *buf,
				       size_t bufsize) noexcept
{
	return ::readlink(filename.c_str(), buf, bufsize);
}

int fileattrObj::chmod_internal(mode_t mode) noexcept
{
	return ::chmod(filename.c_str(), mode);
}

int fileattrObj::chown_internal(uid_t uid, gid_t group) noexcept
{
	return ::chown(filename.c_str(), uid, group);
}

std::string fileattrObj::whoami()
{
	return filename;
}

#if 0
{
#endif
}
