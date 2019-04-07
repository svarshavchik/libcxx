/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_wordexp_H
#define x_wordexp_H

#include <x/namespace.h>
#include <x/exception.H>
#include <string>
#include <vector>
#include <wordexp.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! An interface to wordexp(3)

void wordexp(//! String to expand
	     const std::string &str,

	     //! Expanded words are appended here
	     std::vector<std::string> &words,

	     //! Flags

	     //! The following flags can be specified:
	     //! \c WRDE_NOCMD, \c WRDE_SHOWERR, \c WRDE_UNDEF.
	     //! Other flags are not supported
	     int flags=0);

#if 0
{
#endif
}
#endif
