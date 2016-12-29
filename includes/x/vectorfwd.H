/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_vectorfwd_H
#define x_vectorfwd_H

#include <x/ptrfwd.H>
#include <x/fdfwd.H>
#include <memory>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<class T, class Allocator> class vectorBase;
template<class T, class Allocator=std::allocator<T> > class vectorObj;

//! A reference to a \c std::vector

//! This template is a reference-counted vector.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::vector<char> vecref=INSERT_LIBX_NAMESPACE::vector<char>::create();
//! \endcode
//!
//! This instantiates a new %vector object, and sets up its initial reference.
//! \c vecref is a reference that can be freely passed around.
//! \c *vecref may be used anywhere the equivalent \c std::vector is used.
//! The %vector gets automatically destroyed when its last reference goes out
//! of scope.
//!
//! \code
//! auto vecref=INSERT_LIBX_NAMESPACE::vector<char>::base::load(INSERT_LIBX_NAMESPACE::fd::base::load("filename"));
//! \endcode
//!
//! The load() constructor calls create(), then takes an opened
//! \ref fd "INSERT_LIBX_NAMESPACE::fd" passes it to
//! \ref vectorObj::load "load"().

template<class T, class Allocator=std::allocator<T> >
using vector=ref<vectorObj<T, Allocator>, vectorBase<T, Allocator> >;

//! A nullable reference pointer to a reference-counted vector object.

//! \see vector

template<class T, class Allocator=std::allocator<T> >
using vectorptr=ptr<vectorObj<T, Allocator>, vectorBase<T, Allocator> >;

//! A reference to a constant reference-counted vector object.

//! \see vector

template<class T, class Allocator=std::allocator<T> >
using const_vector=const_ref<vectorObj<T, Allocator>,
			     vectorBase<T, Allocator> >;

//! A nullable reference pointer to a constant reference-counted vector object.

//! \see vector

template<class T, class Allocator=std::allocator<T> >
using const_vectorptr=const_ptr<vectorObj<T, Allocator>,
				vectorBase<T, Allocator> >;

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

	//! A constructor that loads the contents of the vector from a file.

	static vector<T, Allocator> load(const fd &file)
	{
		auto v=vector<T, Allocator>::create();

		v->load(file);

		return v;
	}
};

#if 0
{
#endif
}
#endif
