/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinstash_H
#define x_mcguffinstash_H

#include <x/mcguffinstashfwd.H>
#include <x/mcguffinstashobj.H>
#include <x/ref.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for a \ref mcguffinstash "mcguffin stash".

//! Provides several convenient typedef aliases.

template<typename key_type,
	 bool multimap,
	 typename cmp_type,
	 class Allocator> class mcguffinstashBase : public ptrrefBase {

public:

	//! Typedef alias

	//! This is the actual std::map or std::multimap with the mcguffins
	typedef typename mcguffinstashObj<key_type, multimap, cmp_type,
					  Allocator>::map_t map_t;

	//! Typedef alias

	//! A \ref mpobj "mutex-protected" map_t

	typedef typename mcguffinstashObj<key_type, multimap, cmp_type,
					  Allocator>::container_t container_t;
};

#ifndef DOXYGEN

extern template mcguffinstash<std::string>
ptrrefBase::objfactory<mcguffinstash<std::string>>::create();

extern template mcguffinstash<std::string, true>
ptrrefBase::objfactory<mcguffinstash<std::string, true>>::create();

extern template mcguffinstash<std::string>::~mcguffinstash();
extern template mcguffinstash<std::string, true>::~mcguffinstash();
#endif

#if 0
{
#endif
}
#endif
