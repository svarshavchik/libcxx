/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_valueopobj_H
#define x_option_valueopobj_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/option_valueobj.H>

#include <sstream>
#include <list>

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
};
};
#endif

//! Implementation class for a bitwise \c AND %option

//! \internal
//! Defines the op() function, used by the \c opvalueObj subclass.

template<typename value_type> class andOpImpl {

public:
	//! Implement the \c AND operation on the value

	static void op(//! Option's value
		       value_type &v,

		       //! Value to \c AND with the option's value
		       value_type opval) noexcept
	{
		v &= opval;
	}
};

//! Implementation class for a bitwise \c OR %option

//! \internal
//! Defines the op() function, used by the \c opvalueObj subclass.

template<typename value_type> class orOpImpl {

public:
	//! Implement the \c OR operation on the value
	static void op(//! Option's value
		       value_type &v,

		       //! Value to \c OR  with the option's value
		       value_type opval) noexcept
	{
		v |= opval;
	}
};

//! Implementation class for a bitwise \c XOR %option

//! \internal
//! Defines the op() function, used by the \c opvalueObj subclass.

template<typename value_type> class xorOpImpl {

public:
	//! Implement the \c XOR operation on the value

	static void op(//! Option's value
		       value_type &v,
		       //! Value to \c XOR with the option's value
		       value_type opval) noexcept
	{
		v ^= opval;
	}
};

//! Base class for an operation %option

//! \internal
//! This is a reference-counted object.
//! An operation %option does not itself store the value, but maintains a
//! a reference to some other, defined value. When the %option is specified,
//! an operation is performed on the other value. The defined operations are
//! a bitwise \c AND, \c OR, or an \c XOR %option with some predefined constant.

template<typename value_type, typename opType>
class opvalueObj : virtual public obj, public valuebaseObj, public opType {

public:
	//! The type of this option.
	typedef value_type value_t;

	//! The type of the reference to the value of this %option.

	typedef ref<valueObj<value_type> > refvalue_t;

private:
	//! The other %option value.
	refvalue_t valueref;
public:

	//! The constant value for the operation.
	value_t opvalue;

	//! Default constructor
	explicit opvalueObj(//! The value where this operation option's value is stored
			    const refvalue_t &valueArg,

			    //! The operation's argument value
			    const value_t &opvalueArg
			    )
		: valueref(valueArg), opvalue(opvalueArg) {}

	//! Default destructor
	~opvalueObj() {}

	//! Set the option's value

	int set(//! The option's value, gets converted to value_type
		const std::string &valueArg,

		//! The locale to parse the %value
		const const_locale &locale) noexcept override
	{
		try {
			this->op(valueref->value, value_string<value_type>
				 ::from_string(valueArg, locale));
		} catch (exception &e)
		{
			return option::parser::base::err_invalidoption;
		}

		return 0;
	}

	//! Set the %option using the default operation value

	int set() noexcept override
	{
		this->op(valueref->value, opvalue);
		return 0;
	}

	//! Implement the reset operation

	void reset() noexcept override
	{
		valueref->reset();
	}

	//! This is a single %option value.

	bool multiple() const noexcept override
	{
		return false;
	}
};

//! An \c AND operation reference.

//! This is a dummy class that contains a \c typedef
//! that defines a reference to the
//! %option operation object.
template<typename refvalue_t=int_value>
class and_op {

public:
	//! The typedef for a reference to the operation object.

	typedef ref<opvalueObj< refvalue_t, andOpImpl<refvalue_t> > > value;

	//! The typedef for a nullable pointer to the operation object.

	typedef ptr<opvalueObj< refvalue_t, andOpImpl<refvalue_t> > > valueptr;
};

//! An \c OR operation reference.

//! This is a dummy class that contains a \c typedef
//! that defines a reference to the
//! %option operation object.
template<typename refvalue_t=int_value>
class or_op {

public:
	//! The typedef for a reference to the operation object.

	typedef ref<opvalueObj< refvalue_t, orOpImpl<refvalue_t> > > value;

	//! The typedef for a nullable pointer to the operation object.

	typedef ptr<opvalueObj< refvalue_t, orOpImpl<refvalue_t> > > valueptr;
};

//! An \c XOR operation reference.

//! This is a dummy class that contains a \c typedef
//! that defines a reference to the
//! %option operation object.
template<typename refvalue_t=int_value>
class xor_op {
	//! The typedef for a reference to an operation object.

	typedef ref<opvalueObj< refvalue_t, xorOpImpl<refvalue_t> > > value;

	//! The typedef for a nullable pointer to the operation object.
	typedef ptr<opvalueObj< refvalue_t, xorOpImpl<refvalue_t> > > valueptr;
};

#if 0
{
	{
#endif
	}
}
#endif
