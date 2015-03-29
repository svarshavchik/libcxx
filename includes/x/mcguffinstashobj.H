/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinstashobj_H
#define x_mcguffinstashobj_H

#include <x/mpobj.H>
#include <x/mcguffinstashfwd.H>

#include <set>
#include <vector>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Helper class for selecting either a map or a multimap

template<typename key_type, bool multimap,
	 typename cmp_type, typename Allocator> class mcguffins_container;

//! A mcguffins container that uses a map.
template<typename key_type, typename cmp_type, typename Allocator>
class mcguffins_container<key_type, false, cmp_type, Allocator> {
public:

	//! The container
	typedef std::map<key_type, ref<obj>, cmp_type, Allocator> container_t;
};

//! A mcguffins container that uses a multimap.

template<typename key_type, typename cmp_type, typename Allocator>
class mcguffins_container<key_type, true, cmp_type, Allocator> {
public:

	//! The container
	typedef std::multimap<key_type, ref<obj>, cmp_type,
			      Allocator> container_t;
};


//! A reference-counted, thread safe, place to keep your mcguffins.

//! \see mcguffinstash

template<typename key_type, bool multimap,
	 typename cmp_type, typename Allocator>
class mcguffinstashObj : virtual public obj {

public:
	//! The type of the container where the mcguffins get stashed.

	typedef typename mcguffins_container<key_type, multimap,
					     cmp_type, Allocator
					     >::container_t map_t;

	//! Mutex-protected container for the mcguffins
	typedef mpobj<map_t> container_t;

	//! All currently held mcguffins
	mutable container_t container;

	//! Constructor
	template<typename ...Args>
	mcguffinstashObj(Args && ...args)
		: container(std::forward<Args>(args)...)
	{
	}

	//! Destructor
	~mcguffinstashObj() noexcept
	{
	}

	//! Add a mcguffin.

	//! Returns the value that's returned by insert() on the underlying
	//! map. Note that the map is mutex-protected, so, for thread safety,
	//! the returned iterator should only be used to check if insert()
	//! succeeded.

	auto insert(const key_type &key, const ref<obj> &m)
	{
		typename container_t::lock lock(container);

		return lock->insert(std::make_pair(key, m));
	}

	//! Add multiple mcguffins

	//! For convenience. Note that void is returned.

	template<typename ...moreArgs>
	inline void insert(const key_type &key, const ref<obj> &m,
			   const key_type &next_key, moreArgs && ...args)
	{
		insert(key, m);
		insert(next_key, std::forward<moreArgs>(args)...);
	}

	//! Remove a mcguffin.

	//! For a multimap, all mcguffins with the given key get removed.

	void remove(const key_type &key)
	{
		// mcguffin goes out of scope only after the lock is released.

		std::vector<ptr<obj>> mcguffin;

		typename container_t::lock lock(container);

		for (auto range=lock->equal_range(key);
		     range.first != range.second; )
		{
			mcguffin.push_back(range.first->second);

			auto iter=range.first;

			++range.first;

			lock->erase(iter);
		}
	}

	//! Remove more than one mcguffin.

	template<typename ...moreArgs>
	inline void remove(const key_type &key,
			   const key_type &next_key, moreArgs && ...args)
	{
		remove(key);
		remove(next_key, std::forward<moreArgs>(args)...);
	}

	//! Remove multiple mcguffins.
	//! Remove all mcguffins
	void remove_all()
	{
		map_t copy;

		typename container_t::lock lock(container);

		copy=*lock;
		lock->clear();
	}

	//! Return a mcguffin

	//! Returns a null ptr if the mcguffin's key doesn't exist.
	//! For a multimap, returns the first key.

	ptr<obj> get(const key_type &key) const
	{
		ptr<obj> p;

		typename container_t::lock lock(container);

		auto iter=lock->find(key);

		if (iter != lock->end())
			p=iter->second;

		return p;
	}

	//! Return all mcguffins with the same key (for a multimap)

	//! Returns an empty vector if the mcguffin's key doesn't exist.

	std::vector<ref<obj>> get_all(const key_type &key) const
	{
		std::vector<ref<obj>> v;

		typename container_t::lock lock(container);

		for (auto range=lock->equal_range(key);
		     range.first != range.second; ++range.first)
		{
			v.push_back(range.first->second);
		}

		return v;
	}

	//! Enumerate mcguffin keys.

	std::set<key_type> enumerate() const
	{
		std::set<key_type> retval;

		typename container_t::lock lock(container);

		for (const auto &iter:*lock)
			retval.insert(iter.first);

		return retval;
	}
};

#ifndef DOXYGEN
extern template class mcguffinstashObj<std::string>;
extern template mcguffinstashObj<std::string>::mcguffinstashObj();

extern template class mcguffinstashObj<std::string, true>;
extern template mcguffinstashObj<std::string, true>::mcguffinstashObj();
#endif

#if 0
{
#endif
}
#endif
