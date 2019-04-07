/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sequencenode_H
#define x_sequencenode_H

#include <x/namespace.h>
#include <x/yaml/node.H>
#include <x/yaml/document.H>
#include <x/yaml/docnode.H>
#include <x/yaml/sequencenodefwd.H>
#include <x/yaml/sequencenodeobj.H>
#include <x/ref.H>
#include <x/ptr.H>

#include <iterator>
#include <cstddef>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! Base class for parsed YAML sequence

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::yaml::sequence::base

class sequencenodeBase : public ptrref_base {

public:

	//! The iterator returned by \ref sequencenodeObj::begin "begin"() and \ref sequencenodeObj::end "end"().

	typedef sequencenodeObj::iterator iterator;
};

template<typename functor_type>
void sequencenodeObj::for_each(functor_type &&functor)
{
	for (std::ptrdiff_t i=0; ; ++i)
	{
		yaml_node_item_t item=({
				documentObj::readlock_t
					lock(this->n->doc->doc);

				auto ptr=this->n->getNode(lock);

				if (i >= ptr->data.sequence.items.top -
				    ptr->data.sequence.items.start)
					break;

				ptr->data.sequence.items.start[i];
			});
		functor(this->n->doc->construct(item));
	}
}

//! An iterator over the contents of a sequence

//! This is an input iterator, except that it also satisfies most of the
//! requirements of a random access iterator. It's tagged as an input iterator
//! because it fails to meet some of the requirements for a random access
//! iterator.
//!
//! Refer to this type as \c INSERT_LIBX_NAMESPACE::yaml::sequence::base::iterator

class sequencenodeObj::iterator
	: public std::iterator<std::input_iterator_tag, node> {

	//! The underlying sequence

	const_sequencenodeptr seq;

	//! Iterator value
	size_t i;

public:

	//! Uninitialized iterator constructor
	iterator();

	//! Constructor used by begin() and end()
	iterator(const_sequencenodeptr &&seqArg, size_t iArg);

	//! Destructor
	~iterator();

	//! Iterator comparator
	bool operator==(const iterator &o) const
	{
		return i == o.i &&
			// Extra credit
			!seq.null() && !o.seq.null() && &*seq == &*o.seq;
	}

	//! Iterator comparator
	bool operator!=(const iterator &o) const
	{
		return !operator==(o);
	}

	//! Iterator operator
	node operator*() const
	{
		return seq->get(i);
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
	node operator[](std::ptrdiff_t n) const
	{
		return seq->get(i+n);
	}

	//! Iterator comparator
	bool operator<(const iterator &o) const
	{
		return i < o.i &&
			// Extra credit
			!seq.null() && !o.seq.null() && &*seq == &*o.seq;
	}

	//! Iterator comparator
	bool operator<=(const iterator &o) const
	{
		return i <= o.i &&
			// Extra credit
			!seq.null() && !o.seq.null() && &*seq == &*o.seq;
	}

	//! Iterator comparator
	bool operator>(const iterator &o) const
	{
		return i > o.i &&
			// Extra credit
			!seq.null() && !o.seq.null() && &*seq == &*o.seq;
	}

	//! Iterator comparator
	bool operator>=(const iterator &o) const
	{
		return i >= o.i &&
			// Extra credit
			!seq.null() && !o.seq.null() && &*seq == &*o.seq;
	}
};

//! Iterator operator

inline sequencenodeObj::iterator operator+(std::ptrdiff_t n,
					   const sequencenodeObj::iterator &i)
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
