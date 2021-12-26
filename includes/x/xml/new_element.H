/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_new_element_H
#define x_xml_new_element_H

#include <x/uriimplfwd.H>
#include <x/explicit_arg.H>
#include <x/namespace.h>
#include <string>
#include <string_view>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

//! Helper class for creating a new element

//! Refer to this class as
//! See \ref writelock "INSERT_LIBX_NAMESPACE::writelock"
//! for more information.
//!
//! \note
//! Because this, itself, is a parameter to another function call,
//! it needs to own its data, because the passed-in parameters may be
//! temporaries; hence everything is stored in std::strings.
//!
//! For example, the uriimpl parameter gets converted to a
//! std::string, invoking a delegated constructor.

class new_element {

public:
	//! New element's name
	std::string name;

	//! New element's prefix
	std::string prefix;

	//! New element's URI
	std::string uri;

	//! Whether this helper class was constructed with a given prefix.
	bool prefix_given;

	//! Create a new element in an unnamed namespace.
	new_element(//! New element's name
		    const std::string_view &nameArg);

	//! Create a new element in a given namespace

	new_element(//! New element's name
		    const std::string_view &namearg,

		    //! New element's URI. This URI must be opened in
		    //! some parent node of the new element.
		    const std::string_view &uriArg);

	//! Create a new element in a given namespace

	new_element(//! New element's name
		    const std::string_view &nameArg,

		    //! New element's URI. This URI must be opened in
		    //! some parent node of the new element.
		    const explicit_arg<uriimpl> &uriArg);

	//! Create a new element in a new namespace
	new_element(//! New element's name
		    const std::string_view &nameArg,

		    //! The new namespace is opened in this element,
		    //! this is its prefix.
		    const std::string_view &prefixArg,

		    //! The new namespace's URI
		    const std::string_view &uriArg);

	//! Destructor

	~new_element();
};

#if 0
{
#endif
}

#endif
