/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newdocumentnode_H
#define x_yaml_newdocumentnode_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/ref.H>
#include <x/yaml/newdocumentnodefwd.H>
#include <x/yaml/newdocumentnodeobj.H>

#include <type_traits>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! Implement \ref newdocumentnode "newdocumentnode"'s root() by calling a functor.

//! Template class for a subclass of newdocumentnodeObj that implements the
//! whose \ref newdocumentnodeObj::root "root()" method by invoking a
//! functor, that should return the root node of the YAML document to write.

template<typename functor_type>
class newdocumentnode_functorObj : public newdocumentnodeObj {

	//! The functor which should return the new YAML document's root node.

	mutable functor_type functor;

	//! Implement root().

	newnode root() const override
	{
		return functor();
	}
public:

	//! Constructor
	newdocumentnode_functorObj(functor_type &&functorArg)
		: functor(std::forward<functor_type>(functorArg))
	{
	}

	//! Destructor
	~newdocumentnode_functorObj() noexcept
	{
	}
};


//! Implement \ref newdocumentnode "newdocumentnode"'s root() by calling a functor.

//! make_newdocumentnode() returns a \ref newdocumentnode "newdocumentnode"
//! whose \ref newdocumentnodeObj::root "root()" method invokes the supplied
//! functor, that should return the root
//! \ref newnode "node" of the YAML document to write.
//!
//! \code
//! std::string s;
//!
//! INSERT_LIBX_NAMESPACE::yaml::writer::write_one_document(INSERT_LIBX_NAMESPACE::yaml::make_newdocumentnode([]
//!       {
//!           return INSERT_LIBX_NAMESPACE::yaml::newmapping<>::create([]
//!                (INSERT_LIBX_NAMESPACE::yaml::newmapping<> &info)
//!                {
//!                    info.container.push_back(std::make_pair(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("fruit"),
//!                                                            INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("apple")));
//!                    info.container.push_back(std::make_pair(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("carrot"),
//!                                                            INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("vegetable")));
//!                });
//!       }),
//!       std::back_insert_iterator<std::string>(s));
//!
//! std::cout << s << std::endl;
//! \endcode
//!
//! This example passes a functor to make_newdocumentnode() that returns
//! a mapping node constructed by make_newmappingnode().
//! \ref writer "writer::write_one_document"() then writes the document
//! to an iterator that appends the written document to a string.

template<typename functor_type>
newdocumentnode make_newdocumentnode(functor_type &&functor)
{
	return ref< newdocumentnode_functorObj<
			    typename std::remove_reference<functor_type>
			    ::type> >
		::create(std::forward<functor_type>(functor));
}

#if 0
{
	{
#endif
	}
}

#endif
