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
