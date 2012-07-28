/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "property_value.H"
#include "property_list.H"
#include "value_string.H"
#include "ymd.H"
#include "hms.H"
#include "fmtsize.H"
#include "exception.H"

#include <iostream>
#include <sstream>
#include <iterator>

std::string test1_test(const std::string &n)
{
	std::list<LIBCXX_NAMESPACE::property::propvalue> hier;

	LIBCXX_NAMESPACE::property::parsepropname(n.begin(), n.end(),
						hier);

	return LIBCXX_NAMESPACE::tostring(LIBCXX_NAMESPACE::property
					::combinepropname(hier));
}

void test1()
{
	if (test1_test("a.b :: c d") != "a::b::c::d" ||
	    test1_test(" :: a:b: ") != "a::b" ||
	    test1_test(".a.b.") != "a::b")
		throw EXCEPTION("test1 failed");

}

class mycb : public LIBCXX_NAMESPACE::eventhandlerObj<LIBCXX_NAMESPACE::
	property::propvalueset_t> {

public:

	LIBCXX_NAMESPACE::property::propvalue val;

	mycb() {}
	~mycb() noexcept {}

	void event(const LIBCXX_NAMESPACE::property::propvalueset_t &arg)

	{
		val=*arg.first;
	}
};

LIBCXX_NAMESPACE::ptr<mycb>
test2_get(const LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::property::listObj> &p,
	  const std::string &n,
	  const std::string &defv)
{
	auto cb=LIBCXX_NAMESPACE::ref<mycb>::create();

	cb->val=LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue,
					  std::string>
		::tostr(defv);
	p->install(cb, LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
		   ::tostr(n), cb->val);
	return cb;
}

void test2()
{
	auto props=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::property::listObj>
		::create();

	props->load(LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property
					      ::propvalue, std::string>
		    ::tostr("prop::1 = prop1val1\n"
			    "prop::2 = prop2val1\n"), true, true);

	props->load(LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property
					      ::propvalue, std::string>
		    ::tostr("prop::2 = prop2val2\n"
			    "\n"
			    "prop3 = prop3val1  # comment\n"), false, true);

	if (test2_get(props, "prop::1", "xx")->val
	    != LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
	    ::tostr("prop1val1") ||
	    test2_get(props, "prop::2", "xx")->val
	    != LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
	    ::tostr("prop2val1") ||
	    test2_get(props, "prop3", "xx")->val
	    != LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
	    ::tostr("prop3val1") ||
	    test2_get(props, "prop4", "yy")->val
	    != LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
	    ::tostr("yy"))
		throw EXCEPTION("test2 failed(1)");

	LIBCXX_NAMESPACE::ptr<mycb> a=test2_get(props, "prop3", "zz"),
		b=test2_get(props, "prop4", "zz");

	if (a->val != LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
	    ::tostr("prop3val1") || b->val != LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
	    ::tostr("zz"))
		throw EXCEPTION("test2 failed(2)");

	props->load(LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
		    ::tostr("prop3"),
		    LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
		    ::tostr("prop3val2"), true, true);

	if (a->val != LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
	    ::tostr("prop3val2"))
		throw EXCEPTION("test2 failed(3)");

	props->load(LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property
					      ::propvalue, std::string>
		    ::tostr("prop3 = prop3val2\n"
			    "prop4=prop4val1\n"), false, true);

	if (a->val != LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
	    ::tostr("prop3val2") || b->val != LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property::propvalue, std::string>
	    ::tostr("zz"))
		throw EXCEPTION("test2 failed(4)");


	LIBCXX_NAMESPACE::property::value<std::string>
		prop3(props, L"prop3", "xyz");

	if (prop3.getValue() != "prop3val2")
		throw EXCEPTION("test2 failed(5)");

	props->load(LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property
					      ::propvalue, std::string>
		    ::tostr("prop3=prop3val3\n"), true, true);

	if (prop3.getValue() != "prop3val3")
		throw EXCEPTION("test2 failed(6)");
}

void foo(char *p)
{
	std::string s;

	LIBCXX_NAMESPACE::ymd n;

	n.toString(p);
}

class mynotifycb : public LIBCXX_NAMESPACE::property::notifyObj {

public:
	LIBCXX_NAMESPACE::property::value<int> *p;

	int i;

	mynotifycb(LIBCXX_NAMESPACE::property::value<int> *pArg)

		: p(pArg), i(0)
	{
	}

	~mynotifycb() noexcept {}

	void event()
	{
		i=p->getValue();
	}

};

void test3()
{
	LIBCXX_NAMESPACE::property::list l=
		LIBCXX_NAMESPACE::property::list::create();

	LIBCXX_NAMESPACE::property::value<int> intprop(l, L"intprop", 1);

	auto notify=LIBCXX_NAMESPACE::ref<mynotifycb>::create(&intprop);

	intprop.installNotify(notify);

	l->load(L"intprop", L"2", true, true);

	if (notify->i != 2)
		throw EXCEPTION("test3 failed");
}

void save_prop(const std::string &name,
	       const std::string &value)
{
	std::stringstream ss;

	std::map<LIBCXX_NAMESPACE::property::propvalue,
		 LIBCXX_NAMESPACE::property::propvalue> props;

	props[LIBCXX_NAMESPACE::property::propvalue(name.begin(), name.end())]
		=LIBCXX_NAMESPACE::property::propvalue(value.begin(),
						     value.end());

	LIBCXX_NAMESPACE::property::save_properties(props,
						  std::ostreambuf_iterator<char>
						  (ss));

	ss.seekg(0);

	std::string l;

	while (!std::getline(ss, l).eof())
	{
		if (*l.c_str() == '#')
			continue;
		std::cout << "[" << l << "]" << std::endl;
	}
}

#define UPD(n)								\
	LIBCXX_NAMESPACE::property::load_properties			\
	(LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE::property		\
				   ::propvalue, std::string>::tostr(n),	\
	 true, true)

int main(int argc, char **argv)
{
	alarm(30);

	try {
		test1();
		test2();
		{
			LIBCXX_NAMESPACE::property::value<int> value(L"", 4);

			value.getValue();
		}

		LIBCXX_NAMESPACE::property::value<double> double_value(L"");

		double_value.setValue( LIBCXX_NAMESPACE::value_string<double>
				       ::fromString("1.2",
						    LIBCXX_NAMESPACE::locale::create("C")));
		std::cout << double_value.getValue() << std::endl;

		UPD("property.int=4\n"
		    "property.str=foo\n");

		LIBCXX_NAMESPACE::property::value<unsigned long long> intvalue(L"property.int");
		LIBCXX_NAMESPACE::property::value<std::string> strvalue(L"property.str");

		LIBCXX_NAMESPACE::property::value<int> int2value(L"property.int2", 5);

		std::cout << "Initial int value: " << intvalue.getValue()
			  << std::endl;

		std::cout << "Initial str value: " << strvalue.getValue()
			  << std::endl;

		std::cout << "Initial int2 value: " << int2value.getValue()
			  << std::endl;

		UPD("property::int=5\n"
		    "property::int2=6\n"
		    "property::str=bar\n");

		std::cout << "int value: " << intvalue.getValue()
			  << std::endl;

		std::cout << "str value: " << strvalue.getValue()
			  << std::endl;

		std::cout << "int2 value: " << int2value.getValue()
			  << std::endl;


		LIBCXX_NAMESPACE::property::value<bool> boolvalue(L"property.bool", true);

		UPD("property::bool=0\n");

		std::cout << "bool value: " << boolvalue.getValue()
			  << std::endl;
		UPD("property::bool=true\n");

		std::cout << "bool value: " << boolvalue.getValue()
			  << std::endl;
		UPD("property::bool=false\n");

		std::cout << "bool value: " << boolvalue.getValue()
			  << std::endl;
		UPD("property::bool=YES\n");

		std::cout << "bool value: " << boolvalue.getValue()
			  << std::endl;

		UPD("property::bool=no\n");

		std::cout << "bool value: " << boolvalue.getValue()
			  << std::endl;

		UPD("property::hms=1 hour, 2 minutes\n"
		    "property::ymd=2 months 1 week\n");

		std::cout <<
			LIBCXX_NAMESPACE::property::value<LIBCXX_NAMESPACE::hms>(L"property::hms",
						   LIBCXX_NAMESPACE::hms(),
						   LIBCXX_NAMESPACE::locale::create("C")
						   ).getValue()
			.verboseString<char>()
			  << std::endl;

		std::cout << (std::string)
			LIBCXX_NAMESPACE::property::value<LIBCXX_NAMESPACE::ymd::interval>(L"property::ymd",
							     LIBCXX_NAMESPACE::ymd::interval(),
							     LIBCXX_NAMESPACE::locale
							     ::create("C")
							     )
			.getValue() << std::endl;

		UPD("property::yyyymmdd=03-Aug-1969\n");

		if (LIBCXX_NAMESPACE::property::value<LIBCXX_NAMESPACE::ymd>
		    (L"property::yyyymmdd").getValue() !=
		    LIBCXX_NAMESPACE::ymd(1969,8,3))
			throw EXCEPTION("ymd property parse failed");

		test3();

		LIBCXX_NAMESPACE::property::value<LIBCXX_NAMESPACE::memsize>
			memsize_prop(L"memsize", LIBCXX_NAMESPACE::memsize(4100));

		std::cout << memsize_prop.getValue().bytes << " bytes"
			  << std::endl;

		{
			LIBCXX_NAMESPACE::property::value<std::string>
				memsize_propstr(L"memsize", "0");

			std::cout << memsize_propstr.getValue()
				  << std::endl;
		}

		UPD("memsize=5000");

		std::cout << memsize_prop.getValue().bytes << " bytes"
			  << std::endl;

		{
			LIBCXX_NAMESPACE::property::value<std::string>
				memsize_propstr(L"memsize", "0");

			std::cout << memsize_propstr.getValue()
				  << std::endl;
		}

		save_prop("foo", "bar");
		save_prop("foo::bar", "foo bar");
		save_prop("foo::bar", " foo bar ");
		save_prop("foo::bar", "foo\nbar\\foo#bar\nfoo\tbar\vfoo");

	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
