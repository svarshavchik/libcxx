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

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class LIBCXX_HIDDEN selfexeObj : virtual public x::obj {

 public:
	std::string procselfexe;

	selfexeObj() : procselfexe(fileattr::create( "/proc/" PROC_SELF
						     PROC_SELF_EXE, true)
				   ->readlink())
	{
	}
	~selfexeObj() noexcept
	{
	}
};

static singleton<selfexeObj> procselfexe;

pidinfo::pidinfo(pid_t pid)
{
	locale l=locale::create("C");

	if (pid == getpid())
	{
		exe=procselfexe.get()->procselfexe;
	}
	else
	{
		std::ostringstream o;

		{
			globlocale global_locale(l);

			imbue<std::ostringstream> i(l, o);

			o << "/proc/" << pid << PROC_SELF_EXE;
		}

		exe=fileattr::create(o.str(), true)->readlink();

		if (exe.substr(0, 1) != "/")
		{
			errno=ENOENT;
			throw SYSEXCEPTION(exe);
		}
	}

	{
		std::ostringstream o;

		{
			globlocale global_locale(l);

			imbue<std::ostringstream> i(l, o);

			o << "/proc/" << pid << PROC_START;
		}

		std::ifstream i(o.str());

		if (!i.is_open())
			throw SYSEXCEPTION(o.str());

		std::string s;

		std::getline(i, s);

		std::vector<std::string> words;

		strtok_str(s, " ", words);

		if (words.size() >= PROC_START_FIELD)
			start_time=words[PROC_START_FIELD-1];
	}
}

pidinfo::~pidinfo()
{
}
#if 0
{
#endif
}
