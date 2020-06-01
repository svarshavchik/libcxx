/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_listvalueobj_H
#define x_option_listvalueobj_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/option_valuebaseobj.H>
#include <x/option_parser.H>
#include <x/value_string.H>
#include <sstream>
#include <list>
#include <stdint.h>

namespace LIBCXX_NAMESPACE::option {
#if 0
};
#endif

//! An %option that's specified multiple times.

//! \internal
//! The option's values are collected in a list.

template<typename value_type>
class listvalueObj : virtual public obj, public valuebaseObj {

public:
	//! The type of this %option.
	typedef value_type value_t;

	//! Default constructor
	listvalueObj() noexcept {}

	//! Default destructor
	~listvalueObj() {}

	//! The option's values

	std::list<value_type> values;

	//! Set the option's value

	int set(//! The option's value, gets converted to value_type
		const std::string &valueArg,

		//! The locale to parse the %value
		const const_locale &localeArg) noexcept override
	{
		try {
			values.push_back(value_string<value_type>
					 ::from_string(valueArg, localeArg));
		} catch (exception &e)
		{
			return option::parser::base::err_invalidoption;
		}

		return 0;
	}

	//! Implement the reset operation

	void reset() noexcept override
	{
		values.clear();
	}

	//! Set the %option to the default value

	int set() noexcept override
	{
		return option::parser::base::err_invalidoption; // No default value
	}

	//! This %option takes multiple values

	bool multiple() const noexcept override
	{
		return true;
	}

};

//! A reference to a list boolean values
typedef ref<listvalueObj<bool> > bool_list;

//! A reference to an int-value %option values
typedef ref<listvalueObj<int> > int_list;

//! A reference to an unsigned int-value %option values
typedef ref<listvalueObj<unsigned int> > uint_list;

//! A reference to a long-value %option values
typedef ref<listvalueObj<long> > long_list;

//! A reference to an unsigned long-value %option values
typedef ref<listvalueObj<unsigned long> > ulong_list;

//! A reference to a longlong-value %option values
typedef ref<listvalueObj<long long> > longlong_list;

//! A reference to an unsigned longlong-value %option values
typedef ref<listvalueObj<unsigned long long> > ulonglong_list;

//! A reference to an untyped (a string) %option values
typedef ref<listvalueObj<std::string> > string_list;

//! A reference to an int16_t %option values
typedef ref<listvalueObj<int16_t> > int16_list;

//! A reference to an int32_t %option values
typedef ref<listvalueObj<int32_t>  > int32_list;

//! A reference to an int64_t %option values
typedef ref<listvalueObj<int64_t> > int64_list;

//! A reference to an uint16_t %option values
typedef ref<listvalueObj<uint16_t> > uint16_list;

//! A reference to an uint32_t %option values
typedef ref<listvalueObj<uint32_t> > uint32_list;

//! A reference to an uint64_t %option values
typedef ref<listvalueObj<uint64_t> > uint64_list;

//! A reference to an hms %option value
typedef ref<listvalueObj<hms> > hms_list;

//! A reference to an ymd %option value
typedef ref<listvalueObj<ymd> > ymd_list;

//! A reference to an ymd::interval %option value
typedef ref<listvalueObj<ymd::interval> > ymd_interval_list;

//! A reference to an uriimpl %option value
typedef ref<listvalueObj<uriimpl> > uri_list;

//! A nullable pointer reference to a list boolean values
typedef ptr<listvalueObj<bool> > bool_listptr;

//! A nullable pointer reference to an int-value %option values
typedef ptr<listvalueObj<int> > int_listptr;

//! A nullable pointer reference to an unsigned int-value %option values
typedef ptr<listvalueObj<unsigned int> > uint_listptr;

//! A nullable pointer reference to a long-value %option values
typedef ptr<listvalueObj<long> > long_listptr;

//! A nullable pointer reference to an unsigned long-value %option values
typedef ptr<listvalueObj<unsigned long> > ulong_listptr;

//! A nullable pointer reference to a longlong-value %option values
typedef ptr<listvalueObj<long long> > longlong_listptr;

//! A nullable pointer reference to an unsigned longlong-value %option values
typedef ptr<listvalueObj<unsigned long long> > ulonglong_listptr;

//! A nullable pointer reference to an untyped (a string) %option values
typedef ptr<listvalueObj<std::string> > string_listptr;

//! A nullable pointer reference to an int16_t %option values
typedef ptr<listvalueObj<int16_t> > int16_listptr;

//! A nullable pointer reference to an int32_t %option values
typedef ptr<listvalueObj<int32_t>  > int32_listptr;

//! A nullable pointer reference to an int64_t %option values
typedef ptr<listvalueObj<int64_t> > int64_listptr;

//! A nullable pointer reference to an uint16_t %option values
typedef ptr<listvalueObj<uint16_t> > uint16_listptr;

//! A nullable pointer reference to an uint32_t %option values
typedef ptr<listvalueObj<uint32_t> > uint32_listptr;

//! A nullable pointer reference to an uint64_t %option values
typedef ptr<listvalueObj<uint64_t> > uint64_listptr;

//! A nullable pointer reference to an hms %option value
typedef ptr<listvalueObj<hms> > hms_listptr;

//! A nullable pointer reference to an ymd %option value
typedef ptr<listvalueObj<ymd> > ymd_listptr;

//! A nullable pointer reference to an ymd::interval %option value
typedef ptr<listvalueObj<ymd::interval> > ymd_interval_listptr;

//! A nullable pointer reference to an uriimpl %option value
typedef ptr<listvalueObj<uriimpl> > uri_listptr;

#if 0
{
#endif
}
#endif
