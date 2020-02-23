/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_mappingnodeobj_H
#define x_yaml_mappingnodeobj_H

#include <x/namespace.h>
#include <x/yaml/nodeobj.H>
#include <x/yaml/nodefwd.H>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! A parsed mapping node in a yaml document

class mappingnodeObj : public nodeObj {

	//! The underlying node

	docnode n;

public:
	//! Constructor
	mappingnodeObj(const docnode &nArg,
		       yaml_node_type_t nodetypeArg,
		       const std::string &tagArg);

	//! Destructor

	~mappingnodeObj();

	//! Return the number of elements in the mapping
	size_t size() const;

	//! Return a mapping given a 0-based index
	std::pair<node, node> get(size_t i) const;

	//! Iterate over the mapping.

	template<typename functor_type>
	void for_each(functor_type &&functor);

	friend class documentObj;

	class iterator;

	//! The iterator is a constant iterator
	typedef iterator const_iterator;

	//! Return the beginning iterator for the sequence
	iterator begin() const;

	//! Return the ending iterator for the sequence
	iterator end() const;

};

#if 0
{
	{
#endif
	}
}

#endif
