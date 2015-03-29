/*
** Copyright 2012 Double Precision, Inc.
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

//! A template for a reference-counted stream buffer

//! This template defines a subclass of \c std::basic_streambuf and \c obj.
//! The template implements
//! reference-counted stream buffers. This object gets accessed through a
//! reference. When the last reference goes out of scope, this object gets
//! destroyed.
//!
//! \see ::streambuf, ::wstreambuf

template<class charT, class traits= std::char_traits<charT> >

class basic_streambufObj : virtual public std::basic_streambuf<charT, traits>,
			   virtual public obj {

public:
	//! The constructor

	basic_streambufObj() noexcept {}

	//! Default destructor
	~basic_streambufObj() noexcept {}
};

//! A reference to a \c std::streambuf

//! This is a reference to a \c std::streambuf object that stores characters.
//!

typedef ref<basic_streambufObj<char> > streambuf;

//! A reference to a wide character \c std::streambuf

//! This is a reference to a \c std::streambuf object that stores wide
//! characters.

typedef ref<basic_streambufObj<wchar_t> > wstreambuf;

//! A pointer to a \c std::streambuf

//! This is a pointer to a \c std::streambuf object that stores characters.
//!

typedef ptr<basic_streambufObj<char> > streambufptr;

//! A pointer to a wide character \c std::streambuf

//! This is a pointer to a \c std::streambuf object that stores wide
//! characters.

typedef ptr<basic_streambufObj<wchar_t> > wstreambufptr;


#if 0
{
#endif
}
#endif
