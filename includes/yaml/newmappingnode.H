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

//! \code
//! return INSERT_LIBX_NAMESPACE::yaml::newmapping::create([]
//!     (INSERT_LIBX_NAMESPACE::yaml::newmapping &info)
//!     {
//!         info.anchor="capitals";
//!         std::list<std::pair<INSERT_LIBX_NAMESPACE::yaml::newnode,
//!                             INSERT_LIBX_NAMESPACE::yaml::newnode>> list;
//!
//!         list.push_back(std::make_pair(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("New Jersey"), INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("Trenton")));
//!         list.push_back(std::make_pair(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("New York"), INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("Albany")));
//!      });
//! \endcode
//!
//! create() takes a functor as an argument. The functor
//! receives an instance of the \ref newmapping "newmapping" type,
//! by reference. This object has the following members:
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
//! The functor optionally sets these class members, then invokes the %c ()
//! operator. The functor gets called to construct the new mapping,
//! for the new YAML document.
//!
//! The %c () operator takes a beginning and an ending iterator
//! of an input sequence over a \c std::pair of \ref newnode "node"s
//! or \ref const_newnode "const_newnode"s, which define the contents
//! of the mapping the YAML document. Each \c std::pair represents a
//! single mapping entry.

class newmapping {

	//! The writer object
	writerObj &obj;

	//! Constructor invoked by newsequencenodeObj

	newmapping(writerObj &objArg) : obj(objArg),
					style(YAML_ANY_MAPPING_STYLE),
					implicit(true)
	{
	}
public:
	template<typename functor_type> friend class newmappingnodeObj;

	//! Node's anchor, if set.

	std::string anchor;

	//! Node's tag

	std::string tag;

	//! The formatting style, from the LibYAML library.
	yaml_mapping_style_t style;

	//! If the tag may be omitted.
	bool implicit;

	//! Construct a mapping node in a new YAML document.

	template<typename functor_type>
	static newnode create(functor_type &&functor)
	{
		return ref<newmappingnodeObj<typename std::remove_reference
					     <functor_type>::type> >
			::create(functor);
	}

	//! Invoked by the functor callback to emit a mapping.

	template<typename iter_type>
	void operator()(//! Beginning iterator
			iter_type beg_iter,

			//! Ending iterator
			iter_type end_iter)
	{
		obj.write_mapping_start(anchor, tag, implicit, style);

		while (beg_iter != end_iter)
		{
			const_newnode(beg_iter->first)->write(obj);
			const_newnode(beg_iter->second)->write(obj);
			++beg_iter;
		}
		obj.write_mapping_end();
	}
};

#if 0
{
	{
#endif
	}
}

#endif
