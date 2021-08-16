/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_attribute_H
#define x_xml_attribute_H

#include <x/namespace.h>
#include <string>
#include <functional>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

//! An attribute of an XML element node.

class attribute {
public:
	//! Attribute name
	std::string attrname;

	//! Attribute namespace. You should be able to convert this to
	//! a \ref uriimpl "INSERT_LIBX_NAMESPACE::uriimpl".

	std::string attrnamespace;

	//! Constructor
	attribute(const std::string &attrnameArg,
		  const std::string &attrnamespaceArg);

	//! Destructor
	~attribute();

	//! Operator
	inline bool operator==(const attribute &o) const
	{
		return attrname == o.attrname &&
			attrnamespace == o.attrnamespace;
	}

	//! Operator
	//! Comparison operator for associative containers.

	bool operator<(const attribute &o) const
	{
		if (attrname < o.attrname)
			return true;

		if (o.attrname < attrname)
			return false;

		return attrnamespace < o.attrnamespace;
	}
};

#if 0
{
#endif
}

namespace std {

	template<> struct hash<LIBCXX_NAMESPACE::xml::attribute> {

		size_t operator()(const LIBCXX_NAMESPACE::xml::attribute &)
			const;
	};
}

#endif
