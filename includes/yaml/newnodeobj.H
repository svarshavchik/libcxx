/*
** Copyright 2012 Double Precision, Inc.
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

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
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

	~newnodeObj() noexcept;

	friend class writerObj;
	template<typename container_type, typename functor_type>
	friend class newsequencenodeObj;
	template<typename container_type, typename functor_type>
	friend class newmappingnodeObj;
	template<typename container_type>
	friend class newnode_functorObj;

private:

	//! Emit the contents of this new YAML document node.

	//! \internal

	virtual void write(writerObj &obj) const=0;
};

#if 0
{
	{
#endif
	}
}

#endif
