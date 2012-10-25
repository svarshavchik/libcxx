/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newmappingnode_H
#define x_yaml_newmappingnode_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/ref.H>
#include <x/yaml/newmappingnodefwd.H>
#include <x/yaml/newmappingnodeobj.H>

#include <type_traits>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! A mapping in a new YAML document written by writer::write().

//! The template parameter specifies the container type that holds a
//! \c std::pair
//! of \ref newnode "nodes". The default container type is
//! std::list<std::pair<const_newnode, const_newnode> >, but any container that
//! iterates over \c std::pair of
//! \ref const_newnode "const_newnode"s or
//! \ref newnode "newnode"s can be specified.
//!
//! create() constructs a \ref newnode "node" that writes a YAML mapping.
//!
//! \code
//! return INSERT_LIBX_NAMESPACE::yaml::newmapping<>::create([]
//!     (INSERT_LIBX_NAMESPACE::yaml::newmapping<> &info)
//!     {
//!         info.anchor="capitals";
//!         info.container.push_back(std::make_pair(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("New Jersey"), INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("Trenton")));
//!         info.container.push_back(std::make_pair(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("New York"), INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("Albany")));
//!      });
//! \endcode
//!
//! create() takes a functor as an argument. The functor
//! receives an instance of the \ref newmapping "newmapping" type,
//! by reference. This object has the following members:
//!
//! - \ref newmapping::container "container": the container type specified
//! by the template parameter (which defaults to
//! \c std::list<std::pair<const_newnode,const_newnode>> ).
//!
//! - \ref newmapping::anchor "anchor": an optional anchor for an alias
//! reference to this mapping node.
//!
//! - \ref newmapping::tag "tag": an optional tag for this node.
//!
//! - \ref newmapping::style "style": formatting style.
//!
//! - \ref newmapping::implicit "implicit": whether the implicit tag can
//! be ommited.
//!
//! The functor should fill the
//! \ref container "container" with the mapping, and optionally set the
//! other class members. The functor gets called to construct the new mapping,
//! for the new YAML document.

template<typename container_type>
struct newmapping {

	//! The container for the mapping.
	container_type container;

	//! Node's anchor, if set.

	std::string anchor;

	//! Node's tag

	std::string tag;

	//! The formatting style, from the LibYAML library.
	yaml_mapping_style_t style;

	//! If the tag may be omitted.
	bool implicit;

	//! Constructor

	newmapping() : style(YAML_ANY_MAPPING_STYLE),
		       implicit(true)
	{
	}

	//! Construct a mapping node in a new YAML document.

	template<typename functor_type>
	static newnode create(functor_type &&functor)
	{
		return ref<newmappingnodeObj<container_type,
					     typename std::remove_reference
					     <functor_type>::type> >
			::create(functor);
	}
};

#if 0
{
	{
#endif
	}
}

#endif
