/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_orderedcache_H
#define x_orderedcache_H

#include <x/namespace.h>
#include <map>
#include <cstddef>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Helper class for the \ref ordered_cache "orderer_cache" template.

//! \internal
//!
//! Figures out what the ordering function returns.

template<typename orderer_type,
	 typename key_type,
	 typename value_type> class ordered_cache_helper {

public:
	//! Dummy ordering function
	orderer_type o;

	//! Dummy key
	const key_type k;

	//! Dummy value
	const value_type v;

	//! Dummy class instance
	static ordered_cache_helper<orderer_type, key_type, value_type> *p;

	//! What the ordering function returns
	typedef decltype(p->o(k, v)) order_t;
};


//! Default orderer class for an \ref ordered_cache "ordered cache"

//! Order the cache entries in the order they were add()ed
//! to the cache. The orderer is simply an internal counter.

class ordered_cache_fifo {

	//! Internal counter
	size_t counter=0;
public:

	//! Order a new entry.
	template<typename key_type, typename value_type>
	size_t operator()(const key_type &key,
			  const value_type &value)
	{
		return ++counter;
	}
};

//! Ordered cache traits

//! This template defines a class that, in turns gets specified as the first
//! parameter to the \ref ordered_cache "ordered_cache" template, and
//! defines the ordered cache's key type, value type, and ordering type.
//! The first and the second template parameter are required, and specify
//! the cache key and value. The remaining template parameters are optional.
//!
//! When it's time to prune the cache after it reaches its maximum size,
//! each entry's ordinal value determines which entry gets tossed. The third
//! template
//! parameter is a class with an operator() that gets passed they key and
//! the data value in the cache.
//!
//! The () operator returns some ordinal value, and when the cache is full,
//! the cache entry with the least ordinal value is the one that gets removed.
//! If multiple keys have the same ordinal value, it is not specified which one
//! of them gets the boot. The
//! \ref ordered_cache_fifo "default ordering class"
//! ignores the key and the data values, and returns a simple counter value,
//! so the default implementation drops the oldest entry in the cache.
//!
//! The fourth template parameter gives the key comparator type.
//!
//! The fifth template parameters gives the ordinal value type's comparator
//! type.

template<typename key_typeClass,
	 typename value_typeClass,
	 typename orderer_lambdaClass=ordered_cache_fifo,
	 typename key_comparator_tClass=std::less<key_typeClass>,
	 typename order_comparator_tClass=std::less<
		 typename ordered_cache_helper<orderer_lambdaClass,
					       key_typeClass,
					       value_typeClass>::order_t> >
class ordered_cache_traits {

public:
	//! Key type
	typedef key_typeClass key_type;

	//! Value type
	typedef value_typeClass value_type;

	//! Selected orderer
	typedef orderer_lambdaClass orderer_lambda;

	//! Key comparator class
	typedef key_comparator_tClass key_comparator_t;

	//! Ordinal comparator class
	typedef order_comparator_tClass order_comparator_t;
};

//! An ordered cache

//! Maintains a container of some maximum fixed size, set when this is
//! constructed. The container holds key/value tuples. key must be unique.
//! add() adds a key/value tuple to the container, if key exists the existing
//! entry's value is updated to the new value. remove() removes a key, if it
//! exists. find() looks up a key.
//!
//! If add() finds that the container has reached its maximum size, some
//! existing entry gets removed.
//!
//! The first template parameter must be an \ref ordered_cache_traits "ordered_cache_traits" class.
//!
//! By default, the ordinal value of each key/value tuple is set only by add()
//! when the key/value tuple gets added to the cache. Calling add() again with
//! the same key only updates the value part of the tuple, if it's still in
//! the cache.
//!
//! Setting the second optional template parameter to \c true has the effect
//! of implementing most-recently-used caching properties. Namely:
//!
//! When find() or add() finds an existing cache entry, the ordinal template
//! class's operator() is called again to obtain the new ordinal value for
//! the cached key/value tuple, and it gets repositioned in the cache
//! accordingly.

template<typename traits_type, bool mru=false>
class ordered_cache {

	//! Key type
	typedef typename traits_type::key_type key_type;

	//! Value type
	typedef typename traits_type::value_type value_type;

	//! Selected orderer
	typedef typename traits_type::orderer_lambda orderer_lambda;

	//! Key comparator class
	typedef typename traits_type::key_comparator_t key_comparator_t;

	//! Ordinal comparator class
	typedef typename traits_type::order_comparator_t order_comparator_t;

	//! What the cache entries are ordered by
	typedef typename ordered_cache_helper<orderer_lambda, key_type,
					      value_type>::order_t order_t;

	//! Requested cache size
	size_t cachesize;

	//! Ordering function
	orderer_lambda orderer;

	//! Container for ordered cache entries

	//! Keyed by an ordering value instance, value is pair of key/value

	typedef std::multimap<order_t, std::pair<key_type, value_type>,
			      order_comparator_t> cache_by_order_t;

	//! Ordered cache entries
	cache_by_order_t cache_by_order;

	//! Container for looking up ordered cached entries
	std::map<key_type, typename cache_by_order_t::iterator,
		 key_comparator_t> cache;

public:

	//! Constructor
	ordered_cache(//! Size of the cache
		      size_t cachesizeArg,

		      //! Ordering lambda/class
		      const orderer_lambda &ordererArg=orderer_lambda(),

		      //! Cache key comparator
		      const key_comparator_t &key_comparatorArg=key_comparator_t(),

		      //! Comparator for the ordering value
		      const order_comparator_t &order_comparatorArg
		      =order_comparator_t())
		: cachesize(cachesizeArg),
		  orderer(ordererArg),
		  cache_by_order(order_comparatorArg),
		  cache(key_comparatorArg)
	{
	}

	//! Remove an entry from the cache

	//! Returns true of the key was found in the cache, and removed.
	bool remove(const key_type &key)
	{
		auto lookup=cache.find(key);

		if (lookup == cache.end())
			return false;

		cache_by_order.erase(lookup->second);
		cache.erase(lookup);
		return true;
	}

	//! Add something to the cache

	//! If the cache exceeds its maximum size, remove least-ordered value.

	void add(const key_type &key,
		 const value_type &value)
	{
		std::pair<key_type, value_type> key_and_value(key, value);

		auto new_cache_entry=
			cache_by_order.insert(std::make_pair(orderer(key,
								     value),
							     std::make_pair
							     (key, value)));
	again:
		auto lookup=cache.insert(std::make_pair(key, new_cache_entry));

		if (!lookup.second) // Key already exists, update its value.
		{
			if (mru)
			{
				// Remove old entry.
				cache_by_order.erase(lookup.first->second);
				cache.erase(lookup.first);
				goto again;
			}

			lookup.first->second->second.second=value;
			cache_by_order.erase(new_cache_entry);
				// Duplicate entry not needed.
		}

		while (cache_by_order.size() > cachesize)
			remove(cache_by_order.begin()->second.first);
	}

	//! Search for an entry in the cache

	//! Returns a nullptr if the key was not found in the cache, otherwise
	//! a pointer to the cached key's value.

	const value_type *find(const key_type &key)
	{
		auto lookup=cache.find(key);

		if (lookup == cache.end())
			return nullptr;

		if (mru)
		{
			// Grab the value
			const auto &value=lookup->second->second.second;

			// Make a new entry in the ordering cache
			auto updated_cache_entry=
				cache_by_order.insert(std::make_pair
						      (orderer(key, value),
						       std::make_pair
						       (key, value)));

			// Remove the old entry
			cache_by_order.erase(lookup->second);

			// Update the link
			lookup->second=updated_cache_entry;
		}

		return &lookup->second->second.second;
	}
};

#if 0
{
#endif
}
#endif
