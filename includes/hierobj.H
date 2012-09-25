/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_hierobj_H
#define x_hierobj_H

#include <x/sipobj.H>
#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/weakmap.H>
#include <x/namespace.h>
#include <iterator>
#include <list>
#include <map>
#include <set>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Helper class that tests that the second template parameter to hierObj is a ref.

template<typename node_type> class hierObjNodeTypeMustBeRef;

//! Helper class that tests that the second template parameter to hierObj is a ref.

template<typename objClass_t, typename baseClass_t>
class hierObjNodeTypeMustBeRef< ref<objClass_t, baseClass_t> > {

public:

	//! Alias for the type of the reference-counted class whose ref is the second template parameter to hierObj

	typedef objClass_t objClass;

	//! Alias for the base class the ref that is the second template parameter to hierObj

	typedef baseClass_t baseClass;
};

//! Throw an exception

void cant_clone_writelock() __attribute__((noreturn));

//! The reference-counted \ref hier implementation.

//! Although many of the class members are public, the actual container,
//! is private, and is only accessible by insert(), erase() and search().
//!
//! Additionally, create_readlock() and create_writelock() instantiate a lock
//! object, for more fine-grained access.
//!
//! These methods comprise the public interface to this class.
//! \see hier

template<typename hier_type, typename node_type>
class hierObj : virtual public obj {

	//! The node_type template class parameter must be a ref.

	//! A compile error here means that the second argument to this
	//! template function is not a
	//! \ref ref "INSERT_LIBX_NAMESPACE::ref".

	typedef typename hierObjNodeTypeMustBeRef<node_type>::objClass objClass;

	//! The node_type template class parameter must be a ref.

	//! A compile error here means that the second argument to this
	//! template function is not a
	//! \ref ref "INSERT_LIBX_NAMESPACE::ref".

	typedef typename hierObjNodeTypeMustBeRef<node_type>
	::baseClass baseClass;

	//! Define a typedef to this template class.

	typedef hierObj<hier_type, node_type> this_t;

	//! A reference to this class
	typedef ref<this_t> thisref;

protected:
	class elemObj;

	//! A reference to a wrapper for an entry in the hierarchical tree
	typedef ref<elemObj> elem;

	//! A nullable pointer reference to a wrapper for an entry in the hierarchical tree
	typedef ptr<elemObj> elemptr;

	//! A container for all inferior nodes in the hierarchy, of a given node.

	//! It's just a map.
	typedef std::map<hier_type, elem> childnodes_map_t;

	//! A container for all entries that are currently in the hierarchy
	typedef std::map<std::list<hier_type>, elem> allnodes_map_t;

	//! A wrapper for an entry in the hierarchy.

	class elemObj: virtual public obj {

	public:
		//! The parent node.

		//! The root node has a nullptr here.
		elemObj *parent;

		//! This node in parent->childnodes_map.
		typename childnodes_map_t::iterator parent_entry;

		//! Any children nodes
		childnodes_map_t childnodes_map;

		//! The entry for this node, if it is defined
		ptr<objClass, baseClass> entry;

		//! If entry is not NULL, this node's entry in allnodes_map_t;
		typename allnodes_map_t::iterator allnodes_entry;

		elemObj() : parent(nullptr) {}

		~elemObj() noexcept {}

		//! Add an inferior node

		//! Adds the inferior node to childnodes_map, sets the new
		//! child's parent pointer.

		void newchild(const hier_type &name,
			      const elem &child)
		{
			child->parent=this;
			child->parent_entry=
				childnodes_map.insert(std::make_pair(name,
								     child))
				.first;
		}

		//! Install an entry in this node in the hierarchy

		//! Can only be called if entry is currently null. Inserts
		//! This wrapper into allnodes, saves the iterator in
		//! allnodes_entry.

		void setentry(const std::list<hier_type> &myname,
			      allnodes_map_t &allnodes,
			      const ref<objClass, baseClass> &newentry)
		{
			entry=newentry;
			try {
				allnodes_entry=allnodes
					.insert(std::make_pair(myname,
							       elem(this)))
					.first;
			} catch (...) {
				entry=ptr<objClass, baseClass>();
				throw;
			}
		}
	};

	//! And here's the entire hierarchy
	class tree {

	public:
		//! The dummy root node
		elem rootnode;

		//! All the entries that exist in the hierarchy
		allnodes_map_t allnodes;

		tree() : rootnode(elem::create())
		{
		}
		~tree() noexcept {}
	};

	//! A container for the hierarchy tree.
	typedef sipobj<tree> container_t;

	//! The hierarchy tree
	container_t container;
public:
	hierObj() {}
	~hierObj() noexcept {}

protected:

	//! Base class for a read or a writer lock.

	//! container_lockbaseObj holds a reference to the container hierarchy,
	//! the current element in the hierarchy, and its name.

	class container_lockbaseObj : virtual public obj {

	public:
		//! The hierObj that's locked
		thisref h;

		//! Current element
		elem node;

		//! Current element's name, if it exists
		std::list<hier_type> name;

		//! Constructor
		container_lockbaseObj(thisref &&refArg,
				      const elem &nodeArg)
			: h(std::move(refArg)),
			  node(nodeArg) {}

		//! Clone this lock, implemented in the subclass

		virtual ref<container_lockbaseObj> clone_lock()=0;

		//! Copy constructor used to clone a lock object
		container_lockbaseObj(const container_lockbaseObj &o)
			: h(o.h), node(o.node), name(o.name)
		{
		}

		~container_lockbaseObj() noexcept {}
	};

	//! container_t::readlock or container_t::writelock lives here.

	template<typename lock_t>
	class container_lockObj : virtual public obj {

	public:

		//! Either a \c readlock or a \c writelock
		lock_t lockval;

		//! Constructor
		container_lockObj(thisref &&refArg)
			: lockval(refArg->container) {}

		//! Destructor
		~container_lockObj() noexcept {}
	};

	//! A superclass of container_nodeObj with a reference to container_lockObj

	//! container_nodeObj needs to construct the lock object before
	//! it constructs container_lockbaseObj, which refers to stuff that's
	//! protected by container_lockObj's lock. It subclasses this first,
	//! which gets constructed before container_lockbaseObj.

	template<typename lock_t>
	class refcontainer_lockObj {

	protected:

		//! The container lock

		ref<container_lockObj<lock_t> > container_lock;

	public:
		//! Constructor
		refcontainer_lockObj(thisref &&refArg)
			: container_lock(ref< container_lockObj<lock_t> >
					 ::create(std::move(refArg)))
		{
		}

		//! Copy constructor

		refcontainer_lockObj(const refcontainer_lockObj<lock_t> &o)
			: container_lock(o.container_lock)
		{
		}

		//! Destructor
		~refcontainer_lockObj() noexcept {}
	};

	//! A node in a locked hierarchy container.

	//! Multiply inherit from a reader or a writer lock, and
	//! container_lockbaseObj.

	template<typename lock_t>
	class container_nodeObj : public refcontainer_lockObj<lock_t>,
				  public container_lockbaseObj {
	public:

		//! Return the held lock.
		const lock_t &lock() const
		{
			return this->container_lock->lockval;
		}

		//! Return the held lock.
		lock_t &lock()
		{
			return this->container_lock->lockval;
		}

		//! Constructor

		container_nodeObj(thisref &&refArg)
			: refcontainer_lockObj<lock_t>(std::move(refArg)),
			  container_lockbaseObj(std::move(refArg),
						this->container_lock->lockval
						->rootnode)
		{
		}

		~container_nodeObj() noexcept {}

		//! Clone this lock object.

		//! This creates another lock object that shared the same
		//! underlying container lock, but has its own
		//! container_lockbaseObj, that can point to a different node
		//! in the hierarchy.

		ref<container_lockbaseObj> clone_lock() override
		{
			return ref<container_nodeObj<lock_t> >::create(*this);
		}

		//! Copy constructor used to clone this object.

		container_nodeObj(const container_nodeObj<lock_t> &o)
			: refcontainer_lockObj<lock_t>(o),
			  container_lockbaseObj(o)
		{
		}
	};


	//! A reader lock on the hierarchy container.
	typedef ref<container_nodeObj<typename container_t::readlock>
		    > container_readlock;

	//! A nullable pointer reference to a \ref container_readlock "container_readlock".

	typedef ptr<container_nodeObj<typename container_t::readlock>
		    > container_readlockptr;

	//! A writer lock on the hierarchy container.
	typedef ref<container_nodeObj<typename container_t::writelock>
		    > container_writelock;

public:
	class readlockObj;
	class writelockObj;

	//! Use \ref hierBase "INSERT_LIBX_NAMESPACE::<...>::base::readlock" to refer to this type.

	typedef ref<readlockObj> readlock;

	//! Use \ref hierBase "INSERT_LIBX_NAMESPACE::<...>::base::readlockptr" to refer to this type.

	typedef ptr<readlockObj> readlockptr;

	//! Use \ref hierBase "INSERT_LIBX_NAMESPACE::<...>::base::const_readlock" to refer to this type.
	typedef const_ref<readlockObj> const_readlock;

	//! Use \ref hierBase "INSERT_LIBX_NAMESPACE::<...>::base::const_readlockptr" to refer to this type.
	typedef const_ptr<readlockObj> const_readlockptr;

	//! Use \ref hierBase "INSERT_LIBX_NAMESPACE::<...>::base::writelock" to refer to this type.
	typedef ref<writelockObj> writelock;

	//! Use \ref hierBase "INSERT_LIBX_NAMESPACE::<...>::base::writelockptr" to refer to this type.
	typedef ptr<writelockObj> writelockptr;

	//! Use \ref hierBase "INSERT_LIBX_NAMESPACE::<...>::base::const_writelock" to refer to this type.
	typedef const_ref<writelockObj> const_writelock;

	//! Use \ref hierBase "INSERT_LIBX_NAMESPACE::<...>::base::const_writelockptr" to refer to this type.
	typedef const_ptr<writelockObj> const_writelockptr;


	//! A reader lock.

	//! This is the object referenced by \ref readlock. It implements
	//! methods that examine the contents of the hierarchical container.
	//!
	//! The lock always refers to some node in the hierarchy. Initially
	//! it is the root node. The node in the hierarchy the lock object
	//! refers to may or may not have a value. to_child() and to_parent()
	//! reposition the lock object to other nodes in the hierarchy (if
	//! they exist).

	class readlockObj : virtual public obj {

		//! The container for the reader lock being held internally.

		ref<container_lockbaseObj> lock;

	public:
		//! Constructor
		readlockObj(//! The reader lock container, created by create_readlock()
			    ref<container_lockbaseObj> &&lockArg)
			: lock(std::move(lockArg))
		{
		}

		//! Destructor
		~readlockObj() noexcept {}

		//! Clone this lock, creating another read lock.

		//! The read lock is shared, but the new lock object has its
		//! own current node pointer.
		//!
		//! This throws a null pointer exception if this is a subclass
		//! of writelockObj, because write locks cannot be cloned.

		virtual readlock clone() const
		{
			return readlock::create(lock->clone_lock());
		}

		//! Return the value of the current node in the hierarchy.

	        const ptr<objClass, baseClass> &entry() const
		{
			return lock->node->entry;
		}

		//! Return \c true if the current node in the hierarchy has a value
		bool exists() const
		{
			return !entry().null();
		}

		//! Return \c true unless the current node is the root node.
		bool parent() const
		{
			return lock->node->parent != NULL;
		}

		//! Return \c true if the current node has a child node of the given name.

		bool child(const hier_type &h) const
		{
			const childnodes_map_t
				&children=lock->node->childnodes_map;

			return children.find(h) != children.end();
		}

		//! Return the names of the current node's child nodes.

		std::set<hier_type> children() const
		{
			std::set<hier_type> s;

			for (const auto &c:lock->node->childnodes_map)
			{
				s.insert(c.first);
			}
			return s;
		}

	private:

		//! Search for a child node of the given node, with a value.

		//! Returns a null reference pointer if the child node does
		//! not exist, or does not have a value. If \c nearest_parent
		//! is set, and there's a parent node that's a child of the
		//! current node, in the given path, with a value, then
		//! return the closest parent node to the requested child
		//! hierarchy.
		//!
		//! \note
		//! If nearest_parent is set, the current, starting node has
		//! a value, and the specified hierarchy does not exist, or
		//! has no value, and none of the intermediate nodes in the
		//! hierarchy have a value, the starting node is returned.

		elemptr get_child(//! Child hierarchy to search
				  const std::list<hier_type> &h,

				  //! If \c true and the node does not have a
				  //! value, return the nearest parent node
				  //! that has a value.
				  bool nearest_parent) const
		{
			//! Start with the current node

			elem p=lock->node;
			elemptr parent;

			for (const auto &n:h)
			{
				// Keep track of the most recent value seen.
				if (!p->entry.null())
					parent=p;

				const childnodes_map_t
					&children=p->childnodes_map;

				typename childnodes_map_t::const_iterator
					c=children.find(n);

				// Ran out of the hierarchy, return either
				// the most recent node seen with the value,
				// or nothing.
				if (c == children.end())
					return nearest_parent
						? parent:elemptr();

				p=c->second;
			}

			if (p->entry.null())
			{
				// Stopped at an entry that does not have a
				// value. If we asked for the nearest parent,
				// return that, otherwise we failed.
				if (nearest_parent)
					return parent;
				return elemptr();
			}

			return p;
		}
	public:

		//! Search for a child node.

		//! Returns \c true if the specified direct or indirect
		//! child node has a value.

		bool child(//! The child hierarchy to search
			   const std::list<hier_type> &h,

			   //! If the child node does not have a value, but
			   //! there's a parent node at or below the starting
			   //! node (including the starting node itself), then
			   //! use that node.

			   bool nearest_parent) const
		{
			return !get_child(h, nearest_parent).null();
		}

		//! Move the current node to the parent node

		//! Returns \c true, unless the current node is the root node.
		//! \note
		//! Moves this lock to point to the parent node even if it
		//! has no value.
		bool to_parent() const
		{
			elem cur=lock->node;

			if (!cur->parent)
				return false;

			lock->name.pop_back();
			lock->node=elem(cur->parent);
			return true;
		}

		//! Move the current node to the specified child node with a value.

		//! Single-purpose optimization of get_child().
		//! Returns \c true if the specified child node exists.
		//! The lock gets repointed to that node.
		//! Returns \c false if the specified child does not exist.

		bool to_child(const hier_type &h) const
		{
			elem cur=lock->node;

			const childnodes_map_t
				&children=cur->childnodes_map;

			typename childnodes_map_t::const_iterator
				p=children.find(h);

			if (p == children.end())
				return false;

			lock->name.push_back(h);
			lock->node=p->second;
			return true;
		}

		//! Move the current node a direct or indirect child node.

		//! Calls child() to search for a node, then sets the lock's
		//! current node to that child node, if it was found.
		//! Returns \c true if the lock's current node was moved to
		//! a found node.

		bool to_child(//! See child()
			      const std::list<hier_type> &h,

			      //! See child()
			      bool nearest_parent) const
		{
			elemptr p=get_child(h, nearest_parent);

			if (p.null())
				return false;
			lock->name=p->allnodes_entry->first;
			lock->node=p;
			return true;
		}

		//! Returns the name of the current node the lock is pointing to.
		const std::list<hier_type> &name() const
		{
			return lock->name;
		}
	};

	//! Base class of writelockObj instantiates a writer lock on the container.

	class writelockbaseObj : virtual public obj {

	protected:
		//! The writer lock.
		container_writelock lock;

		//! Constructor
		writelockbaseObj(thisref &&ref)
			: lock(container_writelock::create(std::move(ref)))
		{
		}

		//! Destructor
		~writelockbaseObj() noexcept
		{
		}
	};

	//! A writer lock.

	//! This is the object referenced by \ref writelock. It subclasses
	//! readlockObj, and implements additional methods that modify
	//! the current node in the hierarchy that the lock object points to.

	class writelockObj : public writelockbaseObj, public readlockObj {

	public:
		//! Constructor

		writelockObj(thisref &&refArg)
			: writelockbaseObj(std::move(refArg)),
			  readlockObj(ref<container_lockbaseObj>
				      (writelockbaseObj::lock))
		{
		}

		//! Destructor
		~writelockObj() noexcept {}

		//! Block cloning of write locks.

		virtual readlock clone() const override
		{
			cant_clone_writelock();
		}

		//! Insert a value in the hierarchy.

		template<typename callback_type,
			 typename replace_type>
		bool insert(//! This functor instantiates a new value that's added to the hierarchy.

			    //! It gets invoked unless the value already exists
			    //! and replace() returns false.

			    callback_type &&callback,

			    //! Insert the following child nodes from the
			    //! current node. An empty list updates the
			    //! current node.
			    const std::list<hier_type> &name,

			    //! If this functor returns \c false, an existing
			    //! value does not get replaced.
			    replace_type && replace)
		{
			// Be optimistic. Assemble the new value's name:
			// Current node's name, plus the requested child path.
			std::list<hier_type> full_name
				=writelockbaseObj::lock->name;

			full_name.insert(full_name.end(),
					 name.begin(),
					 name.end());

			// For each name in the child node's hierarchical's
			// name, walk down the hierarchy, as long as it exists,
			// to locate the requested node, if it exists.

			auto b=name.begin(), e=name.end();

			elem p=writelockbaseObj::lock->node;

			bool flag=false;
			// Gets set if ran to the end of the hierarchy, and
			// ended up creating a new hierarchy sublevel, from
			// scratch.

			while (b != e)
			{
				auto iter=p->childnodes_map.find(*b);

				if (iter != p->childnodes_map.end())
				{
					p=iter->second;
					++b;
					// Still exists.
					continue;
				}

				// Reached the end of the road. Create the
				// missing nodes in the hierarchy.

				std::list<elem> newelems;

				auto first_new_parent=p;
				auto first_new=b;

				// Create a node for each missing hierarchy
				// name.

				while (b != e)
				{
					auto next=elem::create();

					// Link parent to each new child.
					if (!newelems.empty())
						newelems.back()
							->newchild(*b, next);

					newelems.push_back(next);
					++b;
				}

				// The last one created gets the new value.

				p=newelems.back();

				p->setentry(full_name,
					    writelockbaseObj::lock->lock()
					    ->allnodes, callback());

				// Attach the first new node as a child of the
				// last existing node.
				try {
					first_new_parent
						->newchild(*first_new,
							   newelems.front());
				} catch (...) {
					writelockbaseObj::lock->lock()->allnodes
						.erase(p->allnodes_entry);
					throw;
				}
				flag=true;
				break;
			}

			elem orig_node=writelockbaseObj::lock->node;

			if (!flag) // Already set the entry, above
			{
				if (!p->entry.null()
				    && !replace((node_type)p->entry))
					return false;

				auto new_value=callback();

				if (!p->entry.null())
				{
					p->entry=new_value;
				}
				else
				{
					// Need to initialize a new node
					p->setentry(full_name,
						    writelockbaseObj::lock->
						    lock()->allnodes,
						    new_value);
				}
			}

			writelockbaseObj::lock->name=full_name;
			writelockbaseObj::lock->node=p;
			return true;
		}

		//! Erase a value from the hierarchy

		//! Returns true if the value was found and it was erased.

		bool erase(//! Erase this child node of the current node.
			   //! Pass an empty list for the current node
			   const std::list<hier_type> &name)
		{
			if (!this->to_child(name, false))
				return false;

			writelockbaseObj::lock->node=
				do_erase(writelockbaseObj::lock->node,
					 true);
			return true;
		}

		//! Prune a value in the hierarchy

		//! Removes all values inferior to the given value,
		//! but not the value itself. Call erase() to erase the
		//! value, in addition to prune().

		void prune(//! Prune this child node of the current node.
			   //! Pass an empty list for the current node
			   const std::list<hier_type> &name)
		{
			prune_if([]
				 (const node_type &ignore,
				  const std::list<hier_type> &name)
				 {
					 return true;
				 }, name);
		}

		//! Prune values if a callback functor returns true.

		template<typename Functor>
		void prune_if(//! Only prune inferior nodes if this functor returns true
			      Functor && functor)
		{
			prune_if(std::forward<Functor>(functor),
				 std::list<hier_type>());
		}

		//! Prune values if a callback functor returns true.

		template<typename Functor>
		void prune_if(//! Only prune inferior nodes if this functor returns true
			      Functor && functor,

			      //! Prune this child node of the current node.
			      //! Pass an empty list for the current node
			      const std::list<hier_type> &name)
		{

			//! First, move to the requested child

			for (const auto &h:name)
			{
				if (!this->to_child(h))
				{
					// Child does not exist,
					// just position it to the nearest
					// node with a value.

					position_to_parent_with_value();
					return;
				}
			}

			// Before pruning the current node, position the lock
			// to the nearest value.
			auto elem=writelockbaseObj::lock->node;
			position_to_parent_with_value();
			prune_inferiors(elem, std::forward<Functor>(functor));
		}

	private:

		//! Before pruning, position the current node, safely

		void position_to_parent_with_value()
		{
			while (this->entry().null())
			{
				if (!this->to_parent())
					return; // Root node
			}
		}

		//! Iterate over inferior nodes, in order to erase them.

		//! Iterates over all inferior nodes of the current node,
		//! and calls prune() for each one.
		template<typename Functor>
		void prune_inferiors(//! Must be passed by value.

				     elem p,

				     //! Whether to prune this node
				     Functor && functor)
		{
			for (typename childnodes_map_t::iterator
				     b=p->childnodes_map.begin(),
				     e=p->childnodes_map.end(),
				     cur=b; cur != e; cur=b)
			{
				++b;
				do_prune(cur->second,
					 std::forward<Functor>(functor));
			}
		}

		//! Prune a node.

		//! Call do_erase(), then prune_inferiors. Subtle point:
		//! If this node has no inferiors, calling do_erase() will
		//! remove it. However, we still have a valid reference to it,
		//! in which case prune_inferiors() will be a harmless
		//! no-op.
		template<typename Functor>
		void do_prune(//! Must be passed by value,

			      elem p,

			      //! Whether to prune this node
			      Functor && functor)
		{
			if (!p->entry.null() &&
			    functor((node_type)p->entry,
				    p->allnodes_entry->first))
				do_erase(p, false);
			prune_inferiors(p, std::forward<Functor>(functor));
		}

		//! Remove a node from a hierarchy

		//! This is called both to remove the originally specified
		//! node, and optionally its inferior nodes.
		//!
		//! Returns the nearest parent node with a value, or the
		//! root node, after removal.

		elem do_erase(//! Node to remove
			      elem p,

			      //! Whether to update lock->name().
			      //! This is \c true when removing the original
			      //! node passed to erase(), and \c false
			      //! when pruning its inferior nodes.

			      bool update_name)
		{
			// Copy the original element on the stack, so if
			// something in the destructor blows up, it will
			// be only after we're completely done, here.

			auto save=p->entry;

			p->entry=ptr<objClass, baseClass>();
			
			writelockbaseObj::lock->lock()->allnodes
				.erase(p->allnodes_entry);

			// Remove unused nodes in the hierarchy.
			// - they have no inferior nodes
			// - they do not have an entry
			// - they are not the root node.

			while (p->entry.null() &&
			       p->childnodes_map.empty()
			       && p->parent)
			{
				elem parent=elem(p->parent);

				parent->childnodes_map.erase(p->parent_entry);
				p=parent;

				if (update_name)
					// Doing the original node
					writelockbaseObj::lock->name.pop_back();
			}
			return p;
		}
	public:
		//! Erase the current node.

		bool erase()
		{
			std::list<hier_type> current;

			return erase(current);
		}

		//! Prune the current node.

		void prune()
		{
			std::list<hier_type> current;

			prune(current);
		}
	};

	//! Create a reader lock on this container

	//! The reader lock allows the container's contents to be examined,
	//! but not modified. Multiple reader locks can be created.
	readlock create_readlock() const
	{
		return readlock::create(container_readlock
					::create(thisref(const_cast<this_t *>
							 (this))));
	}

	//! Create a writer lock on this container

	//! The writer lock allows the container's contents to be modified.
	//! It blocks all other read and writer locks from getting created.

	writelock create_writelock()
	{
		return writelock::create(thisref(this));
	}

	//! Convenience function that inserts a new entry

	//! Instantiate a writer lock, invoke its %insert() method.

	template<typename callback_type, typename replace_type>
	bool insert(//! This functor instantiates a new entry that's added to the hierarchy.

		    //! It gets invoked unless the entry exists
		    //! and replace() returns false.

		    callback_type &&callback,

		    //! Insert the following entry into the hierarchy.
		    const std::list<hier_type> &name,

		    //! Callback to check if an existing node should be replaced
		    replace_type && replace)
	{
		return create_writelock()
			->insert(std::forward<callback_type>(callback),
				 name,
				 std::forward<replace_type>(replace));
	}

	//! Search for a value in the hierarchy

	//! Creates a reader lock, then searches for the given entry or its
	//! nearest existing parent.
	//!
	//! Returns a reader lock on the root node, if the value does not exist
	//! and it has no parent with a value.
	readlock search(const std::list<hier_type> &name) const
	{
		auto lock=create_readlock();

		lock->to_child(name, true);

		return lock;
	}

	//! Erase a value from the hierarchy

	//! Creates a writer lock, and then uses it to erase an existing
	//! value in the container.
	//!
	//! Returns true if the value was found and it was erased.

	bool erase(//! The value to erase

		   const std::list<hier_type> &name)
	{
		return create_writelock()->erase(name);
	}

	//! Prune child nodes from a hierarchy

	//! Creates a writer lock, and then uses it to prune an existing
	//! value in the container.
	//!
	//! \note
	//! The value itself is not removed, only it inferior values, if any.

	void prune(//! The value to prune

		   const std::list<hier_type> &name)
	{
		return create_writelock()->prune(name);
	}

	//! Like prune() but remove an inferior node only if the functor says so.

	//! Takes an additional argument, a functor that takes two parameters,
	//! a value, and the value's name, and the inferior node gets removed
	//! if the functor returns true.

	template<typename Functor>
	void prune_if(//! The value to prune

		      const std::list<hier_type> &name,
		      //! Only prune inferior nodes if this functor returns true
		      Functor && functor)
	{
		return create_writelock()->
			prune_if(std::forward<Functor>(functor), name);
	}

	//! The input iterator iterates over names of all values in the contanier

	//! A reader lock is acquired until the iterator reaches the ending
	//! iterator value.
	//!
	//! Use \ref hierBase "INSERT_LIBX_NAMESPACE::<...>::base::iterator" to refer to this type.

	class iterator : public std::iterator<std::input_iterator_tag,
					      std::list<hier_type>,
					      void, void, void> {

		//! The lock on the underlying container
		container_readlockptr lock;

		//! Current iterator value over the internal hierarchy map.

		typename allnodes_map_t::const_iterator p;

		//! Ending iterator value over the internal hierarchy map.
		typename allnodes_map_t::const_iterator e;

	public:

		//! Ending iterator constructor
		iterator() {}

		//! Beginning iterator constructor
		iterator(const container_readlock &lockArg)
			: lock(lockArg),
			  p(lock->lock()->allnodes.begin()),
			  e(lock->lock()->allnodes.end())
		{
			if (p == e)
				lock=container_readlockptr();
		}

		//! Destructor
		~iterator() {}

		//! Input iterator operator
		iterator &operator++()
		{
			if (!lock.null() && ++p == e)
				lock=container_readlockptr();
			return *this;
		}

		//! Input iterator operator

		iterator operator++(int)
		{
			auto cpy= *this;

			operator++();
			return cpy;
		}

		//! Input iterator operator

		bool operator==(const iterator &o) const
		{
			if (!lock.null() && !o.lock.null())
			{
				return &*lock == &*o.lock && p == o.p;
			}

			return lock.null() && o.lock.null();
		}

		//! Input iterator operator

		bool operator!=(const iterator &o) const
		{
			return !operator==(o);
		}

		//! Input iterator operator

		std::list<hier_type> operator*() const
		{
			if (lock.null())
				return std::list<hier_type>();

			return p->first;
		}

		//! Acquiring a new reader lock on the current key

		hierObj<hier_type, node_type>::readlock clone() const
		{
			auto l=readlock::create(lock->clone_lock());

			l->to_child( operator*(), false );
			return l;
		}
	};

	//! Construct a beginning iterator over the names of the nodes in the hierarchy.

	iterator begin() const
	{
		return iterator(container_readlock::create(thisref(const_cast<this_t *>
							 (this))));
	}

	//! Construct an ending iterator over the names of the nodes in the hierarchy.

	iterator end() const
	{
		return iterator();
	}
};

#if 0
{
#endif
}
#endif
