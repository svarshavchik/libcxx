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
#include <limits>
#include <typeinfo>

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

//! Define the unary operator on signed typesafe numbers only

//! \internal

template<typename number_1>
using number_default_uneg_op=
	typename std::enable_if<std::numeric_limits<typename number_1
						    ::value_type>::is_signed,
				number_1>::type;

//! Metaprogramming: whether the given type is a \ref number "typesafe number".

template<typename type>
class is_number_type : public std::false_type {};

//! Metaprogramming: whether the given type is a \ref number "typesafe number".

template<typename value_type_t, typename tag, typename base_class>
class is_number_type<number<value_type_t, tag, base_class>>
	: public std::true_type {};

//! Check for overflows/truncation of \ref number "typesafe numbers".

template<typename to_value_type, typename from_value_type,
	 bool is_typesafe_number=is_number_type<from_value_type>::value>
class number_overflows;

//! Helper class used by \ref number_overflows "number_overflows".

//! Determines whether an integer is smaller than what can be represented
//! by another integer type after it gets converted to the integer type.
//!
//! This default template specialization is used only when both integer
//! types are unsigned.

template<typename to_value_type, typename from_value_type,
	 bool to_is_signed=std::numeric_limits<to_value_type>::is_signed,
	 bool from_is_signed=std::numeric_limits<from_value_type>::is_signed>
class number_min_overflows {

public:

	template<typename other_value_type>
	static inline constexpr bool is_overflows(other_value_type value)
	{
		return value < std::numeric_limits<to_value_type>::min();
	}
};

//! Specialization.

//! This specialization is used when the original integer that's being
//! converted to another integer type is unsigned.
//!
//! An unsigned integer value cannot possibly be less than an integer value
//! of any type, because the smallest unsigned integer value is zero,
//! so this is always false.

template<typename to_value_type, typename from_value_type,
	 bool to_is_signed>
class number_min_overflows<to_value_type, from_value_type,
			   to_is_signed, false> {

public:
	template<typename other_value_type>
	static inline constexpr bool is_overflows(other_value_type value)
	{
		return false;
	}
};

//! Specialization.

//! This specialization is used when a signed integer value gets converted
//! to an unsigned integer type. If the signed integer value is zero, it
//! underflows, so we just compare it against 0.

template<typename to_value_type, typename from_value_type>
class number_min_overflows<to_value_type, from_value_type, false, true> {
public:
	template<typename other_value_type>
	static inline constexpr bool is_overflows(other_value_type value)
	{
		return value < 0;
	}
};

//! Helper class used by \ref number_overflows "number_overflows".

//! Determines whether an integer is larger than what can be represented
//! by another integer type after it gets converted to the integer type.
//!
//! This default template specialization is used when the integer value is
//! a signed type. In order to prevent "comparing unsigned with signed"
//! compiler warnings, the integer value is converted to an unsigned integer
//! value of the same type, but only after check that the integer value is
//! not negative.
//!
//! If it is negative it can't possibly exceed max() of whatever integer
//! type it is being converted to.
//!
//! After the integer value is determined not to be negative, both the
//! integer and the to-type's max() get converted to an unsigned type, and
//! compared as unsigned types.

template<typename to_value_type, typename from_value_type,
	 bool to_is_signed=std::numeric_limits<to_value_type>::is_signed,
	 bool from_is_integer=std::numeric_limits<from_value_type>::is_integer,
	 bool from_is_signed=std::numeric_limits<from_value_type>::is_signed>
class number_max_overflows {

public:

	template<typename other_value_type>
	static inline constexpr bool is_overflows(other_value_type value)
	{
		return value >= 0 &&
			static_cast<typename std::make_unsigned
				    <other_value_type>::type>(value) >
			static_cast<typename std::make_unsigned<to_value_type>
				    ::type>
			(std::numeric_limits<to_value_type>::max());
	}
};

//! Specialization.

//! This specialization is used when an unsigned integer type is getting
//! converted to another integer type, signed or unsigned. Since the
//! integer value is an unsigned type, we can skip the test for a negative
//! value used in the default template.

template<typename to_value_type, typename from_value_type, bool to_is_signed>
class number_max_overflows<to_value_type, from_value_type,
			   to_is_signed, true, false> {

public:

	template<typename other_value_type>
	static inline constexpr bool is_overflows(other_value_type value)
	{
		return static_cast<typename std::make_unsigned
				   <other_value_type>::type>(value) >
			static_cast<typename std::make_unsigned<to_value_type>
				    ::type>
			(std::numeric_limits<to_value_type>::max());
	}
};

template<typename to_value_type, typename from_value_type,
	 bool to_is_signed, bool from_is_signed>
class number_max_overflows<to_value_type, from_value_type, to_is_signed, false,
			   from_is_signed>
{

public:

	template<typename other_value_type>
	static inline constexpr bool is_overflows(other_value_type value)
	{
		return value >= 0 &&
			static_cast<from_value_type>(value) >
			static_cast<from_value_type>
			(std::numeric_limits<to_value_type>::max());
	}
};

//! Check for overflows/truncation of \ref number "typesafe numbers".

//! Specialization for checking of an overflow during conversion from a
//! natural integer type.

template<typename to_value_type, typename from_value_type>
class number_overflows<to_value_type, from_value_type, false> {

public:

	//! Factor in signed-ness and numeric_limits to determine an overflow.
	template<typename other_value_type>
	static inline constexpr bool overflows(other_value_type value)
	{
		return number_min_overflows<to_value_type, from_value_type>
			::is_overflows(value)
			||
			number_max_overflows<to_value_type, from_value_type>
			::is_overflows(value);
	}

	//! Factor in signed-ness and numeric_limits to truncate the number.

	template<typename other_value_type>
	static inline constexpr to_value_type truncate(other_value_type value)
	{
		if (number_min_overflows<to_value_type, from_value_type>
		    ::is_overflows(value))
			return std::numeric_limits<to_value_type>::min();

		if (number_max_overflows<to_value_type, from_value_type>
		    ::is_overflows(value))
			return std::numeric_limits<to_value_type>::max();

		return to_value_type(value);
	}
};

//! Check for overflows/truncation of \ref number "typesafe numbers".

//! Specialization for checking of an overflow during conversion from another
//! typesafe number.

template<typename to_value_type, typename from_value_type>
class number_overflows<to_value_type, from_value_type, true> {

public:

	//! Convenience typedef
	typedef typename from_value_type::value_type underlying_type;

	//! Forward the type-safe value to the specialization for natural integers.
	template<typename other_value_type>
	static inline constexpr bool overflows(other_value_type &&value)
	{
		return number_overflows<to_value_type, underlying_type,
					false>
			::overflows( (underlying_type)(value));
	}

	//! Forward the type-safe value to the specialization for natural integers.
	template<typename other_value_type>
	static inline constexpr to_value_type truncate(other_value_type &&value)
	{
		return number_overflows<to_value_type, underlying_type,
					false>
			::truncate( (underlying_type)(value));
	}

};

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

	//! Define resulting type for the unary negation operator
	template<typename number_1>
	using resulting_uneg_op=number_default_uneg_op<number_1>;
};

//! Throw an exception due to numerical overflow.

void numerical_overflow(const char *ftype,
			const char *ttype) __attribute__((noreturn));

//! Throw an exception due to numerical overflow in the >> operator.

void input_numerical_overflow(const char *ttype) __attribute__((noreturn));

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
	constexpr number(number_t vn) : n(vn)
	{
		if (overflows(vn))
			numerical_overflow(typeid(number_t).name(),
					   typeid(value_type_t).name());
	}

	//! Return the underlying number.
	constexpr explicit operator value_type_t() const { return n; }

	//! Assignment operator
	template<typename number_t,
		 typename V=typename std::enable_if<std::is_integral<number_t>::value>::type>
	constexpr number &operator=(number_t vn) {
		if (overflows(vn))
			numerical_overflow(typeid(number_t).name(),
					   typeid(value_type_t).name());

		n=vn;
		return *this;
	}

	//! If the given number overflows or underflows, truncate it to max() or min().

	template<typename other_value_type>
        static constexpr value_type_t truncate(other_value_type &&vn) {
		return number_overflows<value_type,
					typename std::decay<other_value_type>
					::type>
			::truncate(std::forward<other_value_type>(vn));
	}

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

	//! Unary negation operation.
	constexpr auto operator-() const
	{
		typedef typename base_class
			::template resulting_uneg_op<number>
			res_type;

		return res_type(-n);
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

	//! Returns true if the given number cannot be represented by this type

	template<typename other_value_type>
	static constexpr bool overflows(other_value_type && value)
	{
		return number_overflows<value_type,
					typename std::decay<other_value_type>
					::type>
			::overflows(std::forward<other_value_type>(value));
	}
};

//! Comparison operator overload for \ref number "type safe numbers".

template<typename value_type,
	 typename value_type_t, typename tag, typename base_class,
	 typename V=typename std::enable_if<std::is_integral<value_type>
					    ::value>::type>
constexpr bool operator==(value_type &&vn,
			  const number<value_type_t, tag, base_class> &n)
{
	return n == std::forward<value_type>(vn);
}

//! Comparison operator overload for \ref number "type safe numbers".

template<typename value_type,
	 typename value_type_t, typename tag, typename base_class,
	 typename V=typename std::enable_if<std::is_integral<value_type>
					    ::value>::type>
constexpr bool operator!=(value_type &&vn,
			  const number<value_type_t, tag, base_class> &n)
{
	return n != std::forward<value_type>(vn);
}

//! Comparison operator overload for \ref number "type safe numbers".

template<typename value_type,
	 typename value_type_t, typename tag, typename base_class,
	 typename V=typename std::enable_if<std::is_integral<value_type>
					    ::value>::type>
constexpr bool operator<(value_type &&vn,
			  const number<value_type_t, tag, base_class> &n)
{
	return n > std::forward<value_type>(vn);
}

//! Comparison operator overload for \ref number "type safe numbers".

template<typename value_type,
	 typename value_type_t, typename tag, typename base_class,
	 typename V=typename std::enable_if<std::is_integral<value_type>
					    ::value>::type>
constexpr bool operator<=(value_type &&vn,
			  const number<value_type_t, tag, base_class> &n)
{
	return n >= std::forward<value_type>(vn);
}

//! Comparison operator overload for \ref number "type safe numbers".

template<typename value_type,
	 typename value_type_t, typename tag, typename base_class,
	 typename V=typename std::enable_if<std::is_integral<value_type>
					    ::value>::type>
constexpr bool operator>(value_type &&vn,
			  const number<value_type_t, tag, base_class> &n)
{
	return n < std::forward<value_type>(vn);
}

//! Comparison operator overload for \ref number "type safe numbers".

template<typename value_type,
	 typename value_type_t, typename tag, typename base_class,
	 typename V=typename std::enable_if<std::is_integral<value_type>
					    ::value>::type>
constexpr bool operator>=(value_type &&vn,
			  const number<value_type_t, tag, base_class> &n)
{
	return n <= std::forward<value_type>(vn);
}

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

//! Overload >> operator for output streams.

template<typename CharT, typename Traits,
	 typename value_type_t, typename tag, typename base_class>
auto &operator>>(std::basic_istream<CharT, Traits> &i,
		 number<value_type_t, tag, base_class> &n)
{
	auto &res=i >> n.n;

	if (res.fail())
		input_numerical_overflow(typeid(value_type_t).name());

	return res;
}

#if 0
{
#endif
}
#endif
