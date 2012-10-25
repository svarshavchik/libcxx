/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newsequencenode_H
#define x_yaml_newsequencenode_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/ref.H>
#include <x/yaml/newsequencenodefwd.H>
#include <x/yaml/newsequencenodeobj.H>

#include <type_traits>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! A sequence in a new YAML document written by writer::write().

//! The template parameter specifies the container type that holds a sequence
//! of \ref newnode "nodes". The default container type is
//! std::list<const_newnode>, but any container that iterates over
//! \ref const_newnode "const_newnode"s or
//! \ref newnode "newnode"s can be specified.
//!
//! create() constructs a \ref newnode "node" that writes a YAML sequence.
//!
//! \code
//! return INSERT_LIBX_NAMESPACE::yaml::newsequence<>::create([]
//!     (INSERT_LIBX_NAMESPACE::yaml::newsequence<> &info)
//!     {
//!         info.anchor="fruits";
//!         info.container.push_back(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("apple"));
//!         info.container.push_back(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("pear"));
//!         info.container.push_back(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("cherry"));
//!      });
//! \endcode
//!
//! create() takes a functor as an argument. The functor
//! receives an instance of the \ref newsequence "newsequence" type,
//! by reference. This object has the following members:
//!
//! - \ref newsequence::container "container": the container type specified
//! by the template parameter (which defaults to
//! \c std::list<const_newnode> ).
//!
//! - \ref newsequence::anchor "anchor": an optional anchor for an alias
//! reference to this sequence node.
//!
//! - \ref newsequence::tag "tag": an optional tag for this node.
//!
//! - \ref newsequence::style "style": formatting style.
//!
//! - \ref newsequence::implicit "implicit": whether the implicit tag can
//! be ommited.
//!
//! The functor should fill the
//! \ref container "container" with the sequence, and optionally set the
//! other class members. The functor gets called to construct the new sequence,
//! for the new YAML document.

template<typename container_type>
struct newsequence {

	//! The container for the sequence.
	container_type container;

	//! Node's anchor, if set.
	std::string anchor;

	//! Node's tag
	std::string tag;

	//! The formatting style, from the LibYAML library.
	yaml_sequence_style_t style;

	//! If the tag may be omitted.
	bool implicit;

	newsequence() : style(YAML_ANY_SEQUENCE_STYLE),
			implicit(true)
	{
	}

	//! Construct a sequence node in a new YAML document.

	template<typename functor_type>
	static newnode create(functor_type &&functor)
	{
		return ref<newsequencenodeObj<container_type,
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
