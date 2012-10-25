/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newmappingnodeobj_H
#define x_yaml_newmappingnodeobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/yaml/newnodeobj.H>
#include <x/yaml/writerfwd.H>

#include <yaml.h>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! A mapping new node in a new yaml document.

template<typename container_type, typename functor_type>
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

	~newmappingnodeObj() noexcept
	{
	}

	friend class writerObj;

private:
	//! Emit the contains of the mapping.

	//! \internal

	void write(writerObj &obj) const override
	{
		newmapping<container_type> info;

		functor(info);

		obj.write_mapping_start(info.anchor, info.tag,
				       info.implicit, info.style);

		for (const auto & n:info.container)
		{
			const_newnode(n.first)->write(obj);
			const_newnode(n.second)->write(obj);
		}
		obj.write_mapping_end();
	}
};

#if 0
{
	{
#endif
	}
}

#endif
