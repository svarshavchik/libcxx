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

//! Implements methods common to weakmapObj and weakmcguffinmapObj; all
//! methods except insert(), find_or_create(), and the assignment operator.
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
class weakmapBaseObj : public weakContainerObj<T, M, Allocator> {

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

		~assignmentHelper() noexcept
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

	typedef weakContainerObj<T, M, Allocator> weak_container_t;

	//! The iterator class, defined in the superclass

	typedef typename weak_container_t::iterator iterator;

	//! The internal read lock object in the superclass

	typedef typename weak_container_t::rlock rlock;

	//! Its container

	typedef typename weak_container_t::container_t container_t;

	//! Default constructor
	explicit weakmapBaseObj(const C &comp=C(),
				const Allocator &a=Allocator()) noexcept
		: weak_container_t(comp, a)
	{
	}

	//! Default destructor
	~weakmapBaseObj() noexcept
	{
	}

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

	//! Calculate the return type from weakmap::find_or_create().

	//! The callback lambda must return a ref. A compilation error here
	//! means that the callback lambda is not returning a ref.
	//!
	//! It's a ptr, since find_or_create() can fail.

	template<typename objClass, typename baseType>
	static ptr<objClass, baseType>
	weak_return_value(ref<objClass, baseType> &&value)
	{
		return ptr<objClass, baseType>();
	}

	//! Calculate the return type from mcguffinmap::find_or_create().

	//! The callback lambda must return a ref. A compilation error here
	//! means that the callback lambda is not returning a ref.
	//!
	//! It's a ptr, since find_or_create() can fail.

	template<typename objClass, typename baseType>
	static std::pair<ptr<obj>, ptr<objClass, baseType>>
	mcguffin_return_value(ref<objClass, baseType> &&value)
	{
		return std::pair<ptr<obj>, ptr<objClass, baseType>>();
	}

	//! Try valiantly to create, after we couldn't find anything.

	template<typename objClass, typename baseType,
		 typename key_type,
		 typename ptr_type>
	static inline ptr<objClass, baseType>
	insert_into_map(ptr_type *m,
			const key_type &key,
			const ref<objClass, baseType> &value)
	{
		if (!m->insert(key, value))
			return ptr<objClass, baseType>();
		return value;
	}

	//! Try valiantly to create, after we couldn't find anything.

	template<typename objClass, typename baseType,
		 typename key_type,
		 typename ptr_type>
	static std::pair<ptr<obj>, ptr<objClass, baseType>>
		insert_into_mcguffin(ptr_type *m,
				     const key_type &key,
				     const ref<objClass, baseType> &value)
	{
		auto mcguffin=m->insert(key, value);

		if (mcguffin.null())
			return std::pair<ptr<obj>, ptr<objClass, baseType>>();

		return std::pair<ptr<obj>, ptr<objClass, baseType>>
			(mcguffin, value);
	}
};

//! Helper class for implementing correct weakmapObj::find_or_create() semantics

//! Specialization for a weakmultimap
//! \internal

template<typename K, typename V, typename C, typename A>
class handle_find_or_create<std::multimap<K, V, C, A>> {

 public:
	//! Calculate the return type from weakmap::find_or_create().

	//! The callback lambda must return a ref. A compilation error here
	//! means that the callback lambda is not returning a ref.

	template<typename objClass, typename baseType>
	static ref<objClass, baseType>
	weak_return_value(ref<objClass, baseType> &&value)
	{
		return std::move(value);
	}

	//! Calculate the return type from mcguffinmap::find_or_create().

	//! The callback lambda must return a ref. A compilation error here
	//! means that the callback lambda is not returning a ref.
	//!
	//! It's a ptr, since find_or_create() can fail.

	template<typename objClass, typename baseType>
	static std::pair<ref<obj>, ref<objClass, baseType>>
	mcguffin_return_value(ref<objClass, baseType> &&value)
	{
		return std::pair<ref<obj>, ref<objClass, baseType>>
			(ref<obj>::create(), value);
	}

	//! Ho-hum.

	template<typename objClass, typename baseType,
		 typename key_type,
		 typename ptr_type>
	static inline ref<objClass, baseType>
		insert_into_map(ptr_type *m,
				const key_type &key,
				const ref<objClass, baseType> &value)
	{
		m->insert(key, value);
		return value;
	}

	//! Ho-hum.

	template<typename objClass, typename baseType,
		 typename key_type,
		 typename ptr_type>
		static std::pair<ref<obj>, ref<objClass, baseType>>
		insert_into_mcguffin(ptr_type *m,
				     const key_type &key,
				     const ref<objClass, baseType> &value)
	{
		return std::pair<ref<obj>, ref<objClass, baseType>>
			(m->insert(key, value), value);
	}
};

//! A map with weak references.

//! \see weakmapBaseObj

template<typename K,
	 typename T,
	 typename C,
	 typename M,
	 typename Allocator>
class weakmapObj : public weakmapBaseObj<K, T, C, M, Allocator> {

	//! find_or_create() atomicity
	std::recursive_mutex insert_mutex;
public:

	//! Base class typedef, for convenience.
	typedef weakmapBaseObj<K, T, C, M, Allocator> base_t;

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
		ref<weakmapObj<K, T, C, M, Allocator>>> assignmentHelper_t;

	//! Constructor
	explicit weakmapObj(const C &comp=C(),
			    const Allocator &a=Allocator()) noexcept
		: base_t(comp, a)
	{
	}

	//! Destructor
	~weakmapObj() noexcept
	{
	}

	//! Find a key in the map, if not exists, create the value using the passed lambda
	template<typename lambda_type>
	auto find_or_create(//! The key to search for
			    const K &key,

			    //! If not found, call lambda
			    lambda_type &&lambda)
		-> decltype( handle_find_or_create<M>
			     ::weak_return_value(lambda()))
	{
		std::unique_lock<std::recursive_mutex>
			insert_lock(insert_mutex);

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

		ptrValue->addOnDestroy(destroyed);
		destroyed->install(weakmapiteratorOps<M>::iter(i));
		return true;
	}

	//! The [] operator.

	//! The [] operation returns an assignmentHelper object. Assigning to
	//! this object gets translated into an insert() operation.

	assignmentHelper_t operator[](const K &k)
	{
		return assignmentHelper_t
			(k, ref<weakmapObj<K, T, C, M, Allocator> >(this));
	}
};

#if 0
{
#endif
}
#endif
