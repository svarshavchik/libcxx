/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newnodeobj_H
#define x_yaml_newnodeobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/yaml/writerfwd.H>
#include <x/yaml/newnodefwd.H>
#include <x/yaml/newsequencenodefwd.H>
#include <x/yaml/newmappingnodefwd.H>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

//! A node in a new yaml document

//! This is a common superclass for nodes in documents passed to
//! writer::write().

class newnodeObj : virtual public obj {

public:
	//! Constructor
	newnodeObj();

	//! Destructor

	~newnodeObj();

	friend class writerObj;
	friend class newsequence;
	friend class newmapping;
	template<typename container_type>
	friend class newnode_functorObj;

private:

	//! Emit the contents of this new YAML document node.

	//! \internal

	virtual void write(writerObj &obj) const=0;
};

#if 0
{
#endif
}

#endif
