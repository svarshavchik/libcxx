/*
** Copyright 2020-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sorted_range_H
#define x_sorted_range_H

#include <vector>
#include <algorithm>
#include <functional>
#include <vector>
#include <tuple>
#include <exception>
#include <type_traits>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Default updater for a \ref sorted_range "sorted range".

struct default_sorted_range_update {

	template<typename T> inline void operator()(const T &) {}
};

//! Sorted ranges

//! Maintains a sorted container, a vector, of [begin, end) values. The
//! first template parameter is a type that has two members, begin and end.
//!
//! The second template parameter optionally defines a three-way comparator
//! that implements strong ordering of begin and end values.
//!
//! The third template parameter optionally defines an updater class.
//!
//! The fourth optional
//! template parameter defines the underlying container, a vector by default.
//!
//! [begin, end) specifies a range of value starting with "begin" and up to
//! but not including "end". add() adds a new range to the sorted list,
//! eliminating any overlapping ranges and merging adjacent ranges
//! automatically, as needed. remove() removes a range from the
//! sorted list, handling overlapping of the specified range and existing
//! ranges. An overloaded << operator adds another range into this range.
//!
//! Non-mutable members of std::vector are inherited.
//!
//! \note
//! remove() may cause reallocation of the underlying vector.
//!
//! \par Updater class
//!
//! An optional updater class implements an overloaded () operator that
//! gets called to report the actual changes to the sorted ranges. This
//! template handles the "begin" and "end" values and ignores anything else
//! in the range class, this provides the means to keep it updated.
//!
//! The updater class's operator() receives the following objects:
//!
//! * \c sorted_ranges::combined - an existing sorted range was added vt
//! combining it with the new add()ed range, or two ranges get combined
//! into a single range, combining overlapping range.
//!
//! * \c sorted_range::removed - this existing range gets removed from
//! the list entirely.
//!
//! * \c sorted_range::removed_partially - this existing range's begin or
//! end values were adjusted, but not removed from the list.
//!
//! * \c sorted_range::split - an existing range in the list was split into
//! two separate ranges.
//!
//! \par Modifiers
//!
//! extract() takes a single range as a parameter and returns a new sorted_range
//! containing only the overlapping ranges between the given range and the
//! original sorted range list.
//!
//! shift() takes a value and adds it to each range's begin and end values.
//!
//! range() returns a tuple with the entire range, the begin value of the
//! first range and the end value of the last range. If the sorted range is
//! empty range() returns default-constructed begin and end values.

template<typename T,
	 typename comparator=std::compare_three_way,
	 typename updater=default_sorted_range_update,
	 typename container=std::vector<T>>
class sorted_range : private container {

public:

	//! This container type.
	typedef sorted_range<T, comparator, updater, container> container_t;

	//! Update callback object.
	struct combined {
		//! Range that remains in the sorted range list.
		T &a;

		//! Secondary range

		//! This is the range that's not getting added to the
		//! sorted list, or is getting removed from the list.
		const T &b;
	};

	//! Update callback object.
	struct removed {
		T &a;
	};

	//! Update callback object.
	struct removed_partially {
		T &a;
	};

	struct split {
		//! Existing range, ending value modified
		T &a;

		//! 2nd range, a new range

		//! This range was copied from the existing range, then its
		//! beginning value was modified.

		T &b;
	};

	//! The comparator instance
	comparator c;

	//! The updater instance
	updater u;

	//! Default constructor
	sorted_range()=default;

	//! Construct with a comparator instance.
	template<typename C>
	sorted_range(C &&c) : c{std::forward<C>(c)}
	{
	}

	//! Construct with a comparator and updater instance
	template<typename C, typename U>
	sorted_range(C &&c,
		     U &&u) : c{std::forward<C>(c)},
			      u{std::forward<U>(u)}
	{
	}

	using container::value_type;
	using container::size_type;
	using container::difference_type;

	using container::const_iterator;
	using container::const_reference;
	using container::const_pointer;
	using container::const_reverse_iterator;

	typedef container::const_iterator iterator;
	typedef container::const_reference reference;
	typedef container::const_pointer pointer;
	typedef container::const_reverse_iterator reverse_iterator;

	//! Add a range.

	//! Returns an iterator with the added range.

	template<typename V>
	container::const_iterator add(V &&v)
	{
		return add_internally(std::forward<V>(v));
	}

	//! Add a range

	//! \overload
	container::const_iterator add(const T&v)
	{
		return add_internally(v);
	}

	//! Add another range to this one.

	template<typename V>
	auto &operator<<(V &&v)
	{
		auto b=v.begin(), e=v.end();

		// If we're adding a new range at the end, inserting
		// in reverse order should micro-optimize the repeated
		// invocations of upper_bound.

		while (b != e)
			add_internally(*--e);
		return *this;
	}
private:

	//! Internal implementation that returns a mutable iterator.

	template<typename V>
	std::vector<T>::iterator add_internally(V &&v)
	{
		auto res=c(v.begin, v.end);

		if (res == std::strong_ordering::equal)
			return container::end();

		if (res == std::strong_ordering::greater)
			// begin cannot be > end
			throw std::invalid_argument{"end less than begin"};

		auto b=container::begin();
		auto e=container::end();

		auto iter=std::upper_bound
			(b, e, v,
			 [&](const T &a, const T &b)
			 {
				 return c(a.begin, b.begin)
					 == std::strong_ordering::less;
			 });

		if (iter == b ||
		    // v.begin > iter[-1].end
		    c(v.begin, iter[-1].end)
		    == std::strong_ordering::greater)
			// Completely new range, either starts before the
			// first range or after the end of the preceding
			// range.
			iter=this->insert(iter, std::forward<V>(v));
		else
		{
			--iter;

			u(combined{*iter, v});

			// v.end <= iter->end
			if (c(v.end,iter->end)
			    != std::strong_ordering::greater)
				 // Completely enclosed by the preceding range.
			{
				return iter;
			}
		}

		iter->end=v.end;

		++iter;

		while (iter != this->end() &&

		       // iter[-1].end >= iter->begin
		       c(iter[-1].end,iter->begin)
		       != std::strong_ordering::less)
		{
			// merge ranges.

			// iter[-1].end < iter->end
			if (c(iter[-1].end, iter->end)
			    == std::strong_ordering::less)
				iter[-1].end=iter->end;

			u(combined{iter[-1], *iter});

			iter=this->erase(iter);
		}
		return --iter;
	}
public:
	//! Remove an existing range.

	void remove(const auto &v)
	{
		if (c(v.begin, v.end) ==
		    std::strong_ordering::equal)
			return;

		// We add this range, and then we only need to handle the
		// case where we guarantee the range exists.

		auto iter=add_internally(v);

		if (c(iter->begin, v.begin) ==
		    std::strong_ordering::equal)
		{
			if (c(iter->end, v.end) ==
			    std::strong_ordering::equal)
			{
				u(removed{*iter});

				// Removing entire range.
				this->erase(iter);
				return;
			}

			// Removing the beginning of this range.
			iter->begin=v.end;
			u(removed_partially{*iter});
			return;
		}

		if (iter->end == v.end)
		{
			// begin not the same, end is the same, removing
			// the end of the range.
			iter->end=v.begin;
			u(removed_partially{*iter});
			return;
		}

		// Removing from the middle of the range.

		// Create the remaining end.
		auto cpy= *iter;

		cpy.begin=v.end;

		// Make the existing range he remaining beginning.
		iter->end=v.begin;

		// Add the remaining end.
		iter=this->insert(++iter, cpy);
		u(split{iter[-1], *iter});
	}

	using container::cbegin;
	using container::cend;

	inline constexpr auto begin() const noexcept
	{
		return this->cbegin();
	}

	inline constexpr auto end() const noexcept
	{
		return this->cend();
	}

	inline constexpr container::const_reference
	at(container::size_type pos) const
	{
		return container::at(pos);
	}

	inline constexpr container::const_reference
	operator[](container::size_type pos) const
	{
		return container::operator[](pos);
	}

	inline constexpr container::const_reference front() const
	{
		return container::front();
	}

	inline constexpr container::const_reference back() const
	{
		return container::back();
	}

	inline constexpr const T *data() const
	{
		return container::data();
	}

	using container::empty;
	using container::size;
	using container::max_size;
	using container::reserve;
	using container::shrink_to_fit;
	using container::clear;

	friend auto operator<=>(const container_t &a,
				const container_t &b)
	{
		const container &av=a, &bv=b;

		return av <=> bv;
	}

	friend auto operator==(const container_t &a,
			       const container_t &b)
	{
		const container &av=a, &bv=b;

		return av == bv;
	}

	friend auto operator!=(const container_t &a,
			       const container_t &b)
	{
		return !operator==(a, b);
	}

	//! Extract all matching ranges.

	container_t extract(const T &v) const
	{
		container_t r;

		auto b=container::begin();
		auto e=container::end();

		auto iter=std::upper_bound
			(b, e, v,
			 [&](const T &a, const T &b)
			 {
				 return c(a.begin, b.begin)
					 == std::strong_ordering::less;
			 });

		if (iter != b)
			--iter;
		for (; iter != e; ++iter)
		{
			// !(iter->begin < v.end) a.k.a. iter->begin >= v.end
			// there won't be any more.
			if (!(c(iter->begin, v.end)
			      == std::strong_ordering::less))
				break;

			// !(iter->end > v.begin) a.k.a. iter->end <= v.begin,
			// not just yet
			if (!(c(iter->end, v.begin)
			      == std::strong_ordering::greater))
				continue;

			// Truncate the range.
			r.push_back({c(iter->begin, v.begin) ==
					std::strong_ordering::less
					? v.begin: iter->begin,
					c(iter->end, v.end) ==
					std::strong_ordering::less
					? iter->end: v.end});
		}

		return r;
	}

	//! Shift the range in the sorted container

	template<typename V>
	void shift(const V &v)
	{
		for (auto &r: static_cast<container &>(*this))
		{
			r.begin += v;
			r.end += v;
		}
	}

	//! Return the entire range

	auto range() const
	{
		typedef decltype(std::declval<T &>().begin) begin_t;
		typedef decltype(std::declval<T &>().end) end_t;

		if (empty())
			return std::tuple{ begin_t{}, end_t{} };

		return std::tuple{front().begin, back().end};
	}
};

#if 0
{
#endif
}
#endif
