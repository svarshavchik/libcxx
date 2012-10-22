/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_exceptionobj_H
#define x_exceptionobj_H

#include <string>
#include <sstream>
#include <x/namespace.h>
#include <x/obj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! An error message that describes a thrown exception.

//! This is a reference-counted object, accessed using the
//! exception reference class. It is not typically
//! accessed directly, but rather constructed by a
//! \ref EXCEPTION "EXCEPTION macro".

class exceptionObj : virtual public obj, public std::ostringstream {

	//! Buffer returned by what().
	std::string what_buf;

public:
	//! This buffer stores the stack backtrace dump.

	//! save() prepends the accumulated error message text to the backtrace.

	std::string backtrace;

	//! Return a native C pointer, called by std::exception::what's override.
	const char *what() noexcept;

	// The default constructor,
	exceptionObj() noexcept;

	//! The default destructor.
	~exceptionObj() noexcept;

	//! Record the file and the line number that threw the exception.

	//! \internal
	//! This should be the first method invoked for a new
	//! exception object.
	//! \return a reference to this object.

	void fileline(//! Name of the file that throws the exception.
		      const char *file,
		      //! The line number that throws the exception.
		      int line) noexcept;

	//! Dummy enumeration used by the exception macros to mark the end of the constructed message.
	enum libx_exception_done_t {
		libx_exception_done,
	};

	//! Prepend a prefix to the error message text.

	void prepend(const std::string &s);

	//! Create what_buf from the current contents of this exception

	void save() noexcept;

	//! Log this exception

	//! Calls save(), then logs the exception.

	void log() noexcept;

	//! Obtain the assembled error message.

	//! \return the collected error
	//! message.
	operator std::string() const noexcept
	{
		return str();
	}

	//! Rethrow this exception

	//! Subclasses can override, and rethrow the correct reference to the
	//! 'true' exception objects.

	virtual void rethrow() __attribute__((noreturn));
};

#if 0
{
#endif
}
#endif

