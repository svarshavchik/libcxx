/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakcontainerobj_H
#define x_weakcontainerobj_H

#include <x/obj.H>
#include <x/weakptr.H>
#include <x/ondestroy.H>
#include <x/exception.H>
#include <x/sipobj.H>
#include <x/mpobj.H>
#include <x/namespace.h>
#include <iterator>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#ifndef LIBCXX_WEAKDEBUG_HOOK
#define LIBCXX_WEAKDEBUG_HOOK(x) do { } while(0)
#endif

//! The weak container iterator

//! The \ref weakContainerObj "container" is inside another somewhat-important object.
//!
//! Iterators keep a read lock on the iterator lock. When a weak object
//! gets destroyed, the destructor callback tries a write lock.
//! If it succeeds, there are no read iterators, so the weak pointer
//! can be removed from the container, otherwise it gets placed into
//! the pending delete list.
//!
//! When a read iterator goes out of scope and releases its read lock,
//! it also tries to acquire a write lock. If it succeeds, it was the
//! last open read iterator, and it purges the pending delete list.

template<typename container_t>
class weak_container_iterator_t {

public:
	//! A read lock on iterator_t allows a write lock on the container

	mutable container_t container;

	//! Forward the constructor

	template<typename ...Args>
	weak_container_iterator_t(Args && ...args)
		: container(std::forward<Args>(args)...) {}
	~weak_container_iterator_t()=default;
};

//! Weak container implementation

//! This template instantiates a container of weak references to objects.
//!
//! The first template parameter must be a ptr<obj> type, a reference to
//! an object.
//!
//! The second template is an STL container, and the third template is an
//! STL allocator.
//!
//! The second template should be an STL container that stores weak references
//! to the first template parameter. Currently, implementations can be
//! std::list<weakptr<T>, Allocator>, std::map<key, weakptr<T>, Compare,
//! Allocator> and std::multimap<key, weakptr<T>, Compare, Allocator>.

template<typename T, typename C>
class weakContainerObj : virtual public obj {

protected:

	//! The container is a somewhat-important object.

	typedef sipobj<C> container_t;

	//! See https://bugzilla.redhat.com/show_bug.cgi?id=1632063

	using iterator_t=weak_container_iterator_t<container_t>;

	//! Container of an iterator lock.

	typedef sipobj<iterator_t> iterator_container_t;

	//! The iterator lock.

	iterator_container_t iterator_container;

	//! A container for pending deletes.

	//! Note that the pending delete container must always be locked before
	//! the iterator container.

	typedef mpobj<std::list<typename C::iterator> > pendeletes_t;

	//! A list of weak references pending deletion

	mutable pendeletes_t pendeletes;

	//! A helper cleanup object

	//! This is a destructor callback that removes the weak reference
	//! completely from the iterator, when the object is destroyed after
	//! all strong references go out of scope.

	class cleanupObj : virtual public obj {

	public:
		//! The type of the reference back to my container.

		typedef ref< weakContainerObj<T, C> > container_ref_t;

		//! A reference back to my container

		container_ref_t container_ref;

		//! The iterator in the container

		typename C::iterator iter;

		//! Whether the iterator has been installed

		bool installed;

		//! Default constructor
		cleanupObj(weakContainerObj<T, C> *p)
			: container_ref(p)
		{
		}

		//! Default destructor
		~cleanupObj() {}

		//! Called when this iterator object is fully initialized.

		void install(typename C::iterator && iterArg)
		{
			iter=iterArg;
			installed=true;
		}

		//! The object is destroyed

		//! All strong references to this object are out of scope,
		//! remove the weak reference.
		void destroyed()
		{
			if (!installed)
				return;

			// If there are no read iterators, purge me.

			typename pendeletes_t::lock
				pendinglock(container_ref->pendeletes);

			typename iterator_container_t::writelock
				lock(container_ref->iterator_container,
				     std::try_to_lock_t());

			pendinglock->push_back(iter);

			if (lock.locked())
			{
				container_ref->remove_postponed_deletes
					(pendinglock, lock);
			}
			else
			{
				LIBCXX_WEAKDEBUG_HOOK("Placed on the pending list");
			}
		}
	};

	//! Base class of the iterator read lock.

	//! Iterators maintain a reference to this readlock, as long as they
	//! point to some reference in the container.
	//!
	//! The subclass contains the actual read lock. When this read lock
	//! goes out of scope and the object gets destroyed, the subclass
	//! gets destroyed first, and it releases the read lock. Then,
	//! this object gets destroyed, which triggers deletion of destroyed
	//! iterators.

	class rlockBaseObj : virtual public obj {

	protected:
		//! Our container

		ref<weakContainerObj<T, C> > wc;

		//! The constructor takes a read lock on the container, which constructs the rlockObj by passing this.

		rlockBaseObj(const weakContainerObj<T, C> *wcArg)
			: wc(const_cast<weakContainerObj<T, C> *>
			     (wcArg))
		{
		}

		//! The destructor invokes remove_pending_deletes()

		~rlockBaseObj()
		{
			// If there are no read iterators, purge me.

			typename pendeletes_t::lock
				pendinglock(wc->pendeletes);

			typename iterator_container_t::writelock
				lock(wc->iterator_container,
				     std::try_to_lock_t());

			if (lock.locked())
				wc->remove_postponed_deletes(pendinglock, lock);
		}
	};

	//! The iterator read lock subclass

	//! This subclass maintains the actual read lock. The superclass's
	//! destruction triggers pending deletes cleanup.

	class rlockObj : public rlockBaseObj,

			 //! The iterator lock

			 iterator_container_t::readlock {
	public:
		//! The constructor takes a read lock on the container, which constructs the rlockObj by passing this.

		rlockObj(const weakContainerObj<T, C> *wcArg)
			: rlockBaseObj(wcArg),
			  iterator_container_t
			  ::readlock(this->wc->iterator_container)
		{
		}

		//! Syntactical sugar

		container_t &container()
		{
			return iterator_container_t::readlock::operator->()
				->container;
		}

		//! The destructor releases the lock

		~rlockObj()
		{
		}
	};

	//! Delete postponed destroyed weak pointers

	//! While iterator locks were being held, destroyed weak pointers could
	//! not be removed from the container. They can be, now.

	void remove_postponed_deletes(typename pendeletes_t::lock &pendinglock,
				      typename iterator_container_t::writelock
				      &ilock)
	{
		typename container_t::writelock lock(ilock->container);

		while (!pendinglock->empty())
		{
			LIBCXX_WEAKDEBUG_HOOK("Destroyed weak reference");
			lock->erase(pendinglock->front());
			pendinglock->pop_front();
		}
	}

	//! A read lock on this container

	typedef ptr<rlockObj> rlock;

public:
	friend class cleanupObj;

	//! The type representing the size of the container

	typedef typename C::size_type size_type;

	//! The type representing the contents of the container

	typedef typename C::value_type value_type;

protected:
	//! Construct the container, initially empty.
	template<typename ...Args>
	explicit weakContainerObj(Args && ...args) noexcept
		: iterator_container(std::forward<Args>(args)...)
	{
	}
public:
	//! Default destructor
	~weakContainerObj()
	{
	}

	//! Return the container's STL allocator

	auto get_allocator() const noexcept
	{
		rlock r=rlock::create(this);

		return typename container_t::readlock(r->container())
			->get_allocator();
	}

	//! An iterator over the weak container

	class iterator {

		//! The read lock on the container

		rlock r;

		//! The underlying container

		ptr< weakContainerObj<T, C> > container_ref;

		//! The container iterator
		typename C::const_iterator iter;

	public:

		//! Iterator trait
		typedef std::input_iterator_tag iterator_category;

		//! Iterator trait
		typedef C::iterator::value_type value_type;

		//! Iterator trait
		typedef std::ptrdiff_t difference_type;

		//! Iterator trait
		typedef value_type *pointer;

		//! Iterator trait
		typedef value_type &reference;

		friend class weakContainerObj<T, C>;

		//! Construct an empty iterator

		iterator() noexcept {}

		//! Default destructor
		~iterator()
		{
		}

		//! Compare two iterators

		bool operator==(const iterator &o) const noexcept
		{
			return container_ref == o.container_ref &&
				iter == o.iter;
		}

		//! Compare two iterators

		bool operator!=(const iterator &o) const noexcept
		{
			return !operator==(o);
		}

		//! The * operator

		const typename C::const_iterator::value_type &operator*()
			const
		{
			*container_ref; // Check for null container
			return *iter;
		}

		//! The -> operator

		const typename C::const_iterator::value_type *operator->()
			const
		{
			return &operator*();
		}

		//! Increment this iterator

		//! If reached the end of the container, the read lock gets
		//! released.

		iterator &operator++() noexcept
		{
			{
				typedef typename weakContainerObj<T, C>
					::container_t::readlock readlock_t;

				readlock_t lock(r->container());

				if (++iter != lock->end())
					return *this;
			}

			*this=iterator();
			return *this;
		}

		//! Post-increment the iterator

		iterator operator++(int) noexcept
		{
			iterator c(*this);

			++*this;

			return c;
		}
	};

	//! Return the beginning iterator

	iterator begin() const
	{
		auto lock=rlock::create(this);

		auto iter=typename container_t::readlock(lock->container())
			->begin();
		// Sequence required here, to release the readlock, because
		// another one gets acquired in begin().

		return this->begin(lock, iter);
	}

protected:
	//! Convert a native container iterator to the weak container iterator

	iterator begin(//! Acquired lock on this weak container
		       const rlock &r,

		       //! The native iterator
		       typename C::const_iterator iter)
		const
	{
		iterator i;

		i.iter=iter;

		if (i.iter == typename container_t::readlock(r->container())
		    ->end())
			return end();

		i.container_ref=const_cast<weakContainerObj<T, C> *>(this);

		i.r=r;

		return i;
	}
public:
	//! Return the ending iterator

	static iterator end()
	{
		return iterator();
	}

	//! Check if the container is empty

	bool empty() const
	{
		rlock r=rlock::create(this);

		return typename container_t::readlock(r->container())->empty();
	}

	//! Return the size of the container

	size_type size() const
	{
		rlock r=rlock::create(this);

		return typename container_t::readlock(r->container())->size();
	}
};

#if 0
{
#endif
}
#endif
