/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_sectioninfofwd_H
#define x_mime_sectioninfofwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::mime {
#if 0
}
#endif

class sectioninfoObj;

//! MIME section information.

//! This is a reference to an object that gives the size of a MIME entity.
//! After the iterator returned by make_entity_parser() iterates over an
//! eof value, the contents of this object report the size of the MIME
//! entity.

typedef ref<sectioninfoObj> sectioninfo;

//! A nullable reference pointer to a \ref sectioninfo "sectioninfo".

typedef ptr<sectioninfoObj> sectioninfoptr;

//! A reference to a constant \ref sectioninfo "sectioninfo".

typedef const_ref<sectioninfoObj> const_sectioninfo;

//! A nullable reference pointer to a constant \ref sectioninfo "sectioninfo".

typedef const_ptr<sectioninfoObj> const_sectioninfoptr;

#if 0
{
#endif
}
#endif
