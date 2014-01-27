/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakmapobj_H
#define x_weakmapobj_H

#include <x/weakcontainerobj.H>
#include <x/ptr.H>
#include <map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Different processing of std::map and std::multimap insert() method.

//! The insert() methods of std::map and std::multimap return different types.
//! This class encapsulates the type-specific processing.

template<typename C>
class weakmapiteratorOps;

//! Specialization for processing of the return value from std::map::insert().

template<>
template<typename K, typename V, typename C, typename A>
class weakmapiteratorOps<std::map<K, V, C, A> > {

	//! Shorthand for the map

	typedef std::map<K, V, C, A> map_type;

public:
	//! The type returned by std::map's insert() method

	typedef typename std::pair<typename map_type::iterator, bool>
	map_insert_ret_type;

	//! Check if the insert() method succeeded

	static inline bool inserted(const map_insert_ret_type &i) noexcept
	{
		return i.second;
	}

	//! Return the inserted iterator

	static typename map_type::iterator iter(const map_insert_ret_type &i)
		noexcept
	{
		return i.first;
	}
};

//! Specialization for processing of the return value from std::multimap::insert().

template<>
template<typename K, typename V, typename C, typename A>
class weakmapiteratorOps<std::multimap<K, V, C, A> > {

	//! Shorthand for the map

	typedef std::multimap<K, V, C, A> map_type;

public:
	//! The type returned by std::map's insert() method

	typedef typename map_type::iterator map_insert_ret_type;

	//! Check if the insert() method succeeded

	static inline bool inserted(const map_insert_ret_type &i) noexcept
	{
		return true;
	}

	//! Return the inserted iterator

	static typename map_type::iterator iter(const map_insert_ret_type &i)
		noexcept
	{
		return i;
	}
};

//! Weak reference map container implementation

//! This subclass of weakContainerObj implements the container as a
//! std::map, and provides the insert() method and an assignment operator.
//!
//! The first template parameter is the map's key.
//! The second template parameter must be a reference to an object.
//! The third parameter is an STL comparison object.
//! The fourth template parameter should be
//! either std::map<K, weakptr<T>, C, Allocator> or
//! std::multimap<K, weakptr<T>, C, Allocator>.
//! The fifth
//! parameter is an STL allocator, that's used to allocate
//! std::pair<K,weakptr<T> > objects
//! that get added to the map.

template<typename K,
	 typename T,
	 typename C,
	 typename M,
	 typename Allocator>
class weakmapObj : public weakContainerObj<T, M, Allocator> {

public:
	//! Helper object for implementing the assignment operation.

	//! operator[] instantiates this object, and saves the key value, and
	//! a reference to the original weak map. The assignment operator
	//! proceeds with iniating the insert() operation, now that all the
	//! key pieces are available.

	class assignmentHelper {

		//! The saved key value
		const K k;

		//! A reference to the original map
		ref<weakmapObj<K, T, C, M, Allocator> > mapref;

	public:
		//! Constructor

		assignmentHelper(//! The key
				 const K &kArg,

				 //! The original map.
				 const ref<weakmapObj<K, T, C, M, Allocator> >
				 &refArg)
			: k(kArg), mapref(refArg)
		{
		}

		~assignmentHelper() noexcept
		{
		}

		//! The assignment operator

		//! The assignment operation gets converted into an insert()
		//! operation on the underlying weak map.

		bool operator=(//! The reference being stored in the map
			       const T &value)
		{
			return mapref->insert(std::make_pair(k, value));
		}
	};

	//! The superclass

	typedef weakContainerObj<T, M, Allocator> weak_container_t;

	//! The iterator class, defined in the superclass

	typedef typename weak_container_t::iterator iterator;

	//! The internal read lock object in the superclass

	typedef typename weak_container_t::rlock rlock;

	//! Its container

	typedef typename weak_container_t::container_t container_t;

	//! Default constructor
	explicit weakmapObj(const C &comp=C(),
			    const Allocator &a=Allocator()) noexcept
		: weak_container_t(comp, a)
	{
	}

	//! Default destructor
	~weakmapObj() noexcept
	{
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

		ptrValue->addOnDestroy(destroyed);
		destroyed->install(weakmapiteratorOps<M>::iter(i));
		return true;
	}

	//! The [] operator.

	//! The [] operation returns an assignmentHelper object. Assigning to
	//! this object gets translated into an insert() operation.

	assignmentHelper operator[](const K &k)
	{
		return assignmentHelper(k, ref<weakmapObj<K, T, C, M,
					Allocator> >(this));
	}

	//! Search the map for a key

	iterator find(//! The key to search for
		      const K &key) const
	{
		rlock lock = rlock::create(this);

		return this->begin(lock, typename weak_container_t::container_t
				   ::readlock(lock->container())->find(key));
	}

	//! Find a key in the map, if not exists, create the value using the passed lambda
	template<typename lambda_type>
	ref<typename T::obj_type> find_or_create(//! The key to search for
						 const K &key,

						 //! If not found, call lambda
						 lambda_type &&lambda)
	{
		std::unique_lock<std::mutex> object_lock(this->objmutex);

		{
			auto range=this->equal_range(key);

			while (range.first != range.second)
			{
				auto ptr=range.first->second.getptr();
				if (!ptr.null())
					return ptr;
				++range.first;
			}
		}
		ref<typename T::obj_type> value=lambda();
		insert(key, value);

		return value;
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

#if 0
{
#endif
}
#endif
