/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>
#include "x/basiciofiltercodecvtin.H"
#include "x/basicstreamcodecvtobj.H"

static void testctow(const char *wcstr, const LIBCXX_NAMESPACE::locale &l,
		     size_t inbufsize, size_t outbufsize)
{
	LIBCXX_NAMESPACE::ctow_codecvt codecvt(l);

	std::vector<wchar_t> outbuffer;

	outbuffer.resize(outbufsize);

	while (*wcstr)
	{
		size_t l=strlen(wcstr);

		if (l > inbufsize)
			l=inbufsize;

		codecvt.next_in=wcstr;
		codecvt.avail_in=l;

		codecvt.next_out=&outbuffer[0];
		codecvt.avail_out=outbufsize;

		codecvt.filter();

		wcstr=codecvt.next_in;

		size_t i;

		for (i=0; i<(size_t)(codecvt.next_out - &outbuffer[0]); ++i)
		{
			std::cout << ' ' << std::hex
				  << std::setfill('0')
				  << std::setw(4)
				  << outbuffer[i]
				  << std::setw(0) << std::dec;
		}
	}

	do
	{
		codecvt.next_out=&outbuffer[0];
		codecvt.avail_out=outbufsize;

		codecvt.filter();

		size_t i;

		for (i=0; i<(size_t)(codecvt.next_out - &outbuffer[0]); ++i)
		{
			std::cout << ' ' << std::hex
				  << std::setfill('0')
				  << std::setw(4)
				  << outbuffer[i]
				  << std::setw(0) << std::dec;
		}
	} while (codecvt.next_out > &outbuffer[0]);
	std::cout << std::endl;
}

static void testwtoc_istreambuffer(const wchar_t *wcstr,
				   const LIBCXX_NAMESPACE::const_locale &l,
				   size_t inbufsize, size_t outbufsize)
{
	std::string ws;

	{
		std::wistringstream is(wcstr);

		std::vector<wchar_t> inpbuf;

		std::vector<char> outbuf;

		LIBCXX_NAMESPACE::wtoc_istreambuf wci(is, l);
		
		inpbuf.resize(inbufsize);
		outbuf.resize(outbufsize);

		std::istream wi(&wci);

		wci.setbuf(&outbuf[0], outbuf.size());
		wci.setinputbuf(&inpbuf[0], inpbuf.size());

		std::getline(wi, ws);
	}

	std::cout << ws << std::endl;
}

static void testctow_istreambuffer(const char *wcstr,
				   const LIBCXX_NAMESPACE::const_locale &l,
				   size_t inbufsize, size_t outbufsize)
{
	std::wstring ws;

	{
		std::istringstream is(wcstr);

		std::vector<char> inpbuf;

		std::vector<wchar_t> outbuf;

		LIBCXX_NAMESPACE::ctow_istreambuf wci(is, l);
		
		inpbuf.resize(inbufsize);
		outbuf.resize(outbufsize);

		std::wistream wi(&wci);

		wci.setbuf(&outbuf[0], outbuf.size());
		wci.setinputbuf(&inpbuf[0], inpbuf.size());

		std::getline(wi, ws);
	}

	testwtoc_istreambuffer(ws.c_str(), l, inbufsize, outbufsize);
}

int main()
{
	static const char helloworld[]={0x48, (char)0xc3, (char)0xa8, 0x6c, 0x6c, (char)0xc5, (char)0x8d, 0x20, 0x77, (char)0xc3, (char)0xb6, 0x72, 0x6c, 0x64, 0};

	try {
		testctow(helloworld, LIBCXX_NAMESPACE::locale::base::utf8(), 1024, 1024);
		testctow(helloworld, LIBCXX_NAMESPACE::locale::base::utf8(), 1, 1024);
		testctow(helloworld, LIBCXX_NAMESPACE::locale::base::utf8(), 1024, 1);
		testctow(helloworld, LIBCXX_NAMESPACE::locale::base::utf8(), 1, 1);

		testctow_istreambuffer(helloworld, LIBCXX_NAMESPACE::locale::base::utf8(), 1024, 1024);
		testctow_istreambuffer(helloworld, LIBCXX_NAMESPACE::locale::base::utf8(), 1, 1024);
		testctow_istreambuffer(helloworld, LIBCXX_NAMESPACE::locale::base::utf8(), 1024, 1);
		testctow_istreambuffer(helloworld, LIBCXX_NAMESPACE::locale::base::utf8(), 1, 1);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl
			  << e->backtrace;
		exit(1);
	}
}
