/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/exception.H"
#include "x/codecvtiter.H"
#include "x/tostring.H"
#include "x/property_properties.H"

#include <algorithm>
#include <iterator>

void test1()
{
	std::string cs="H\xc3\xb3\xc3\xb3la";
	std::wstring ws=L"Hóóla";

	std::wstring wcs;

	LIBCXX_NAMESPACE::property
		::load_property(LIBCXX_NAMESPACE::stringize
				<LIBCXX_NAMESPACE::property::propvalue,
				 std::string>
				::tostr(LIBCXX_NAMESPACE_STR
					"::iofilter::codecvtsize"),
				LIBCXX_NAMESPACE::stringize
				<LIBCXX_NAMESPACE::property::propvalue,
				 std::string>
				::tostr("256"), true, true);

	typedef LIBCXX_NAMESPACE::ocodecvtiter<std::back_insert_iterator
					     <std::wstring> >::ctow_iter_t
		towiter_t;

	typedef LIBCXX_NAMESPACE::ocodecvtiter_bytype<std::back_insert_iterator
						    <std::wstring>,
						    char, wchar_t>::iter_t
		towiter2_t;

	{	
		towiter_t::iter cvtin=towiter_t
			::create(std::back_insert_iterator<std::wstring>
				 (wcs), LIBCXX_NAMESPACE::locale::base::utf8());

		std::copy(cs.begin(), cs.end(), cvtin);
		cvtin.flush();
	}

	if (wcs != ws)
		throw EXCEPTION("test1 failed (1)");

	for (size_t i=0; i<6; i++)
	{
		cs=cs + cs;
		ws=ws + ws;
	}

	wcs.clear();

	{	
		towiter2_t::iter cvtin=towiter2_t
			::create(std::back_insert_iterator<std::wstring>
				 (wcs), LIBCXX_NAMESPACE::locale::base::utf8());

		std::copy(cs.begin(), cs.end(), cvtin);
		cvtin.flush();
	}

	if (wcs != ws)
		throw EXCEPTION("test1 failed (2)");

	std::string ccs=
		LIBCXX_NAMESPACE::tostring(wcs, LIBCXX_NAMESPACE::locale::base::utf8());

	if (ccs != cs)
		throw EXCEPTION("test1 failed (3)");

}

class onlywide {

public:
	std::wstring s;

	static const LIBCXX_NAMESPACE::stringable_t stringable=
		LIBCXX_NAMESPACE::class_towstring;

	template<typename out_iter>
	out_iter toWideString(out_iter p,
			      const LIBCXX_NAMESPACE::const_locale &l) const
	{
		return std::copy(s.begin(), s.end(), p);
	}

	template<typename in_iter>
	static onlywide fromWideString(const in_iter &b, const in_iter &e,
				       const LIBCXX_NAMESPACE::const_locale &l)
	{
		onlywide w;

		w.s=std::wstring(b, e);

		return w;
	}
};

template<LIBCXX_NAMESPACE::stringable_t class_t>
class onlynarrow {

public:
	std::string s;

	static const LIBCXX_NAMESPACE::stringable_t stringable=class_t;

	template<typename out_iter>
	out_iter toString(out_iter p,
			      const LIBCXX_NAMESPACE::const_locale &l) const
	{
		return std::copy(s.begin(), s.end(), p);
	}

	template<typename in_iter>
	static onlynarrow<class_t>
	fromString(const in_iter &b, const in_iter &e,
				     const LIBCXX_NAMESPACE::const_locale &l)
	{
		onlynarrow<class_t> w;

		w.s=std::string(b, e);

		return w;
	}
};

class wideandnarrow : public onlywide,
		      public onlynarrow< LIBCXX_NAMESPACE::class_tostring> {

public:

	static const LIBCXX_NAMESPACE::stringable_t stringable=
		LIBCXX_NAMESPACE::class_toboth;

	template<typename in_iter>
	static wideandnarrow
	fromWideString(const in_iter &b, const in_iter &e,
		       const LIBCXX_NAMESPACE::const_locale &l)
	{
		wideandnarrow wn;

		static_cast<onlywide &>(wn).s=
			onlywide::fromWideString(b, e, l).s;

		return wn;
	}


	template<typename in_iter>
	static wideandnarrow
	fromString(const in_iter &b, const in_iter &e,
				     const LIBCXX_NAMESPACE::const_locale &l)
	{
		wideandnarrow wn;

		static_cast<onlynarrow<LIBCXX_NAMESPACE::class_tostring> &>
			(wn).s=onlynarrow<LIBCXX_NAMESPACE::class_tostring>
			::fromString(b, e, l).s;

		return wn;
	}
};

template<typename class_t> 
void test2_test(const std::string &str)
{
	std::string msg="H\xc3\xb3\xc3\xb3la";
	std::wstring wmsg=L"Hóóla";

	class_t w=LIBCXX_NAMESPACE::value_string<class_t>
		::fromString(msg, LIBCXX_NAMESPACE::locale::base::utf8());

	if (LIBCXX_NAMESPACE::value_string<class_t>
	    ::toString(w, LIBCXX_NAMESPACE::locale::base::utf8()) != msg)
		throw EXCEPTION(str + " failed (1)");

	w=LIBCXX_NAMESPACE::value_string<class_t>
		::fromWideString(wmsg, LIBCXX_NAMESPACE::locale::base::utf8());

	if (LIBCXX_NAMESPACE::value_string<class_t>
	    ::toWideString(w, LIBCXX_NAMESPACE::locale::base::utf8()) != wmsg)
		throw EXCEPTION(str + " failed (2)");
}


void test2()
{
	test2_test<onlywide>("test2.onlywide");

	test2_test<onlynarrow< LIBCXX_NAMESPACE::class_tostring > >("test2.tostring");

	test2_test<onlynarrow< LIBCXX_NAMESPACE::class_tostring_utf8 > >("test2.tostring_utf8");
	test2_test<wideandnarrow>("test2.wideandnarrow");

}

template<typename ctow_t, typename wtoc_t>
void test3(const std::string &msg)
{
	std::string msg2;
	std::wstring wmsg;

	LIBCXX_NAMESPACE::locale utf8=LIBCXX_NAMESPACE::locale::base::utf8();

	std::copy( ctow_t::create(msg.begin(), msg.end(), utf8),
		   typename ctow_t::iter(),
		   std::back_insert_iterator<std::wstring>(wmsg));

	std::copy( wtoc_t::create(wmsg.begin(), wmsg.end(), utf8),
		   typename wtoc_t::iter(),
		   std::back_insert_iterator<std::string>(msg2));

	if (msg != msg2)
		throw EXCEPTION("test3 failed");
}

int main(int argc, char **argv)
{
	try {
		test1();
		test2();

		std::string msg="H\xc3\xb3\xc3\xb3la";

		for (int i=0; i<10; i++)
		{
			test3<LIBCXX_NAMESPACE::icodecvtiter
			      <std::string::const_iterator>
			      ::ctow_iter_t,
			      LIBCXX_NAMESPACE::icodecvtiter
			      <std::wstring::const_iterator>
			      ::wtoc_iter_t>(msg);


			test3<LIBCXX_NAMESPACE::icodecvtiter_bytype
			      <std::string::const_iterator, char, wchar_t>
			      ::iter_t,
			      LIBCXX_NAMESPACE::icodecvtiter_bytype
			      <std::wstring::const_iterator, wchar_t, char>
			      ::iter_t>(msg);

			std::string msg2;

			std::back_insert_iterator<std::string> msg2_oiter(msg2);

			std::copy(LIBCXX_NAMESPACE::icodecvtiter_bytype
				  <std::string::const_iterator, char, char>
				  ::iter_t::create(msg.begin(), msg.end()),
				  LIBCXX_NAMESPACE::icodecvtiter_bytype
				  <std::string::const_iterator, char, char>
				  ::iter_t::iter(),
				  LIBCXX_NAMESPACE::ocodecvtiter_bytype
				  < std::back_insert_iterator<std::string>,
				    char, char>::iter_t::create(msg2_oiter)
				  ).flush();

			msg=msg + msg2;
		}
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
