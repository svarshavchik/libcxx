/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newdocumentnodeobj_H
#define x_yaml_newdocumentnodeobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/yaml/newnodefwd.H>
#include <x/yaml/writerfwd.H>
#include <map>

#include <yaml.h>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! A node in a new yaml document

//! This is a common superclass for nodes in documents passed to
//! writer::write().

class newdocumentnodeObj : virtual public obj {

protected:

	//! Initialized by the constructor to the default yaml version

	yaml_version_directive_t version;

	//! Subclass can initialize this to the list of custom tags

	//! This is a map keyed by tag handle, value is the tag prefix

	std::map<std::string, std::string> tags;

	//! Explicit start document marker

	bool start_implicit;

	//! Explicit end document marker

	bool end_implicit;
public:
	//! Constructor
	newdocumentnodeObj();

	//! Destructor

	~newdocumentnodeObj() noexcept;

	//! The subclass must define root() to return the new document's root node.

	virtual newnode root() const=0;

	friend class writerObj;

};

#if 0
{
	{
#endif
	}
}

#endif
