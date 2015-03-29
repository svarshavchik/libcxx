/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinstashfwd_H
#define x_mcguffinstashfwd_H

#include <x/ptrfwd.H>
#include <x/obj.H>
#include <string>
#include <map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename key_type=std::string,
	 bool multimap=false,
	 typename cmp_type=std::less<key_type>,
	 class Allocator = std::allocator<std::pair<const key_type, ref<obj>>
					  >> class mcguffinstashObj;

template<typename key_type=std::string,
	 bool multimap=false,
	 typename cmp_type=std::less<key_type>,
	 class Allocator = std::allocator<std::pair<const key_type, ref<obj>>
					  >> class mcguffinstashBase;

//! A convenient place to keep one's mcguffins.

//! This is a facade for a \ref ref "reference-counted object" that
//! contains an internal std::map of
//! \c INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj>>
//! with a few simple thread-safe methods to access them. In other words,
//! a convenient place to store mcguffins identified by some key,
//! defaulting to a std::string.

template<typename key_type=std::string,
	 bool multimap=false,
	 typename cmp_type=std::less<key_type>,
	 class Allocator = std::allocator<std::pair<const key_type, ref<obj>>
				     >>
	using mcguffinstash=
	ref<mcguffinstashObj<key_type, multimap, cmp_type, Allocator>,
	    mcguffinstashBase<key_type, multimap, cmp_type, Allocator>>;

//! A nullable pointer reference to a \ref mcguffinstash "mcguffin stash".

template<typename key_type=std::string,
	 bool multimap=false,
	 typename cmp_type=std::less<key_type>,
	 class Allocator = std::allocator<std::pair<const key_type, ref<obj>>
				     >>
	using mcguffinstashptr=
	ptr<mcguffinstashObj<key_type, multimap, cmp_type, Allocator>,
	    mcguffinstashBase<key_type, multimap, cmp_type, Allocator>>;

//! A reference to a constant \ref mcguffinstash "mcguffin stash".

template<typename key_type=std::string,
	 bool multimap=false,
	 typename cmp_type=std::less<key_type>,
	 class Allocator = std::allocator<std::pair<const key_type, ref<obj>>
				     >>
	using const_mcguffinstash=
	const_ref<mcguffinstashObj<key_type, multimap, cmp_type, Allocator>,
		  mcguffinstashBase<key_type, multimap, cmp_type, Allocator>>;

//! A nullable pointer reference to a constant \ref mcguffinstash "mcguffin stash".

template<typename key_type=std::string,
	 bool multimap=false,
	 typename cmp_type=std::less<key_type>,
	 class Allocator = std::allocator<std::pair<const key_type, ref<obj>>
				     >>
	using const_mcguffinstashptr=
	const_ptr<mcguffinstashObj<key_type, multimap, cmp_type, Allocator>,
		  mcguffinstashBase<key_type, multimap, cmp_type, Allocator>>;

#if 0
{
#endif
}
#endif
