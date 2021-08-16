/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/yaml/mappingnode.H"
#include "x/yaml/node.H"
#include "x/yaml/docnode.H"
#include "x/yaml/document.H"
#include <x/exception.H>

#include "gettext_in.h"

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

mappingnodeObj::mappingnodeObj(const docnode &nArg,
			       yaml_node_type_t nodetypeArg,
			       const std::string &tagArg)
	: nodeObj(nodetypeArg, tagArg), n(nArg)
{
}

mappingnodeObj::~mappingnodeObj()
{
}

size_t mappingnodeObj::size() const
{
	documentObj::readlock_t lock(n->doc->doc);

	auto ptr=n->getNode(lock);

	return ptr->data.mapping.pairs.top - ptr->data.mapping.pairs.start;
}

std::pair<node, node> mappingnodeObj::get(size_t i) const
{
	yaml_node_pair_t pair=({
			documentObj::readlock_t lock(n->doc->doc);

			auto ptr=n->getNode(lock);

			if (i >= (size_t)(ptr->data.mapping.pairs.top -
					  ptr->data.mapping.pairs.start))
				throw EXCEPTION(_("YAML mapping index out of range"));

			ptr->data.mapping.pairs.start[i];
		});

	return std::make_pair(n->doc->construct(pair.key),
			      n->doc->construct(pair.value));
}

mappingnodeObj::iterator mappingnodeObj::begin() const
{
	return iterator(const_mappingnodeptr(this), 0);
}

mappingnodeObj::iterator mappingnodeObj::end() const
{
	return iterator(const_mappingnodeptr(this), size());
}

mappingnodeObj::iterator::iterator()
{
}

mappingnodeObj::iterator::iterator(const_mappingnodeptr &&mapArg, size_t iArg)
	: map(std::move(mapArg)), i(iArg)
{
}

mappingnodeObj::iterator::~iterator()
{
}

#if 0
{
#endif
}
