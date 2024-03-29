/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_parserobj_H
#define x_xml_parserobj_H

#include <x/obj.H>
#include <x/xml/docfwd.H>
#include <x/refiterator.H>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

//! An output iterator that parses an XML document

class parserObj : public outputrefiteratorObj<char> {

	using outputrefiteratorObj<char>::operator=;
public:
	//! Constructor
	parserObj();

	//! Destructor
	~parserObj();

	//! Invoke done() after parsing an XML document to retrieve it.
	virtual doc done()=0;
};

#if 0
{
#endif
}
#endif
