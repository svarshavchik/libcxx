/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include <cmath>
#include <cstdlib>
#include "x/serialize.H"
#include "x/deserialize.H"
#include "x/tzfile.H"
#include "x/ymdhms.H"
#include "x/locale.H"
#include "x/fd.H"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <set>
#include <list>
#include <iterator>
#include <algorithm>

static void testserialize1()
{
	std::stringstream ss;

	bool zero=false;
	uint8_t one=1;
	int8_t two=2;
	uint16_t three=3;
	int16_t four=4;
	uint32_t five=5;
	int32_t six=6;
	uint64_t seven=7;
	int64_t nine=9;
	std::pair<char, char> pairchar;

	std::map<std::string, std::string> strmap;

	pairchar.first='A';
	pairchar.second='B';

	strmap["A"]="B";
	strmap["C"]="D";

	std::ostreambuf_iterator<char> iter=ss.rdbuf();

	LIBCXX_NAMESPACE::serialize::iterator<std::ostreambuf_iterator<char> >
		serializer(iter);

	serializer(zero);
	serializer(one);
	serializer(two);
	serializer(three);
	serializer(four);
	serializer(five);
	serializer(six);
	serializer(seven);
	serializer(nine);

	serializer("One");
	serializer("One");
	serializer(std::string("One"));
	serializer(std::string("One"));
	serializer(pairchar);
	serializer(strmap);

	ss.rdbuf()->pubseekpos(0);

	std::stringstream::int_type ch;

	std::cout << std::hex;

	while ((ch=ss.get()) != std::stringstream::traits_type::eof())
	{
		std::cout << std::setw(2) << std::setfill('0') << ch
			  << std::endl;
	}
	std::cout << std::dec;

	ss.seekg(0);
	ss.clear();
	ss.rdbuf()->pubseekpos(0);

	std::istreambuf_iterator<char> i(ss.rdbuf()), ee;

	LIBCXX_NAMESPACE::deserialize
		::iterator<std::istreambuf_iterator<char> >
		deserializer(i, ee);

	zero=99;
	one=two=three=four=five=six=seven=nine=zero;

	deserializer(zero);
	std::cout << "zero=" << (int)zero << std::endl;
	deserializer(one);
	std::cout << "one=" << (int)one << std::endl;
	deserializer(two);
	std::cout << "two=" << (int)two << std::endl;

	deserializer(three);
	std::cout << "three=" << three << std::endl;
	deserializer(four);
	std::cout << "four=" << four << std::endl;
	deserializer(five);
	std::cout << "five=" << five << std::endl;
	deserializer(six);
	std::cout << "six=" << six << std::endl;
	deserializer(seven);
	std::cout << "seven=" << seven << std::endl;
	deserializer(nine);
	std::cout << "nine=" << nine << std::endl;

	std::string string1;

	deserializer(string1);

	std::cout << "std::string(\"One\")=" << string1 << std::endl;

	std::vector<char> string2;

	deserializer(string2);

	std::cout << "std::string(\"One\")=";

	std::copy(string2.begin(), string2.end(),
		  std::ostreambuf_iterator<char>(std::cout.rdbuf()));
	std::cout << std::endl;

	std::list<char> string3;

	deserializer(string3);

	std::cout << "std::string(\"One\")=";
	std::copy(string3.begin(), string3.end(),
		  std::ostreambuf_iterator<char>(std::cout.rdbuf()));
	std::cout << std::endl;

	std::set<char> string4;

	deserializer(string4);
	std::cout << "std::set(\"One\")=";
	std::copy(string4.begin(), string4.end(),
		  std::ostreambuf_iterator<char>(std::cout.rdbuf()));
	std::cout << std::endl;

	deserializer(pairchar);

	std::cout << "std::pair=" << pairchar.first << ','
		  << pairchar.second << std::endl;
	strmap.clear();
	deserializer(strmap);

	std::map<std::string, std::string>::const_iterator
		b(strmap.begin()), e(strmap.end());

	while (b != e)
	{
		std::cout << "strmap[" << b->first << "]="
			  << b->second << std::endl;
		++b;
	}

	LIBCXX_NAMESPACE::serialize::sizeof_iterator counter;

	counter(strmap);

	std::cout << "strmap: " << counter.counter() << " bytes" << std::endl;
}

class objectone {

public:
	int intvalue;
	std::string stringvalue;

	template<typename iter_type>
	void serialize(iter_type &ser)
	{
		ser(intvalue);
		ser(stringvalue);
	}
};

namespace LIBCXX_NAMESPACE {
	namespace serialization {

		template<>
		class object_name<objectone> {

		public:
			operator const char *() const noexcept
			{
				return "obj1";
			}
		};
	}
}

void testserialize2()
{
	std::vector<objectone> vec;

	vec.push_back( objectone());

	vec[0].intvalue=1;
	vec[0].stringvalue="string";

	std::stringstream ss;
	std::ostreambuf_iterator<char> iter=ss.rdbuf();

	LIBCXX_NAMESPACE::serialize::iterator<std::ostreambuf_iterator<char> >
		serializer(iter);

	serializer(vec);

	ss.rdbuf()->pubseekpos(0);

	std::stringstream::int_type ch;

	std::cout << std::hex;

	while ((ch=ss.get()) != std::stringstream::traits_type::eof())
	{
		std::cout << std::setw(2) << std::setfill('0') << ch
			  << std::endl;
	}
	std::cout << std::dec;

	ss.seekg(0);
	ss.clear();
	ss.rdbuf()->pubseekpos(0);

	vec.clear();

	std::istreambuf_iterator<char> bb(ss.rdbuf()), ee;

	LIBCXX_NAMESPACE::deserialize::object(vec, bb, ee);

	std::cout << "vec.size=" << vec.size() << std::endl;

	if (vec.size() == 1)
	{
		std::cout << "vec.int=" << vec[0].intvalue << std::endl;

		std::cout << "vec.string=" << vec[0].stringvalue << std::endl;
	}
}

static void testserialize3()
{
	static const float f=-1;
	static const double d=atan(1)*4;
	static const long double ld=.125L;

	std::stringstream ss;
	std::ostreambuf_iterator<char> iter=ss.rdbuf();

	LIBCXX_NAMESPACE::serialize::iterator<std::ostreambuf_iterator<char> >
		serialize(iter);

	serialize(f)(d)(ld);

	float fr=0;
	double dr=0;
	long double ldr=0;

	std::istreambuf_iterator<char> i(ss.rdbuf()), e;

	LIBCXX_NAMESPACE::deserialize::iterator
		<std::istreambuf_iterator<char> > deserialize(i, e);

	LIBCXX_NAMESPACE::deserialize::deserialize_float<true, float> foo;

	deserialize(fr)(dr)(ldr);

	std::cout << abs(fr-f) << ' ' << abs(dr-d) << ' ' << (ldr-ld) << std::endl;
}

template<typename chk_type>
void compile_chk()
{
	std::stringstream sb;

	chk_type c;

	std::ostreambuf_iterator<char> iter=sb.rdbuf();

	LIBCXX_NAMESPACE::serialize::iterator<std::ostreambuf_iterator<char> >
		serialize(iter);

	serialize(c);

	std::istreambuf_iterator<char> i(sb.rdbuf()), e;

	LIBCXX_NAMESPACE::deserialize
		::iterator<std::istreambuf_iterator<char> > deserializer(i, e);

	deserializer(c);
}

class test4aobj : virtual public LIBCXX_NAMESPACE::obj {

public:
	test4aobj() noexcept
	{
	}

	~test4aobj()
	{
	}

	int intnum1;
	int intnum2;

	template<typename iter_type>
	void serialize(iter_type &i)
	{
		i(intnum1)(intnum2);
	}
};

typedef LIBCXX_NAMESPACE::ptr<test4aobj> test4a;

class test4bobj : virtual public LIBCXX_NAMESPACE::obj {

public:
	test4a a, b;

	test4bobj() noexcept
	{
	}

	~test4bobj()
	{
	}

	template<typename iter_type>
	void serialize(iter_type &i)
	{
		i(a)(b);
	}
};

typedef LIBCXX_NAMESPACE::ptr<test4bobj> test4b;

void testserialize4()
{
	test4b t4b(test4b::create());

	t4b->b=test4a::create();
	t4b->b->intnum1=1;
	t4b->b->intnum2=2;

	size_t cnt=LIBCXX_NAMESPACE::serialize::object(t4b);

	std::vector<char> buffer;

	buffer.resize(cnt);

	LIBCXX_NAMESPACE::serialize::object(t4b, buffer.begin());

	test4b t4b2;

	LIBCXX_NAMESPACE::deserialize::object(t4b2, buffer);

	if (t4b2.null() || t4b2->b->intnum1 != 1 || t4b2->b->intnum2 != 2)
		std::cout << "testserialize4 failed" << std::endl;
}

void testserialize5()
{
	LIBCXX_NAMESPACE::tzfile tz1(LIBCXX_NAMESPACE::tzfile::base::local());

	std::vector<char> buffer;

	buffer.resize(LIBCXX_NAMESPACE::serialize::object(tz1));

	LIBCXX_NAMESPACE::serialize::object(tz1, buffer.begin());

	LIBCXX_NAMESPACE::tzfileptr tz2;

	LIBCXX_NAMESPACE::deserialize::object(tz2, buffer);

	time_t t(time(0));

	LIBCXX_NAMESPACE::ymdhms now1(t, tz1),
		now2(t, tz2);

	LIBCXX_NAMESPACE::ymd y1(now1), y2(now2);
	LIBCXX_NAMESPACE::hms t1(now1), t2(now2);

	if (y1 != y1 || t1 != t2)
		std::cout << "testserialize5 failed"
			  << std::endl;

	buffer.resize(LIBCXX_NAMESPACE::serialize::object(now1));
	LIBCXX_NAMESPACE::serialize::object(now1, buffer.begin());

	LIBCXX_NAMESPACE::deserialize::object(now2, buffer);

	if ((std::string)now1 != (std::string)now2)
		std::cout << "testserialize5, part 2 failed: "
			  << (std::string)now1
			  << ", "
			  << (std::string)now2 << std::endl;

}

void testserialize6()
{
	LIBCXX_NAMESPACE::locale::create("")->global();

	auto l=LIBCXX_NAMESPACE::locale::base::global();

	std::vector<char> buffer;

	buffer.resize(LIBCXX_NAMESPACE::serialize::object(l));

	LIBCXX_NAMESPACE::serialize::object(l, buffer.begin());

	auto l2=LIBCXX_NAMESPACE::locale::create("");

	LIBCXX_NAMESPACE::deserialize::object(l2, buffer);
}

class testserialize7_s {

public:
	std::string stringnum;

	template<typename iter_type>
	void serialize(iter_type &i)
	{
		i(stringnum, 16);
	}
};

void testserialize7()
{
	testserialize7_s t7s;

	t7s.stringnum="AAA";

	std::vector<char> buffer;

	buffer.resize(LIBCXX_NAMESPACE::serialize::object(t7s));

	LIBCXX_NAMESPACE::serialize::object(t7s, buffer.begin());

	testserialize7_s t7s2;

	LIBCXX_NAMESPACE::deserialize::object(t7s2, buffer);

	if (t7s2.stringnum != "AAA")
		throw EXCEPTION("testserialize7 test 1 failed");

	t7s.stringnum="01234567890123456789";

	buffer.resize(LIBCXX_NAMESPACE::serialize::object(t7s));

	LIBCXX_NAMESPACE::serialize::object(t7s, buffer.begin());

	try {
		LIBCXX_NAMESPACE::deserialize::object(t7s2, buffer);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		return;
	}

	throw EXCEPTION("testserialize7 test 2 failed");
}

class Aobj : virtual public LIBCXX_NAMESPACE::obj {

public:
	int n;

	Aobj(int nArg) noexcept : n(nArg)
	{
	}

	~Aobj()
	{
	}

	template<typename iter_type>
	void serialize(iter_type &iter)
	{
		iter(n);
	}
};


class Bobj : virtual public LIBCXX_NAMESPACE::obj {

public:
	std::string s;

	Bobj(const std::string &sArg) noexcept : s(sArg)
	{
	}

	~Bobj()
	{
	}

	template<typename iter_type>
	void serialize(iter_type &iter)
	{
		iter(s);
	}
};

class abtraits {

public:
	template<typename iter_type>
	static bool classlist(iter_type &iter)
	{
		return iter.template serialize<Aobj>() ||
			iter.template serialize<Bobj>();
	}

	template<typename obj_type>
	static LIBCXX_NAMESPACE::ptr<obj_type> classcreate()
;
};

template<>
LIBCXX_NAMESPACE::ptr<Aobj> abtraits::classcreate<Aobj>()
{
	return LIBCXX_NAMESPACE::ptr<Aobj>::create(-1);
}

template<>
LIBCXX_NAMESPACE::ptr<Bobj> abtraits::classcreate<Bobj>()
{
	return LIBCXX_NAMESPACE::ptr<Bobj>::create("XX");
}

void all_compile_chk()
{
	compile_chk<std::string>();
	compile_chk<std::deque<int> >();
	compile_chk<std::deque<std::string> >();
	compile_chk<std::list<int> >();
	compile_chk<std::list<std::string> >();
	compile_chk<std::vector<int> >();
	compile_chk<std::vector<std::string> >();
	compile_chk<std::set<int> >();
	compile_chk<std::set<std::string> >();
	compile_chk<std::multiset<int> >();
	compile_chk<std::multiset<std::string> >();
	compile_chk<std::map<int, int> >();
	compile_chk<std::map<std::string, std::string> >();
	compile_chk<std::multimap<int, int> >();
	compile_chk<std::multimap<std::string, std::string> >();
}

void testserialize9()
{
	std::string foo("0bar0");

	std::vector<char> serbuf;

	{
		typedef std::back_insert_iterator<std::vector<char> >  iter;

		iter i(serbuf);

		LIBCXX_NAMESPACE::serialize::iterator<iter> serializer(i);

		serializer.range(foo.begin()+1, foo.end()-1);
	}

	std::vector<char>::iterator b(serbuf.begin()), e(serbuf.end());

	LIBCXX_NAMESPACE::deserialize
		::iterator<std::vector<char>::iterator>
		deserializer(b, e);

	std::string str;
	deserializer(str);

	if (str != "bar")
		throw EXCEPTION("testserialiaze9 failed");
}

template<typename fd_type>
void testserialize10_deser(const std::vector<char> &serbuf,
			   fd_type &foo)
{
	std::vector<char>::const_iterator b(serbuf.begin()), e(serbuf.end());

	LIBCXX_NAMESPACE::deserialize
		::iterator<std::vector<char>::const_iterator>
		deserializer(b, e);
	deserializer(foo);

	std::string s;

	foo->seek(0, SEEK_SET);
	std::getline(*foo->getistream(), s);

	if (s == "test")
		return;

	throw EXCEPTION("fd serialization failed");
}

void testserialize10()
{
	std::vector<char> serbuf;

	{
		typedef std::back_insert_iterator<std::vector<char> >  iter;

		iter i(serbuf);

		LIBCXX_NAMESPACE::serialize::iterator<iter> serializer(i);

		LIBCXX_NAMESPACE::fd tmpfile(LIBCXX_NAMESPACE::fd::base::tmpfile());

		(*tmpfile->getostream()) << "test" << std::endl << std::flush;

		tmpfile->seek(0, SEEK_SET);

		serializer(tmpfile);
	}

	{
		LIBCXX_NAMESPACE::fdptr foo=LIBCXX_NAMESPACE::fd::base::tmpfile();

		testserialize10_deser(serbuf, foo);
	}

	{
		LIBCXX_NAMESPACE::fd foo(LIBCXX_NAMESPACE::fd::base::tmpfile());

		testserialize10_deser(serbuf, foo);
	}
}

enum foobar {
	foo,
	bar
};

LIBCXX_SERIALIZE_ENUMERATION(foobar, char);

void testserialize11()
{
	foobar foo_v=foo;
	foobar bar_v=bar;

	std::vector<char> serbuf;

	{
		typedef std::back_insert_iterator<std::vector<char> >  iter;

		iter i(serbuf);

		LIBCXX_NAMESPACE::serialize::iterator<iter> serializer(i);

		serializer(foo_v);
		serializer(bar_v);
	}

	std::vector<char>::iterator b(serbuf.begin()), e(serbuf.end());

	LIBCXX_NAMESPACE::deserialize
		::iterator<std::vector<char>::iterator>
		deserializer(b, e);

	foobar foo_v2, bar_v2;

	deserializer(foo_v2);
	deserializer(bar_v2);

	if (foo != foo_v2 || bar != bar_v2)
		throw EXCEPTION("test11 failed");

}

int main(int argc, char *argv[])
{
	try {
		std::cout << "test1" << std::endl;
		testserialize1();
		std::cout << "test2" << std::endl;
		testserialize2();
		std::cout << "test3" << std::endl;
		testserialize3();
		std::cout << "test4" << std::endl;
		testserialize4();
		std::cout << "test5" << std::endl;
		testserialize5();
		std::cout << "test6" << std::endl;
		testserialize6();
		std::cout << "test7" << std::endl;
		testserialize7();
		std::cout << "test9" << std::endl;
		testserialize9();
		std::cout << "test10" << std::endl;
		testserialize10();
		std::cout << "test11" << std::endl;
		testserialize11();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
