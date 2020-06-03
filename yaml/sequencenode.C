/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/yaml/sequencenode.H"
#include "x/yaml/document.H"
#include "x/yaml/docnode.H"

#include "gettext_in.h"

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

sequencenodeObj::sequencenodeObj(const docnode &nArg,
				 yaml_node_type_t nodetypeArg,
				 const std::string &tagArg)
	: nodeObj(nodetypeArg, tagArg), n(nArg)
{
}

sequencenodeObj::~sequencenodeObj()
{
}

size_t sequencenodeObj::size() const
{
	documentObj::readlock_t lock(n->doc->doc);

	auto ptr=n->getNode(lock);

	return ptr->data.sequence.items.top - ptr->data.sequence.items.start;
}

node sequencenodeObj::get(size_t i) const
{
	yaml_node_item_t item=({
			documentObj::readlock_t lock(n->doc->doc);

			auto ptr=n->getNode(lock);

			if (i >= (size_t)(ptr->data.sequence.items.top -
					  ptr->data.sequence.items.start))
				throw EXCEPTION(_("YAML sequence index out of range"));

			ptr->data.sequence.items.start[i];
		});

	return n->doc->construct(item);
}

sequencenodeObj::iterator sequencenodeObj::begin() const
{
	return iterator(const_sequencenodeptr(this), 0);
}

sequencenodeObj::iterator sequencenodeObj::end() const
{
	return iterator(const_sequencenodeptr(this), size());
}

sequencenodeObj::iterator::iterator()
{
}

sequencenodeObj::iterator::iterator(const_sequencenodeptr &&seqArg, size_t iArg)
	: seq(std::move(seqArg)), i(iArg)
{
}

sequencenodeObj::iterator::~iterator()
{
}

#if 0
{
#endif
}
