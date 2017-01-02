/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mappingnode_H
#define x_mappingnode_H

#include <x/namespace.h>
#include <x/yaml/mappingnodefwd.H>
#include <x/yaml/mappingnodeobj.H>
#include <x/yaml/document.H>
#include <x/yaml/docnode.H>
#include <x/yaml/node.H>
#include <x/ref.H>
#include <x/ptr.H>

#include <cstddef>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! Base class for parsed YAML mapping

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::yaml::mapping::base

class mappingnodeBase : public ptrrefBase {

public:

	//! The iterator returned by \ref mappingnodeObj::begin "begin"() and \ref mappingnodeObj::end "end"().

	typedef mappingnodeObj::iterator iterator;
};

template<typename functor_type>
void mappingnodeObj::for_each(functor_type &&functor)
{
	for (std::ptrdiff_t i=0; ; ++i)
	{
		yaml_node_pair_t pair=({
				documentObj::readlock_t
					lock(this->n->doc->doc);

				auto ptr=this->n->getNode(lock);

				if (i >= ptr->data.mapping.pairs.top -
				    ptr->data.mapping.pairs.start)
					break;

				ptr->data.mapping.pairs.start[i];
			});
		functor(this->n->doc->construct(pair.key),
			this->n->doc->construct(pair.value));
	}
}

//! An iterator over the contents of a mapping

//! This is an input iterator, except that it also satisfies most of the
//! requirements of a random access iterator. It's tagged as an input iterator
//! because it fails to meet some of the requirements for a random access
//! iterator.

class mappingnodeObj::iterator
	: public std::iterator<std::input_iterator_tag,
			       std::pair<node, node>> {

	//! The underlying mapping

	const_mappingnodeptr map;

	//! Iterator value
	size_t i;

public:

	//! Uninitialized iterator constructor
	iterator();

	//! Constructor used by begin() and end()
	iterator(const_mappingnodeptr &&mapArg, size_t iArg);

	//! Destructor
	~iterator();

	//! Iterator comparator
	bool operator==(const iterator &o) const
	{
		return i == o.i &&
			// Extra credit
			!map.null() && !o.map.null() && &*map == &*o.map;
	}

	//! Iterator comparator
	bool operator!=(const iterator &o) const
	{
		return !operator==(o);
	}

	//! Iterator operator
	std::pair<node, node> operator*() const
	{
		return map->get(i);
	}

	//! Iterator operator
	iterator &operator++()
	{
		++i;
		return *this;
	}

	//! Iterator operator
	iterator operator++(int)
	{
		iterator cpy=*this;

		++i;
		return cpy;
	}

	//! Iterator operator
	iterator &operator--()
	{
		--i;
		return *this;
	}

	//! Iterator operator
	iterator operator--(int)
	{
		iterator cpy=*this;

		--i;
		return cpy;
	}

	//! Iterator operator
	iterator &operator+=(std::ptrdiff_t n)
	{
		i += n;
		return *this;
	}

	//! Iterator operator
	iterator &operator-=(std::ptrdiff_t n)
	{
		i -= n;
		return *this;
	}

	//! Iterator operator
	iterator operator+(std::ptrdiff_t n) const
	{
		iterator cpy=*this;

		cpy += n;
		return cpy;
	}

	//! Iterator operator
	iterator operator-(std::ptrdiff_t n) const
	{
		iterator cpy=*this;

		cpy -= n;
		return cpy;
	}

	//! Iterator operator
	std::ptrdiff_t operator-(const iterator &o) const
	{
		return i - o.i;
	}

	//! Iterator operator
	std::pair<node, node> operator[](std::ptrdiff_t n) const
	{
		return map->get(i+n);
	}

	//! Iterator comparator
	bool operator<(const iterator &o) const
	{
		return i < o.i &&
			// Extra credit
			!map.null() && !o.map.null() && &*map == &*o.map;
	}

	//! Iterator comparator
	bool operator<=(const iterator &o) const
	{
		return i <= o.i &&
			// Extra credit
			!map.null() && !o.map.null() && &*map == &*o.map;
	}

	//! Iterator comparator
	bool operator>(const iterator &o) const
	{
		return i > o.i &&
			// Extra credit
			!map.null() && !o.map.null() && &*map == &*o.map;
	}

	//! Iterator comparator
	bool operator>=(const iterator &o) const
	{
		return i >= o.i &&
			// Extra credit
			!map.null() && !o.map.null() && &*map == &*o.map;
	}
};

//! Iterator operator

inline mappingnodeObj::iterator operator+(std::ptrdiff_t n,
					  const mappingnodeObj::iterator &i)
{
	return i + n;
}

#if 0
{
	{
#endif
	}
}
#endif
