/*
** Copyright 2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

/*! \file

This file defines a typesafe number.

*/

#ifndef x_exception_H
#define x_exception_H

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A typesafe numerical aliases.


template<typename base_type, typename tag>
class number {

	base_type value;

public:
	number() : value(base_type()) {}
	explicit number(base_type v) : value(v) {}

	explicit operator base_type() const { return value; }

	number &operator=(base_type v) { value=v; return *this; }
	number &operator=(const number &v) { value=v.value; return *this; }

	number operator+(base_type v) const { return number(value+v); }
	number operator-(base_type v) const { return number(value-v); }
	number operator*(base_type v) const { return number(value*v); }
	number operator/(base_type v) const { return number(value/v); }
	number operator%(base_type v) const { return number(value%v); }
	number operator+(const number &v) const { return number(value+v.value); }
	number operator-(const number &v) const { return number(value-v.value); }
	number operator*(const number &v) const { return number(value*v.value); }
	number operator/(const number &v) const { return number(value/v.value); }
	number operator%(const number &v) const { return number(value%v.value); }

	number &operator+=(base_type v) { value+=v; return *this; }
	number &operator-=(base_type v) { value-=v; return *this; }
	number &operator*=(base_type v) { value*=v; return *this; }
	number &operator/=(base_type v) { value/=v; return *this; }
	number &operator%=(base_type v) { value%=v; return *this; }
	number &operator+=(const number &v) { value+=v.value; return *this; }
	number &operator-=(const number &v) { value-=v.value; return *this; }
	number &operator*=(const number &v) { value*=v.value; return *this; }
	number &operator/=(const number &v) { value/=v.value; return *this; }
	number &operator%=(const number &v) { value%=v.value; return *this; }
};

#if 0
{
#endif
}
#endif
