/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_runfwd_H
#define x_runfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <x/singletonfwd.H>
#include <x/threads/weaklocalfwd.H>

#include <list>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class runthreadbaseObj;
class logger;

template<typename ret_type> class run_asyncthreadObj;

//! Helper class for construing parameters for a called to object->run().

//! \internal

template<typename param_type> class run_param {

public:

	//! The parameter to an object's run() method that gets passed for an INSERT_LIBX_NAMESPACE::run() parameter of the template class type.

	typedef typename std::decay<param_type>::type &type;
};

//! Helper function for construing parameters for a called to object->run().

//! \internal

template<typename param_type>
typename run_param<param_type>::type &invoke_run_param_helper();

template<typename args_type> class run_method_impl;
template<typename refobj_type, typename ret_type, typename ...Args>
class run_asyncthreadparamObj;

//! Invoke a thread object's run() method in another thread

//! This is a separate namespace for all templates that
//! implement run().

class run_async {

	//! Alias

	//! See https://bugzilla.redhat.com/show_bug.cgi?id=1632063

	template<typename args_type> class run_method_impl;
	template<typename refobj_type, typename ret_type, typename ...Args>
	using runthreadparamObj=run_asyncthreadparamObj<refobj_type, ret_type,
							Args...>;

	//! Thrown an exception, things are not fully set up.

	//! \internal
	static void no_local_variables() __attribute__((noreturn));

	//! Current thread name
	static __thread const char *thread_name;

	//! Default thread name
	static const char default_name[] LIBCXX_HIDDEN;

public:

	class localscope;

	template<typename ref_type, typename baseClass>
	class singleton;

	friend class logger;
	class singletonbase;

	//! Put an object on the local thread's stack, and return a mcguffin reference to it.

	//! \internal

	template<typename objClass, typename baseType>
	static weakthreadlocal<objClass, baseType>
	makeweakthreadlocal(const ref<objClass, baseType> &refobj);

	class statics;

	//! Local thread name for the duration of the thread

	class name {

		//! The thread name

		std::string n;

	public:
		//! Constructor
		name(const std::string &nArg);
		//! Destructor
		~name();
	};

	//! Alias

	//! See https://bugzilla.redhat.com/show_bug.cgi?id=1632063

	template<typename ret_type>
	using runthreadObj=run_asyncthreadObj<ret_type>;

	//! run() forwards here, in order to access private members of this class.

	template<typename objClass, typename baseType,
		 typename ...Args> static inline auto
	run_thread(//! The thread object with a run() method.
	    const ref<objClass, baseType> &p,

	    //! run() method's parameters.
	    Args && ...args);
};

//! A reference to a base class of a running thread handle, that implements wait() and terminated()

typedef ref<runthreadbaseObj> runthreadbase;

//! A nullable reference pointer to a base class of a running thread handle, that implements wait() and terminated()
typedef ptr<runthreadbaseObj> runthreadbaseptr;

//! A reference to a constant base class of a running thread handle, that implements terminated()

typedef const_ref<runthreadbaseObj> const_runthreadbase;

//! A nullable reference pointer to a constant base class of a running thread handle, that implements terminated()

typedef const_ptr<runthreadbaseObj> const_runthreadbaseptr;


#if 0
{
#endif
}
#endif
