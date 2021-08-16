/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_valueobj_H
#define x_option_valueobj_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/option_valuebaseobj.H>
#include <x/option_parserfwd.H>
#include <x/value_string.H>
#include <x/hms.H>
#include <x/ymd.H>
#include <x/uriimpl.H>
#include <x/fmtsize.H>
#include <sstream>
#include <list>
#include <stdint.h>

namespace LIBCXX_NAMESPACE::option {
#if 0
};
#endif

//! Default implementation of set() for \ref valueObj "option values"

//! For values that do not have defaults.

template<typename value_type, typename=void> struct option_value_set {

	static inline int default_value(value_type &)
	{
		return option::parser::base::err_invalidoption;
	}
};

//! Specialization of \ref valueObj "option values"

//! Use default_value() to return the option's default value.

template<typename value_type>
struct option_value_set<value_type,
			std::void_t<decltype( value_string<value_type>
					      ::default_value())>> {

	static inline int default_value(value_type &value)
	{
		value=value_string<value_type>::default_value();

		return 0;
	}
};

//! An %option that's specified once

//! \internal
//! This is a reference-counted object that stores the option's value.

template<typename value_type>
class valueObj : virtual public obj, public valuebaseObj {

public:
	//! The type of this %option.
	typedef value_type value_t;

	//! Default constructor
	valueObj(//! Option's initial value
		 const value_t &valueArg=value_string<value_t>::initial_value())
		noexcept : value(valueArg),
			   initial_value(valueArg)
	{
	}

	//! Default destructor
	~valueObj()=default;

	//! The option's values

	value_type value;

	//! The option's initial value

	value_type initial_value;

	//! Set the option's value

	int set(//! The option's value, gets converted to value_type
		const std::string &valueArg,

		//! The locale to parse the %value
		const const_locale &localeArg) noexcept override
	{
		try {
			value=value_string<value_type>::from_string(valueArg,
								   localeArg);
		} catch (exception &e)
		{
			return option::parser::base::err_invalidoption;
		}

		return 0;
	}

	//! Set the %option to the default value

	int set() noexcept override
	{
		return option_value_set<value_type>::default_value(value);
	}

	//! Implement the reset operation

	void reset() noexcept override
	{
		value=initial_value;
	}

	//! This is a single %option value.

	bool multiple() const noexcept override
	{
		return false;
	}
};

//! A reference to a boolean %option value, a simple flag
typedef ref<valueObj<bool> > bool_value;

//! A reference to an int-value %option value
typedef ref<valueObj<int> > int_value;

//! A reference to an unsigned int-value %option value
typedef ref<valueObj<unsigned int> > uint_value;

//! A reference to a long-value %option value
typedef ref<valueObj<long> > long_value;

//! A reference to an unsigned long-value %option value
typedef ref<valueObj<unsigned long> > ulong_value;

//! A reference to a longlong-value %option value
typedef ref<valueObj<long long> > longlong_value;

//! A reference to an unsigned longlong-value %option value
typedef ref<valueObj<unsigned long long> > ulonglong_value;

//! A reference to an untyped (a string) %option value
typedef ref<valueObj<std::string> > string_value;

//! A reference to an int16_t %option value
typedef ref<valueObj<int16_t> > int16_value;

//! A reference to an int32_t %option value
typedef ref<valueObj<int32_t> > int32_value;

//! A reference to an int64_t %option value
typedef ref<valueObj<int64_t> > int64_value;

//! A reference to an uint16_t %option value
typedef ref<valueObj<uint16_t> > uint16_value;

//! A reference to an uint32_t %option value
typedef ref<valueObj<uint32_t> > uint32_value;

//! A reference to an uint64_t %option value
typedef ref<valueObj<uint64_t> > uint64_value;

//! A reference to an hms %option value
typedef ref<valueObj<hms> > hms_value;

//! A reference to an ymd %option value
typedef ref<valueObj<ymd> > ymd_value;

//! A reference to an ymd::interval %option value
typedef ref<valueObj<ymd::interval> > ymd_interval_value;

//! A reference to an uriimpl %option value
typedef ref<valueObj<uriimpl> > uri_value;

//! A reference to a \ref memsize "memsize" %option value
typedef ref<valueObj<memsize> > memsize_value;

//! A nullable pointer to a boolean %option value, a simple flag
typedef ptr<valueObj<bool> > bool_valueptr;

//! A nullable pointer to an int-value %option value
typedef ptr<valueObj<int> > int_valueptr;

//! A nullable pointer to an unsigned int-value %option value
typedef ptr<valueObj<unsigned int> > uint_valueptr;

//! A nullable pointer to a long-value %option value
typedef ptr<valueObj<long> > long_valueptr;

//! A nullable pointer to an unsigned long-value %option value
typedef ptr<valueObj<unsigned long> > ulong_valueptr;

//! A nullable pointer to a longlong-value %option value
typedef ptr<valueObj<long long> > longlong_valueptr;

//! A nullable pointer to an unsigned longlong-value %option value
typedef ptr<valueObj<unsigned long long> > ulonglong_valueptr;

//! A nullable pointer to an untyped (a string) %option value
typedef ptr<valueObj<std::string> > string_valueptr;

//! A nullable pointer to an int16_t %option value
typedef ptr<valueObj<int16_t> > int16_valueptr;

//! A nullable pointer to an int32_t %option value
typedef ptr<valueObj<int32_t> > int32_valueptr;

//! A nullable pointer to an int64_t %option value
typedef ptr<valueObj<int64_t> > int64_valueptr;

//! A nullable pointer to an uint16_t %option value
typedef ptr<valueObj<uint16_t> > uint16_valueptr;

//! A nullable pointer to an uint32_t %option value
typedef ptr<valueObj<uint32_t> > uint32_valueptr;

//! A nullable pointer to an uint64_t %option value
typedef ptr<valueObj<uint64_t> > uint64_valueptr;

//! A nullable pointer to an hms %option value
typedef ptr<valueObj<hms> > hms_valueptr;

//! A nullable pointer to an ymd %option value
typedef ptr<valueObj<ymd> > ymd_valueptr;

//! A nullable pointer to an ymd::interval %option value
typedef ptr<valueObj<ymd::interval> > ymd_interval_valueptr;

//! A nullable pointer to an uriimpl %option value
typedef ptr<valueObj<uriimpl> > uri_valueptr;

//! A nullable pointer to a \ref memsize "memsize" %option value
typedef ptr<valueObj<memsize> > memsize_valueptr;

#if 0
{
#endif
}
#endif
