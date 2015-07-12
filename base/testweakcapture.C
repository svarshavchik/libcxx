#include "x/weakcapture.H"
#include <iostream>
#include <cstdlib>

template<typename foo> int Foo(const foo &f)
{
	int val=0;

	if (f.get([&val]
		  (const LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj> &a,
		   const LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj> &b)
		  {
			  val |= 1;
		  }))
	{
		val |= 2;
	}
	return val;
}

int main()
{
	auto a=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create(),
		b=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create();

	auto captured=LIBCXX_NAMESPACE::make_weak_capture(a, b);

	if (Foo(captured) != 3)
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

	return 0;
}
