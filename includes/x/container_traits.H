/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_container_traits_H
#define x_container_traits_H

#include <string>
#include <deque>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Type of a container

enum container_id {
	unknown_container,
	associative_container,
	sequence_container,
	basic_string_container
};

template<class container_type> class container_traits_common;

//! C++ library container traits

//! \c container_traits<T>::type returns a container_id value giving the
//! container category if T is a container. The container_id
//! \c unknown_container indicates that this is not a known STL container.
//!
//! The supported containers are:
//! std::basic_string, std::deque, std::list, std::vector, std::map,
//! std::multimap, std::set, and std::multiset.
//!
//! All others returns a \c unknown_container.
//!
//! Additionally, \c container_traits<T>::is_constant is \c true or \c false,
//! indicating whether or not type \c T has a const qualifier.
//!
//! \par Container operations
//!
//! The following functions provide a common %interface for inserting
//! a container value into a container:
//!
//! container_traits<T>::reserve(T &container, size_t n) - invokes the
//! container's reserve() method. If this container does not have a reserve()
//! method this function is a noop.
//!
//! reserve() is defined for std::vector and std::basic_string specializations.
//! For other containers, this function does nothing.
//!
//! container_traits<T>::insert_chk(T &container, const T::value_type &value) -
//! invokes the container's insert() method to add the value to the container.
//! Returns \c true or \c false indicating if the new value was inserted.
//! A \c false value may be returned when inserting a new value into a
//! std::map or a std::set that already contains a value with the same key.
//! For all other containers, including sequences, insert_chk() always returns
//! \c true. For sequences types, the new value is added to the end of the
//! sequence.

template<class container_type>
class container_traits : public container_traits_common<container_type> {

public:
	//! Default implementation, this is not a constant

	static constexpr bool is_constant=false;
};


//! Specialization for a const container

template<class container_type>
class container_traits<const container_type>
	: public container_traits_common<container_type> {

public:

	//! This is a constant container

	static constexpr bool is_constant=true;
};

//! Default container category is unknown

//! \see container_traits
//!
template<class container_type>
class container_traits_common {

public:
	//! This is an unknown container

	static constexpr container_id type=unknown_container;
};

//! This is an associative container

//! \see container_traits
//!
class container_traits_associative {

public:
	//! This is an associative container
	static constexpr container_id type=associative_container;
};

//! This is a sequence container

//! \see container_traits
//!
class container_traits_sequence {

public:
	//! This is a sequence container
	static constexpr container_id type=sequence_container;
};

//! A \c basic_string specialization

//! \see container_traits
//!

template<class char_type, class traits_type, class allocator_type>
class container_traits_common<std::basic_string<char_type, traits_type,
						allocator_type> >
{
public:
	//! This is a basic_string

	static constexpr container_id type=basic_string_container;

	//! The insert_chk() implementation

	static bool insert_chk(std::basic_string<char_type, traits_type,
			       allocator_type> &c, char_type ch) noexcept
	{
		c.push_back(ch);
		return true;
	}

	//! The reserve() implementation

	static void reserve(std::basic_string<char_type, traits_type,
			    allocator_type> &c,
			    typename std::basic_string<char_type, traits_type,
			    allocator_type>::size_type s) noexcept
	{
		c.reserve(s);
	}
};

//! A \c std::deque specialization

//! \see container_traits
//!

template <class value_type, class allocator_type >
class container_traits_common<std::deque<value_type, allocator_type> >
	: public container_traits_sequence {
public:
	//! The insert_chk() implementation

	static bool insert_chk(std::deque<value_type, allocator_type> &c,
			       const value_type &value) noexcept
	{
		c.push_back(value);
		return true;
	}

	//! The reserve() implementation

	static void reserve(std::deque<value_type, allocator_type> &c,
			    typename std::deque<value_type,
			    allocator_type>::size_type s) noexcept
	{
	}
};

//! A \c std::list specialization

//! \see container_traits
//!
template <class value_type, class allocator_type >
class container_traits_common<std::list<value_type, allocator_type> >
	: public container_traits_sequence {

public:
	//! The insert_chk() implementation

	static bool insert_chk(std::list<value_type, allocator_type> &c,
			       const value_type &value) noexcept
	{
		c.push_back(value);
		return true;
	}

	//! The reserve() implementation

	static void reserve(std::list<value_type, allocator_type> &c,
			    typename std::list<value_type,
			    allocator_type>::size_type s) noexcept
	{
	}
};

//! A \c std::vector specialization

//! \see container_traits
//!
template <class value_type, class allocator_type >
class container_traits_common<std::vector<value_type, allocator_type> >
	: public container_traits_sequence {
public:
	//! The insert_chk() implementation

	static bool insert_chk(std::vector<value_type, allocator_type> &c,
			       const value_type &value) noexcept
	{
		c.push_back(value);
		return true;
	}

	//! The reserve() implementation

	static void reserve(std::vector<value_type, allocator_type> &c,
			    typename std::vector<value_type,
			    allocator_type>::size_type s) noexcept
	{
		c.reserve(s);
	}
};

//! A \c std::map specialization

//! \see container_traits
//!
template <class key_type, class value_type, class compare_type,
          class allocator_type>
class container_traits_common<std::map<key_type, value_type, compare_type,
				       allocator_type> >
	: public container_traits_associative {
public:
	//! The insert_chk() implementation

	static bool insert_chk(std::map<key_type, value_type, compare_type,
			       allocator_type> &c,
			       const std::pair<key_type, value_type>
			       &value) noexcept
	{
		return c.insert(value).second;
	}

	//! The reserve() implementation

	static void reserve(std::map<key_type, value_type, compare_type,
			    allocator_type> &c,
			    typename std::map<key_type, value_type,
			    compare_type, allocator_type>::size_type s) noexcept
	{
	}
};

//! A \c std::multimap specialization

//! \see container_traits
//!
template <class key_type, class value_type, class compare_type,
          class allocator_type>
class container_traits_common<std::multimap<key_type, value_type, compare_type,
					    allocator_type> >
	: public container_traits_associative {
public:
	//! The insert_chk() implementation

	static bool insert_chk(std::multimap<key_type, value_type, compare_type,
			       allocator_type> &c,
			       const std::pair<key_type, value_type>
			       &value) noexcept
	{
		c.insert(value);
		return true;
	}
	//! The reserve() implementation

	static void reserve(std::multimap<key_type, value_type, compare_type,
			    allocator_type> &c,
			    typename std::multimap<key_type, value_type,
			    compare_type, allocator_type>::size_type s) noexcept
	{
	}
};

//! A \c std::set specialization

//! \see container_traits
//!
template <class key_type, class compare_type, class allocator_type>
class container_traits_common<std::set<key_type, compare_type,
				       allocator_type> >
	: public container_traits_associative {
public:
	//! The insert_chk() implementation

	static bool insert_chk(std::set<key_type, compare_type,
			       allocator_type> &c,
			       const key_type &value) noexcept
	{
		return c.insert(value).second;
	}
	//! The reserve() implementation

	static void reserve(std::set<key_type, compare_type,
			    allocator_type> &c,
			    typename std::set<key_type, compare_type,
			    allocator_type>::size_type s) noexcept
	{
	}
};

//! A \c std::multiset specialization

//! \see container_traits
//!
template <class key_type, class compare_type, class allocator_type>
class container_traits_common<std::multiset<key_type, compare_type,
					    allocator_type> >
	: public container_traits_associative {
public:
	//! The insert_chk() implementation

	static bool insert_chk(std::multiset<key_type, compare_type,
			       allocator_type> &c,
			       const key_type &value) noexcept
	{
		c.insert(value);
		return true;
	}

	//! The reserve() implementation

	static void reserve(std::multiset<key_type, compare_type,
			    allocator_type> &c,
			    typename std::multiset<key_type, compare_type,
			    allocator_type>::size_type s) noexcept
	{
	}
};

//! A \c std::unordered_map specialization

//! \see container_traits
//!
template <class key_type, class value_type, class compare_type,
          class allocator_type>
class container_traits_common<std::unordered_map<key_type, value_type,
						 compare_type,
						 allocator_type> >
	: public container_traits_associative {
public:
	//! The insert_chk() implementation

	static bool insert_chk(std::unordered_map<key_type, value_type,
			       compare_type,
			       allocator_type> &c,
			       const std::pair<key_type, value_type>
			       &value) noexcept
	{
		return c.insert(value).second;
	}

	//! The reserve() implementation

	static void reserve(std::unordered_map<key_type, value_type,
			    compare_type,
			    allocator_type> &c,
			    typename std::unordered_map<key_type, value_type,
			    compare_type, allocator_type>::size_type s) noexcept
	{
	}
};

//! A \c std::unordered_multimap specialization

//! \see container_traits
//!
template <class key_type, class value_type, class compare_type,
          class allocator_type>
class container_traits_common<std::unordered_multimap<key_type, value_type,
						      compare_type,
						      allocator_type> >
	: public container_traits_associative {
public:
	//! The insert_chk() implementation

	static bool insert_chk(std::unordered_multimap<key_type, value_type,
			       compare_type,
			       allocator_type> &c,
			       const std::pair<key_type, value_type>
			       &value) noexcept
	{
		c.insert(value);
		return true;
	}
	//! The reserve() implementation

	static void reserve(std::unordered_multimap<key_type, value_type, compare_type,
			    allocator_type> &c,
			    typename std::unordered_multimap<key_type,
			    value_type,
			    compare_type, allocator_type>::size_type s)
		noexcept
	{
	}
};

//! A \c std::unordered_set specialization

//! \see container_traits
//!
template <class key_type, class compare_type, class allocator_type>
class container_traits_common<std::unordered_set<key_type, compare_type,
				       allocator_type> >
	: public container_traits_associative {
public:
	//! The insert_chk() implementation

	static bool insert_chk(std::unordered_set<key_type, compare_type,
			       allocator_type> &c,
			       const key_type &value) noexcept
	{
		return c.insert(value).second;
	}
	//! The reserve() implementation

	static void reserve(std::unordered_set<key_type, compare_type,
			    allocator_type> &c,
			    typename std::unordered_set<key_type, compare_type,
			    allocator_type>::size_type s) noexcept
	{
	}
};

//! A \c std::unordered_multiset specialization

//! \see container_traits
//!
template <class key_type, class compare_type, class allocator_type>
class container_traits_common<std::unordered_multiset<key_type, compare_type,
					    allocator_type> >
	: public container_traits_associative {
public:
	//! The insert_chk() implementation

	static bool insert_chk(std::unordered_multiset<key_type, compare_type,
			       allocator_type> &c,
			       const key_type &value) noexcept
	{
		c.insert(value);
		return true;
	}

	//! The reserve() implementation

	static void reserve(std::unordered_multiset<key_type, compare_type,
			    allocator_type> &c,
			    typename std::unordered_multiset<key_type,
			    compare_type,
			    allocator_type>::size_type s) noexcept
	{
	}
};

#if 0
{
#endif
}
#endif
