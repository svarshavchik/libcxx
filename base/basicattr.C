/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fileattr.H"
#include "x/sysexception.H"
#include "x/fdobj.H"
#include "x/fileattrobj.H"
#if HAVE_XATTR
#include <sys/xattr.h>
#endif
#if HAVE_EXTATTR
#include <sys/param.h>
#include <sys/extattr.h>
#endif
#include <algorithm>
#include <cstring>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void basic_attr::fatal(const char *syscall)
{
	std::ostringstream o;

	o << syscall << "(" << whoami() << ")";
	
	throw SYSEXCEPTION(o.str());
}

#if HAVE_XATTR

class basic_attr::listattr_internal_args {
public:
	char *list;
	size_t size;
};
#else
#if HAVE_EXTATTR
class basic_attr::listattr_internal_args {
public:
	int ns;
	void *list;
	size_t size;
};

static const struct {
	int ns;
	const char *n;
} extattr_ns[]={ {EXTATTR_NAMESPACE_USER, "user"},
		 {EXTATTR_NAMESPACE_SYSTEM, "system"}
};

static const int extattr_ns_size=sizeof(extattr_ns)/sizeof(extattr_ns[0]);

int extattr_parsens(int &ns, const char * &name) noexcept LIBCXX_HIDDEN;

int extattr_parsens(int &ns, const char * &name) noexcept
{
	for (int i=0; i<extattr_ns_size; ++i)
	{
		size_t l=strlen(extattr_ns[i].n);

		if (strncmp(name, extattr_ns[i].n, l) == 0 && name[l] == '.')
		{
			name += l+1;
			ns=extattr_ns[i].ns;
			return 0;
		}
	}

	errno=EPERM;
	return -1;
}
#endif
#endif

ssize_t fdObj::listattr_internal(const listattr_internal_args &args)
		noexcept
{
	if (filedesc >= 0)
	{
#if HAVE_XATTR
		return flistxattr(filedesc, args.list, args.size);
#else
#if HAVE_EXTATTR
		return extattr_list_fd(filedesc, args.ns, args.list, args.size);
#endif
#endif
	}
	return 0;
}

ssize_t fileattrObj::listattr_internal(const listattr_internal_args &args)
	noexcept
{
#if HAVE_XATTR
	return readsymlinks
		? llistxattr(filename.c_str(), args.list, args.size) 
		: listxattr(filename.c_str(), args.list, args.size);
#else
#if HAVE_EXTATTR
	return readsymlinks
		? extattr_list_link(filename.c_str(), args.ns,
				    args.list, args.size)
		: extattr_list_file(filename.c_str(), args.ns,
				    args.list, args.size);
#else
	errno=ENXIO;
	return -1;
#endif
#endif
}

#if HAVE_XATTR
std::set<std::string> basic_attr::listattr()
{
	std::vector<char> buf;
	ssize_t l;

	listattr_internal_args args;

	do
	{
		args.list=NULL;
		args.size=0;

		args.size=listattr_internal(args);
		buf.resize(args.size ? args.size:1);

		args.list=&buf[0];

	} while ((l=listattr_internal(args)) == (ssize_t)-1 &&
		 errno == ERANGE);

	if (l < 0)
		fatal("listxattr");

	std::set<std::string> newset;

	char *b=&buf[0], *e=&buf[l];

	while (b < e)
	{
		char *p=std::find(b, e, 0);

		newset.insert(std::string(b, p));
		if ((b=p) < e)
			++b;
	}

	return newset;
}
#else
#if HAVE_EXTATTR

std::set<std::string> basic_attr::listattr()
{
	listattr_internal_args args;
	std::set<std::string> attrset;

	for (size_t i=0; i<extattr_ns_size; ++i)
	{
		std::vector<char> buf;
		ssize_t n;

		args.ns=extattr_ns[i].ns;

		do
		{
			args.list=NULL;
			args.size=0;
			n=listattr_internal(args);

			if (n < 0)
			{
				if (errno == EPERM)
				{
					buf.clear();
					break;
				}
				fatal("listattr");
			}

			buf.resize(n ? n:1);
			args.list=&buf[0];
			args.size=n;
		} while (listattr_internal(args) != n);

		for (char *b=&buf[0], *e=(n < 0 ? b:b+n); b != e; )
		{
			unsigned char n= *b++;

			char *p=b;

			while (b != e && n)
			{
				--n;
				++b;
			}

			attrset.insert(std::string(extattr_ns[i].n) + "."
				       + std::string(p, b));
		}
	}
	return attrset;
}

#else

#endif
#endif

basic_attr::basic_attr() noexcept
{
}

basic_attr::~basic_attr() noexcept
{
}

int basic_attr::stat_internal(struct ::stat *buf)
	noexcept
{
	errno=EINVAL;
	return -1;
}

ssize_t basic_attr::listattr_internal(const listattr_internal_args &args)
	noexcept
{
	errno=EINVAL;
	return -1;
}

ssize_t basic_attr::getattr_internal(const char *name,
				     void *value,
				     size_t size)
	noexcept
{
	errno=EINVAL;
	return -1;
}

int basic_attr::setattr_internal(const char *name,
				 const void *value,
				 size_t size,
				 int flags)
	noexcept
{
	errno=EINVAL;
	return -1;
}

int basic_attr::removeattr_internal(const char *name)
	noexcept
{
	errno=EINVAL;
	return -1;
}

ssize_t basic_attr::readlink_internal(char *buf,
				      size_t bufsize) noexcept
{
	errno=EINVAL;
	return -1;
}

filestat basic_attr::stat()
{
	filestat s=filestat::create();

	if (stat_internal(&*s) < 0)
		throw SYSEXCEPTION(whoami());
	return s;
}

void basic_attr::get_chk(ssize_t l)
{
	if (l == (ssize_t)-1)
		fatal("getxattr");
}

void basic_attr::setattr(const char *name,
			 const void *value,
			 size_t size,
			 int flags)
{
	switch (flags) {
	case 0:
		break;
	case setattr_create:
		flags=XATTR_CREATE;
		break;
	case setattr_replace:
		flags=XATTR_REPLACE;
		break;
	default:
		errno=EINVAL;
		fatal("setattr");
	}

	if (setattr_internal(name, value, size, flags))
		fatal("setxattr");
}

void basic_attr::removeattr(const char *name)
{
	if (removeattr_internal(name))
		fatal("removexattr");
}

std::string basic_attr::readlink()
{
	std::vector<char> buf;
	ssize_t n;

	do
	{
		buf.resize(buf.size() + 1024);

		n=readlink_internal(&buf[0], buf.size());

		if (n < 0)
			fatal("readlink");
	} while ((size_t)n == buf.size());

	return std::string(&buf[0], &buf[n]);
}

int basic_attr::chmod_internal(mode_t mode) noexcept
{
	errno=EINVAL;
	return -1;
}

int basic_attr::chown_internal(uid_t userid, gid_t groupid) noexcept
{
	errno=EINVAL;
	return -1;
}

void basic_attr::chmod(mode_t mode)
{
	if (chmod_internal(mode) < 0)
		fatal("chmod");
}

void basic_attr::chown(uid_t userid,
		       gid_t groupid)
{
	if (chown_internal(userid, groupid) < 0)
		fatal("chown");
}

#if 0
{
#endif
}
