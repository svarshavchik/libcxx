/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newnode_H
#define x_yaml_newnode_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/ref.H>
#include <x/yaml/newnodefwd.H>
#include <x/yaml/newnodeobj.H>

#include <type_traits>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

//! Helper class used by make_newnode()

template<typename functor_type>
class newnode_functorObj : public newnodeObj {

	//! The saved functor

	mutable functor_type functor;

public:
	//! Constructor

	template<typename functorArgType>
	newnode_functorObj(functorArgType &&functorArg)
		: functor(std::forward<functorArgType>(functorArg))
	{
	}

	//! Destructor

	~newnode_functorObj()
	{
	}

private:

	//! Call the functor to obtain the next node in the YAML document.

	void write(writerObj &obj) const override
	{
		newnode(functor())->write(obj);
	}
};

//! Invoke a functor to obtain the node in a YAML document.

template<typename functor_type>
newnode make_newnode(functor_type &&functor)
{
	return ref< newnode_functorObj<
			    typename std::remove_reference<functor_type>
			    ::type> >
		::create(std::forward<functor_type>(functor));
}

#if 0
{
#endif
}

#endif
