/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_nodeobj_H
#define x_yaml_nodeobj_H

#include <x/namespace.h>
#include <x/exception.H>
#include <x/yaml/docnodefwd.H>

#include <yaml.h>
#include <iterator>
#include <iostream>
#include <list>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! A node in a yaml document

class nodeObj : virtual public obj {

protected:

public:
	//! Constructor
	nodeObj(yaml_node_type_t nodetypeArg,
		const std::string &tagArg);

	//! Destructor

	~nodeObj();

	//! The type of this node
	const yaml_node_type_t nodetype;

	//! The node tag
	const std::string tag;
};

#if 0
{
	{
#endif
	}
}

#endif
