/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/pidinfo.H"
#include "x/singleton.H"
#include "x/fileattr.H"
#include "x/sysexception.H"
#include "x/strtok.H"
#include "x/locale.H"
#include "x/imbue.H"
#include <sstream>
#include <fstream>
#include <iostream>

#if HAVE_SYSCTL_KERN_PROC
extern "C" {
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/param.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <kvm.h>
};
#endif

#include "pidinfo_internal.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class LIBCXX_HIDDEN selfexeObj : virtual public x::obj {

 public:
	std::string procselfexe;

	selfexeObj();
	~selfexeObj() noexcept;

#if HAVE_SYSCTL_KERN_PROC
	bool found(dev_t dev, ino_t ino);
#endif

};

#if HAVE_SYSCTL_KERN_PROC

bool pid2devino(pid_t pid, dev_t &dev, ino_t &ino)
{
	int mib[4];
	size_t len=0;

	mib[0]=CTL_KERN;
	mib[1]=KERN_PROC;
	mib[2]=KERN_PROC_FILEDESC;
	mib[3]=pid;
	
	if (sysctl(mib, 4, NULL, &len, NULL, 0) < 0)
		return false;

	char buf[len];

	if (sysctl(mib, 4, buf, &len, NULL, 0) < 0)
		return false;

	for (size_t i=0; i<len; )
	{
		struct kinfo_file *kf=(struct kinfo_file *)(buf+i);

		i += kf->kf_structsize;

		if (kf->kf_fd == KF_FD_TYPE_TEXT &&
		    kf->kf_type == KF_TYPE_VNODE &&
		    kf->kf_vnode_type == KF_VTYPE_VREG)
		{
			dev=kf->kf_un.kf_file.kf_file_fsid;
			ino=kf->kf_un.kf_file.kf_file_fileid;
			return true;
		}
	}
		
	return false;
}

bool pid2args(pid_t pid, std::vector<std::string> &args)
{
	int mib[4];
	size_t len=0;

	mib[0]=CTL_KERN;
	mib[1]=KERN_PROC;
	mib[2]=KERN_PROC_ARGS;
	mib[3]=pid;

	if (sysctl(mib, 4, NULL, &len, NULL, 0) < 0)
		return false;

	char buf[len];

	if (sysctl(mib, 4, buf, &len, NULL, 0) < 0)
		return false;

	const char *p=buf;

	for (size_t i=0; i<len; ++i)
	{
		if (buf[i] == 0)
		{
			args.push_back(p);
			p=buf+i+1;
		}
	}
	return true;
}

selfexeObj::selfexeObj()
{
	// Extract PATH. Prepend the current directory to PATH.

	std::vector<std::string> path;

	{
		std::vector<char> buffer;

		buffer.resize(256);

		while (getcwd(&buffer[0], buffer.size()) == NULL)
		{
			if (errno != ERANGE)
			{
				perror("getcwd");
				_exit(1);
			}

			buffer.resize(buffer.size()*2);
		}
		path.push_back(&buffer[0]);
	}
	{
		const char *c=getenv("PATH");

		if (c)
			strtok_str(c, ":", path);
	}

	// Locate this pid's TEXT's file, look up its dev and ino.
	// Locate this pid's command arguments.

	dev_t dev;
	ino_t ino;
	std::vector<std::string> args;

	pid_t p=getpid();

	if (!pid2devino(p, dev, ino))
	{
		std::cerr << "Could not look up KERN_PROC_FILEDESC for myself"
			  << std::endl;
		_exit(1);
	}

	if (!pid2args(p, args) || args.empty())
	{
		std::cerr << "Could not look up KERN_PROC_ARGS for myself"
			  << std::endl;
		_exit(1);
	}

	// If this process's argv[0] is absolute, only check that.

	if (*args[0].c_str() == '/')
	{
		procselfexe=args[0];

		if (found(dev, ino))
			return;
	}
	else
	{
		// Otherwise, walk through PATH until we find ourselves.

		for (auto &dir:path)
		{
			procselfexe=dir + "/" + args[0];

			if (found(dev, ino))
				return;
		}
	}

	std::cerr << "Could not convert " << args[0]
		  << " to an absolute pathname, argv[0] is invalid or access"
		" denied to a component of PATH"
		  << std::endl;
	_exit(1);
}

// stat() this pathname, and check that its dev/ino matches this proc's TEXT.

bool selfexeObj::found(dev_t dev, ino_t ino)
{
	struct stat stat_doublecheck;

	char *p=realpath(procselfexe.c_str(), NULL);

	if (!p)
		return false;

	char buf[strlen(p)+1];
	strcpy(buf, p);
	free(p);
	procselfexe=buf;

	return stat(buf, &stat_doublecheck) == 0 &&
		stat_doublecheck.st_dev == dev &&
		stat_doublecheck.st_ino == ino;
}

std::string exestarttime(pid_t p)
{
	int mib[4];
	size_t len=0;

	mib[0]=CTL_KERN;
	mib[1]=KERN_PROC;
	mib[2]=KERN_PROC_ALL;
	mib[3]=0;

	while (1)
	{
		if (sysctl(mib, 3, NULL, &len, NULL, 0) < 0)
			break;

		char buf[len];

		if (sysctl(mib, 3, buf, &len, NULL, 0) < 0)
		{
			if (errno == ENOMEM)
				continue;
			break;
		}

		for (size_t i=0; i<len; )
		{
			struct kinfo_proc *kp=(struct kinfo_proc *)(buf+i);

			i += kp->ki_structsize;

			if (kp->ki_pid == p)
			{
				std::ostringstream o;

				imbue<std::ostringstream>
					i(locale::create("C"), o);

				o << kp->ki_start.tv_sec << "."
				  << kp->ki_start.tv_usec;

				return o.str();
			}
		}
		break;
	}

	return "";
}

#else

selfexeObj::selfexeObj()
	: procselfexe(fileattr::create("/proc/self/exe", true)->readlink())
{
}

std::string exestarttime(pid_t p)
{
	std::ostringstream o;

	{
		imbue<std::ostringstream> i(locale::create("C"), o);

		o << "/proc/" << p << "/stat";
	}

	std::ifstream i(o.str());

	std::string start_time;

	if (!i.is_open())
		return start_time;

	std::string s;

	std::getline(i, s);

	std::vector<std::string> words;

	LIBCXX_NAMESPACE::strtok_str(s, " ", words);

	if (words.size() >= 22)
		start_time=words[21];	// starttime
	return start_time;
}


#endif

selfexeObj::~selfexeObj() noexcept
{
}

static singleton<selfexeObj> procselfexe;

class LIBCXX_HIDDEN selfexeinit {

 public:
	selfexeinit();
	~selfexeinit() noexcept;
};

selfexeinit::selfexeinit()
{
	procselfexe.get();
}

selfexeinit::~selfexeinit() noexcept
{
}

std::string exename()
{
	return procselfexe.get()->procselfexe;
}

#if 0
{
#endif
}
