/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sorted_vector_H
#define x_sorted_vector_H

#include <vector>
#include <algorithm>
#include <functional>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename K> class sorted_vector_key;

//! Define the value in a \ref sorted_vector "sorted_vector"
template<typename K, typename T> class sorted_vector_value {

public:

	//! A sorted vector stores a tuple of the key, and type
	typedef std::pair<sorted_vector_key<K>, T> value_type;
};

//! Specialization for a sorted vector that does not store values, only keys.

template<typename K> class sorted_vector_value<K, void> {

public:

	//! If the sorted vector's value is specifed as a \c void, there is no second value in the tuple.

	class value_type {
	public:
		//! Only the key.

		sorted_vector_key<K> first;

		//! Constructor
		inline value_type(const sorted_vector_key<K> &firstArg)
			: first(firstArg)
		{
		}
	};
};

template<typename K,
	 typename T,
	 typename C=std::less<K>,
	 typename Allocator=std::allocator<typename sorted_vector_value
					   <K, T>::value_type>
	 > class sorted_vector;

//! The key container used by a \ref sorted_vector "sorted vector".

//! Blocks modifying of a sorted_vector's key. The key is private,
//! with the sorted_vector being a friend, thus it can access it.
//! An operator K() lets you see, but not touch.

template<typename K> class sorted_vector_key {

	//! The key value
	K value;

 public:

	//! Constructor
	sorted_vector_key(const K &valueArg) : value(valueArg)
	{
	}

	template<typename, typename, typename, typename>
	friend class sorted_vector;

	//! Peek at the value.
	operator K() const { return value; }
};

//! A sorted vector

//! Define a vector of key+value tuples, with the values kept in the
//! vector in key order.
//!
//! Two required template parameters, the key type, and the value type.
//!
//! The vector's value_type consists of a "first", which is a
//! \ref sorted_vector_key, that encapsulates the value, and a "second",
//! the value.
//!
//! Most, but not all methods from the vector superclass are public.
//! Sorted vector's modifications must be via these methods.

template<typename K,
	 typename T,
	 typename C,
	 typename Allocator
	 > class sorted_vector
	: private std::vector<typename sorted_vector_value
			      <K, T>::value_type
			      > {

	//! Vector superclass type
	typedef std::vector<typename sorted_vector_value
			    <K, T>::value_type
			    > vec_t;

	//! The comparator.
	C c;

 public:

	//! Constructor
	explicit sorted_vector(const C &comp=C(),
			       const Allocator &a=Allocator())
		: vec_t(a), c(comp)
	{
	}

	//! Destructor
	~sorted_vector()
	{
	}

	using vec_t::reference;
	using vec_t::const_reference;
	using vec_t::iterator;
	using vec_t::const_iterator;
	using vec_t::size_type;
	using vec_t::difference_type;
	using vec_t::value_type;
	using vec_t::allocator_type;
	using vec_t::pointer;
	using vec_t::const_pointer;
	using vec_t::reverse_iterator;
	using vec_t::const_reverse_iterator;

	using vec_t::begin;
	using vec_t::end;
	using vec_t::rbegin;
	using vec_t::rend;
	using vec_t::crbegin;
	using vec_t::crend;
	using vec_t::front;
	using vec_t::back;

	using vec_t::size;
	using vec_t::max_size;
	using vec_t::reserve;
	using vec_t::shrink_to_fit;

	using vec_t::erase;
	using vec_t::clear;
	using vec_t::empty;

	using vec_t::at;

protected:

	//! Internal search

	//! \internal
	auto upper_bound_search(const K &k)
	{
		return std::upper_bound(begin(), end(), k,
					[this]
					(const K &k,
					 auto &tuple)
					{
						return c(k, tuple.first.value);
					});
	}

	//! Internal search

	//! \internal
	auto upper_bound_search(const K &k) const
	{
		return std::upper_bound(begin(), end(), k,
					[this]
					(const K &k,
					 auto &tuple)
					{
						return c(k, tuple.first.value);
					});
	}
 public:

	//! Find the value that corresponds to the given key.

	//! Logarithmic complexity. If the value for the given key does not
	//! exist, returns the next lower key.
	//!
	//! Returns an iterator. to the value for the requested key, or
	//! the next lower key.
	auto find(const K &k)
	{
		auto iter=upper_bound_search(k);

		if (iter == begin())
			return end(); // Before the first vector value.

		return --iter;
	}

	//! Find the value that corresponds to the given key.

	//! Logarithmic complexity. If the value for the given key does not
	//! exist, returns the next lower key.
	//!
	//! Returns an iterator. to the value for the requested key, or
	//! the next lower key.

	auto find(const K &k) const
	{
		auto iter=upper_bound_search(k);

		if (iter == begin())
			return end(); // Before the first vector value.

		return --iter;
	}

	//! Remove keys.

	//! Removes all values whose keys are within the range specified
	//! by the lower bound and the upper bound values, and returns \c true.
	//!
	//! Returns \c false if lower_bound is not less than upper_bound,
	//! or if the first value in the vector is not less than upper_bound.

	bool remove(const K &lower_bound,
		    const K &upper_bound)
	{
		if (c(upper_bound, lower_bound))
			return false;

		if (!c(lower_bound, upper_bound))
			return true; // Empty range, this is a no-op.

		auto b=find(lower_bound),
			e=find(upper_bound);

		if (e == end())
			return false;

		if (b == end())
			b=begin();
		else
		{
			if (c(b->first.value, lower_bound))
				++b;
		}

		if (c(e->first.value, upper_bound))
			++e;
		vec_t::erase(b, e);
		return true;
	}

	//! Remove and shift keys.

	//! Calls duplicate() on the upper_bound key, creating it if it doesn't
	//! exist. Then, removes all existing keys between the lower bound and
	//! upper_bound, then subtracts (upper_bound-lower_bounds) from
	//! all keys starting with upper_bound.
	//!
	//! Returns \c false if the first value in the vector is not less than
	//! upper_bound, or if lower_bound is not less than upper_bound.

	bool remove_range(const K &lower_bound,
			  const K &upper_bound)
	{
		if (c(upper_bound, lower_bound))
			return false;

		if (!c(lower_bound, upper_bound))
			return true;

		auto subtract=upper_bound-lower_bound;

		this->duplicate(upper_bound);

		auto b=find(lower_bound),
			e=find(upper_bound);

		if (e == end())
			return false;

		if (b == end())
			b=begin();
		else
		{
			if (c(b->first.value, lower_bound))
				++b;
		}

		auto ep=end();

		while (e != ep)
		{
			*b=*e;
			b->first.value -= subtract;

			++b;
			++e;
		}

		vec_t::erase(b, ep);
		return true;
	}

	//! Make sure that the value for the given key exists

	//! If the given key does not exist, the next lower key's value gets
	//! copied to the key.
	//!
	//! Returns \c false, if the requested key compares less than the
	//! first key in the vector (or the existing vector is empty);
	//! or if the key already exists.
	//!
	//! Otherwise, duplicates the key, and returns \c true.

	bool duplicate(const K &k)
	{
		if (empty())
			return false;

		auto iter=find(k);

		if (iter == end())
			return false;

		if (!c(iter->first.value, k))
			return true;

		auto copy=*iter;
		copy.first.value=k;
		vec_t::insert(++iter, copy);
		return true;
	}

	//! Remove consecutive keys with duplicate values.

	//! Removes consecutive key/value tuples with values that compare
	//! as equal.
	//!
	//! Only remove equal values for adjacent keys.

	void uniq()
	{
		uniq(std::equal_to<T>());
	}

	//! Remove consecutive keys with duplicate values, as determined by the comparator.

	//! \overload
	//!
	//! The template parameter is comparator functor that returns \c true
	//! if two values are equal.

	template<typename comparator>
	void uniq(comparator &&c)
	{
		if (empty())
			return;

		auto p=begin();

		for (auto b=p, e=end(); b != e; ++b)
		{
			if (!c(b->second,p->second))
			{
				if (++p != b)
					*p=*b;
			}
		}

		++p;
		vec_t::erase(p, end());
	}

	//! Insert a new key and value

	//! Returns \c false if the key already exists in the sorted vector,
	//! and the value does not get inserted; otherwise inserts the value,
	//! and returns true.

	template<typename ...Args>
	bool insert_value(const K &lower_bound,
			  Args && ...nonvoid_value)
	{
		auto p=find(lower_bound);
		auto e=end();

		if (p == e)
			p=begin();
		else
		{
			if (!c(p->first.value, lower_bound))
				return false;

			++p;
		}

		vec_t::insert(p, typename
			      vec_t::value_type(sorted_vector_key<K>
						(lower_bound),
						std::forward<Args>
						(nonvoid_value)...));
		return true;
	}

	//! Insert and shift keys

	//! Returns \c false if the given key compares less than the first
	//! value in the sorted vector, or range is 0, otherwise returns
	//! \c true, and:
	//!
	//! 1. Calls duplicate(lower_bound), creating a value for the key, if
	//! it does not exist already.
	//!
	//! 2. Adds 'range' to all keys starting with lower_bound (possibly
	//! the newly-duplicated key).
	//!
	//! 3. Finally inserts the new value for the given key.

	template<typename range_t, typename ...Args>
	bool insert_range(const K &lower_bound,
			  range_t range,
			  Args && ...nonvoid_value)
	{
		if (range < 1)
			return false;

		duplicate(lower_bound);

		auto p=find(lower_bound);

		if (p == end())
			return false;

		auto e=end();

		for (auto b=p; b != e; ++b)
			b->first.value += range;

		vec_t::insert(p, typename
			      vec_t::value_type(sorted_vector_key<K>
						(lower_bound),
						std::forward<Args>
						(nonvoid_value)...));
		return true;
	}

	//! Adjust keys

	//! Add 'diff' to all keys starting on or after the specified
	//! lower_bound.
	//!
	//! Returns \c false if adding 'diff' would result in the first
	//! modified keys getting sorted before its previous value,
	//! \c true otherwise.

	template<typename diff_type>
	bool adjust_keys(const K &lower_bound, diff_type diff)
	{
		auto e=end();
		auto p=find(lower_bound);

		if (p == e)
			p=begin(); // Before the first value, this is ok.

		if (p == e)
			return true; // Must be empty, this is ok.

		// If the given value does not exist, do not modify the
		// preceding value.

		if (c(p->first.value, lower_bound))
			++p;

		if (p == e)
			return true; // everything is before, this is ok.

		// Check to make sure we won't adjust the first value
		// to be lower than the preceding value.
		if (p != begin())
		{
			auto q=p;

			--q;

			if (!c(q->first.value, p->first.value+diff))
				return false;
		}

		while (p != e)
		{
			p->first.value += diff;

			++p;
		}
		return true;
	}
};

#if 0
{
#endif
}
#endif
