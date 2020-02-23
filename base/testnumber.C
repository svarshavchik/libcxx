/*
** Copyright 2016-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/number.H"
#include "x/number_hash.H"
#include "x/exception.H"
#include <iostream>
#include <sstream>
#include <unordered_map>

class foo;
class foobar : public LIBCXX_NAMESPACE::number_default_base {

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

	if (6 < n2 ||
	    6 == n2 ||
	    6 <= n2 ||
	    4 > n2 ||
	    4 >= n2 ||
	    5 != n2)
		throw EXCEPTION("Comparisons are busted");

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

	o << number_t(4);

	if (o.str() != "4")
		throw EXCEPTION("<< failed");

	std::ostringstream o2;

	o2 << LIBCXX_NAMESPACE::number<char,void>{127}
	<< ' '
		   << LIBCXX_NAMESPACE::number<char,void>{-128}
	<< ' '
		   << LIBCXX_NAMESPACE::number<unsigned char, void>{255};

	if (o2.str() != "127 -128 255")
		throw EXCEPTION("<< failed");
}

template<typename type1, typename type2>
class add_op_define;

template<typename type1, typename type2>
class sub_op_define;

template<typename type1, typename type2>
class mul_op_define;

template<typename type1, typename type2>
class divmod_op_define;

template<typename type1, typename type2>
using my_add_op=typename add_op_define<type1, type2>::type;

template<typename type1, typename type2>
using my_sub_op=typename sub_op_define<type1, type2>::type;

template<typename type1, typename type2>
using my_mul_op=typename mul_op_define<type1, type2>::type;

template<typename type1, typename type2>
using my_divmod_op=typename divmod_op_define<type1, type2>::type;

class coord_base;
class dim_base;
class dim_squared_base;

class my_default_base {

public:
	template<typename number_1, typename number_2>
	using resulting_add_op=my_add_op<number_1, number_2>;

	//! Define resulting type for subtraction operations.
	template<typename number_1, typename number_2>
	using resulting_sub_op=my_sub_op<number_1, number_2>;

	//! Define resulting type for multiplication operations.
	template<typename number_1, typename number_2>
	using resulting_mul_op=my_mul_op<number_1, number_2>;

	//! Define resulting type for division operations.
	template<typename number_1, typename number_2>
	using resulting_div_op=my_divmod_op<number_1, number_2>;

	//! Define resulting type for modulus operations.
	template<typename number_1, typename number_2>
	using resulting_mod_op=my_divmod_op<number_1, number_2>;
};

typedef LIBCXX_NAMESPACE::number<int, coord_base, coord_base> coord_t;

typedef LIBCXX_NAMESPACE::number<unsigned, dim_base, dim_base> dim_t;

typedef LIBCXX_NAMESPACE::number<unsigned long long, dim_squared_base, dim_squared_base> dim_squared_t;

template<>
class add_op_define<coord_t, dim_t> {

public:
	typedef coord_t type;
};

template<>
class add_op_define<coord_t, dim_squared_t> {

public:
	typedef coord_t type;
};

template<>
class add_op_define<dim_t, coord_t> {

public:
	typedef coord_t type;
};

template<>
class add_op_define<dim_squared_t, coord_t> {

public:
	typedef coord_t type;
};

template<>
class sub_op_define<coord_t, coord_t> {

public:
	typedef dim_t type;
};

template<>
class sub_op_define<dim_t, dim_t> {

public:
	typedef dim_t type;
};

template<>
class sub_op_define<dim_t, dim_squared_t> {

public:
	typedef dim_squared_t type;
};

template<>
class sub_op_define<dim_squared_t, dim_squared_t> {

public:
	typedef dim_squared_t type;
};

template<>
class sub_op_define<dim_squared_t, dim_t> {

public:
	typedef dim_squared_t type;
};

template<>
class mul_op_define<dim_t, dim_t> {

public:
	typedef dim_squared_t type;
};

template<>
class mul_op_define<dim_t, dim_squared_t> {

public:
	typedef dim_squared_t type;
};

template<>
class mul_op_define<dim_squared_t, dim_t> {

public:
	typedef dim_squared_t type;
};

template<>
class mul_op_define<dim_squared_t, dim_squared_t> {

public:
	typedef dim_squared_t type;
};

template<>
class divmod_op_define<dim_t, dim_t> {

public:
	typedef dim_t type;
};

template<>
class divmod_op_define<dim_t, dim_squared_t> {

public:
	typedef dim_t type;
};

template<>
class divmod_op_define<dim_squared_t, dim_t> {

public:
	typedef dim_squared_t type;
};

template<>
class divmod_op_define<dim_squared_t, dim_squared_t> {

public:
	typedef dim_squared_t type;
};

class my_base {

public:
	template<typename number_1, typename number_2>
	using resulting_add_op=typename add_op_define<number_1, number_2>::type;

	template<typename number_1, typename number_2>
	using resulting_sub_op=typename sub_op_define<number_1, number_2>::type;

	template<typename number_1, typename number_2>
	using resulting_mul_op=typename mul_op_define<number_1, number_2>::type;

	template<typename number_1, typename number_2>
	using resulting_div_op=typename divmod_op_define<number_1, number_2>::type;

	template<typename number_1, typename number_2>
	using resulting_mod_op=typename divmod_op_define<number_1, number_2>::type;
};

class coord_base : public my_base {};
class dim_base : public my_base {};
class dim_squared_base : public my_base {};

struct test_assignment {
	coord_t a;
};

template<typename ...Args>
void iamused(Args && ...args)
{
}

void test_custom_ops()
{
	coord_t a,b;

	test_assignment t={4};

	++a;
	a = a+2;
	a += 2;

	dim_t c=a-b;

	dim_squared_t d=c*c;
	iamused(t, d);
}

void testoverflows()
{
	typedef LIBCXX_NAMESPACE::number<unsigned short, foo> snumber_t;
	typedef LIBCXX_NAMESPACE::number<int, foobar> inumber_t;

	if (snumber_t::overflows(0))
	{
		std::cout << "Shouldn't overflow." << std::endl;
		exit(1);
	}

	if (!snumber_t::overflows(-1))
	{
		std::cout << "Should overflow." << std::endl;
		exit(1);
	}

	if (snumber_t::truncate(-1) != 0)
	{
		std::cout << "Did not truncate to 0" << std::endl;
		exit(1);
	}

	inumber_t minus_1{-1};

	if (snumber_t::truncate(minus_1) != 0)
	{
		std::cout << "Did not truncate to 0" << std::endl;
		exit(1);
	}

	if (snumber_t::truncate(std::numeric_limits<long long>::max())
	    != std::numeric_limits<unsigned short>::max())
	{
		std::cout << "Did not truncate to max()" << std::endl;
	}

	inumber_t i=4;

	if (snumber_t::overflows(i))
	{
		std::cout << "Shouldn't overflow." << std::endl;
		exit(1);
	}

	i= -1;

	if (!snumber_t::overflows(i))
	{
		std::cout << "Should overflow." << std::endl;
		exit(1);
	}

	int n=0;

	try {
		snumber_t s{-1};
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Expected exception: " << e << std::endl;
		++n;
	}

	snumber_t s{0};
	try {
		s= -1;
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Expected exception: " << e << std::endl;
		++n;
	}

	if (n != 2)
	{
		std::cout << "Should've had more exceptions." << std::endl;
		exit(1);
	}

	LIBCXX_NAMESPACE::number<short, short> z1( (unsigned int) 0);
	LIBCXX_NAMESPACE::number<unsigned short, short> z2( (int) 0);

	if (! LIBCXX_NAMESPACE::number<unsigned, unsigned>::overflows(-1))
	{
		std::cout << "Assigning negative to an unsigned didn't overflow"
			  << std::endl;
		exit(1);
	}


	z1=z1.truncate( std::numeric_limits<short>::max() + 1.0 );

	if (z1 != std::numeric_limits<short>::max())
	{
		std::cout << "Double did not truncate to max short."
			<< std::endl;
			   exit(1);
	}

	z1=z1.truncate( std::numeric_limits<short>::min() - 1.0 );

	if (z1 != std::numeric_limits<short>::min())
	{
		std::cout << "Double did not truncate to min short."
			<< std::endl;
			   exit(1);
	}

	z2=z2.truncate( std::numeric_limits<unsigned short>::max() + 1.0 );

	if (z2 != std::numeric_limits<unsigned short>::max())
	{
		std::cout << "Double did not truncate to max unsigned short."
			  << std::endl;
			   exit(1);
	}

	z2=z2.truncate( -1.0 );

	if (z2 != 0)
	{
		std::cout << "Double did not truncate to 0."
			<< std::endl;
			   exit(1);
	}
}

void testtruncate()
{
	typedef LIBCXX_NAMESPACE::number<unsigned short, foo> snumber_t;
	typedef LIBCXX_NAMESPACE::number<int, foobar> inumber_t;

	if (snumber_t::truncate(-2) != 0)
		throw EXCEPTION("-2 should overflow to 0 for an unsigned type");

	if (snumber_t::truncate((unsigned long long)-1)
	    != std::numeric_limits<unsigned short>::max())
		throw EXCEPTION("<huge> didn't overflow to max unsigned short");

	if (inumber_t::truncate( (unsigned long long)-1)
	    != std::numeric_limits<int>::max())
		throw EXCEPTION("<huge> didn't overflow to max int");

	inumber_t n{3};

	auto nn= -n;

	inumber_t *nnp=&nn;

	if ((*nnp) != -3)
		throw EXCEPTION("Unary negation does not work");
}


void testhash()
{
	std::unordered_map<number_t, int> m;

	m.insert({number_t(4), 4});
}

void testinput()
{
	std::istringstream i{"100 40000"};

	LIBCXX_NAMESPACE::number<int16_t, int16_t> char_safe1, char_safe2;

	i >> char_safe1;

	if (char_safe1 != 100)
		throw EXCEPTION(">> failed (" << char_safe1 << ")");

	bool caught=false;

	try {
		i >> char_safe2;
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		caught=true;
		std::cout << "Expected exception: " << e << std::endl;
	}

	if (!caught)
		throw EXCEPTION(">> didn't catch overflow.");
}

int main()
{
	try {
		testnumber();
		testoverflows();
		testtruncate();
		testhash();
		testinput();
		if (compare<number_t, number2_t>::bar() ||
		    !compare<number_t, number_t>::bar())
		{
			std::cout << "Something's wrong" << std::endl;
			exit(1);
		}

		if (LIBCXX_NAMESPACE::number<double, void>::truncate(-8.0)
		    != -8)
			throw EXCEPTION("double.truncate is wrong");
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
