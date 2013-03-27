/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_docobj_H
#define x_xml_docobj_H

#include <x/obj.H>

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

//! A parsed XML document.

class docObj : virtual public obj {

public:
	//! Constructor
	docObj();

	//! Destructor
	~docObj() noexcept;

	//! A \ref ref "reference" to this object gets returned by readlock()

	class readlockObj : virtual public obj {

	public:
		//! Constructor
		readlockObj();

		//! Destructor
		~readlockObj() noexcept;

		//! Create another read lock

		//! The cloned lock points to the same node that this lock
		//! points to. An exception gets thrown if this is a write
		//! lock, write locks cannot be cloned.

		virtual ref<readlockObj> clone() const=0;

		//! Go to the document's root node.

		//! Returns \c false if the document is empty, and has no
		//! root node.
		virtual bool get_root()=0;

		//! Go to the document's parent node.

		//! Returns \c true if the current node is not the document's
		//! root node, and positions the current node to the parent
		//! node, and \c false in all other cases.

		virtual bool get_parent()=0;

		//! Go to the document's first child node

		//! Returns \c true if the current node has child nodes,
		//! and positions the current node to the first child node,
		//! and \c false in all other cases.

		virtual bool get_first_child()=0;

		//! Go to the document's last child node

		//! Returns \c true if the current node has child nodes,
		//! and positions the current node to the last child node,
		//! and \c false in all other cases.

		virtual bool get_last_child()=0;

		//! Go to the next sibling node.

		//! Returns \c true and moves the current node to its next
		//! sibling node if the current node has a next sibling node,
		//! returns \c false in all other cases.

		virtual bool get_next_sibling()=0;

		//! Go to the previous sibling node.

		//! Returns \c true and moves the current node to its previous
		//! sibling node if the current node has a previous sibling
		//! node, returns \c false in all other cases.

		virtual bool get_previous_sibling()=0;

		//! Go to the document's first child node that's an element node

		//! Returns \c true if the current node has child nodes that
		//! are element nodes,
		//! and positions the current node to the first child element
		//! node and \c false in all other cases.

		virtual bool get_first_element_child()=0;

		//! Go to the document's last child node that's an element node

		//! Returns \c true if the current node has child nodes that
		//! are element nodes,
		//! and positions the current node to the last child element
		//! node and \c false in all other cases.

		virtual bool get_last_element_child()=0;

		//! Go to the next sibling node that's an element node.

		//! If the current node has a next sibling node that's an
		//! element node, returns \c true and moves the current node to
		//! its next sibling node; returns \c false in all other
		//! cases.

		virtual bool get_next_element_sibling()=0;

		//! Go to the previous sibling node that's an element node.

		//! If the current node has a previous sibling node that's an
		//! element node, returns \c true and moves the current node to
		//! its previous sibling node; returns \c false in all other
		//! cases.

		virtual bool get_previous_element_sibling()=0;

		//! Return the current node's type.

		//! Returns an empty string if the lock is not positioned
		//! on a node, this is a new lock.

		virtual std::string type() const=0;

		//! Return the current node's path.

		//! Returns an empty string if the lock is not positioned
		//! on a node, this is a new lock.

		virtual std::string path() const=0;
	};

	//! A \ref ref "reference" to this object gets returned by writelock()

	class writelockObj : public readlockObj {
	public:
		//! Constructor
		writelockObj();

		//! Destructor
		~writelockObj() noexcept;
	};

	//! Create a read lock on the document.

	//! Blocks if a write lock exists.
	virtual ref<readlockObj> readlock()=0;

	//! Create a write lock on the document.

	//! Blocks if a read or a write lock exists.
	virtual ref<writelockObj> writelock()=0;
};

#if 0
{
	{
#endif
	}
}
#endif
