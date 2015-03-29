/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/logger.H"
#include "x/exception.H"
#include "x/property_properties.H"
#include "x/dir.H"

#include <fstream>
#include <vector>
#include <algorithm>

LOG_FUNC_SCOPE_DECL(testlogger, testlogger_outer);
LOG_FUNC_SCOPE_DECL(testlogger::warnlevel, testlogger_inner_warnlevel);
LOG_FUNC_SCOPE_DECL(testlogger::secondlogfile, testlogger_inner_secondlogfile);
LOG_FUNC_SCOPE_DECL(testlogger::secondfile::noinherit,
		    testlogger_inner_noinherit);
LOG_FUNC_SCOPE_DECL(testlogger::loglevelupdate, testlogger_loglevelupdate);

#define UPD(str)							\
	LIBCXX_NAMESPACE::property::load_properties			\
	(LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue,	\
				   std::string>::tostr(str),		\
		true, true,						\
	 LIBCXX_NAMESPACE::property::errhandler::errthrow(),		\
	 LIBCXX_NAMESPACE::locale::create("C"))

static void testlogger()
{
	LOG_FUNC_SCOPE(testlogger_outer);

	LOG_ERROR("Error message 1");
	LOG_WARNING("Warning message 1");

	{
		LOG_FUNC_SCOPE(testlogger_inner_warnlevel);
		LOG_ERROR("Error message 2");
		LOG_WARNING("Warning message 2");
	}

	{
		LOG_FUNC_SCOPE(testlogger_inner_secondlogfile);
		LOG_ERROR("Error message 3 (1)");
		LOG_ERROR("Error message 3 (2)");

		{
			LOG_FUNC_SCOPE(testlogger_inner_noinherit);
			LOG_ERROR("Error message 4");
		}

		std::vector<std::string> logfiles;

		{
			LIBCXX_NAMESPACE::dir testlogdir=
				LIBCXX_NAMESPACE::dir::create("testlogdir");

			logfiles.insert(logfiles.end(),
					testlogdir->begin(),
					testlogdir->end());
		}

		std::vector<std::string>::iterator
			b(logfiles.begin()), e(logfiles.end());

		while (b != e)
		{
			std::string filename=*b++;

			filename="testlogdir/" + filename;
			std::ifstream i(filename.c_str());

			if (i.is_open())
			{
				std::string l;

				while (!std::getline(i, l).eof())
				{
					std::cerr << l << std::endl;
				}
				std::cerr << std::flush;
			}
		}
	}

	{
		LOG_FUNC_SCOPE(testlogger_loglevelupdate);

		LOG_WARNING("test property update 1");

		UPD("testlogger::loglevelupdate::@log::level=warning\n");

		LOG_WARNING("test property update 2");

		UPD("testlogger::loglevelupdate::@log::handler::"
		    "default::format=notime3\n");

		LOG_WARNING("test property update 3");
	}
}
			     
int main(int argc, char **argv)
{
	try {
		testlogger();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
