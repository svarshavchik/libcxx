/*
** Copyright 2014-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinmapobj_H
#define x_mcguffinmapobj_H

#include <x/mcguffincontainerobj.H>
#include <x/weakcontainerobj.H>
#include <x/weakmapobj.H>
#include <map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Implement a weak mcguffin map container object.

//! \internal
//!
//! \see mcguffinmap.
//!
//! Template parameters: map key, map ref<> value, comparator class,
//! the std::map or the std::multimap class that gets forwarded to
//! the superclass, the allocator class.

template<typename K,
	 typename ref_type,
	 typename M>
class mcguffinmapObj : public weakmapBaseObj<K, ref_type, M> {

	//! find_or_create() atomicity
	std::recursive_mutex insert_mutex;

public:

	//! Base class typedef, for convenience.
	typedef weakmapBaseObj<K, ref_type, M> base_t;

	//! Import typedef from the base class.

	typedef typename base_t::weak_container_t weak_container_t;

	//! Import typedef from the base class.

	typedef typename base_t::iterator iterator;

	//! Import typedef from the base class.

	typedef typename base_t::rlock rlock;

	//! Import typedef from the base class.

	typedef typename base_t::container_t container_t;

	//! Convenience typedef
	typedef typename base_t::template assignmentHelper<
		ref<mcguffinmapObj<K, ref_type, M>>
		> assignmentHelper_t;

	//! ref_type is ref<objClass, baseType, define the ptr for it.

	typedef typename mcguffincontainerObj<ref_type>::ptr_t ptr_t;

	//! Default constructor
	template<typename ...Args>
	explicit mcguffinmapObj(Args && ...args) noexcept
		: base_t(std::forward<Args>(args)...)
	{
	}

	//! Default destructor
	~mcguffinmapObj()=default;

	//! Implement a map-like insert()

	//! A weak reference to the object gets added to the map, keyed by
	//! the given key. A mcguffin gets returned for the object in the map.
	//! a null ptr() gets returned if the key already exists in the
	//! underlying std::map.

	ptr<obj> insert(//! The key/value being added to the map
			const std::pair<const K, ptr_t> &newValue)
	{
		return insert(newValue.first, newValue.second);
	}

	//! A more sane insert().

	ptr<obj> insert(//! The key for the new weak pointer.

			const K &keyValue,

			//! The pointer

			const ptr_t &ptrValue)
	{
		if (ptrValue.null())
			return ptr<obj>();

		std::unique_lock<std::recursive_mutex>
			insert_lock(insert_mutex);

		auto lock=weak_container_t::rlock::create(this);

		typename container_t::writelock w(lock->container());

		ref<typename weak_container_t::cleanupObj>
			destroyed(ref<typename weak_container_t::cleanupObj>
				  ::create(this));


		auto mcguffin=ref<mcguffincontainerObj<ref_type>>
				 ::create(ptrValue);

		auto wrapper=mcguffin->create_wrapper();

		typename weakmapiteratorOps<M>::map_insert_ret_type
			i=w->insert(std::make_pair(keyValue,
						   ptr<
						   typename
						   mcguffincontainerObj
						   <ref_type>::wrapperObj>
						   (wrapper)));

		if (!weakmapiteratorOps<M>::inserted(i))
			return ptr<obj>();

		wrapper->ondestroy([destroyed]{destroyed->destroyed();});
		destroyed->install(weakmapiteratorOps<M>::iter(i));
		return mcguffin;
	}

	//! The [] operator.

	//! The [] operation returns an assignmentHelper object. Assigning to
	//! this object gets translated into an insert() operation.

	assignmentHelper_t operator[](const K &k)
	{
		return assignmentHelper_t
			(k, ref<mcguffinmapObj<K, ref_type, M>
			 >(this));
	}

	//! Find a key in the map, if not exists, create the value using the passed lambda
	template<typename functor,
		 typename lambda_must_return_a_ref=
		 std::enable_if_t<std::is_same_v
				  <decltype(std::declval<functor &&>()()),
				   typename refptr_traits
				   <decltype(std::declval<functor &&>()())>
				   ::ref_t>>>

	auto find_or_create(//! The key to search for
			    const K &key,

			    //! If not found, call lambda
			    functor &&lambda)
	{
		typedef decltype(handle_find_or_create<M>
				 ::insert_into_mcguffin(this, key,
							lambda())) ret_t;

		std::unique_lock<std::recursive_mutex>
			insert_lock(insert_mutex);

		{
			auto range=this->equal_range(key);

			while (range.first != range.second)
			{
				auto mcguffin=range.first
					->second.get_internal_mcguffin();

				if (!mcguffin.null())
					return ret_t{mcguffin,
							mcguffin->strongref};
				++range.first;
			}
		}

		auto value=lambda();

		return handle_find_or_create<M>
			::insert_into_mcguffin(this, key, value);
	}
};

#if 0
{
#endif
}
#endif
