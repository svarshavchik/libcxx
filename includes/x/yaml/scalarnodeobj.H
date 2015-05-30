/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_scalarnodeobj_H
#define x_yaml_scalarnodeobj_H

#include <x/namespace.h>
#include <x/yaml/nodeobj.H>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! A scalar node in a parsed yaml document

class scalarnodeObj : public nodeObj {

public:
	//! Constructor
	scalarnodeObj(const docnode &nArg,
		      yaml_node_type_t nodetypeArg,
		      const std::string &tagArg);

	//! Destructor

	~scalarnodeObj() noexcept;

	friend class documentObj;

	//! The scalar's value

	const std::string value;
};

#if 0
{
	{
#endif
	}
}

#endif