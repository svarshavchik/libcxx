/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_document_H
#define x_yaml_document_H

#include <x/namespace.h>
#include <x/yaml/documentobj.H>
#include <x/ref.H>
#include <x/ptr.H>
#include <yaml.h>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

//! A YAML document

//! This
//! \ref ref "reference to a reference-counted" object that represents one
//! parsed YAML document. It defines a single method, root() that returns
//! a \ref nodeptr "INSERT_LIBX_NAMESPACE::yaml::nodeptr", the root node
//! of the parsed YAML document, or a null pointer if the document was empty.

typedef ref<documentObj> document;

//! A nullable pointer reference to a \ref document "YAML document".

typedef ptr<documentObj> documentptr;

#if 0
{
#endif
}
#endif
