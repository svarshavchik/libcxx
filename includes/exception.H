/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

/*! \file

This file defines the EXCEPTION macro.

*/

#ifndef x_exception_H
#define x_exception_H

#include <x/namespace.h>
#include <x/ref.H>
#include <x/exceptionobj.H>
#include <x/exceptionfwd.H>
#include <string>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A thrown exception

//! Errors encountered by this library throw these \c exception objects.
//! Typical usage:
//! \code
//!
//! try {
//!
//!    // Some operations
//!
//! } catch (const INSERT_LIBX_NAMESPACE::exception &e)
//! {
//!    std::cerr << e << std::endl << e->backtrace() << std::endl;
//! }
//! \endcode
//!
//! You may throw these exceptions yourself, but don't construct these objects
//! directly, use the \link #EXCEPTION \endlink or the
//! \link #SYSEXCEPTION \endlink macros.

class exception : public ref<exceptionObj>, public std::exception {

public:

	//! Default constructor

	//! Constructs an exception with empty text. Often used to
	//! capture the current stack trace.

	exception();

	using ref<exceptionObj>::operator*;
	using ref<exceptionObj>::operator->;

	//! Construct a generic exception

	exception(//! Exception thrown from this source file
		  const char *file,

		  //! Exception thrown from this line
		  int line);

	//! The default destructor
	~exception() noexcept;

	//! Internal constructor

	exception(exceptionObj *obj) noexcept;

	//! Internal constructor
	exception(const ref<exceptionObj> &refp) noexcept;

	//! Inherited from std::exception

	const char *what() const noexcept;

	//! A new exception under construction, pretending to be a std::ostream.

	template<typename Arg_T>
	exception &operator<<(const Arg_T &a)
	{
		(std::ostringstream &)**this << a;

		return *this;
	}

	//! The exception message construction is done

	void done();
};

//! Throw a generic exception.

//! This macro may be used to throw a generic \c exception:
//!
//! \code
//! if ( somecondition )
//! {
//!      throw EXCEPTION("Operation failed: " + funcname);
//! }
//! \endcode
//!
//! This macro constructs an
//! \c INSERT_LIBX_NAMESPACE::exception object, and fills it with
//! the requisite information. The macro argument can generally be anything
//! that's valid on the right side of a std::ostream <<.

#define EXCEPTION(libx_exception_msg)					\
	(::LIBCXX_NAMESPACE::exception(__FILE__, __LINE__)		\
	 << libx_exception_msg <<					\
	 ::LIBCXX_NAMESPACE::exceptionObj::libx_exception_done )

//! Specialization for a magic value at the end of the streamed exception message.

template<>
inline exception &exception::operator<<(const exceptionObj::libx_exception_done_t &dummy)
{
	done();
	return *this;
}

//! A helper \c operator<< method.

//! This function allows an exception object to be dumped into an
//! \c std::ostream. See the example code in the description of
//! the exception typedef.

std::ostream &operator<<(std::ostream &o, const exception &e);

#if 0
{
#endif
}
#endif
