/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_vectorfwd_H
#define x_vectorfwd_H

#include <x/ptrfwd.H>
#include <memory>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<class T, class Allocator> class vectorBase;
template<class T, class Allocator=std::allocator<T> > class vectorObj;

//! A reference to a \c std::vector

//! This template defines a reference to a %vector. Since C++ does not (yet)
//! have template typedefs, the typedef is defined within a templated class.
//!
//! \code
//! vector<char>::ref_t vecref=vector<char>::ref_t::create();
//! \endcode
//!
//! This instantiates a new %vector object, and sets up its initial reference.
//! \c vecref is a reference that can be freely passed around.
//! \c *vecref may be used anywhere the equivalent \c std::vector is used.
//! The %vector gets automatically destroyed when its last reference goes out
//! of scope
//!
//! \see ptr
//!
//! \note
//! This is a template alias for a \ref ref "ref" to an vectorObj.

template<class T, class Allocator=std::allocator<T> >
#ifdef DOXYGEN
class vector{}; // TODO
#else
using vector=ref<vectorObj<T, Allocator>, vectorBase<T, Allocator> >;
#endif

//! A nullable reference pointer to a reference-counted vector object.

//! \see vector

template<class T, class Allocator=std::allocator<T> >
#ifdef DOXYGEN
class vectorptr{}; // TODO
#else
using vectorptr=ptr<vectorObj<T, Allocator>, vectorBase<T, Allocator> >;
#endif

//! A reference to a constant reference-counted vector object.

//! \see vector

template<class T, class Allocator=std::allocator<T> >
#ifdef DOXYGEN
class const_vector{}; // TODO
#else
using const_vector=const_ref<vectorObj<T, Allocator>, vectorBase<T, Allocator> >;
#endif

//! A nullable reference pointer to a constant reference-counted vector object.

//! \see vector

template<class T, class Allocator=std::allocator<T> >
#ifdef DOXYGEN
class const_vectorptr{}; // TODO
#else
using const_vectorptr=const_ptr<vectorObj<T, Allocator>, vectorBase<T, Allocator> >;
#endif

//! Refer to this class as \c vector<>::base or vectorptr<>::base

//! \internal
//! Imports \c typedefs for \c iterator and \c const_iterator

template<class T, class Allocator>
class vectorBase : public ptrrefBase {

public:
	//! Import the \c iterator typedef

	typedef typename vectorObj<T, Allocator>::iterator iterator;

	//! Import the \c const_iterator typedef

	typedef typename vectorObj<T, Allocator>::const_iterator const_iterator;

	//! A convenient typedef for the container type

	typedef typename vectorObj<T, Allocator>::container container;
};

#if 0
{
#endif
}
#endif
