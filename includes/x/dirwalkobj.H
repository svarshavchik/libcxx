/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_dirwalkobj_H
#define x_dirwalkobj_H

#include <x/obj.H>
#include <x/dir.H>
#include <x/dirwalktreeobj.H>
#include <string>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! This object that holds the name of a directory whose contents will be read recursively

//! This class only holds the name of the directory, and a flag whether
//! the contents will be read in prefix or suffix order. The begin() methods
//! actually gets the whole process going.

class dirwalkObj : virtual public obj {

	//! The directory name
	std::string dirname;

	//! The suffix order flag
	bool ispostorder;

public:
	//! Constructor

	dirwalkObj(//! Instantiate a dirwalk object that refers to a specific subdirectory.

		   const std::string &dirnameArg,

		   //! Whether to return entries in suffix order

		   bool ispostorderArg=false);

	//! Default destructor

	~dirwalkObj();


	//! An %iterator for the directory's contents

	//! This %iterator gets returned by begin() and end(). This is an
	//! input %iterator that returns the contents of the directory
	//! hierarchy.

	class iterator : public std::iterator<std::input_iterator_tag,
					      dir::base::entry> {

		//! A reference to an open directory

		//! An end() %iterator has this reference set to null.

		ptr<dirwalktreeObj> r;

		//! This %iterator's current entry.
		dir::base::entry val;

		//! Construct the begin() %iterator.

		//! \internal
		//!
		iterator(const ptr<dirwalktreeObj> &rArg);

	public:
		//! Construct an end() %iterator

		iterator() noexcept;

		//! Default destructor.

		~iterator();

		friend class dirwalkObj;

		//! Copy constructor

		iterator(const iterator &) noexcept;

		//! Assignment operator

		iterator &operator=(const iterator &) noexcept;

		//! The * operator

		const dir::base::entry &operator*() const noexcept
		{
			return val;
		}

		//! The -> operator

		const dir::base::entry *operator->() const noexcept
		{
			return &val;
		}

		//! Advance the %iterator to the next directory entry.

		//! This is the pre-increment operator.
		//!
		iterator &operator++();

		//! Advance the %iterator to the next directory entry.

		//! This is the post-increment operator. It returns a copy of
		//! this %iterator object, which gets incremented after the
		//! copy is made.

		iterator operator++(int);

		//! Test two %iterator objects for equality.

		//! \return \c true, if this and the other %iterator are the
		//! ending %iterator, or if both point to the same directory
		//! entry, \c false otherwise.

		bool operator==(//! Another existing %iterator
				const iterator &o) const noexcept;

		//! Test two %iterator objects for inequality.

		//! \return the logic opposite of operator==().
		//!
		bool operator!=(const iterator &o) const noexcept
		{
			return !operator==(o);
		}
	};

	//! Return the begin() %iterator

	iterator begin() const;

	//! Return the end() iterator

	iterator end() const noexcept;

};

#if 0
{
#endif
}
#endif
