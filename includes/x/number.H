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
#include <utility>
#include <iostream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! \internal

class number_default_base {};

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

template<typename value_type_t, typename tag,
	 typename base_class=number_default_base>
class number : public base_class {

public:
	//! The actual value.
	value_type_t n;

	//! The real number type
	typedef value_type_t value_type;

	//! Default constructor
	number() : n(value_type_t()) {}

	//! Construct from the underlying number
	explicit number(value_type_t vn) : n(vn) {}

	//! Return the underlying number.
	explicit operator value_type_t() const { return n; }

	//! Assignment operator
	number &operator=(value_type_t vn) { n=vn; return *this; }

	//! Assignment operator
	number &operator=(const number &v) { n=v.n; return *this; }

	//! Addition operator
	number operator+(value_type_t vn) const { return number(n+vn); }
	//! Subtraction operator
	number operator-(value_type_t vn) const { return number(n-vn); }
	//! Multiplication operator
	number operator*(value_type_t vn) const { return number(n*vn); }
	//! Division operator
	number operator/(value_type_t vn) const { return number(n/vn); }
	//! Modulus operator
	number operator%(value_type_t vn) const { return number(n%vn); }
	//! Addition operator
	number operator+(const number &v) const { return number(n+v.n); }
	//! Subtraction operator
	number operator-(const number &v) const { return number(n-v.n); }
	//! Multiplication operator
	number operator*(const number &v) const { return number(n*v.n); }
	//! Division operator
	number operator/(const number &v) const { return number(n/v.n); }
	//! Modulus operator
	number operator%(const number &v) const { return number(n%v.n); }

	//! Pre-increment operator
	number &operator++()
	{
		++n;
		return *this;
	}

	//! Pre-decrement operator
	number &operator--()
	{
		--n;
		return *this;
	}

	//! Post-increment operator
	number operator++(int)
	{
		auto copy=*this;
		++*this;
		return copy;
	}

	//! Post-decrement operator
	number operator--(int)
	{
		auto copy=*this;
		--*this;
		return copy;
	}

	//! Addition assignment operator
	number &operator+=(value_type_t vn) { n+=vn; return *this; }
	//! Subtraction assignment operator
	number &operator-=(value_type_t vn) { n-=vn; return *this; }
	//! Multiplication assignment operator
	number &operator*=(value_type_t vn) { n*=vn; return *this; }
	//! Division assignment operator
	number &operator/=(value_type_t vn) { n/=vn; return *this; }
	//! Modulus assignment operator
	number &operator%=(value_type_t vn) { n%=vn; return *this; }
	//! Addition assignment operator
	number &operator+=(const number &v) { n+=v.n; return *this; }
	//! Subtraction assignment operator
	number &operator-=(const number &v) { n-=v.n; return *this; }
	//! Multiplication assignment operator
	number &operator*=(const number &v) { n*=v.n; return *this; }
	//! Division assignment operator
	number &operator/=(const number &v) { n/=v.n; return *this; }
	//! Modulus assignment operator
	number &operator%=(const number &v) { n%=v.n; return *this; }

	//! Comparison operator
	bool operator==(value_type vn) const { return n == vn; }
	//! Comparison operator
	bool operator!=(value_type vn) const { return n != vn; }
	//! Comparison operator
	bool operator<(value_type vn) const { return n < vn; }
	//! Comparison operator
	bool operator<=(value_type vn) const { return n <= vn; }
	//! Comparison operator
	bool operator>(value_type vn) const { return n > vn; }
	//! Comparison operator
	bool operator>=(value_type vn) const { return n >= vn; }

	//! Comparison operator
	bool operator==(const number &v) const { return n == v.n; }
	//! Comparison operator
	bool operator!=(const number &v) const { return n != v.n; }
	//! Comparison operator
	bool operator<(const number &v) const { return n < v.n; }
	//! Comparison operator
	bool operator<=(const number &v) const { return n <= v.n; }
	//! Comparison operator
	bool operator>(const number &v) const { return n > v.n; }
	//! Comparison operator
	bool operator>=(const number &v) const { return n >= v.n; }
};

//! Addition operator overload
//!
//! \see number

template<typename value_type_t, typename tag, typename base_class,
	 typename number_t>
inline number<value_type_t, tag, base_class>
operator+(number_t &&n,
	  const number<value_type_t, tag, base_class> &o)
{
	return o.operator+(std::forward<number_t>(n));
}

//! Subtraction operator overload
//!
//! \see number

template<typename value_type_t, typename tag, typename base_class,
	 typename number_t>
inline number<value_type_t, tag, base_class>
operator-(number_t &&n,
	  const number<value_type_t, tag, base_class> &o)
{
	return o.operator-(std::forward<number_t>(n));
}

//! Multiplication operator overload
//!
//! \see number

template<typename value_type_t, typename tag, typename base_class,
	 typename number_t>
inline number<value_type_t, tag, base_class>
operator*(number_t &&n,
	  const number<value_type_t, tag, base_class> &o)
{
	return o.operator*(std::forward<number_t>(n));
}

//! Division operator overload
//!
//! \see number

template<typename value_type_t, typename tag, typename base_class,
	 typename number_t>
inline number<value_type_t, tag, base_class>
operator/(number_t &&n,
	  const number<value_type_t, tag, base_class> &o)
{
	return o.operator/(std::forward<number_t>(n));
}

//! Modulus operator overload
//!
//! \see number

template<typename value_type_t, typename tag, typename base_class,
	 typename number_t>
inline number<value_type_t, tag, base_class>
operator%(number_t &&n,
	  const number<value_type_t, tag, base_class> &o)
{
	return o.operator%(std::forward<number_t>(n));
}

//! Overload << operator for output streams.

template<typename CharT, typename Traits,
	 typename value_type_t, typename tag, typename base_class>
inline auto &operator<<(std::basic_ostream<CharT, Traits> &o,
			const number<value_type_t, tag, base_class> &n)
{
	return o << static_cast<value_type_t>(n);
}

#if 0
{
#endif
}
#endif
