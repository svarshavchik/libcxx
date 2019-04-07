/*
** Copyright 2014-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakmcguffincontainerobj_H
#define x_weakmcguffincontainerobj_H

#include <x/mcguffincontainerfwd.H>
#include <x/ptrfwd.H>
#include <x/weakptr.H>
#include <x/mpobj.H>
#include <x/refptr_traits.H>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Internal mcguffin object used by \ref mcguffinlist "mcguffinlist", \ref mcguffinmap "mcguffinmap", and \ref mcguffinmultimap "mcguffinmultimap".

//! This specialization is the only defined mcguffincontainerObj.
//! The value template parameter for these three weak mcguffin containers must
//! be an \c INSERT_LIBX_NAMESPACE::ref. This specialization enforces that.
//!
//! The actual value in the underlying weak container is
//! \ref container_element_t "container_element_t".
//!
//! \internal

template<typename ref_type>
class mcguffincontainerObj<ref_type,
			   std::void_t<std::enable_if_t<
				   std::is_base_of_v<
				   ref<typename ref_type::obj_type,
				       typename ref_type::base>, ref_type>>>
			   > : virtual public obj {

 public:
	//! The value \c INSERT_LIBX_NAMESPACE::ref type.

	typedef ref_type ref_t;

	//! The value \c INSERT_LIBX_NAMESPACE::ptr type.
	typedef typename refptr_traits<ref_type>::ptr_t ptr_t;

	//! This specialization.

	typedef mcguffincontainerObj<ref_type> container_t;

	//! Actual weak container value.

	//! This is the actual object that gets stored in the weak container.
	//! (wrapped inside the \ref container_element_t "container_element_t".
	//! A strong reference to this object is owned by the container
	//! value's mcguffin. When the mcguffin goes out of scope and gets
	//! destroyed, the only strong reference to this wrapper object
	//! destroys the weak pointer to this wrapper object that's in the
	//! weak container.

	class wrapperObj : virtual public obj {

	public:

		//! The weak pointer to my own mcguffin.

		//! The weak pointer allows for erase() to be implemented.

		weakptr<ptr<container_t>> container;

		//! Remove this object from its weak container.
		void erase()
		{
			auto p=container.getptr();

			if (!p.null())
				p->erase();
		}

		//! Constructor
		wrapperObj(const ptr<container_t> &containerArg)
			: container(containerArg) {}

		//! Destructor
		~wrapperObj() {}
	};

	//! Thread-safe strong reference to the wrapper object.
	typedef mpobj<ptr<wrapperObj>> wrapper_t;

	//! The wrapper object that's placed in the weak container.

	wrapper_t wrapper;

	//! Remove the only strong reference to the container wrapper object.

	//! This should end up destroying the weak pointer to the wrapper
	//! object in the container.

	void erase()
	{
		typename wrapper_t::lock lock(wrapper);

		*lock=ptr<wrapperObj>();
	}

	//! The strong reference to the value logically inserted into the weak container.
	const ref_t strongref;

	//! Constructor
	mcguffincontainerObj(const ref_t &strongrefArg)
		: strongref(strongrefArg)
	{
	}

	//! Destructor
	~mcguffincontainerObj()
	{
	}

	//! Create a wrapper object for the container.

	//! This creates a wrapper object and saves a strong reference to it.
	//! This is called by the weak mcguffin container's insert().

	ref<wrapperObj> create_wrapper()
	{
		auto w=ref<wrapperObj>::create(ptr<container_t>(this));

		typename wrapper_t::lock lock(wrapper);

		*lock=w;

		return w;
	}

	//! The actual value in the weak mcguffin container's underlying weak container.

	//! Implements getptr() just like a non-mcguffin weak container.
	//! Also implements two new methods.

	class container_element_t {

		//! A weak reference to the wrapper object.

		//! The weak container value behavior comes from this.
		weakptr<ptr<wrapperObj>> p;

	public:

		//! Constructor
		container_element_t(const ptr<wrapperObj> &pArg)
			: p(pArg)
		{
		}

		//! Erase this element from the weak mcguffin container.

		//! Does nothing if this is a zombie, a weak container entry
		//! for the value whose mcguffin is already destroyed, however
		//! the container entry for it cannot be removed until all the
		//! weak container iterators go out of scope and get destroyed.

		void erase() const
		{
			auto q=p.getptr();

			if (!q.null())
				q->erase();
		}

		//! Return the opaque mcguffin for this weak container entry.

		//! A null pointer gets returned when this is a zombie,
		//! a weak container entry
		//! for the value whose mcguffin is already destroyed, however
		//! the container entry for it cannot be removed until all the
		//! weak container iterators go out of scope and get destroyed.

		ptr<obj> mcguffin() const
		{
			return get_internal_mcguffin();
		}

		//! Return the real mcguffin for this value.

		//! \internal
		ptr<container_t> get_internal_mcguffin() const
		{
			ptr<container_t> s;

			auto r=p.getptr();

			if (!r.null())
				s=r->container.getptr();

			return s;
		}

		//! Implement getptr()

		//! Implement getptr() for the weak mcguffin container that
		//! works like the one for the non-mcguffin weak container.
		//!
		//! The inserted value gets returned, not the wrapper object
		//! that's the actual value in the underlying weak container.
		//!
		//! A null pointer gets returned when this is a zombie,
		//! a weak container entry
		//! for the value whose mcguffin is already destroyed, however
		//! the container entry for it cannot be removed until all the
		//! weak container iterators go out of scope and get destroyed.
		ptr_t getptr() const
		{
			ptr_t q;

			auto s=get_internal_mcguffin();

			if (!s.null())
				q=s->strongref;

			return q;
		}
	};
};

#if 0
{
#endif
}
#endif
