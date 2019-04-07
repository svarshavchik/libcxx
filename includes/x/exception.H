/*
** Copyright 2012-2019 Double Precision, Inc.
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
	~exception();

	//! Internal constructor

	exception(exceptionObj *obj) noexcept;

	//! Internal constructor
	exception(const ref<exceptionObj> &refp) noexcept;

	//! Inherited from std::exception

	const char *what() const noexcept override;

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

//! Helper class for custom exceptions

//! \see CUSTOM_EXCEPTION

template<typename custom_exception_class>
class custom_exceptionObj : public custom_exception_class,
			    public exceptionObj {

public:

	//! Constructor

	//! The constructor forwards file and line parameters to
	//! exceptionObj::fileline() (after construction), and the remaining
	//! parameters to the custom exception class constructor.
	template<typename ...Args>
	custom_exceptionObj(Args && ...args)
		: custom_exception_class(std::forward<Args>(args)...)
	{
	}

	//! Destructor.

	virtual ~custom_exceptionObj()
	{
	}
};

//! Internal helper class to determine whether the custom exception class has describe()

//! \internal

template<typename T> struct custom_exception_implements_describe
{
	//! Yes return value

	typedef char yes[1];

	//! No return value

	typedef char no[2];

	//! Helper for check()

	static exception &dummy_exception();

	//! The class implements describe()

	template<typename S>
	static yes &check(decltype( ((S *)nullptr)->describe(dummy_exception())) *);

	//! The class does not describe()

	template<typename S>
	static no &check(...);

	//! Determine whether the class implements describe()

	static constexpr bool value = sizeof(check<T>(0)) == sizeof(yes);
};

//! Invoke a custom exception class's describe() method, if it has one.

//! \internal

template<bool implements_describe> class invoke_custom_exception_describe;

//! Specialization: the class does not implement describe()

//! \internal

template<>
struct invoke_custom_exception_describe<false> {

	//! Invoke objname(), as the default.

	template<typename T>
	static void describe(exception &e, T *p)
	{
		e << p->objname();
	}
};

//! Specialization: the class implements describe()

//! \internal

template<>
struct invoke_custom_exception_describe<true> {

	//! Invoke describe()

	template<typename T>
	static void describe(exception &e, T *p)
	{
		p->describe(e);
	}
};

//! Internal helper class to determine whether the custom exception class defines a 'base' typedef.

//! \internal

template<typename T> struct custom_exception_defines_base
{
	//! Yes return value

	typedef char yes[1];

	//! No return value

	typedef char no[2];

	//! The class defines a base typedef

	template<typename S>
	static yes &check(typename S::base *);

	//! The class does not define a base typedef

	template<typename S>
	static no &check(...);

	//! Determine whether the class implements describe()

	static constexpr bool value = sizeof(check<T>(0)) == sizeof(yes);
};

//! Return the custom exception class's \c base typedef, if it exists.

//! \internal

template<bool defines_base_class> class custom_exception_base_class;

//! Specialization: the class does not define a \c base typedef

//! \internal

template<>
struct custom_exception_base_class<false> {

	//! The class does not define a \c base typedef

	template<typename T>
	struct base_class {

		//! Use void instead
		typedef void base_t;
	};

};

//! Specialization: the class defines a \c base typedef

//! \internal

template<>
struct custom_exception_base_class<true> {

	//! The class defines a \c base typedef

	template<typename T>
	struct base_class {

		//! Use it.
		typedef typename T::base base_t;
	};

};

//! Custom subclass of an exception

//! \see CUSTOM_EXCEPTION

template<typename custom_exception_class,
	 typename base_class_t=typename custom_exception_base_class<
		 custom_exception_defines_base<custom_exception_class>::value
		 >::template base_class<custom_exception_class>::base_t>
class custom_exception : public exception {

public:

	//! This is not a real \ref ref "reference", but let's pretend we have a base class

	typedef base_class_t base;

	//! Inherit << from exception superclass

	using exception::operator <<;

	//! Override * operator

	//! We know that the underlying reference must point to a custom class.
	//!

	custom_exception_class &operator*() const
	{
		return dynamic_cast<custom_exception_class &>(exception::operator*());
	}

	//! Override -> operator

	//! We know that the underlying reference must point to a custom class.
	//!

	custom_exception_class *operator->() const
	{
		return &operator*();
	}

	//! Copy constructor

	custom_exception(const custom_exception<custom_exception_class, base_class_t> &c)
		: exception(c)
	{
	}

	//! Move constructor

	custom_exception(custom_exception<custom_exception_class, base_class_t> &&c)
		: exception(std::move(c))
	{
	}

	//! Copy constructor from a non-const reference.

	custom_exception(custom_exception<custom_exception_class, base_class_t> &c)
		: exception(c)
	{
	}

	//! Constructor

	template<typename ...Args>
	custom_exception(Args && ...args)
		: exception(ref<custom_exceptionObj<custom_exception_class>>
			    ::create(std::forward<Args>(args)...))
	{
	}

	//! After the constructor finishes, CUSTOM_EXCEPTION invokes this.

        const custom_exception<custom_exception_class> &log(const char *file,
							    int line)
	{
		exception::operator->()->fileline(file, line);

		invoke_custom_exception_describe<
			custom_exception_implements_describe<
				custom_exception_class>::value>
			::describe(*this,
				   &dynamic_cast< custom_exceptionObj<
				   custom_exception_class> &>(exception::operator*()));

		(*this) << exceptionObj::libx_exception_done;
		return *this;
	}
};

//! Throw a custom exception

//! \code
//! class myExceptionClass {
//!
//! public:
//!    myExceptionClass(int error_code);
//!
//! };
//!
//! // ...
//!
//! throw INSERT_LIBX_NAMESPACE::CUSTOM_EXCEPTION(myException, -100);
//!
//! // ...
//!
//! catch (const INSERT_LIBX_NAMESPACE::custom_exception<myExceptionClass> &e)
//! {
//!     doSomething(e->error_code);
//! }
//! \endcode
#define CUSTOM_EXCEPTION(className, ...) \
	(::LIBCXX_NAMESPACE::custom_exception<className>(__VA_ARGS__).log(__FILE__, __LINE__))

#if 0
{
#endif
}
#endif
