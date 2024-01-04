/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/locale.H"
#include "x/messages.H"
#include "x/exception.H"
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
	auto l=LIBCXX_NAMESPACE::locale::create(localeName);

	auto cmsgs=LIBCXX_NAMESPACE::messages::create("xtest", "xtest", l);
	LIBCXX_NAMESPACE::messages::create("xtest", "xtest", l);

	bool caught=false;

	try {
		LIBCXX_NAMESPACE::messages::create("xtest", ".", l);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		caught=true;

		std::cout << "Caught expected exception: " << e
			  << std::endl;
	}

	if (!caught)
		throw EXCEPTION("Did not catch expected exception");

	dumphex(cmsgs->get("test1"));
	dumphex(cmsgs->get("test2"));

	std::cout << LIBCXX_NAMESPACE::gettextmsg(cmsgs->get
						  ("%ignore%[%% %3%: %1%%2%]"),
						  std::dec, 10, "counters")
		  << std::endl;

	std::cout << LIBCXX_NAMESPACE::gettextmsg(cmsgs->get
						  ("%cnt%%1% rec",
						   "%cnt%%1% recs", 1),
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

int main(int argc, char *argv[])
{
	try {
		testmessages("en_US.UTF-8");
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
