/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

/*! \file

This file defines the SYSEXCEPTION macro.

*/

#ifndef x_sysexception_H
#define x_sysexception_H

#include <x/namespace.h>
#include <x/exception.H>
#include <errno.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for sysexception that's constructed first and captures errno

struct sysexception_base {

	//! The captured value of errno.

	int errcode;

	sysexception_base() LIBCXX_HIDDEN;
};

//! A system exception.

//! This is a subclass of
//! \ref exception "INSERT_LIBX_NAMESPACE::exception", that's thrown as a
//! result of a failed
//! system call. The error message includes \c strerror(errno).
//!
//! This object is normally not constructed directly, but indirectly
//! via SYSEXCEPTION.

class sysexception : public sysexception_base, public exception {

public:
	using exception::operator *;
	using exception::operator ->;

	//! Default constructor
	sysexception();

	//! Construct a system exception.
	sysexception(//! Exception thrown from this source file
		     const char *file,

		     //! Exception thrown from this line
		     int line);

	//! The default destructor.
	~sysexception() noexcept;

	//! Return the error code -- the value of errno.
	int getErrorCode() const noexcept
	{
		return errcode;
	}

	//! A new exception under construction, pretending to be a std::ostream.

	template<typename Arg_T>
	sysexception &operator<<(const Arg_T &a)
	{
		(std::ostringstream &)**this << a;

		return *this;
	}

	//! The exception message construction is done

	void done();
};

//! Specialization for a magic value at the end of the streamed exception message.

template<>
inline sysexception &sysexception::operator<<(const exceptionObj::libx_exception_done_t &dummy)
{
	done();
	return *this;
}

//! Throw an exception after a failed system call.

//! This macro is used just like \link #EXCEPTION \endlink, except that it
//! includes the error message from \c errno instead of the file+line.
//! \see #EXCEPTION

#define SYSEXCEPTION(libx_exception_msg)				\
	(::LIBCXX_NAMESPACE::sysexception(__FILE__, __LINE__)		\
	 << libx_exception_msg <<					\
	 ::LIBCXX_NAMESPACE::exceptionObj::libx_exception_done )

#if 0
{
#endif
}
#endif
