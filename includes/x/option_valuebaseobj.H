/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_valuebaseobj_H
#define x_option_valuebaseobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/ptr.H>
#include <x/locale.H>
#include <x/option_mutexfwd.H>

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
};
};
#endif

//! Command line option base class

//! \internal
//! This is a base class for object that hold option values.

class valuebaseObj : virtual public obj {

public:
	friend class mutexObj;

	//! Constructor
	valuebaseObj() noexcept;

	//! Destructor
	~valuebaseObj();

	//! Set the %value of this %option

	//! \return 0 if the %option was succesfully set, or a non-zero error
	//! code defined in option::parser.

	int pubset(//! The %value to set
		   const std::string &valueArg,

		   //! The locale to parse the %value
		   const const_locale &localeArg) noexcept;

	//! Set the %value of this %option to the default %value

	//! \return 0 if the %option was succesfully set, or a non-zero error
	//! code defined in option::parser.

	int pubset() noexcept;

	//! Reset back to the initial value

	void pubreset() noexcept;

	//! Check if this value was set

	bool isSet() const noexcept
	{
		return specified;
	}

private:
	//! Mutually-exclusive %option flag

	mutexptr value_mutex;

	//! This %option
	bool specified;

	//! Implement setting the value of this option.

	//! \return 0 if the option was succesfully set, or a non-zero error
	//! code defined in option::parser.
	//! Must be implemented in the subclass.
	//! The subclass must set the \c specified member.

	virtual int set(//! The %value to set the option to
			const std::string &valueArg,

			//! The locale to parse the %value
			const const_locale &localeArg) noexcept LIBCXX_HIDDEN =0;

	//! Implement setting the value of this option to the default value

	//! \return 0 if the option was succesfully set, or a non-zero error
	//! code defined in option::parser.
	//! Must be implemented in the subclass.
	//! The subclass must set the \c specified member.

	virtual int set() noexcept LIBCXX_HIDDEN =0;

	//! Implement the reset back to the initial value

	virtual void reset() noexcept LIBCXX_HIDDEN =0;

public:
	//! Return an indication whether this option takes multiple values

	//! \return \c true if this option can take multiple values.
	//! Must be implemented in the subclass.

	virtual bool multiple() const noexcept LIBCXX_HIDDEN =0;
};

#if 0
{
	{
#endif
	}
}
#endif
