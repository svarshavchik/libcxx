/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newaliasnodeobj_H
#define x_yaml_newaliasnodeobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/yaml/newnodeobj.H>
#include <x/yaml/writerfwd.H>

#include <yaml.h>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

//! An alias node in a new yaml document.

class newaliasnodeObj : public newnodeObj {

public:

	//! Node's anchor.

	std::string anchor;

	//! Constructor
	newaliasnodeObj(const std::string &anchorArg);

	//! Destructor

	~newaliasnodeObj();

	friend class writerObj;

private:
	//! Emit this alias.

	//! \internal
	void write(writerObj &obj) const override;
};

#if 0
{
#endif
}

#endif
