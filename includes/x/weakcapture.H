/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakcapture_H
#define x_weakcapture_H

#include <x/weakptr.H>
#include <x/tuple_param_pack.H>
#include <tuple>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Template for converting a ref to a \ref weakptr "weak pointer".

template<typename ref_type> class ref_to_weak;

//! Specialization that defines ref_to_weak<ref<ptr,base>>::%type

template<typename obj_class, typename base_class>
class ref_to_weak<ref<obj_class, base_class>> {

 public:

	//! Define the weak pointer type.

	typedef weakptr<ptr<obj_class, base_class>> type;
};


//! Template for converting a \ref weakptr "weak pointer" to a strong pointer.

//! Used by recover_weak_capture().

template<typename ref_type> class weak_to_ptr;

//! Specialization that defines weak_to_ptr<weakptr<ptr, base>>::%type

template<typename obj_class, typename base_class>
class weak_to_ptr<weakptr<ptr<obj_class, base_class>>> {

 public:

	//! Define the strong pointer type.

	typedef ptr<obj_class, base_class> type;
};


//! Check that all weak pointers were recovered to strong pointers.

//! Used by get_weak_capture().

inline bool recaptured_weak_pointers()
{
	return true;
}

//! Check that all weak pointers were recovered to strong pointers.

//! Used by get_weak_capture().

template<typename ptr_type, typename ...Args>
inline bool recaptured_weak_pointers(const ptr_type &p,
				     Args && ...args)
{
	return recaptured_weak_pointers(std::forward<Args>(args)...)
		&& !p.null();
}

//! Convert a tuple of weak pointers to a param pack of getptr().

//! Used by get_weak_capture().

template<typename opaque> class get_captured_weak_pointer;

//! Convert a tuple of weak pointers to a param pack of getptr().

//! Used by \ref weak_capture "INSERT_LIBX_NAMESPACE::weak_capture".
//!
//! This specialization gets invoked with a tuple of weak pointers,
//! and uses \ref tuple_2_param_pack "tuple_2_param_pack" to grab the
//! weak pointers from the tuple, convert them to a string pointers,
//! and forward them to a lambda.
//!
//! \internal

template<size_t ...n>
class get_captured_weak_pointer<x::tuple_param_pack<n...>> {

 public:

	//! First, take the weak ptr tuple, and return a strong ptr tuple.

	//! This converts all weak ptrs to strong ptrs.

	template<typename tuple_type>
		static inline auto get_weak_pointers(const tuple_type &tuple)
	{
		return std::make_tuple(std::get<n>(tuple).getptr()...);
	}

	//! Check that all weak pointers were recovered to strong ptrs.

	template<typename tuple_type>
		static inline bool got_all_weak_pointers(const tuple_type &tuple)
	{
		return recaptured_weak_pointers(std::get<n>(tuple)...);
	}

	//! And now forward everything to a lambda.

	template<typename tuple_type, typename functor_type>
		static inline void call_functor(const tuple_type &tuple,
						functor_type &&functor)
	{
		functor(std::get<n>(tuple)...);
	}
};


//! Internal object used by make_weak_capture()

//! \internal

template<typename ...Args>
class weak_capture {

	//! Weakly-captured references.

	std::tuple<Args...> weak_refs;
public:

	//! Constructor.
	template<typename ...constructorArgs>
	inline weak_capture(constructorArgs && ...args)
		: weak_refs(std::forward<constructorArgs>(args)...)
	{
	}


	//! Recover previously captured refs.

	//! Invokes the lamba with recaptured weak pointers.
	//!
	//! A strong pointer is recovered from each weak pointer in the tuple.
	//! If all strong pointers were recovered, the passed lambda gets
	//! invoked, with all strongly-recovered pointers passed as its
	//! arguments, and get() returns \c true.
	//!
	//! If at least one weak pointer could not be recovered, the referenced
	//! object went out of scope and got destroyed, the lambda does not
	//! get invoked, and get() returns \c false.

	template<typename functor_type>
	inline bool get(functor_type &&functor) const
	{
		auto pointers=
			get_captured_weak_pointer<typename
						  tuple_2_param_pack
						  <sizeof...(Args)>
						  ::type>
			::template get_weak_pointers(weak_refs);

		if (!get_captured_weak_pointer<typename
		    tuple_2_param_pack<sizeof...(Args)>
		    ::type>
		    ::template got_all_weak_pointers(pointers))
			return false;

		get_captured_weak_pointer<typename tuple_2_param_pack
					  <sizeof...(Args)>
					  ::type>
			::template call_functor(pointers,
						std::forward<functor_type>
						(functor));
		return true;
	}
};

//! Take a list of strong references, and return a tuple of their weak pointers.

//! make_weak_capture() takes a variadic list of \ref ref "refs". Only
//! refs can be passed to make_weak_capture(). make_weak_capture() returns
//! essentially a tuple of weak pointers to the referenced objects.
//!
//! The class returned by make_weak_capture implements a get() method.
//!
//! At some later point in time, all of the objects can be recovered
//! by the get() method.
//!
//! This a design pattern that's often used with callbacks:
//!
//! \code
//! class aObj;
//! class bObj;
//!
//! INSERT_LIBX_NAMESPACE::ref<aObj> a;
//! INSERT_LIBX_NAMESPACE::ref<bObj> b;
//!
//! auto callback=INSERT_LIBX_NAMESPACE::callback<void>::create
//!  ( [ captured_refs=INSERT_LIBX_NAMESPACE::make_weak_capture(a, b) ]
//!    {
//!       captured_refs.get([]
//!                         (const INSERT_LIBX_NAMESPACE::ref<aObj> &a,
//!                          const INSERT_LIBX_NAMESPACE::ref<bObj> &b)
//!                         {
//!                               // ....
//!                         });
//!    });
//! \endcode
//!
//! At the time the \ref callback "callback" object gets created, the
//! two strong refs are weakly captured. The callback object does not keep
//! a strong reference to the captured objects.
//!
//! When the callback gets invoked, the get() method recovers the strong
//! references to the objects, if they still exist, and forwards them to the
//! inner lambda, then returns \c true.
//!
//! If any captured object already went out of scope and got destroyed,
//! get() returns \c false without invoking the inner lambda.

template<typename ...Args>
inline auto make_weak_capture(Args && ...args)
{
	return weak_capture<typename
			    ref_to_weak<typename
					std::decay<Args>::type>::type...>
		(std::forward<Args>(args)...);
}

#if 0
{
#endif

}
#endif
