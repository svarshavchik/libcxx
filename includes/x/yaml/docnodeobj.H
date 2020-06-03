/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_docnodeobj_H
#define x_yaml_docnodeobj_H

#include <x/namespace.h>
#include <x/exception.H>
#include <x/yaml/document.H>

#include <iterator>
#include <iostream>
#include <list>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

class scalarnodeObj;
class mappingnodeObj;
class sequencenodeObj;

//! A wrapper representing a node in the native yaml_document_t.

class docnodeObj : virtual public obj {

private:
	//! Constructor
	docnodeObj(const document &docArg);

	//! My document

	document doc;

public:
	//! Destructor

	~docnodeObj();

	friend class documentObj;
	friend class nodeObj;

	friend class scalarnodeObj;
	friend class mappingnodeObj;
	friend class sequencenodeObj;

private:
	//! Retrieve the underlying YAML node.

	virtual const yaml_node_t
	*getNode(const documentObj::readlock_t &lock)=0;

	//! Retrieve the underlying YAML node.

	virtual yaml_node_t *getNode(const documentObj::writelock_t &lock)=0;

	class rootnodeObj;
	class nonrootnodeObj;
};

#endif

#if 0
{
#endif
}

