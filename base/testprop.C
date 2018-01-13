/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/property_value.H"
#include "x/property_list.H"
#include "x/value_string.H"
#include "x/ymd.H"
#include "x/hms.H"
#include "x/fmtsize.H"
#include "x/exception.H"

#include <iostream>
#include <sstream>
#include <iterator>

std::string test1_test(const std::string &n)
{
	std::list<std::string> hier;

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

	std::string val;

	mycb() {}
	~mycb() {}

	void event(const LIBCXX_NAMESPACE::property::propvalueset_t &arg)
		override
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

	cb->val=defv;
	p->install(cb, n, cb->val);
	return cb;
}

void test2()
{
	auto props=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::property::listObj>
		::create();

	props->load("prop::1 = prop1val1\n"
		    "prop::2 = prop2val1\n", true, true);

	props->load("prop::2 = prop2val2\n"
		    "\n"
		    "prop3 = prop3val1  # comment\n", false, true);

	if (test2_get(props, "prop::1", "xx")->val
	    != "prop1val1" ||
	    test2_get(props, "prop::2", "xx")->val
	    != "prop2val1" ||
	    test2_get(props, "prop3", "xx")->val
	    != "prop3val1" ||
	    test2_get(props, "prop4", "yy")->val
	    != "yy")
		throw EXCEPTION("test2 failed(1)");

	LIBCXX_NAMESPACE::ptr<mycb> a=test2_get(props, "prop3", "zz"),
		b=test2_get(props, "prop4", "zz");

	if (a->val != "prop3val1" || b->val != "zz")
		throw EXCEPTION("test2 failed(2)");

	props->load("prop3",
		    "prop3val2", true, true);

	if (a->val != "prop3val2")
		throw EXCEPTION("test2 failed(3)");

	props->load("prop3 = prop3val2\n"
		    "prop4=prop4val1\n", false, true);

	if (a->val != "prop3val2" || b->val != "zz")
		throw EXCEPTION("test2 failed(4)");


	LIBCXX_NAMESPACE::property::value<std::string>
		prop3(props, "prop3", "xyz");

	if (prop3.get() != "prop3val2")
		throw EXCEPTION("test2 failed(5)");

	props->load("prop3=prop3val3\n", true, true);

	if (prop3.get() != "prop3val3")
		throw EXCEPTION("test2 failed(6)");
}

class mynotifycb : public LIBCXX_NAMESPACE::property::notifyObj {

public:
	LIBCXX_NAMESPACE::property::value<int> *p;

	int i;

	mynotifycb(LIBCXX_NAMESPACE::property::value<int> *pArg)

		: p(pArg), i(0)
	{
	}

	~mynotifycb() {}

	void event() override
	{
		i=p->get();
	}

};

void test3()
{
	LIBCXX_NAMESPACE::property::list l=
		LIBCXX_NAMESPACE::property::list::create();

	LIBCXX_NAMESPACE::property::value<int> intprop(l, "intprop", 1);

	auto notify=LIBCXX_NAMESPACE::ref<mynotifycb>::create(&intprop);

	intprop.installNotify(notify);

	l->load("intprop", "2", true, true);

	if (notify->i != 2)
		throw EXCEPTION("test3 failed");
}

void save_prop(const std::string &name,
	       const std::string &value)
{
	std::stringstream ss;

	std::map<std::string,
		 std::string> props;

	props[std::string(name.begin(), name.end())]
		=std::string(value.begin(), value.end());

	LIBCXX_NAMESPACE::property
		::save_properties(props, std::ostreambuf_iterator<char>(ss));

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
	(n, true, true)

int main(int argc, char **argv)
{
	alarm(30);

	try {
		test1();
		test2();
		{
			LIBCXX_NAMESPACE::property::value<int> value("", 4);

			value.get();
		}

		LIBCXX_NAMESPACE::property::value<double> double_value("");

		double_value.set( LIBCXX_NAMESPACE::value_string<double>
				       ::fromString("1.2",
						    LIBCXX_NAMESPACE::locale::create("C")));
		std::cout << double_value.get() << std::endl;

		UPD("property.int=4\n"
		    "property.str=foo\n");

		LIBCXX_NAMESPACE::property::value<unsigned long long> intvalue("property.int");
		LIBCXX_NAMESPACE::property::value<std::string> strvalue("property.str");

		LIBCXX_NAMESPACE::property::value<int> int2value("property.int2", 5);

		std::cout << "Initial int value: " << intvalue.get()
			  << std::endl;

		std::cout << "Initial str value: " << strvalue.get()
			  << std::endl;

		std::cout << "Initial int2 value: " << int2value.get()
			  << std::endl;

		UPD("property::int=5\n"
		    "property::int2=6\n"
		    "property::str=bar\n");

		std::cout << "int value: " << intvalue.get()
			  << std::endl;

		std::cout << "str value: " << strvalue.get()
			  << std::endl;

		std::cout << "int2 value: " << int2value.get()
			  << std::endl;


		LIBCXX_NAMESPACE::property::value<bool> boolvalue("property.bool", true);

		UPD("property::bool=0\n");

		std::cout << "bool value: " << boolvalue.get()
			  << std::endl;
		UPD("property::bool=true\n");

		std::cout << "bool value: " << boolvalue.get()
			  << std::endl;
		UPD("property::bool=false\n");

		std::cout << "bool value: " << boolvalue.get()
			  << std::endl;
		UPD("property::bool=YES\n");

		std::cout << "bool value: " << boolvalue.get()
			  << std::endl;

		UPD("property::bool=no\n");

		std::cout << "bool value: " << boolvalue.get()
			  << std::endl;

		UPD("property::hms=1 hour, 2 minutes\n"
		    "property::ymd=2 months 1 week\n");

		std::cout <<
			LIBCXX_NAMESPACE::property::value<LIBCXX_NAMESPACE::hms>("property::hms",
						   LIBCXX_NAMESPACE::hms(),
						   LIBCXX_NAMESPACE::locale::create("C")
						   ).get()
			.verboseString()
			  << std::endl;

		std::cout << (std::string)
			LIBCXX_NAMESPACE::property::value<LIBCXX_NAMESPACE::ymd::interval>("property::ymd",
							     LIBCXX_NAMESPACE::ymd::interval(),
							     LIBCXX_NAMESPACE::locale
							     ::create("C")
							     )
			.get() << std::endl;

		UPD("property::yyyymmdd=03-Aug-1969\n");

		if (LIBCXX_NAMESPACE::property::value<LIBCXX_NAMESPACE::ymd>
		    ("property::yyyymmdd").get() !=
		    LIBCXX_NAMESPACE::ymd(1969,8,3))
			throw EXCEPTION("ymd property parse failed");

		test3();

		LIBCXX_NAMESPACE::property::value<LIBCXX_NAMESPACE::memsize>
			memsize_prop("memsize", LIBCXX_NAMESPACE::memsize(4100));

		std::cout << memsize_prop.get().bytes << " bytes"
			  << std::endl;

		{
			LIBCXX_NAMESPACE::property::value<std::string>
				memsize_propstr("memsize", "0");

			std::cout << memsize_propstr.get()
				  << std::endl;
		}

		UPD("memsize=5000");

		std::cout << memsize_prop.get().bytes << " bytes"
			  << std::endl;

		{
			LIBCXX_NAMESPACE::property::value<std::string>
				memsize_propstr("memsize", "0");

			std::cout << memsize_propstr.get()
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
