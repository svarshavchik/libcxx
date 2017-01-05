/*
** Copyright 2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/number.H"
#include <iostream>
#include <sstream>

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

void test_custom_ops()
{
	coord_t a,b;

	test_assignment t={4};


	++a;
	a = a+2;
	a += 2;

	dim_t c=a-b;

	dim_squared_t d=c*c;
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
