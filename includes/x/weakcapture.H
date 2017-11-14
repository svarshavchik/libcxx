/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakcapture_H
#define x_weakcapture_H

#include <x/weakptr.H>
#include <x/refptr_traits.H>
#include <x/tuple_param_pack.H>
#include <tuple>
#include <optional>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename> struct weak_capture_unpack;

//! Helper class used by \ref weak_capture "weak_capture".

//! \internal
template<size_t ...n>
struct weak_capture_unpack<tuple_param_pack<n...>> {

	//! Use weakptr::getptr() to attempt to recover all weak references
	template<typename ...tuple_t>
	static auto get(const std::tuple<tuple_t...> &t)
	{
		return std::tuple{ std::get<n>(t).getptr()... };
	}

	//! Check if all weak references have been recovered

	template<typename ...tuple_t>
	static bool got(const std::tuple<tuple_t...> &t)
	{
		return (std::get<n>(t) && ...);
	}
};

//! Internal object used by make_weak_capture()

//! \internal

template<typename ...ref_tuple_t>
class weak_capture : std::tuple<weakptr<ptr_cast<ref_tuple_t>>...> {

	//! Convenient alias.

	typedef std::tuple<weakptr<ptr_cast<ref_tuple_t>>...> superclass_t;

	//! A corresponding tuple of ptrs
	typedef std::tuple<ptr_cast<ref_tuple_t>...> ptr_tuple_t;

	//! Figure out our helper class.
	typedef weak_capture_unpack<typename tuple_2_param_pack<
					    sizeof...(ref_tuple_t)>::type
				    > unpack_t;

public:

	using superclass_t::superclass_t;

	//! Recover weakly-captured pointers.

	auto get() const
	{
		auto ptrs=unpack_t::get(*this);

		std::optional<std::tuple<ref_tuple_t...>> got;

		if (unpack_t::got(ptrs))
			got=ptrs;

		return got;
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
//! auto callback=INSERT_LIBX_NAMESPACE::functionref<void()>::create
//!  ( [ captured_refs=INSERT_LIBX_NAMESPACE::make_weak_capture(a, b) ]
//!    {
//!       auto got=captured_refs.get();
//!
//!       if (got)
//!       {
//!           auto& [a, b] = *got;
//!
//!                ...
//!       }
//!    });
//! \endcode
//!
//! At the time the \ref functionref "callback" object gets created, the
//! two strong refs get weakly captured into \ref weakptr "weak pointers".
//!
//! make_weak_capture() returns an object that implements a method called get().
//! The get() method returns a std::optional containing a std::tuple of
//! original references, recovered via getptr(). The std::tuple contains only
//! \ref ref "INSERT_LIBX_NAMESPACE::ref"s.
//!
//! If any of the weakly-captured object already went out of scope and got
//! destroyed, get() returns an empty std::optional.

template<typename ...Args>
inline auto make_weak_capture(Args && ...args)
{
	return weak_capture<ref_cast<std::decay_t<Args>>...>{
		std::forward<Args>(args)...
			};
}

#if 0
{
#endif

}
#endif
