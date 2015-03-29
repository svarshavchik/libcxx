/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_dir_H
#define x_dir_H

#include <x/ref.H>
#include <x/dirfwd.H>
#include <x/dirobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Base class of directory objects.

//! Refer to this base class as \c dir::base or \c dirptr::base

class dirBase : public ptrrefBase {

public:
	//! A convenience typedef
	typedef dirObj::iterator iterator;

	//! A convenience typedef

	typedef dirObj::entry entry;

	//! Recursive directory delete.

	//! This method implements, essentially, the \c rm \c -rf command.
	//!
	static void rmrf(//! The directory to delete.

			 //! An attempt is made to recursively delete the
			 //! contents of this directory.
			 const std::string &dirName,

			 //! Remove the directory itself.

			 //! If \c true, the directory itself is removed,
			 //! if \c false, the directory is spared.

			 bool removeit=true
			 );

	//! Recursive directory delete.

	//! \overload
	//!
	static void rmrf(const char *dirName)
	{
		rmrf(std::string(dirName));
	}

	//! Recursive directory create

	//! Create a directory, with any necessary parent directories.
	//!
	static void mkdir(//! Directory name
			  const char *dirName,
			  //! Permissions on the newly-created directory
			  mode_t m=0777);

	//! Recursive directory create

	//! \overload
	//!

	static void mkdir(const std::string &s, mode_t m=0777)
	{
		mkdir(s.c_str(), m);
	}

	//! Create a parent directory

	//! Given a filename, create its parent directory, if necessary.

	static void mkdir_parent(//! The filename
				 const std::string &f,

				 //! Permissions
				 mode_t m=0777);
};

#if 0
{
#endif
}
#endif
