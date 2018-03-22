/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakmapobj_H
#define x_weakmapobj_H

#include <x/weakcontainerobj.H>
#include <x/refptr_traits.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Different processing of std::map and std::multimap insert() method.

//! The insert() methods of std::map and std::multimap return different types.
//! This class encapsulates the type-specific processing.

template<typename C>
class weakmapiteratorOps;


//! Weak reference map container implementation

//! Implements methods common to weakmapObj and weakmcguffinmapObj; all
//! methods except insert(), find_or_create(), and the assignment operator.
//!
//! The first template parameter is the map's key.
//! The second template parameter must be a reference to an object.
//! The third template parameter should be
//! either std::map<K, weakptr<T>, C, Allocator> or
//! std::multimap<K, weakptr<T>, C, Allocator>.

template<typename K,
	 typename T,
	 typename M>
class weakmapBaseObj : public weakContainerObj<T, M> {

public:
	//! Helper object for implementing the assignment operation.

	//! operator[] instantiates this object, and saves the key value, and
	//! a reference to the original weak map. The assignment operator
	//! proceeds with iniating the insert() operation, now that all the
	//! key pieces are available.

	template<typename ref_type>
	class assignmentHelper {

		//! The saved key value
		const K k;

		//! A reference to the original map
		ref_type mapref;

	public:
		//! Constructor

		assignmentHelper(//! The key
				 const K &kArg,

				 //! The original map.
				 const ref_type &refArg)
			: k(kArg), mapref(refArg)
		{
		}

		~assignmentHelper()
		{
		}

		//! The assignment operator

		//! The assignment operation gets converted into an insert()
		//! operation on the underlying weak map.

		auto operator=(//! The reference being stored in the map
			       const T &value)
			->decltype(mapref->insert(std::make_pair(k, value)))
		{
			return mapref->insert(std::make_pair(k, value));
		}
	};

	//! The superclass

	typedef weakContainerObj<T, M> weak_container_t;

	//! The iterator class, defined in the superclass

	typedef typename weak_container_t::iterator iterator;

	//! The internal read lock object in the superclass

	typedef typename weak_container_t::rlock rlock;

	//! Its container

	typedef typename weak_container_t::container_t container_t;

	//! Default constructor
	template<typename ...Args>
	explicit weakmapBaseObj(Args && ...args) noexcept
		: weak_container_t(std::forward<Args>(args)...)
	{
	}

	//! Default destructor
	~weakmapBaseObj()=default;

	//! Search the map for a key

	iterator find(//! The key to search for
		      const K &key) const
	{
		rlock lock = rlock::create(this);

		return this->begin(lock, typename weak_container_t::container_t
				   ::readlock(lock->container())->find(key));
	}

	//! Count number of entries in the map

	size_t count(//! The key to search for)
		     const K &key) const
	{
		rlock lock = rlock::create(this);

		return typename weak_container_t::container_t
			::readlock(lock->container())->count(key);
	}

	//! Retrieve the lower bound iterator for a given key

	iterator lower_bound(//! The key to search for
			     const K &key) const
	{
		rlock lock = rlock::create(this);

		return this->begin(lock, typename weak_container_t::container_t
				   ::readlock(lock->container())
				   ->lower_bound(key));
	}

	//! Retrieve the upper bound iterator for a given key

	iterator upper_bound(//! The key to search for
			     const K &key) const
	{
		rlock lock = rlock::create(this);

		return this->begin(lock, typename weak_container_t::container_t
				   ::readlock(lock->container())
				   ->upper_bound(key));
	}

	//! Retrieve the range iterator pair for a given key

	std::pair<iterator, iterator> equal_range(//! The key to search for
						  const K &key) const
	{
		rlock lock = rlock::create(this);

		std::pair<typename M::const_iterator,
			  typename M::const_iterator>
			range=typename weak_container_t::container_t
			::readlock(lock->container())->equal_range(key);

		return std::make_pair(this->begin(lock, range.first),
				      this->begin(lock, range.second));
	}
};

//! Helper class for implementing correct weakmapObj::find_or_create() semantics

//! \internal

template<typename underlyingMapType>
class handle_find_or_create {

public:

	//! Try valiantly to create, after we couldn't find anything.

	template<typename ref_type,
		 typename key_type,
		 typename ptr_type>
	static inline typename refptr_traits<ref_type>::ptr_t
	insert_into_map(ptr_type *m,
			const key_type &key,
			const ref_type &value)
	{
		if (!m->insert(key, value))
			return {};
		return value;
	}

	//! Try valiantly to create, after we couldn't find anything.

	template<typename ref_type,
		 typename key_type,
		 typename ptr_type>
	static std::pair<ptr<obj>,
			 typename refptr_traits<ref_type>::ptr_t>
	insert_into_mcguffin(ptr_type *m,
			     const key_type &key,
			     const ref_type &value)
	{
		typedef typename refptr_traits<ref_type>::ptr_t ptr_t;

		auto mcguffin=m->insert(key, value);

		if (mcguffin.null())
			return { ptr<obj>{}, ptr_t{} };

		return {mcguffin, value};
	}
};

//! A map with weak references.

//! \see weakmapBaseObj

template<typename K,
	 typename T,
	 typename M>
class weakmapObj : public weakmapBaseObj<K, T, M> {

	//! find_or_create() atomicity
	std::recursive_mutex insert_mutex;
public:

	//! Base class typedef, for convenience.
	typedef weakmapBaseObj<K, T, M> base_t;

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
		ref<weakmapObj<K, T, M>>> assignmentHelper_t;

	//! Constructor
	template<typename ...Args>
	explicit weakmapObj(Args && ...args) noexcept
		: base_t(std::forward<Args>(args)...)
	{
	}

	//! Destructor
	~weakmapObj()=default;

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
				 ::insert_into_map(this, key, lambda())) ret_t;

		std::unique_lock<std::recursive_mutex>
			insert_lock(insert_mutex);

		{
			auto range=this->equal_range(key);

			while (range.first != range.second)
			{
				auto ptr=range.first->second.getptr();
				if (!ptr.null())
					return ret_t{ptr};
				++range.first;
			}
		}
		auto value=lambda();

		return handle_find_or_create<M>
			::insert_into_map(this, key, value);
	}

	//! Implement a map-like insert()

	//! A weak reference to the object gets added to the map, keyed by
	//! the given key.

	bool insert(//! The key/value being added to the map
		    const std::pair<const K, T> &newValue)
	{
		return insert(newValue.first, newValue.second);
	}

	//! A more sane insert()

	bool insert(//! The key for the new weak pointer.

		    const K &keyValue,

		    //! The pointer

		    const T &ptrValue)
	{
		if (ptrValue.null())
			return false;

		std::unique_lock<std::recursive_mutex>
			insert_lock(insert_mutex);

		auto lock=weak_container_t::rlock::create(this);

		typename container_t::writelock w(lock->container());

		ref<typename weak_container_t::cleanupObj>
			destroyed(ref<typename weak_container_t::cleanupObj>
				  ::create(this));

		typename weakmapiteratorOps<M>::map_insert_ret_type
			i=w->insert(std::make_pair(keyValue,
						   weakptr<T>(ptrValue))
				    );

		if (!weakmapiteratorOps<M>::inserted(i))
			return false;

		ptrValue->ondestroy([destroyed]{destroyed->destroyed();});

		destroyed->install(weakmapiteratorOps<M>::iter(i));
		return true;
	}

	//! The [] operator.

	//! The [] operation returns an assignmentHelper object. Assigning to
	//! this object gets translated into an insert() operation.

	assignmentHelper_t operator[](const K &k)
	{
		return assignmentHelper_t
			(k, ref<weakmapObj<K, T, M> >(this));
	}
};

#if 0
{
#endif
}
#endif
