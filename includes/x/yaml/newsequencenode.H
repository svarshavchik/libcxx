/*
** Copyright 2012-2019 Double Precision, Inc.
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

//! \code
//! return INSERT_LIBX_NAMESPACE::yaml::newsequence::create([]
//!     (INSERT_LIBX_NAMESPACE::yaml::newsequence &info)
//!     {
//!         info.anchor="fruits";
//!         std::list<INSERT_LIBX_NAMESPACE::yaml::newscalarnode> list;
//!         list.push_back(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("apple"));
//!         list.push_back(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("pear"));
//!         list.push_back(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("cherry"));
//!         info(list.begin(), list.end());
//!      });
//! \endcode
//!
//! create() takes a functor as an argument. The functor
//! receives an instance of the \ref newsequence "newsequence" type,
//! by reference. This object has the following members:
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
//! The functor optionally sets these class members, then invokes the %c ()
//! operator. The functor gets called to construct the new sequence,
//! for the new YAML document.
//!
//! The %c () operator takes a beginning and an ending iterator
//! of an input sequence over \ref newnode "node"s
//! or \ref const_newnode "const_newnode"s, which define the contents
//! of the sequence in the YAML document.

class newsequence {

	//! The writer object
	writerObj &obj;

	//! Constructor invoked by newsequencenodeObj

	newsequence(writerObj &objArg) : obj(objArg),
					 style(YAML_ANY_SEQUENCE_STYLE),
					 implicit(true)
	{
	}

public:
	template<typename functor_type> friend class newsequencenodeObj;

	//! Node's anchor, if set.
	std::string anchor;

	//! Node's tag
	std::string tag;

	//! The formatting style, from the LibYAML library.
	yaml_sequence_style_t style;

	//! If the tag may be omitted.
	bool implicit;

	//! Construct a sequence node in a new YAML document.

	template<typename functor_type>
	static newnode create(functor_type &&functor)
	{
		return ref<newsequencenodeObj<typename std::remove_reference
					      <functor_type>::type> >
			::create(functor);
	}

	//! Invoked by the functor callback to emit a sequence.

	template<typename iter_type>
	void operator()(//! Beginning iterator
			iter_type beg_iter,

			//! Ending iterator
			iter_type end_iter)
	{
		obj.write_sequence_start(anchor, tag, implicit, style);

		while (beg_iter != end_iter)
		{
			const_newnode(*beg_iter)->write(obj);
			++beg_iter;
		}
		obj.write_sequence_end();
	}
};

#if 0
{
	{
#endif
	}
}

#endif
