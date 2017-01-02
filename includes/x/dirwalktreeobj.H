/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_dirwalktreeobj_H
#define x_dirwalktreeobj_H

#include <x/obj.H>
#include <x/exception.H>
#include <x/dir.H>
#include <list>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! An object that holds a directory whose contents are being recursively read

//! \internal
//! This object's methods are not accessed directly, but rather via the
//! methods of dirwalk and dirwalk::iterator objects.

class dirwalktreeObj : virtual public obj {

	//! An opened directory, somewhere in the hierarchy

	class entry {
	public:
		//! The current iterator for this directory
		dir::base::iterator b;

		//! The ending iterator for this directory
		dir::base::iterator e;

		entry() LIBCXX_HIDDEN {}
		~entry() LIBCXX_HIDDEN {}
	};

	//! Opened directories in the hierarchy.

	//! The tail end of the list is the current directory under iteration.
	std::list<entry> dirtree;

	//! Whether the hierarchy needs to be iterated in suffix order.

	bool ispostorder;
public:

	//! Open a hidrectory hierarchy

	dirwalktreeObj(//! The directory path
		       const std::string &dirnameArg,

		       //! \c true if the iteration should be in suffix order.
		       bool ispostorderArg);

	//! Default destructor

	~dirwalktreeObj();

	//! Retrieve the current directory entry

	const dir::base::entry &current() noexcept
	{
		return *dirtree.back().b;
	}

	//! Advance to the first entry in the directory hierarchy

	//! \internal
	//!
	//! \return \c true if the directory hierarchy is empty

	bool initial() LIBCXX_HIDDEN;

	//! Advance to the next entry in the directory hierarchy

	//! \return \c true if the end of the directory hierarchy was reached.
	bool increment() LIBCXX_HIDDEN;

	//! When using suffix order, if the current entry is a subdirectory, open it (recursively)
	void postorder_opendirs() LIBCXX_HIDDEN;
};

#if 0
{
#endif
}
#endif
