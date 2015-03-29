/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_runthread_invoke_H
#define x_runthread_invoke_H

#include <x/namespace.h>
#include <x/tuple_param_pack.H>
#include <future>
#include <tuple>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Helper class that invokes the run() method of an object in a new thread.

//! \internal
//!
//! The first template parameter is the return value from run(), given to us
//! to save the trouble of messy decltypes. The second template parameter must
//! be \c INSERT_LIBX_NAMESPACE::tuple_2_param_pack<sizeof...(tupl_type)>.
//!
//! run_invoke_helper::run invokes the run method of the object, with the
//! arguments coming from a tuple passed as a second argument, that get
//! converted to a parameter pack.

template<typename ret_type, typename seq_num> class run_invoke_helper;

//! Helper class that invokes the run() method of the thread object.

//! \internal
//! This specialization defines run() which unpacks the tuple, and invokes the
//! thread object's run() method with the parameters from the tuple.

template<typename ret_type, size_t ...n>
class run_invoke_helper<ret_type, tuple_param_pack<n...> > {

public:

	//! Take the reference to an object, and a tuple, then invoke the object's run() method, with the tuple supplying the parameters.

	template<typename refobj_t, typename tuple_type>
	static inline ret_type
	run(//! This is a \ref ref "reference" to an object with a run() method.
	    const refobj_t &refobj,

	    //! The tuple with the arguments for the object's run().
	    tuple_type &tuple)
	{
		return refobj->run(std::forward<decltype(std::get<n>(tuple))>(std::get<n>(tuple))...);
	}
};

//! A further specialization for a void-returning run()

template<size_t ...n>
class run_invoke_helper<void, tuple_param_pack<n...> > {

public:

	//! Take the reference to an object, and a tuple, then invoke the object's run() method, with the tuple supplying the parameters.

	template<typename refobj_t, typename tuple_type>
	static inline void
	run(//! This is a \ref ref "reference" to an object with a run() method
	    const refobj_t &refobj,

	    //! The tuple with the arguments for the object's run().
	    tuple_type &tuple)
	{
		refobj->run(std::forward<decltype(std::get<n>(tuple))>(std::get<n>(tuple))...);
	}
};

//! Helper class for invoking run() with its argument given in a tuple.

//! The template parameter is the return value from some object's run() method.
//! The template class defines a static invoke() method whose first parameter
//! is a reference to an object, whose run() method gets invoked with the
//! arguments coming from a tuple that's passed as a second parameter to
//! invoke().

template<typename ret_type> class run_invoke {

public:

	//! Invoke an object's run() method, with parameters.

	//! Returns the value returned by run().

	template<typename refobj_type,
		 typename ...Args>
	static inline ret_type invoke(//! This is a \ref ref "reference" to an object with a run() method
				      const refobj_type &refobj,
				      //! The tuple with the arguments for the object's run().
				      std::tuple<Args...> &args)
	{
		typedef typename tuple_2_param_pack<sizeof...(Args)>::type
			param_nums;

		return run_invoke_helper<ret_type, param_nums>::run(refobj,
								    args);
	}
};

//! A specialization for a run() method that returns void.

template<>
class run_invoke<void> {

public:

	//! Invoke an object's run() method, with parameters.

	//! Returns the value returned by run().

	template<typename refobj_type,
		 typename ...Args>
	static inline void invoke(//! This is a \ref ref "reference" to an object with a run() method
				  const refobj_type &refobj,
				  //! The tuple with the arguments for the object's run().
				  std::tuple<Args...> &args)
	{
		typedef typename tuple_2_param_pack<sizeof...(Args)>::type
			param_nums;

		run_invoke_helper<void, param_nums>::run(refobj, args);
	}
};

//! Invoke an object's run() method, with some parameters

//! The class template parameter is the return value from the object's run()
//! method. This template class defines a static method invoke() whose first
//! parameter is a promise object, the second parameter is a reference to
//! to an object, and the third argument is a tuple containing the parameters
//! to the run() method.

template<typename ret_type> class run_invoke_set_promise {

public:

	//! Invoke an object's run() method, saving its result in a promise object.

	template<typename refobj_type,
		 typename ...Args>
	static inline void invoke(//! run()'s return value is placed here
				  std::promise<ret_type> &promise,

				  //! This is a \ref ref "reference" to an object with a run() method
				  const refobj_type &refobj,
				  //! The tuple with the arguments for the object's run().
				  std::tuple<Args...> &args)
	{
		promise.set_value(run_invoke<ret_type>::invoke(refobj, args));
	}
};

//! Specialization for invoking a run() that returns a void.

template<>
class run_invoke_set_promise<void> {
public:

	//! A specialized invoke() for a void-returning run()

	//! Invoke run(), then invoke promise.set_value().
	//!
	template<typename refobj_type, typename ...Args>
	static inline void invoke(//! run()'s return value is placed here
				  std::promise<void> &promise,
				  //! This is a \ref ref "reference" to an object with a run() method
				  const refobj_type &refobj,
				  //! The tuple with the arguments for the object's run().
				  std::tuple<Args...> &args)
	{
		run_invoke<void>::invoke(refobj, args);
		promise.set_value();
	}
};

#if 0
{
#endif
}
#endif
