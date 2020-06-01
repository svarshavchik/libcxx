/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_definitionfunctionobj_H
#define x_option_definitionfunctionobj_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/option_listfwd.H>
#include <x/option_definitionbaseobj.H>
#include <x/option_valuebaseobj.H>
#include <x/option_valueobj.H>
#include <x/value_string.H>
#include <string>

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
};
};
#endif

//! Base class for an option that invokes some function, when specified.

template<typename func_type, typename arg_type>
class definitionfuncbaseObj : public definitionbaseObj {

public:
	//! Define an option that's processed by a function
	definitionfuncbaseObj(//! Option short name
			      char32_t shortnameArg,

			      //! Option long name
			      const std::string &longnameArg,

			      //! \c Flags

			      //! OR-ed values: \c option::list::base::hasvalue,
			      //! \c option::list::required.
			      int flagsArg,

			      //! Description of this option
			      const std::string &descriptionArg,

			      //! Description of this option's argument
			      const std::string &argDescriptionArg,

			      //! The function to call
			      func_type funcArg)
		: definitionbaseObj(shortnameArg,
				    longnameArg,
				    flagsArg,
				    descriptionArg,
				    argDescriptionArg),
		  was_set(false),
		  optfunc(funcArg)
	{
	}

	//! Default destructor
	~definitionfuncbaseObj()=default;

private:
	//! Implement the set() method.

	//! Invokes the function.
	//!
	int set(//! The parser object that's setting this option
		 parserObj &parserArg) const override
	{
		if (flags & option::list::base::hasvalue)
			return false;

		int rc=set_default();

		if (rc == 0)
			was_set=true;

		return rc;
	}

	//! Implement the set() method.

	//! Invokes the function.
	//!

	int set(//! The parser object that's setting this option
		 parserObj &parserArg,

		 //! The value for this option
		 const std::string &valueArg) const override
	{
		int rc=option::parser::base::err_invalidoption;

		try {
			rc=set_explicit(value_string<arg_type>
					::from_string(valueArg,
						     locale::base::utf8()));
		} catch (exception &e)
		{
		}

		if (rc == 0)
			was_set=true;
		return rc;
	}

	//! Check if this option was specified

	bool isSet() const noexcept override
	{
		return was_set;
	}

	//! Flag - this %option was set
	mutable bool was_set;

	//! Implement the reset function

	void reset() noexcept override
	{
		was_set=false;
	}

protected:
	//! Handler function
	func_type optfunc;

	//! Set the default value
	virtual int set_default() const=0;

	//! Set an explicit value
	virtual int set_explicit(const arg_type &arg) const=0;
};

//! Specialization for a function that takes a bool arg.

template<>
int definitionfuncbaseObj<int (*)(bool), bool>::set(parserObj &obj)
	const;

//! Specialization for a function that takes a bool arg.

template<>
int definitionfuncbaseObj<int (*)(const bool &), bool>::set(parserObj &obj)
	const;

//! Define an %option that invokes a function, when specified

//! Instantiate this class to define an %option that invokes a function,
//! when it is specified.

template<typename value_type>
class definitionfuncvalueObj
	: public definitionfuncbaseObj<int (*)(value_type), value_type> {

public:
	//! Define an option that's processed by a function
	definitionfuncvalueObj(//! Option short name
			      char32_t shortnameArg,

			      //! Option long name
			      const std::string &longnameArg,

			      //! \c Flags

			      //! OR-ed values: \c option::list::base::hasvalue,
			      //! \c option::list::required.
			      int flagsArg,

			      //! Description of this option
			      const std::string &descriptionArg,

			      //! Description of this option's argument
			      const std::string &argDescriptionArg,

			      //! Function
			      int (*optfuncvalArg)(value_type)) noexcept
		: definitionfuncbaseObj<int (*)(value_type),
					value_type > (shortnameArg,
						      longnameArg,
						      flagsArg,
						      descriptionArg,
						      argDescriptionArg,
						      optfuncvalArg)
	{
	}

	//! Default destructor
	~definitionfuncvalueObj()
	{
	}

private:
	//! Set the default value
	virtual int set_default() const override
	{
		return option::parser::base::err_invalidoption;
	}

	//! Set an explicit value
	virtual int set_explicit(const value_type &arg) const override
	{
		return (*this->optfunc)(arg);
	}
};

//! Define an %option that invokes a function, when specified

//! Instantiate this class to define an %option that invokes a function,
//! when it is specified.

template<typename value_type>
class definitionfunccrvalueObj
	: public definitionfuncbaseObj<int (*)(const value_type &),
				       value_type > {

public:
	//! Define an option that's processed by a function
	definitionfunccrvalueObj(//! Option short name
				 char32_t shortnameArg,

				 //! Option long name
				 const std::string &longnameArg,

				 //! \c Flags

				 //! OR-ed values: \c option::list::base::hasvalue,
				 //! \c option::list::required.
				 int flagsArg,

				 //! Description of this option
				 const std::string &descriptionArg,

				 //! Description of this option's argument
				 const std::string &argDescriptionArg,

				 //! Function
				 int (*optfuncvalArg)(const value_type &))
		noexcept
		: definitionfuncbaseObj<int (*)(const value_type &),
					value_type > (shortnameArg,
						      longnameArg,
						      flagsArg,
						      descriptionArg,
						      argDescriptionArg,
						      optfuncvalArg)
	{
	}

	//! Default destructor
	~definitionfunccrvalueObj()
	{
	}

private:
	//! Set the default value
	virtual int set_default() const
	{
		return option::parser::base::err_invalidoption;
	}

	//! Set an explicit value
	virtual int set_explicit(const value_type &arg) const
	{
		return (*this->optfunc)(arg);
	}
};

//! An option for a pointer to a function that does not take an argument.

class definitionfuncvoidObj
	: public definitionfuncbaseObj<int (*)(void),bool > {

public:
	//! Define an option that's processed by a function
	definitionfuncvoidObj(//! Option short name
			      char32_t shortnameArg,

			      //! Option long name
			      const std::string &longnameArg,

			      //! \c Flags

			      //! OR-ed values: \c option::list::base::hasvalue,
			      //! \c option::list::required.
			      int flagsArg,

			      //! Description of this option
			      const std::string &descriptionArg,

			      //! Description of this option's argument
			      const std::string &argDescriptionArg,

			      //! Function
			      int (*optfuncvalArg)(void)
			      ) noexcept;

	//! Default destructor
	~definitionfuncvoidObj();

private:
	//! Set the default value
	int set_default() const override;

	//! Set an explicit value
	int set_explicit(const bool &arg) const override;
};

#if 0
{
	{
#endif
	}
}
#endif
