/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newscalarnodeobj_H
#define x_yaml_newscalarnodeobj_H

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

//! A scalar new node in a new yaml document.

class newscalarnodeObj : public newnodeObj {

public:

	//! Node's anchor, if set.

	std::string anchor;

	//! Node's tag

	std::string tag;

	//! Node's value

	std::string value;

	//! The formatting style, from the LibYAML library.
	yaml_scalar_style_t style;

	//! If the tag may be omitted for the plain style.
	bool plain_implicit;

	//! If the tag may be omitted for any non-plain style
	bool quoted_implicit;

	//! Constructor
	newscalarnodeObj(const std::string &valueArg,
			 yaml_scalar_style_t styleArg=YAML_ANY_SCALAR_STYLE,
			 bool plain_implicitArg=true,
			 bool quoted_implicitArg=true);

	//! Constructor
	newscalarnodeObj(const std::string &tagArg,
			 const std::string &valueArg,
			 yaml_scalar_style_t styleArg=YAML_ANY_SCALAR_STYLE,
			 bool plain_implicitArg=true,
			 bool quoted_implicitArg=true);

	//! Destructor

	~newscalarnodeObj();

	friend class writerObj;

private:
	//! Emit this scalar value

	//! \internal
	void write(writerObj &obj) const override;
};

#if 0
{
	{
#endif
	}
}

#endif
