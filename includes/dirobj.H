/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_dirobj_H
#define x_dirobj_H

#include <x/obj.H>
#include <x/opendirobj.H>
#include <x/ptr.H>
#include <x/namespace.h>
#include <x/dirfwd.H>

#include <sys/types.h>
#include <dirent.h>
#include <iterator>
#include <string>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! This object that holds the name of a directory whose contents will be read.

//! This class only holds the name of the directory. The begin() methods
//! actually gets the whole process going.

class dirObj : virtual public obj {

	//! The directory's name

	std::string dirname;

	//! Whether the . and .. entries get returned.

	bool showimplicit;

public:
	//! The constructor.

	dirObj(//! The name of the directory.
	       const std::string &dirnameArg,

	       //! Whether \c . and \c .. entries are retrieved.
	       bool showimplicitArg=false) noexcept;

	~dirObj() noexcept;

	//! Retrieve the directory's name.

	//! \return the \c dirnameArg argument that was passed to the
	//! constructor.
	std::string getDirname() const noexcept { return dirname; }

	//! Retrieve the \c showimplicitArg argument that was passed to the
	//! constructor.
	bool getImplicitFlag() const noexcept { return showimplicit; }

	class iterator;

	//! Return the beginning %iterator.

	//! This method returns the starting %iterator for the directory's
	//! contents.
	iterator begin() const;

	//! Return the ending %iterator.

	//! This method returns the ending %iterator for the directory's
	//! content.
	iterator end() const noexcept;

	//! A directory %entry.

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::dir::base::entry or
	//! \c INSERT_LIBX_NAMESPACE::dirptr::base::entry.
	//!
	//! These are the values that are iterated over. This is
	//! a \c std::pair subclass:
	//!
	//! - \c first - the directory %entry.
	//! - \c second - the %entry type.
	//!
	//! \c second may be one of the following files, as defined by
	//! readdir(3):
	//!
	//! \c DT_UNKNOWN, \c DT_REG, \c DT_DIR, \c DT_FIFO, \c DT_SOCK,
	//! \c DT_CHR, or \c DT_BLK.
	//!
	//! \see operator std::string()

	class entry : public std::pair<std::string, unsigned char> {

		//! The underlying object that holds the open directory handle.

		ptr<opendirObj> d;

	public:
		friend class dirObj;

		//! Convert to a \c std::string.

		//! This convenience operator permits this object to be
		//! used in contexts that call for a \c std::string. The
		//! operator returns the \c first member.
		operator std::string() const noexcept { return first; }

		//! Return the full path to the directory %entry.

		//! Affix this directory %entry's name to the directory's
		//! name, to return the full path.

		std::string fullpath() const noexcept;

		//! Retrieve the filetype

		//! If \c second is \c DT_UNKNOWN, lstat the file, and
		//! obtain the file type that way.

		unsigned char filetype() const;

		//! Provide an ordering function

		bool operator<(const entry &o) const noexcept
		{
			return fullpath() < o.fullpath();
		}
	};

};

//! A directory contents %iterator.

//! This %iterator gets returned by begin() and end(). This is an
//! input %iterator that returns the contents of the directory.
//!
//! Refer to this class as \c INSERT_LIBX_NAMESPACE::dir::base::iterator or
//! \c INSERT_LIBX_NAMESPACE::dirptr::base::iterator.

class dirObj::iterator : public std::iterator<std::input_iterator_tag, entry>
{
	//! The original directory object.

	//! This is needed only to get the correct semantics for
	//! operator==().

	const_dirptr nameref;

	//! The value of this %iterator.

	entry val;

public:
	friend class dirObj;

	//! Default constructor.

	iterator() noexcept;

	//! Construct a new %iterator object.

	iterator(//! The underlying opendirObj object.

		 //! This parameter is a \c null reference if this is the
		 //! ending %iterator.
		 const ptr<opendirObj> &dArg,

		 //! The original dir object.

		 //! The %iterator keeps an internal reference to the original
		 //! dir object.
		 const const_dirptr &namerefArg) noexcept;

	//! The default destructor.
	~iterator();

	//! Implement the copy constructor.
	iterator(//! An existing %iterator object.

		 const iterator &o) noexcept;

	//! The assignment operator.

	iterator &operator=(//! An existing %iterator object.

			    const iterator &o) noexcept;

	//! Return the name of the current directory entry.

	//! \return The current directory entry's name, a
	//! \c std::string.
	const entry &operator*() const noexcept
	{
		return val;
	}

	//! Return the name of the current directory entry.

	//! \overload
	//!
	const entry *operator->() const noexcept
	{
		return &val;
	}

	//! Advance the %iterator to the next directory entry.

	//! This is the pre-increment operator.
	//!
	iterator &operator++();

	//! Advance the %iterator to the next directory entry.

	//! This is the post-increment operator. It returns a copy of this
	//! %iterator object, which gets incremented after the copy is made.
	iterator operator++(int);

	//! Test two %iterator objects for equality.

	//! \return \c true, if this and the other %iterator are the ending
	//! %iterator, or if both point to the same directory entry, \c false
	//! otherwise.
	bool operator==(//! Another existing %iterator
			const iterator &o) noexcept;

	//! Test two %iterator objects for inequality.

	//! \return the logic opposite of operator==().
	//!
	bool operator!=(const iterator &o) noexcept
	{
		return !operator==(o);
	}
};

#if 0
{
#endif
}
#endif
