/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_sequencenodeobj_H
#define x_yaml_sequencenodeobj_H

#include <x/namespace.h>
#include <x/yaml/nodeobj.H>
#include <x/yaml/nodefwd.H>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

//! A sequence node in a parsed yaml document

class sequencenodeObj : public nodeObj {

	//! The underlying node

	docnode n;

public:
	//! Constructor
	sequencenodeObj(const docnode &nArg,
			yaml_node_type_t nodetypeArg,
			const std::string &tagArg);

	//! Destructor

	~sequencenodeObj();

	//! Return the number of elements in the sequence
	size_t size() const;

	//! Return a value in the sequence given a 0-based index
	node get(size_t i) const;

	//! Iterate over the mapping.

	//! This produces slightly more efficient code than using iterators.

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
#endif
}

#endif
