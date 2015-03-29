/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>

#include "x/locale.H"
#include "x/basiciofiltercodecvtout.H"
#include "x/basicstreamcodecvtobj.H"
#include "x/exception.H"

static void testwtoc_codecvt(const wchar_t *wcstr,
			     const LIBCXX_NAMESPACE::const_locale &l,
			     size_t inbufsize, size_t outbufsize)
{
	LIBCXX_NAMESPACE::wtoc_codecvt codecvt(l);

	std::vector<char> outbuffer;

	outbuffer.resize(outbufsize);

	while (*wcstr)
	{
		size_t l=0;

		while (wcstr[l])
			++l;

		if (l > inbufsize)
			l=inbufsize;

		codecvt.next_in=wcstr;
		codecvt.avail_in=l;

		codecvt.next_out=&outbuffer[0];
		codecvt.avail_out=outbufsize;

		codecvt.filter();

		wcstr=codecvt.next_in;

		std::cout << std::string(&outbuffer[0], codecvt.next_out);
	}

	do
	{
		codecvt.next_out=&outbuffer[0];
		codecvt.avail_out=outbufsize;

		codecvt.filter();

		std::cout << std::string(&outbuffer[0], codecvt.next_out);
	} while (codecvt.next_out > &outbuffer[0]);
	std::cout << std::endl;
}

static void testwtoc_ostreambuffer(const wchar_t *wcstr,
				   const LIBCXX_NAMESPACE::const_locale &l,
				   size_t inbufsize, size_t outbufsize)
{
	std::ostringstream os;

	{
		std::vector<wchar_t> inpbuf;

		std::vector<char> outbuf;

		LIBCXX_NAMESPACE::wtoc_ostreambuf wco(os, l);

		inpbuf.resize(inbufsize);
		outbuf.resize(outbufsize);

		std::wostream wo(&wco);

		wco.setbuf(&inpbuf[0], inpbuf.size());
		wco.setoutputbuf(&outbuf[0], outbuf.size());

		wo << wcstr;
		wco.sync();
	}

	std::cout << os.str() << std::endl;
}

static void testctow_ostreambuffer(const char *wcstr,
				   const LIBCXX_NAMESPACE::const_locale &l,
				   size_t inbufsize, size_t outbufsize)
{
	std::wostringstream os;

	{
		std::vector<char> inpbuf;

		std::vector<wchar_t> outbuf;

		LIBCXX_NAMESPACE::ctow_ostreambuf wco(os, l);

		inpbuf.resize(inbufsize);
		outbuf.resize(outbufsize);

		std::ostream wo(&wco);

		wco.setbuf(&inpbuf[0], inpbuf.size());
		wco.setoutputbuf(&outbuf[0], outbuf.size());

		wo << wcstr;
		wco.sync();
	}

	testwtoc_ostreambuffer(os.str().c_str(), l, inbufsize, outbufsize);
}

static void testctow_iostreambuffer(const char *wcstr,
				    const LIBCXX_NAMESPACE::const_locale &l,
				    size_t inbufsize, size_t outbufsize)
{
	std::wstringstream os;

	{
		std::vector<char> inpbuf;

		std::vector<wchar_t> outbuf;

		LIBCXX_NAMESPACE::ctow_iostreambuf wco(os, l);

		inpbuf.resize(inbufsize);
		outbuf.resize(outbufsize);

		std::iostream wo(&wco);

		wco.pubsetbuf(&inpbuf[0], inpbuf.size());
		wco.setoutputbuf(&outbuf[0], outbuf.size());
		wco.setinputbuf(&outbuf[0], outbuf.size());

		wo << wcstr;
		wco.sync();
		wo.seekg(0);

		std::string l;

		std::getline(wo, l);
		std::cout << l << std::endl;
	}

	os.seekg(0);
	os.seekp(0);

	{
		std::vector<char> inpbuf;

		std::vector<wchar_t> outbuf;

		LIBCXX_NAMESPACE::ctow_iostreambuf wco(os, l);

		inpbuf.resize(inbufsize);
		outbuf.resize(outbufsize);

		std::iostream wo(&wco);

		wco.pubsetbuf(&inpbuf[0], inpbuf.size());
		wco.setoutputbuf(&outbuf[0], outbuf.size());
		wco.setinputbuf(&outbuf[0], outbuf.size());

		wo << std::string(wcstr, wcstr+4);
		wco.sync();

		wo.seekg(wo.tellp());

		std::string l;

		std::getline(wo, l);
		std::cout << l << std::endl;
	}
}

int main()
{
	static const wchar_t whelloworld[]={
		0x0048,
		0x00e8,
		0x006c,
		0x006c,
		0x014d,
		0x0020,
		0x0077,
		0x00f6,
		0x0072,
		0x006c,
		0x0064,
		0x0000};

	static const char helloworld[]={0x48, (char)0xc3, (char)0xa8, 0x6c, 0x6c, (char)0xc5, (char)0x8d, 0x20, 0x77, (char)0xc3, (char)0xb6, 0x72, 0x6c, 0x64, 0};

	try {
		LIBCXX_NAMESPACE::const_locale l(LIBCXX_NAMESPACE::locale::base::utf8());

		testwtoc_codecvt(whelloworld, l, 1024, 1024);
		testwtoc_codecvt(whelloworld, l, 1, 1024);
		testwtoc_codecvt(whelloworld, l, 1024, 1);
		testwtoc_codecvt(whelloworld, l, 1, 1);

		testctow_ostreambuffer(helloworld, l, 1024, 1024);
		testctow_ostreambuffer(helloworld, l, 1, 1024);
		testctow_ostreambuffer(helloworld, l, 1024, 1);
		testctow_ostreambuffer(helloworld, l, 1, 1);

		testctow_iostreambuffer(helloworld, l, 1024, 1024);
		testctow_iostreambuffer(helloworld, l, 1, 1024);
		testctow_iostreambuffer(helloworld, l, 1024, 1);
		testctow_iostreambuffer(helloworld, l, 1, 1);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
