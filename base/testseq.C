/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/exception.H"
#include "x/fd.H"
#include "x/tostring.H"
#include "x/fdstreambufobj.H"
#include "x/basicstreamcodecvtobj.H"
#include "x/basicstringstreamobj.H"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>

static void testseq()
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
		0x0064};

	static const char helloworld[]={0x48, (char)0xc3, (char)0xa8, 0x6c, 0x6c, (char)0xc5,
					(char)0x8d, 0x20, 0x77, (char)0xc3, (char)0xb6, 0x72,
					0x6c, 0x64, 0};

	LIBCXX_NAMESPACE::locale l(LIBCXX_NAMESPACE::locale::base::utf8());

	{
		std::ostringstream oback;

		LIBCXX_NAMESPACE::wtoc_ostream::streamref_t o=LIBCXX_NAMESPACE::wtoc_ostream::create(oback,
												 l);

		(*o) << std::wstring(whelloworld,
				     &whelloworld[sizeof(whelloworld)/
						 sizeof(whelloworld[0])])
		     << std::flush;

		if (oback.str() != helloworld)
		{
			std::cout << "wtoc_ostream failed (std::ostringstream)"
				  << std::endl;
			return;
		}
	}

	{
		LIBCXX_NAMESPACE::ostringstream oback(LIBCXX_NAMESPACE::ostringstream::create());

		LIBCXX_NAMESPACE::wtoc_ostream::streamref_t o=LIBCXX_NAMESPACE::wtoc_ostream::create(oback,
												 l);

		(*o) << std::wstring(whelloworld,
				     &whelloworld[sizeof(whelloworld)/
						 sizeof(whelloworld[0])])
		     << std::flush;

		if (oback->str() != helloworld)
		{
			std::cout << "wtoc_ostream failed (LIBCXX_NAMESPACE::ostringstream)"
				  << std::endl;
			return;
		}
	}

	{
		LIBCXX_NAMESPACE::wostringstream oback(LIBCXX_NAMESPACE::wostringstream::create());

		LIBCXX_NAMESPACE::ctow_ostream::streamref_t o=LIBCXX_NAMESPACE::ctow_ostream::create(oback,
												 l);

		(*o) << helloworld << std::flush;

		if (oback->str() !=
		    std::wstring(whelloworld,
				 &whelloworld[sizeof(whelloworld)/
					      sizeof(whelloworld[0])]))
		{
			std::cout << "ctow_ostream failed (LIBCXX_NAMESPACE::wostringstream)"
				  << std::endl;
			return;
		}
	}

	{
		std::wostringstream oback;

		LIBCXX_NAMESPACE::ctow_ostream::streamref_t o=LIBCXX_NAMESPACE::ctow_ostream::create(oback,
								       l);
		(*o) << helloworld << std::flush;

		if (oback.str() !=
		    std::wstring(whelloworld,
				 &whelloworld[sizeof(whelloworld)/
					      sizeof(whelloworld[0])]))
		{
			std::cout << "ctow_ostream failed (std::wostringstream)"
				  << std::endl;
			return;
		}
	}

	{
		std::istringstream isrc(helloworld);

		LIBCXX_NAMESPACE::ctow_istream::streamref_t o=LIBCXX_NAMESPACE::ctow_istream::create(isrc,
								       l);

		std::wstring w;

		std::getline( *o, w);

		if (w != 
		    std::wstring(whelloworld,
				 &whelloworld[sizeof(whelloworld)/
					      sizeof(whelloworld[0])]))
		{
			std::cout << "ctow_istream failed (std::istringstream)"
				  << std::endl;
			return;
		}
	}

	{
		LIBCXX_NAMESPACE::istringstream isrc(LIBCXX_NAMESPACE::istringstream::create(helloworld));

		LIBCXX_NAMESPACE::ctow_istream::streamref_t o=LIBCXX_NAMESPACE::ctow_istream::create(isrc,
								       l);

		std::wstring w;

		std::getline( *o, w);

		if (w != 
		    std::wstring(whelloworld,
				 &whelloworld[sizeof(whelloworld)/
					      sizeof(whelloworld[0])]))
		{
			std::cout << "ctow_istream failed (LIBCXX_NAMESPACE::istringstream)"
				  << std::endl;
			return;
		}
	}

	{
		std::wistringstream isrc
			(std::wstring(whelloworld,
				      &whelloworld[sizeof(whelloworld)/
						   sizeof(whelloworld[0])]));


		LIBCXX_NAMESPACE::wtoc_istream::streamref_t o=LIBCXX_NAMESPACE::wtoc_istream::create(isrc,
								       l);

		std::string w;

		std::getline( *o, w);

		if (w != helloworld)
		{
			std::cout << "wtoc_istream failed (std::wistringstream)"
				  << std::endl;
			return;
		}
	}

	{
		LIBCXX_NAMESPACE::wistringstream isrc
			(LIBCXX_NAMESPACE::wistringstream::create
			 (std::wstring(whelloworld,
				       &whelloworld[sizeof(whelloworld)/
						    sizeof(whelloworld[0])])));

		LIBCXX_NAMESPACE::wtoc_istream::streamref_t o=LIBCXX_NAMESPACE::wtoc_istream::create(isrc,
								       l);

		std::string w;

		std::getline( *o, w);

		if (w != helloworld)
		{
			std::cout << "wtoc_istream failed (LIBCXX_NAMESPACE::wistringstream)"
				  << std::endl;
			return;
		}
	}

	{
		LIBCXX_NAMESPACE::wstringstream oback(LIBCXX_NAMESPACE::wstringstream::create());

		LIBCXX_NAMESPACE::ctow_iostream::streamref_t o=LIBCXX_NAMESPACE::ctow_iostream::create(oback,
									 l);

		(*o) << helloworld;

		o->seekg(0);

		std::string w;

		std::getline(*o, w);

		if (oback->str() !=
		    std::wstring(whelloworld,
				 &whelloworld[sizeof(whelloworld)/
					      sizeof(whelloworld[0])]))
		{
			std::cout << "ctow_iostream failed (LIBCXX_NAMESPACE::wstringstream)"
				  << std::endl;
			return;
		}

		if (w != helloworld)
		{
			std::cout << "ctow_iostream failed (std::string)"
				  << std::endl;


		}
	}

	{
		std::wstringstream oback;

		LIBCXX_NAMESPACE::ctow_iostream::streamref_t o=LIBCXX_NAMESPACE::ctow_iostream::create(oback,
												   l);

		(*o) << helloworld;

		o->seekg(0);

		std::string w;

		std::getline(*o, w);

		if (oback.str() !=
		    std::wstring(whelloworld,
				 &whelloworld[sizeof(whelloworld)/
					      sizeof(whelloworld[0])]))
		{
			std::cout << "ctow_iostream failed (std::wstringstream)"
				  << std::endl;
			return;
		}

		if (w != helloworld)
		{
			std::cout << "ctow_iostream failed (std::string)"
				  << std::endl;


		}
	}

	{
		std::stringstream oback;

		LIBCXX_NAMESPACE::wtoc_iostream::streamref_t o=LIBCXX_NAMESPACE::wtoc_iostream::create(oback,
												   l);
		(*o) << std::wstring(whelloworld,
				     &whelloworld[sizeof(whelloworld)/
						  sizeof(whelloworld[0])]);
		o->seekg(0);

		std::wstring w;

		std::getline(*o, w);

		if (oback.str() != helloworld)
		{
			std::cout << "wtoc_iostream failed (std::stringstream)"
				  << std::endl;
			return;
		}

		if (w !=
		    std::wstring(whelloworld,
				 &whelloworld[sizeof(whelloworld)/
					      sizeof(whelloworld[0])]))

		{
			std::cout << "wtoc_iostream failed (std::wstring)"
				  << std::endl;


		}
	}

	{
		LIBCXX_NAMESPACE::stringstream oback(LIBCXX_NAMESPACE::stringstream::create());

		LIBCXX_NAMESPACE::wtoc_iostream::streamref_t o=LIBCXX_NAMESPACE::wtoc_iostream::create(oback,
												   l);
		(*o) << std::wstring(whelloworld,
				     &whelloworld[sizeof(whelloworld)/
						  sizeof(whelloworld[0])]);
		o->seekg(0);

		std::wstring w;

		std::getline(*o, w);

		if (oback->str() != helloworld)
		{
			std::cout << "wtoc_iostream failed (std::stringstream)"
				  << std::endl;
			return;
		}

		if (w !=
		    std::wstring(whelloworld,
				 &whelloworld[sizeof(whelloworld)/
					      sizeof(whelloworld[0])]))

		{
			std::cout << "wtoc_iostream failed (std::wstring)"
				  << std::endl;


		}
	}

	LIBCXX_NAMESPACE::fd tempFd=LIBCXX_NAMESPACE::fd::base::tmpfile();

	LIBCXX_NAMESPACE::iostream tempFdOstream(tempFd->getiostream());

	{
		LIBCXX_NAMESPACE::wtoc_iostream::streamref_t
			wios=LIBCXX_NAMESPACE::wtoc_iostream
			::create(tempFdOstream, l);
		
		(*wios) << std::wstring(whelloworld,
					&whelloworld[sizeof(whelloworld)/
						     sizeof(whelloworld[0])]);

		wios->seekg(0);

		wchar_t buf[sizeof(whelloworld)/sizeof(whelloworld[0])];

		wios->read(buf, sizeof(buf)/sizeof(buf[0]));

		if (wios->gcount() !=
		    sizeof(whelloworld)/sizeof(whelloworld[0]))
			return;

		if (!std::equal(buf, buf+sizeof(buf)/sizeof(buf[0]),
				whelloworld))
			return;
	}

	tempFdOstream->seekg(0);

	{
		char buf[256];

		tempFdOstream->read(buf, sizeof(buf));

		size_t n=tempFdOstream->gcount();

		size_t i;

		for (i=0; i<n; i++)
			std::cout << std::hex
				  << std::setw(2)
				  << std::setfill('0')
				  << (int)(unsigned char)buf[i]
				  << std::setw(0)
				  << std::dec << std::endl;
	}

	{
		size_t whelloworld_s=sizeof(whelloworld)/sizeof(whelloworld[0]);

		size_t bufarrays[]={1, whelloworld_s-1, whelloworld_s,
				    whelloworld_s+1, BUFSIZ};
		
		size_t i;
		for (i=0; i<sizeof(bufarrays)/sizeof(bufarrays[0]); i++)
		{
			std::wstring w(whelloworld, whelloworld+whelloworld_s);

			if (towstring(tostring(w, l), l) != w)
				throw EXCEPTION("tostring or towstring failed");
		}
	}

	std::cout << "Ok" << std::endl;
}

int main(int argc, char **argv)
{
	try {
		testseq();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
