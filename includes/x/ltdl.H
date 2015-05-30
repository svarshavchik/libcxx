/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ltdl_H
#define x_ltdl_H

#include <x/ltdlfwd.H>
#include <x/ltdlobj.H>
#include <x/ref.H>

#include <set>
#include <string>
namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Base class for a \ref ltdl "loaded shared library".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::ltdl::base

class ltdlBase : public ptrrefBase {

public:

	//! Options for loading a shared library

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::ltdl::base::options
	//!
	//! \see ltdl

	typedef ref<ltdlObj::optionsObj> options;

	//! Nullable pointer reference to a \ref options "shared library load options".

	typedef ptr<ltdlObj::optionsObj> optionsptr;

	//! Reference to constant \ref options "shared library load options".
	typedef const_ref<ltdlObj::optionsObj> const_options;

	//! Nullable pointer reference to constant \ref options "shared library load options".

	typedef const_ptr<ltdlObj::optionsObj> const_optionsptr;

	//! Add a directory to the shared library search path

	//! See libtool documentation for more information.
	static void addsearchdir(const std::string &dir);

	//! Insert a directory to the shared library search path

	//! See libtool documentation for more information.
	static void insertsearchdir(const std::string &before,
				    const std::string &dir);

	//! Set shared library directory search path.

	//! See libtool documentation for more information.

	static void setsearchpath(const std::string &dir);

	//! Return current shared library search path.

	//! See libtool documentation for more information.

	static std::string getsearchpath();

	//! Enumerate all shared libraries in the current search path

	//! This returns the names of all shared libraries that can be loaded
	//! from the current search path.
	static std::set<std::string> enumerate();

	//! Enumerate all shared libraries in the specified search path

	//! This returns the names of all shared libraries that can be loaded
	//! from the specified search path.
	static std::set<std::string> enumerate(const std::string &search_path);
};

#if 0
{
#endif
}
#endif