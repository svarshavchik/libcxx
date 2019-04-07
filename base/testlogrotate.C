/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/logger.H"
#include "x/exception.H"

LOG_FUNC_SCOPE_DECL(testlogger, testlogger_outer);

static void testlogger()
{
	LOG_FUNC_SCOPE(testlogger_outer);

	LOG_ERROR("Error message 1");
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
