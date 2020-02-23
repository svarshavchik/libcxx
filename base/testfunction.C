/*
** Copyright 2014-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/functionalrefptrfwd.H"

class fwd_decl {
	LIBCXX_NAMESPACE::functionref<void ()> foo;
};

#include "x/functionalrefptr.H"
#include "x/mcguffinmap.H"
#include "x/exception.H"
#include "x/visitor.H"
#include <iostream>
#include <cstdlib>
#include <variant>
#include <utility>

using namespace LIBCXX_NAMESPACE;

static char first_char(const function<int(const char *)> &func)
{
	return func("Test");
}

static int none(const function<int(void)> &func)
{
	return func();
}

static void noreturn(const function<void(int)>&f, int value)
{
	f(value);
}

static void testfunction()
{
	if (first_char(make_function<int(const char *)>([]
							(const char *p)
							{
								return *p;
							}))
	    != 'T')
		throw EXCEPTION("Test 1 failed");

	int index=1;
	auto lambda = [index]
		(const char *p)
		{
			return p[index];
		};

	if (first_char(make_function<int(const char *)>(lambda)) != 'e')
		throw EXCEPTION("Test 2 failed");


	if (none(make_function<int(void)>([&index]
		{
			return index;
		})) != 1)
		throw EXCEPTION("Test 3 failed");

	auto lambda2=[&index]
		(int n)
		{
			index=n;
		};

	noreturn(make_function<void(int)>(lambda2), 5);

	if (index != 5)
		throw EXCEPTION("Test 4 failed");
}

void testfunctionref()
{
	int index=1;

	LIBCXX_NAMESPACE::functionref<void(int)> obj{
		[&index](int n)
		{
			index += n;
		}};

	LIBCXX_NAMESPACE::functionref<int(int)> obj2{
		[&index](int n)
		{
			return index+n;
		}};

	functionref<void(int)> *p=&obj;

	(*p)(3);

	obj(5);

	if (obj2(1) != 10)
		throw EXCEPTION("Test 6 failed");

	if (index != 9)
		throw EXCEPTION("Test 5 failed");

	auto obj3=obj2;

	obj3=[&index](int n)
		{
			return index+n*2;
		};

	if (obj3(1) != 11)
		throw EXCEPTION("test 7 failed");

	functionptr<int(int)> obj4;

	if (obj4)
		throw EXCEPTION("test 8 failed");

	auto obj5=obj4;

	obj4=nullptr;

	if (obj5)
		throw EXCEPTION("test 9 failed");

	obj5=[](int n)
		{
			return n;
		};

	if (!obj5)
		throw EXCEPTION("test 10 failed");

	if (obj5(7) != 7)
		throw EXCEPTION("test 11 failed");

	obj5=nullptr;

	if (obj5)
		throw EXCEPTION("test 12 failed");
}

void testmcguffinmap()
{
	auto x=LIBCXX_NAMESPACE::mcguffinmap
		<int,
		 LIBCXX_NAMESPACE::functionref<int()>>::create();

	LIBCXX_NAMESPACE::functionref<int()> f=[]{ return 1; };

	auto mcguffin=x->find_or_create(0,
					[&]
					{
						return f;
					});

	auto mcguffin2=x->find_or_create(0,
					 [&]
					 ()
					 ->LIBCXX_NAMESPACE::functionref<int()>
					 {
						 return []{return 1;};
					 });

	if (mcguffin != mcguffin2)
		throw EXCEPTION("testmcguffinmap 0 failed");

	if (x->find(0)->second.getptr()() != 1)
		throw EXCEPTION("testmcguffinmap 1 failed");

	mcguffin2=mcguffin={};

	if (x->find(0) != x->end())
		throw EXCEPTION("testmcguffinmap 2 failed");
}

void foo(int n=5)
{
}

void foo(const functionptr<void()> &f)
{
}

void bar()
{
	foo(3);
	foo([]{});
}

struct one{};
struct two{};

typedef std::variant<one,
		     two,
		     functionref<void (one &)>,
		     functionref<void (two &)>
		     > v_t;

int check_text_param(const v_t &v)
{
	return std::visit
		(visitor
		 {
		  [&](const functionref<void (one &)> &)
		  {
			  return 0;
		  },
		  [&](const functionref<void (two &)> &)
		  {
			  return 1;
		  },
		  [&](const one &)
		  {
			  return 2;
		  },
		  [&](const two &)
		  {
			  return 3;
		  }
		 }, v);
}


int main()
{
	try {
		testfunction();
		testfunctionref();
		testmcguffinmap();
		if (check_text_param(functionref<void (one &)>{
					[](one &)
					{
					}}) != 0)
			throw EXCEPTION("check_text_param(0) failed");
		if (check_text_param(functionref<void (two &)>{
					[](two &)
					{
					}}) != 1)
			throw EXCEPTION("check_text_param(1) failed");
		if (check_text_param(one{}) != 2)
			throw EXCEPTION("check_text_param(2) failed");
		if (check_text_param(two{}) != 3)
			throw EXCEPTION("check_text_param(3) failed");
	} catch (const exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}

	return 0;
}
