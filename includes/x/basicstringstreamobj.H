/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basicstringstreamobj_H
#define x_basicstringstreamobj_H

#include <x/obj.H>
#include <x/ptrfwd.H>

#include <iostream>
#include <sstream>
#include <string>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<class streamType> class basic_stringstreamObj;


//! A reference-counted string stream

//! This template defines a reference-counted string stream.
//! The template parameter is a string stream object defined in the standard C++
//! library: a \c std::basic_istringstream, \c std::basic_ostringstream,
//! or a \c std::basic_stringstream type.
//!
//! The template defines a subclass of the string stream object and obj,
//! implementing a reference-counted stream.
//!
//! Two constructors are provided, one that takes a string parameter that
//! sets the initial contents of the string, and one that does not.
//!
//! \see istringstream, ostringstream, stringstream

template<class streamType>
class basic_stringstreamObj : public streamType, virtual public obj {

public:

	//! Constructor -- a string stream with initial contents

	basic_stringstreamObj(//! The initial string
			      const
			      std::basic_string<typename streamType::char_type,
			      typename streamType::traits_type> &str
			      ) noexcept
		: streamType(str)
	{
	}

	//! Default constructor

	basic_stringstreamObj() noexcept=default;

	//! Default destructor

	~basic_stringstreamObj()=default;
};
//! A reference-counted \c std::istringstream

//! \code
//! istringstream is(istringstream::create("Hello world"));
//! \endcode

typedef ref<basic_stringstreamObj< std::istringstream > > istringstream;

//! A reference-counted \c std::ostringstream

//! \code
//! ostringstream os(ostringstream::create());
//! \endcode

typedef ref<basic_stringstreamObj< std::ostringstream > > ostringstream;

//! A reference-counted \c std::stringstream

//! \code
//! stringstream ss(stringstream::create());
//! \endcode

typedef ref<basic_stringstreamObj< std::stringstream > > stringstream;

//! A pointer to a reference-counted \c std::istringstream

//! \see istringstream

typedef ptr<basic_stringstreamObj< std::istringstream > > istringstreamptr;

//! A pointer to a reference-counted \c std::ostringstream

//! \see ostringstream

typedef ptr<basic_stringstreamObj< std::ostringstream > > ostringstreamptr;

//! A pointer to a reference-counted \c std::stringstream

//! \see stringstream

typedef ptr<basic_stringstreamObj< std::stringstream > > stringstreamptr;

#if 0
{
#endif
}
#endif
