/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_new_attribute_H
#define x_xml_new_attribute_H

#include <x/xml/attribute.H>

namespace LIBCXX_NAMESPACE {
	class uriimpl;

	namespace xml {
#if 0
	};
};
#endif

//! A new attribute of an XML document

//! See \ref writelock "INSERT_LIBX_NAMESPACE::doc::base::writelock"
//! for more information.

class new_attribute : public attribute {
public:
	//! Attribute value
	std::string attrvalue;

	//! Create a new attribute

	new_attribute(//! Attribute name

		      //! It can be specified as 'prefix:name', creating
		      //! the attribute in the given namespace. A
		      //! namespace with the given prefix must be opened
		      //! by this element or one of its parent elements.
		      //! Otherwise the attribute gets created in the
		      //! unknown namespace.
		      const std::string &attrnameArg,

		      //! Attribute's value
		      const std::string &attrvalueArg);

	//! Create a new attribute in a given namespace

	new_attribute(//! Attribute name
		      const std::string &attrnameArg,

		      //! Attribute's namespace URI.

		      //! A namespace with the given URI must be opened
		      //! by this element or one of its parent elements.

		      const std::string &attrnamespaceArg,

		      //! Attribute's value
		      const std::string &attrvalueArg);

	//! Create a new attribute in a given namespace

	new_attribute(//! Attribute name
		      const std::string &attrnameArg,

		      //! Attribute's namespace URI.

		      //! A namespace with the given URI must be opened
		      //! by this element or one of its parent elements.

		      const uriimpl &attrnamespaceArg,

		      //! Attribute's value
		      const std::string &attrvalueArg);

	//! Create a new attribute in a given namespace

	new_attribute(//! Attribute name
		      const std::string &attrnameArg,

		      //! Attribute's namespace URI.

		      //! A namespace with the given URI must be opened
		      //! by this element or one of its parent elements.

		      const char *attrnamespaceArg,

		      //! Attribute's value
		      const std::string &attrvalueArg);

	//! Destructor
	~new_attribute();
};

#if 0
{
	{
#endif
	}
}

namespace std {
	template<> struct hash<LIBCXX_NAMESPACE::xml::new_attribute>
		: public std::hash<LIBCXX_NAMESPACE::xml::attribute> {

		using std::hash<LIBCXX_NAMESPACE::xml::attribute>::operator();
	};
}
#endif
