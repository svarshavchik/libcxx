/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_runthreadname_H
#define x_runthreadname_H

#include <x/namespace.h>
#include <string>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! The name of a thread started by run()

//! Subclass from runthreadname, and implement getName(), to use a
//! custom thread name in logger messages.

class runthreadname {
public:

	//! getName() gets invoked just before the object's run() method.

	virtual std::string getName() const=0;

	//! Construct a default thread name of the form "thread(#####)"

	static std::string get_default_name();

	//! Retrieve a subclass's thread name.

	//! The default implementation calls get_default_name(). A
	//! specialization invokes getName().

	template<typename T, bool> class get_impl {
	public:

		//! Default thread name.

		static inline std::string get(const T &obj)
		{
			return get_default_name();
		}
	};

	//! Select the default implementation, or the specialization for runthreadname subclasses

	template<typename T> class name;
};

//! Specialization for subclasses of runthreadname

template<typename T> class runthreadname::get_impl<T, true> {

public:

	//! Use getName() to obtain the name.

	static inline std::string get(const runthreadname &t)
	{
		return t.getName();
	}
};

//! Helper template to return the name of a running thread object.

template<typename T>
class runthreadname::name
	: public get_impl<T, std::is_base_of<runthreadname, T>::value
			  != false>
{
};

#if 0
{
#endif
}
#endif
