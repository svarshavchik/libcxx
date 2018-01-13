#include "x/weakcapture.H"
#include <iostream>
#include <cstdlib>
#include <type_traits>

template<typename foo> int Foo(const foo &f)
{
	int val=0;

	auto got=f.get();

	if (got)
	{
		auto &[a, b] = *got;

		typedef std::enable_if_t<std::is_same_v
					 <decltype(a),
					  LIBCXX_NAMESPACE
					  ::ref<LIBCXX_NAMESPACE::obj>>> a_t;

		a_t *p1=nullptr;

		typedef std::enable_if_t<std::is_same_v
					 <decltype(b),
					  LIBCXX_NAMESPACE
					  ::ref<LIBCXX_NAMESPACE::obj>>> b_t;

		b_t *p2=nullptr;


		if (p1 || p2)
			;
		val=1;
	}
	return val;
}

int main()
{
	auto a=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create(),
		b=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create();

	auto captured=LIBCXX_NAMESPACE::make_weak_capture(a, b);

	if (Foo(captured) != 1)
	{
		std::cerr << "Test 1 failed" << std::endl;

		exit(1);
	}

	b=a;

	if (Foo(captured) != 0)
	{
		std::cerr << "Test 2 failed" << std::endl;

		exit(1);
	}

	a=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create();
	b=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create();

	if (Foo(captured))
	{
		std::cerr << "Test 3 failed" << std::endl;

		exit(1);
	}

	captured=LIBCXX_NAMESPACE::make_weak_capture(a, b);

	if (Foo(captured) != 1)
	{
		std::cerr << "Test 4 failed" << std::endl;

		exit(1);
	}

	a=b;

	if (Foo(captured) != 0)
	{
		std::cerr << "Test 5 failed" << std::endl;

		exit(1);
	}

	auto captured2=LIBCXX_NAMESPACE::make_weak_capture(a);

	auto got=captured2.get();

	if (got)
		;
	else
	{
		std::cerr << "Test 6 failed" << std::endl;
		exit(1);
	}

	if (!got)
	{
		std::cerr << "Test 7 failed" << std::endl;
		exit(1);
	}

	return 0;
}
