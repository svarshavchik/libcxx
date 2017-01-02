/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_joiniterator_H
#define x_joiniterator_H

#include <iterator>
#include <x/errors.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Degrade a random access iterator to a bidirectional iterator.

//! A \ref joiniterator "joiniterator" over a container that provides random
//! access iterator ends up providing bidirectional iterator only.
//!
//! This is a helper class with a specialization that degrades random access
//! iterator traits to bidirectional iterator traits.

template<typename iterator_category, typename iterator_type>
class join_iterator_type {

public:
	//! The degraded iterator trait class.

	typedef std::iterator<iterator_category,
			      typename std::iterator_traits<
				      iterator_type>::value_type,
			      typename std::iterator_traits<
				      iterator_type>::difference_type,
			      typename std::iterator_traits<
				      iterator_type>::pointer,
			      typename std::iterator_traits<
				      iterator_type>::reference
			      > iterator_t;
};

//! Degrade a random access iterator to a bidirectional iterator.

//! This is a specialization used by
//! \ref joiniterator "joiniterator" over a container that provides random
//! access iterator, but ends up providing bidirectional iterator only.

template<typename iterator_type>
class join_iterator_type<std::random_access_iterator_tag, iterator_type>
	: public join_iterator_type<std::bidirectional_iterator_tag,
				    iterator_type> {
};

//! Define the container's iterator type

//! The template class is a container. Define \c container_iter_t to be
//! the type that's returned by std::begin().

template<typename container_type> class join_container_iterator_type {

public:

	//! Define the type returned by this container's begin().

	typedef decltype(std::begin(*(container_type *)nullptr)) container_iter_t;
};

//! Define the underlying container and iterator.

//! The template parameter is an iterator. Defines \c underlying_container_t
//! to be the type the iterator dereferences. It's presumed to be a container,
//! and define \c underlying_iter_t to be the type returned by the container's
//! begin() method.

template<typename container_container_iter_t>
class join_container_container_iterator_type {

public:

	//! What container_cotnainer_iter_t's operator*() returns.

	typedef decltype(**(container_container_iter_t *)nullptr
			 ) underlying_container_ref_t;

	//! Remove the reference from underlying_container_ref_t.

	typedef typename std::remove_reference<underlying_container_ref_t>
	::type underlying_container_t;

	//! The container's iterator.

	typedef typename join_container_iterator_type<underlying_container_t
						      >::container_iter_t
	underlying_iter_t;
};

//! Define \ref joiniterator "joiniterator's traits.

//! The template parameter is the iterator type parameter to
//! \ref joiniterator "joiniterator".
//!
//! Define \c traits_t to be the traits of the \ref joiniterator "joiniterator".

template<typename container_container_iter_t>
class join_iterator_traits {

public:

	//! The underlying iterator

	typedef typename join_container_container_iterator_type<
	container_container_iter_t>::underlying_iter_t underlying_iter_t;

	//! underlying_iter_t's category.

	typedef typename std::iterator_traits<underlying_iter_t>
	::iterator_category underlying_iter_category_t;

	//! The underlying iterator's traits.

	typedef typename join_iterator_type<underlying_iter_category_t,
					    underlying_iter_t
					    >::iterator_t traits_t;
};

//! A default factory for an ending iterator value

//! Instantiates the ending iterator for the underlying sequence using its
//! default constructor.

template<typename iterator>
class default_ending_iterator_factory {

public:

	//! Use the default constructor

	inline static iterator ending_iterator()
	{
		return iterator();
	}
};

//! A join iterator

//! The template parameter is an iterator over a sequence of containers, and
//! this template produces an iterator over the joined values of the container
//! sequence. The last value in each container is logically followed by the
//! first value in the next container, in the sequence.

template<typename container_iterator_type,
	 typename ending_iterator_factory=default_ending_iterator_factory<
		 typename join_container_container_iterator_type<
			 container_iterator_type>::underlying_iter_t> >
class joiniterator : public join_iterator_traits<container_iterator_type>::traits_t {

	//! Beginning container iterator value
	container_iterator_type iterator_beg;

	//! Ending container iterator value
	container_iterator_type iterator_end;

	//! Current container iterator value
	container_iterator_type iterator_cur;

	//! The iterator type over the values of the containers.
	typedef typename join_container_container_iterator_type<
		container_iterator_type>::underlying_iter_t iterator;

	//! Beginning iterator of the current container
	iterator cur_beg;

	//! Ending iterator of the current container.
	iterator cur_end;

	//! Current iterator value for the current container
	iterator cur_cur;

public:

	//! Declare the traits of this iterator, for \ref refiterator "refiterator".

	typedef typename join_iterator_traits<container_iterator_type
					      >::traits_t iterator_traits;

	//! Default constructor, universal ending iterator value

	//! The default constructor instantiates an iterator value that will
	//! compare equal to any joined sequence's ending iterator value
	joiniterator() : joiniterator(container_iterator_type(), container_iterator_type())
	{
	}

	//! Construct a beginning iterator value

	joiniterator(//! Beginning value of the underlying container sequence
		     container_iterator_type beg,

		     //! Ending value of the underlying container sequence
		     container_iterator_type end) : joiniterator(beg, end, beg)
	{
	}

private:
	//! Internal constructor
	joiniterator(container_iterator_type beg,
		     container_iterator_type end,
		     container_iterator_type cur)
		: iterator_beg(beg), iterator_end(end),
		  iterator_cur(cur),
		  cur_beg(ending_iterator_factory::ending_iterator()),
		  cur_end(ending_iterator_factory::ending_iterator()),
		  cur_cur(ending_iterator_factory::ending_iterator())
	{
		init_next();
	}
public:

	//! Return the value of the ending iterator for this sequence.

	joiniterator<container_iterator_type, ending_iterator_factory> end() 
		const
	{
		return joiniterator<container_iterator_type,
				    ending_iterator_factory>(iterator_beg,
							     iterator_end,
							     iterator_end);
	}

	//! Destructor

	~joiniterator() {}

	//! Iterator operator
	auto operator*() const -> decltype(*cur_cur)
	{
		return *cur_cur;
	}

	//! Iterator operator

	auto operator*() -> decltype(*cur_cur)
	{
		return *cur_cur;
	}

	//! Iterator operator

	joiniterator<container_iterator_type,
		     ending_iterator_factory> &operator++()
	{
		if (iterator_cur == iterator_end)
			null_ptr_deref();

		if (++cur_cur == cur_end)
		{
			++iterator_cur;
			init_next();
		}
		return *this;
	}

	//! Iterator operator

	joiniterator<container_iterator_type,
		     ending_iterator_factory> &operator--()
	{
		while (iterator_cur == iterator_end || cur_cur == cur_beg)
		{
			if (iterator_cur == iterator_beg)
			{
				init_next();
				null_ptr_deref();
			}
			--iterator_cur;

			cur_beg=std::begin(*iterator_cur);
			cur_cur=cur_end=std::end(*iterator_cur);
		}
		--cur_cur;
		return *this;
	}

	//! Iterator operator
	joiniterator<container_iterator_type,
		     ending_iterator_factory> operator++(int)
	{
		joiniterator<container_iterator_type,
			     ending_iterator_factory> cpy=*this;
		operator++();
		return cpy;
	}

	//! Iterator operator
	joiniterator<container_iterator_type,
		     ending_iterator_factory> operator--(int)
	{
		joiniterator<container_iterator_type,
			     ending_iterator_factory> cpy=*this;
		operator--();
		return cpy;
	}

	//! Iterator operator
	bool operator==(const joiniterator<container_iterator_type,
			ending_iterator_factory> &o) const
	{
		bool eof=is_end(), o_eof=o.is_end();

		if (eof && o_eof)
			return true;

		if (eof || o_eof)
			return false;

		return iterator_cur == o.iterator_cur && cur_cur == o.cur_cur;
	}

	//! Iterator operator

	bool operator!=(const joiniterator<container_iterator_type,
			ending_iterator_factory> &o) const
	{
		return !operator==(o);
	}

private:

	//! Whether this iterator is the ending iterator value

	//! \internal
	bool is_end() const
	{
		return iterator_cur == iterator_end;
	}

	//! Advanced the iterator forward, skip any empty containers.

	void init_next()
	{
		while (iterator_cur != iterator_end)
		{
			if ((cur_beg=std::begin(*iterator_cur)) !=
			    (cur_end=std::end(*iterator_cur)))
			{
				cur_cur=cur_beg;
				break;
			}
			++iterator_cur;
		}
	}
};

#if 0
{
#endif
}
#endif

