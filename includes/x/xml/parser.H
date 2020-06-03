/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_parser_H
#define x_xml_parser_H

#include <x/xml/parserfwd.H>
#include <x/xml/parserobj.H>
#include <x/ref.H>
#include <string_view>


//! Load and save documents in XML format

//! This namespace is based on libxml2. These classes implement
//! parsing and creating of XML documents using libxml2.

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

//! Base class for a \ref parser "XML document parser".

//! Refer to this class as %INSERT_LIBX_NAMESPACE::xml::parser::base.

class parserBase {

public:

	//! Create an XML parser

	static parser create(//! A label for the document being parsed, in any error message.
			     const std::string_view &uri);

	//! Create an XML parser with non-default options

	static parser create(//! A label for the document being parsed, in any error message.
			     const std::string_view &uri,

			     //! Non-default options, see libxml/parser.h
			     const std::string_view &options);

	//! Object factory, used by \ref ref "INSERT_LIBX_NAMESPACE::ref::create()".
	template<typename ref_type> class objfactory {
	public:

		//! Forward create() call to the base class.

		template<typename ...Args>
		static ref_type create(Args && ...args)
		{
			return parserBase::create(std::forward<Args>(args)...);
		}
	};

};

#if 0
{
#endif
}
#endif
