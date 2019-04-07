/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basicstreambufobj_H
#define x_basicstreambufobj_H

#include <x/obj.H>
#include <x/ptrfwd.H>

#include <iosfwd>
#include <streambuf>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A reference-counted stream buffer

//! A subclass of \c std::streambuf and \c obj.
//! This object gets accessed through a
//! reference. When the last reference goes out of scope, this object gets
//! destroyed.
//!
//! \see ::streambuf,

class basic_streambufObj : virtual public std::streambuf,
			   virtual public obj {

public:
	//! The constructor

	basic_streambufObj() noexcept {}

	//! Default destructor
	~basic_streambufObj() {}
};

//! A reference to a \c std::streambuf

//! This is a reference to a \c std::streambuf object that stores characters.
//!

typedef ref<basic_streambufObj> streambuf;

//! A pointer to a \c std::streambuf

//! This is a pointer to a \c std::streambuf object that stores characters.
//!

typedef ptr<basic_streambufObj> streambufptr;

#if 0
{
#endif
}
#endif
