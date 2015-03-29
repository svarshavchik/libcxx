/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/iconviofilter.H"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>

static void doiconv(size_t ibufs, size_t obufs,
		    LIBCXX_NAMESPACE::iconviofilter &ic,
		    const char *&inp,
		    size_t &inpleft,
		    char *&outp,
		    size_t &outpleft)
{
	size_t cntin;
	size_t cntout;

	do
	{
		ic.next_in=inp;
		ic.avail_in=inpleft < ibufs ? inpleft:ibufs;
		ic.next_out=outp;
		ic.avail_out=outpleft < obufs ? outpleft:obufs;

		cntin=ic.avail_in;
		cntout=ic.avail_out;

		ic.filter();

		inp += cntin - ic.avail_in;
		inpleft -= cntin - ic.avail_in;

		outp += cntout - ic.avail_out;
		outpleft += cntout - ic.avail_out;

	} while (cntin != ic.avail_in || cntout != ic.avail_out);
}

static void testiconv_int(size_t ibufs, size_t obufs)

{
	static const char str[]={0x48, (char)0xc3, (char)0xb3, 0x6c, 0x61,
				 0x48, (char)0xc3, (char)0xb3, 0x6c, 0x61};

	int32_t outbuf[20];
	size_t n;

	{
		LIBCXX_NAMESPACE::iconviofilter ic("UTF-8", "UCS-4LE");

		const char *inp=str;
		size_t inpleft=sizeof(str);

		char *outp=reinterpret_cast<char *>(outbuf);
		size_t outpleft=sizeof(outbuf);
		doiconv(ibufs, obufs, ic, inp, inpleft, outp, outpleft);

		if ((outp - reinterpret_cast<char *>(outbuf)) % sizeof(int32_t))
			throw EXCEPTION("Produced partial UCS4");

		n=(outp - reinterpret_cast<char *>(outbuf))
			/ sizeof(int32_t);
	}

	char outbuf2[20];
	{
		LIBCXX_NAMESPACE::iconviofilter ic("UCS-4LE", "UTF-8");

		const char *inp=reinterpret_cast<char *>(outbuf);
		size_t inpleft=n*sizeof(int32_t);

		char *outp=outbuf2;
		size_t outpleft=sizeof(outbuf2);
		doiconv(ibufs, obufs, ic, inp, inpleft, outp, outpleft);

		if ((outp - outbuf2) != sizeof(str) ||
		    !std::equal(outbuf2, outp, str))
			throw EXCEPTION("Did not get back original UTF-8");
	}
}

static void testiconv(size_t ibufs, size_t obufs)

{
	try {
		testiconv_int(ibufs, obufs);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::ostringstream o;

		o << "testiconv(" << ibufs << ", " << obufs << "): ";

		e->prepend(o.str());
		throw e;
	}
}

static void testerrconv_int(size_t ibufs, size_t obufs,
			    const char *inp,
			    size_t inpleft,
			    const char *fromcode,
			    const char *tocode)

{
	char outbuf[80];

	LIBCXX_NAMESPACE::iconviofilter ic(fromcode, tocode);

	char *outp=reinterpret_cast<char *>(outbuf);
	size_t outpleft=sizeof(outbuf);

	try {
		doiconv(ibufs, obufs, ic, inp, inpleft, outp, outpleft);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		return;
	}

	throw EXCEPTION("Did not throw an exception for a bad conversion");
}

static void testerrconv(size_t ibufs, size_t obufs,
			const char *utf8str,
			size_t utf8str_len,
			const char *fromcode,
			const char *tocode,
			const char *funcname)

{
	try {
		testerrconv_int(ibufs, obufs, utf8str, utf8str_len,
				fromcode, tocode);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::ostringstream o;

		o << funcname << "(" << ibufs << ", " << obufs << "): ";

		e->prepend(o.str());
		throw e;
	}
}

static void testiconv_ucs4()
{
	std::u32string u32=
		LIBCXX_NAMESPACE::iconviofilter::to_u32string("\xC2\xA0",
							      "UTF-8");

	if (u32.size() != 1 || u32[0] != 0xA0)
		throw EXCEPTION("ucs4 iconviofilter fail");

	if (LIBCXX_NAMESPACE::iconviofilter::from_u32string(u32,
							    "UTF-8")
	    != "\xC2\xA0")
		throw EXCEPTION("ucs4 iconviofilter fail");

	std::string large(512, '\xA0');

	if (LIBCXX_NAMESPACE::iconviofilter::from_u32string
	    (LIBCXX_NAMESPACE::iconviofilter::to_u32string(large,
							   "ISO-8859-1"),
	     "ISO-8859-1") != large)
		throw EXCEPTION("Large ucs4 iconviofilter failed");


}

int main(int argc, char **argv)
{
	try {
		testiconv(32, 32);
		testiconv(4, 32);
		testiconv(32, 4);
		testiconv(4, 4);
		testiconv(1, 4);
		testiconv(4, 1);
		testiconv(1, 1);

		const char utf8[]="UTF-8";
		const char ucs4[]="UCS-4";

		{
			static const char str[]={0x48, (char)0xc3, 0x6c, 0x61,
						 0x48, (char)0xc3, (char)0xb3, 0x6c, 0x61};

			static const char funcname[]="errutf8(middle)";

			testerrconv(32, 32, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(4, 32, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(32, 4, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(4, 4, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(1, 4, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(4, 1, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(1, 1, str, sizeof(str), utf8, ucs4,
				    funcname);
		}

		{
			static const char str[]={0x48, (char)0xc3, (char)0xb3, 0x6c, 0x61,
						 0x48, (char)0xc3};

			static const char funcname[]="errutf8(end)";

			testerrconv(32, 32, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(4, 32, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(32, 4, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(4, 4, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(1, 4, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(4, 1, str, sizeof(str), utf8, ucs4,
				    funcname);
			testerrconv(1, 1, str, sizeof(str), utf8, ucs4,
				    funcname);
		}

		{
			static const int32_t ucs4i[]={0x44, 0x44, 0x44};

			static const char funcname[]="errucs4";

			testerrconv(32, 32,
				    reinterpret_cast<const char *>(ucs4i),
				    sizeof(ucs4i)-1, ucs4, utf8, funcname);;
			testerrconv(4, 32, reinterpret_cast<const char *>(ucs4i),
				    sizeof(ucs4i)-1, ucs4, utf8, funcname);;
			testerrconv(32, 4, reinterpret_cast<const char *>(ucs4i),
				    sizeof(ucs4i)-1, ucs4, utf8, funcname);;
			testerrconv(4, 4, reinterpret_cast<const char *>(ucs4i),
				    sizeof(ucs4i)-1, ucs4, utf8, funcname);;
			testerrconv(1, 4, reinterpret_cast<const char *>(ucs4i),
				    sizeof(ucs4i)-1, ucs4, utf8, funcname);;
			testerrconv(4, 1, reinterpret_cast<const char *>(ucs4i),
				    sizeof(ucs4i)-1, ucs4, utf8, funcname);;
			testerrconv(1, 1, reinterpret_cast<const char *>(ucs4i),
				    sizeof(ucs4i)-1, ucs4, utf8, funcname);;
		}

		testiconv_ucs4();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

