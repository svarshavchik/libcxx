/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "locale.H"
#include "messages.H"
#include "exception.H"
#include <iostream>
#include <iomanip>

void dumphex(std::string s)
{
	std::string::iterator b=s.begin(), e=s.end();

	while (b != e)
	{
		std::cout << std::setw(2) << std::setfill('0')
			  << std::hex
			  << (int)(unsigned char)*b++;
	}
	std::cout << std::endl;
}

void testmessages(std::string localeName)
{
	LIBCXX_NAMESPACE::locale l(LIBCXX_NAMESPACE::locale::create(localeName));

	LIBCXX_NAMESPACE::messages cmsgs(LIBCXX_NAMESPACE::messages::create(l, "xtest", "xtest"));

	dumphex(cmsgs->get("test1"));
	dumphex(cmsgs->get("test2"));

	std::cout << cmsgs->format("%ignore%[%% %3%: %1%%2%]",
				   std::dec, 10, "counters")
		  << std::endl;

	std::cout << cmsgs->formatn("%cnt%%1% rec",
				    "%cnt%%1% recs", 1,
				    1) << std::endl;

	std::string s=LIBCXX_NAMESPACE::gettextmsg(cmsgs->get("%cnt%%1% rec",
							    "%cnt%%1% recs", 2),
						 2);

	std::cout << s << std::endl;
}

void dumpwhex(std::wstring s)
{
	std::wstring::iterator b=s.begin(), e=s.end();

	while (b != e)
	{
		std::cout << std::setw(4) << std::setfill('0')
			  << std::hex
			  << (int)(unsigned char)*b++;
	}
	std::cout << std::endl;
}

void testwmessages(std::string localeName)
{
	LIBCXX_NAMESPACE::locale l(LIBCXX_NAMESPACE::locale::create(localeName));

	LIBCXX_NAMESPACE::wmessages cmsgs(LIBCXX_NAMESPACE::wmessages::create(l, "xtest", "xtest"));

	dumpwhex(cmsgs->get("test1"));
	dumpwhex(cmsgs->get("test2"));

	std::wostringstream wo;

	wo << LIBCXX_NAMESPACE::gettextmsg(cmsgs->get("%ignore%[%% %3%: %1%%2%]"),
					 std::dec, 20, L"counters");

	std::cout << LIBCXX_NAMESPACE::tostring(wo.str(), l) << std::endl;

	std::cout << LIBCXX_NAMESPACE::tostring(std::wstring(LIBCXX_NAMESPACE::gettextmsg(cmsgs->get("test1"))),
					      l) << std::endl;
}

int main(int argc, char *argv[])
{
	try {
		testmessages("en_US.UTF-8");
		testmessages("en_US.ISO8859-1");
		testwmessages("en_US.UTF-8");
		testwmessages("en_US.ISO8859-1");
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
