/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newmappingnodeobj_H
#define x_yaml_newmappingnodeobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/yaml/newnodeobj.H>
#include <x/yaml/writerfwd.H>

#include <yaml.h>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

//! A mapping new node in a new yaml document.

template<typename functor_type>
class newmappingnodeObj : public newnodeObj {

	//! The functor which returns the map to write

	mutable functor_type functor;

public:

	//! Constructor
	template<typename functorArgType>
	newmappingnodeObj(functorArgType &&functorArg)
		: functor(std::forward<functorArgType>(functorArg))
	{
	}

	//! Destructor

	~newmappingnodeObj()
	{
	}

	friend class writerObj;

private:
	//! Emit the contains of the mapping.

	//! \internal

	void write(writerObj &obj) const override
	{
		newmapping info(obj);

		functor(info);
	}
};

#if 0
{
#endif
}

#endif
