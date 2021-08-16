/*
** Copyright 2020-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_createnodefwd_H
#define x_xml_createnodefwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

class createnodeObj;


//! Modifying XML documents

//! This is a \ref ref "reference" to a reference-counted object that
//! gets returned by \ref writelock "INSERT_LIBX_NAMESPACE::xml::writelock"'s
//! create_child(), create_next_sibling(), and
//! create_previous_sibling(), and implements creation of new XML
//! document elements that get inserted accordingly.

typedef ref<createnodeObj> createnode;

//! A nullable pointer reference to a \ref createnode "createnode".

typedef ptr<createnodeObj> createnodeptr;

//! A reference to a constant \ref createnode "createnode".
typedef const_ref<createnodeObj> const_createnode;

//! A nullable reference pointer to a constant \ref createnode "createnode".
typedef const_ptr<createnodeObj> const_createnodeptr;

#if 0
{
#endif
}
#endif
