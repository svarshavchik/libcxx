/*
** Copyright 2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

/*! \file

This file defines a typesafe number.

*/

#ifndef x_number_H
#define x_number_H

#include <x/namespace.h>
#include <ostream>
#include <utility>
#include <type_traits>
#include <iostream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! \internal

class number_default_base;

template<typename value_type_t, typename tag,
	 typename base_class=number_default_base> class number;

//! Define only operations on same \ref number "typesafe numerical aliases".

//! Allows operations on \ref number "typesafe numerical aliases" to be
//! only with the same alias, resulting in the same alias.
//!
//! \internal

template<typename number_1, typename number_2>
using number_default_same_op_only=
	typename std::enable_if<std::is_same<number_1, number_2>::value,
				number_1>::type;

//! Default base class for "typesafe numerical aliases".

//! Specifies \ref number_default_same_op_only "for all arithmetical operations".
//!
//! \internal
class number_default_base {

public:
	//! Define resulting type for addition operations.

	template<typename number_1, typename number_2>
	using resulting_add_op=number_default_same_op_only<number_1, number_2>;

	//! Define resulting type for subtraction operations.
	template<typename number_1, typename number_2>
	using resulting_sub_op=number_default_same_op_only<number_1, number_2>;

	//! Define resulting type for multiplication operations.
	template<typename number_1, typename number_2>
	using resulting_mul_op=number_default_same_op_only<number_1, number_2>;

	//! Define resulting type for division operations.
	template<typename number_1, typename number_2>
	using resulting_div_op=number_default_same_op_only<number_1, number_2>;

	//! Define resulting type for modulus operations.
	template<typename number_1, typename number_2>
	using resulting_mod_op=number_default_same_op_only<number_1, number_2>;
};

//! A typesafe numerical alias.

//! \code
//! class number_tag;
//!
//! typedef number<int, number_tag> number_t;
//!
//! number_t number{4}, number2=number*2;
//!
//! int value=number;
//! \endcode
//!
//! The first template parameter is any numberic type. The second template
//! parameter is not used, except for its reference, so the end result is
//! a discrete template instance. The template class implements all
//! operators on the underlying type.
//!
//! \code
//! typedef number_t::value_type value_type;
//! \endcode
//!
//! The `value_type` type returns the underlying numeric type.
//!
//! \code
//! class base_class {
//!
//! public:
//!
//!     static constexpr int infinite() { return -1; }
//!
//! };
//!
//! typedef number<int, number_tag, base_class> number_t;
//!
//! int n=number_t::infinite();
//!
//! \endcode
//!
//! The optional third template parameter specifies a class that
//! the \ref number "number" inherits from.

template<typename value_type_t, typename tag, typename base_class>
class number : public base_class {

public:
	//! The actual value.
	value_type_t n;

	//! The real number type
	typedef value_type_t value_type;

	//! Default constructor
	constexpr number() : n(value_type_t()) {}

	//! Construct from the underlying number
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr number(number_t vn) : n(vn) {}

	//! Return the underlying number.
	constexpr explicit operator value_type_t() const { return n; }

	//! Assignment operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr number &operator=(number_t vn) { n=vn; return *this; }

	//! Assignment operator
	constexpr number &operator=(const number &v) { n=v.n; return *this; }

	//! Addition operator
	template<typename value_type_t2, typename tag2, typename base_class2>
	constexpr auto operator+(const number<value_type_t2,
				 tag2, base_class2> &v) const
	{
		typedef number<value_type_t2, tag2, base_class2> number2;
		typedef typename base_class
			::template resulting_add_op<number, number2>
			res_type;

		typedef typename res_type::value_type value_type;

		return res_type(value_type(n)+value_type(v.n));
	}

	//! Subtraction operator
	template<typename value_type_t2, typename tag2, typename base_class2>
	constexpr auto operator-(const number<value_type_t2,
				 tag2, base_class2> &v) const
	{
		typedef number<value_type_t2, tag2, base_class2> number2;
		typedef typename base_class
			::template resulting_sub_op<number,number2>
			res_type;

		typedef typename res_type::value_type value_type;

		return res_type(value_type(n)-value_type(v.n));
	}

	//! Multiplication operator
	template<typename value_type_t2, typename tag2, typename base_class2>
	constexpr auto operator*(const number<value_type_t2,
				 tag2, base_class2> &v) const
	{
		typedef number<value_type_t2, tag2, base_class2> number2;
		typedef typename base_class
			::template resulting_mul_op<number, number2>
			res_type;

		typedef typename res_type::value_type value_type;

		return res_type(value_type(n)*value_type(v.n));
	}

	//! Division operator
	template<typename value_type_t2, typename tag2, typename base_class2>
	constexpr number operator/(const number<value_type_t2,
				   tag2, base_class2> &v) const
	{
		typedef number<value_type_t2, tag2, base_class2> number2;
		typedef typename base_class
			::template resulting_div_op<number, number2>
			res_type;

		typedef typename res_type::value_type value_type;

		return res_type(value_type(n)/value_type(v.n));
	}

	//! Modulus operator
	template<typename value_type_t2, typename tag2, typename base_class2>
	constexpr number operator%(const number<value_type_t2,
				   tag2, base_class2> &v) const
	{
		typedef number<value_type_t2, tag2, base_class2> number2;
		typedef typename base_class
			::template resulting_mod_op<number, number2>
			res_type;

		typedef typename res_type::value_type value_type;

		return res_type(value_type(n)%value_type(v.n));
	}

	//! Addition operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr auto operator+(number_t vn) const
	{
		auto me= *this;

		me.n += vn;
		return me;
	}
	//! Subtraction operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr auto operator-(number_t vn) const
	{
		auto me= *this;

		me.n -= vn;
		return me;
	}
	//! Multiplication operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr auto operator*(number_t vn) const
	{
		auto me= *this;

		me.n *= vn;
		return me;
	}

	//! Division operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr auto operator/(number_t vn) const
	{
		auto me= *this;

		me.n /= vn;
		return me;
	}

	//! Modulus operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr auto operator%(number_t vn) const
	{
		auto me= *this;

		me.n %= vn;
		return me;
	}

	//! Pre-increment operator
	constexpr number &operator++()
	{
		++n;
		return *this;
	}

	//! Pre-decrement operator
	constexpr number &operator--()
	{
		--n;
		return *this;
	}

	//! Post-increment operator
	constexpr number operator++(int)
	{
		auto copy=*this;
		++*this;
		return copy;
	}

	//! Post-decrement operator
	constexpr number operator--(int)
	{
		auto copy=*this;
		--*this;
		return copy;
	}

	//! Addition assignment operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr number &operator+=(number_t vn) { n += vn; return *this; }
	//! Subtraction assignment operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr number &operator-=(number_t vn) { n -= vn; return *this; }
	//! Multiplication assignment operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr number &operator*=(number_t vn) { n *= vn; return *this; }
	//! Division assignment operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr number &operator/=(number_t vn) { n /= vn; return *this; }
	//! Modulus assignment operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr number &operator%=(number_t vn) { n %= vn; return *this; }
	//! Addition assignment operator
	template<typename value_type_t2, typename tag2, typename base_class2>
	constexpr number &operator+=(const number<value_type_t2, tag2, base_class2> &v)
	{
		*this = *this + v;
		return *this;
	}

	//! Subtraction assignment operator
	template<typename value_type_t2, typename tag2, typename base_class2>
	constexpr number &operator-=(const number<value_type_t2, tag2, base_class2> &v)
	{
		*this = *this - v;
		return *this;
	}

	//! Multiplication assignment operator
	template<typename value_type_t2, typename tag2, typename base_class2>
	constexpr number &operator*=(const number<value_type_t2, tag2, base_class2> &v)
	{
		*this = *this * v;
		return *this;
	}

	//! Division assignment operator
	template<typename value_type_t2, typename tag2, typename base_class2>
	constexpr number &operator/=(const number<value_type_t2, tag2, base_class2> &v)
	{
		*this = *this / v;
		return *this;
	}

	//! Modulus assignment operator
	template<typename value_type_t2, typename tag2, typename base_class2>
	constexpr number &operator%=(const number<value_type_t2, tag2, base_class2> &v)
	{
		*this = *this % v;
		return *this;
	}

	//! Comparison operator
	constexpr bool operator==(value_type vn) const { return n == vn; }
	//! Comparison operator
	constexpr bool operator!=(value_type vn) const { return n != vn; }
	//! Comparison operator
	constexpr bool operator<(value_type vn) const { return n < vn; }
	//! Comparison operator
	constexpr bool operator<=(value_type vn) const { return n <= vn; }
	//! Comparison operator
	constexpr bool operator>(value_type vn) const { return n > vn; }
	//! Comparison operator
	constexpr bool operator>=(value_type vn) const { return n >= vn; }

	//! Comparison operator
	constexpr bool operator==(const number &v) const { return n == v.n; }
	//! Comparison operator
	constexpr bool operator!=(const number &v) const { return n != v.n; }
	//! Comparison operator
	constexpr bool operator<(const number &v) const { return n < v.n; }
	//! Comparison operator
	constexpr bool operator<=(const number &v) const { return n <= v.n; }
	//! Comparison operator
	constexpr bool operator>(const number &v) const { return n > v.n; }
	//! Comparison operator
	constexpr bool operator>=(const number &v) const { return n >= v.n; }
};

//! Addition operator overload
//!
//! \see number

template<typename value_type_t, typename tag, typename base_class,
	 typename number_t,
	 typename is_numeric=
	 typename std::enable_if<std::is_integral<number_t>::value>::type>
constexpr auto
operator+(number_t &&n,
	  const number<value_type_t, tag, base_class> &o)
{
	return o.operator+(std::forward<number_t>(n));
}

//! Subtraction operator overload
//!
//! \see number

template<typename value_type_t, typename tag, typename base_class,
	 typename number_t,
	 typename is_numeric=
	 typename std::enable_if<std::is_integral<number_t>::value>::type>
constexpr auto
operator-(number_t &&n,
	  const number<value_type_t, tag, base_class> &o)
{
	return number<value_type_t, tag, base_class>(n)-o;
}

//! Multiplication operator overload
//!
//! \see number

template<typename value_type_t, typename tag, typename base_class,
	 typename number_t,
	 typename is_numeric=
	 typename std::enable_if<std::is_integral<number_t>::value>::type>
constexpr auto
operator*(number_t &&n,
	  const number<value_type_t, tag, base_class> &o)
{
	return o.operator*(std::forward<number_t>(n));
}

//! Division operator overload
//!
//! \see number

template<typename value_type_t, typename tag, typename base_class,
	 typename number_t,
	 typename is_numeric=
	 typename std::enable_if<std::is_integral<number_t>::value>::type>
constexpr auto
operator/(number_t &&n,
	  const number<value_type_t, tag, base_class> &o)
{
	return number<value_type_t, tag, base_class>(n)/o;
}

//! Modulus operator overload
//!
//! \see number

template<typename value_type_t, typename tag, typename base_class,
	 typename number_t,
	 typename is_numeric=
	 typename std::enable_if<std::is_integral<number_t>::value>::type>
constexpr auto
operator%(number_t &&n,
	  const number<value_type_t, tag, base_class> &o)
{
	return number<value_type_t, tag, base_class>(n)%o;
}

//! Overload << operator for output streams.

template<typename CharT, typename Traits,
	 typename value_type_t, typename tag, typename base_class>
auto &operator<<(std::basic_ostream<CharT, Traits> &o,
		 const number<value_type_t, tag, base_class> &n)
{
	return o << n.n;
}

#if 0
{
#endif
}
#endif
