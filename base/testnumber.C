/*
** Copyright 2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/number.H"
#include <iostream>
#include <sstream>

class foo;
class foobar {

public:

	static constexpr int infinite() { return -1; }
};

typedef LIBCXX_NAMESPACE::number<int, foo> number_t;
typedef LIBCXX_NAMESPACE::number<int, foo, foobar> number2_t;
typedef LIBCXX_NAMESPACE::number<unsigned long long, foo> ull_t;

template<typename a, typename b, typename c=void>
class compare {

public:
	static int bar()
	{
		return 0;
	}
};

template<typename a, typename b>
class compare<a, b, std::void_t<decltype(std::declval<a &>()=std::declval<b>())>> {

public:
	static int bar()
	{
		return 1;
	}
};

void testnumber()
{
	number_t n1;
	number_t n2(4);

	n1=n2=n1;

	n2=5;
	n1=n2+4;
	n1=n2-4;
	n1=n2*4;
	n1=n2/4;
	n1=n2%4;

	n2=5;
	n1=n2+n2;
	n1=n2-n2;
	n1=n2*n2;
	n1=n2/n2;
	n1=n2%n2;

	n2=5;
	n1= n2 += 4;
	n1= n2 -= 4;
	n1= n2 *= 4;
	n1= n2 /= 4;
	n1= n2 %= 4;

	n2=5;
	n1= n2 += n2;
	n1= n2 -= n2;

	n2=5;
	n1= n2 *= n2;
	n1= n2 /= n2;
	n1= n2 %= n2;

	if (n1 * number2_t::infinite() == 1 ||
	    n1 != 1 ||
	    n1 < 1 ||
	    n1 <= 1 ||
	    n1 > 1 ||
	    n1 >= 1 ||

	    n2 == n1 ||
	    n2 != n1 ||
	    n2 < n1 ||
	    n2 <= n1 ||
	    n2 > n1 ||
	    n2 >= n1)
	{
		n1=n2;
	}

	ull_t u{4};

	u=2+u;
	u=10-u;
	u=20/u;
	u=2*u;
	u=1%u;

	n1=2;

	if (n1++ != 2 || n1-- != 3 || n1 != 2 ||
	    ++n1 != 3 || --n1 != 2)
	{
		std::cout << "Increment operators wrong" << std::endl;
		exit(1);
	}

	std::ostringstream o;

	o << n1;
}

int main()
{
	testnumber();

	if (compare<number_t, number2_t>::bar() ||
	    !compare<number_t, number_t>::bar())
	{
		std::cout << "Something's wrong" << std::endl;
		exit(1);
	}
	return 0;
}
