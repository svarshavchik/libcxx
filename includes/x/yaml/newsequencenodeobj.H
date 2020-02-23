/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newsequencenodeobj_H
#define x_yaml_newsequencenodeobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/yaml/newnodefwd.H>
#include <x/yaml/newnodeobj.H>
#include <x/yaml/writerfwd.H>

#include <yaml.h>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! A sequence new node in a new yaml document, constructed by make_newsequencenode().

template<typename functor_type>
class newsequencenodeObj : public newnodeObj {

	//! The functor which returns the sequence to write

	mutable functor_type functor;

public:

	//! Constructor
	template<typename functorArgType>
	newsequencenodeObj(functorArgType &&functorArg)
		: functor(std::forward<functorArgType>(functorArg))
	{
	}

	//! Destructor

	~newsequencenodeObj()
	{
	}
	friend class writerObj;

private:
	//! Emit the contents of this sequence

	//! \internal
	void write(writerObj &obj) const override
	{
		newsequence info(obj);

		functor(info);
	}
};

#if 0
{
	{
#endif
	}
}

#endif
