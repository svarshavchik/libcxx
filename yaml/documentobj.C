/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/yaml/documentobj.H"
#include "x/yaml/scalarnode.H"
#include "x/yaml/mappingnode.H"
#include "x/yaml/sequencenode.H"
#include "x/yaml/node.H"
#include "x/yaml/scalarnode.H"
#include "x/yaml/sequencenode.H"
#include "x/yaml/mappingnode.H"
#include "x/yaml/docnode.H"

#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
	namespace yaml {
#if 0
	};
};
#endif

#include "yaml_internal.H"

documentObj::documentObj(do_not_initialize *ignore) : doc(&y),
						      initialized(false)
{
}

documentObj::~documentObj()
{
	if (initialized)
	{
		writelock_t lock(doc);

		yaml_document_delete(*lock);
	}
}

node documentObj::root() const
{
	readlock_t lock(doc);

	auto n=ref<docnodeObj::rootnodeObj>
		::create(document(const_cast<documentObj *>(this)));

	auto yn=n->getNode(lock);

	if (!yn)
		return node::create(YAML_NO_NODE, "");

	return construct(n, *yn);
}

node documentObj::construct(yaml_node_item_t i) const
{
	auto d=ref<docnodeObj::nonrootnodeObj>
		::create(document(const_cast<documentObj *>(this)), i);

	readlock_t lock(doc);

	auto p=d->getNode(lock);

	if (!p)
		throw EXCEPTION(_("Internal error: invalid YAML node number"));

	return construct(d, *p);
}

node documentObj::construct(const docnode &d,
			    const yaml_node_t &n) const
{
	const char *tag_str=!n.tag ? "":reinterpret_cast<const char *>(n.tag);

	// Cache the various tags, to reduce the number of sting objects
	// bouncing around.
	std::string tag=({
			tags_t::lock lock(tags);

			*lock->insert(tag_str).first;
		});

	switch (n.type) {
	case YAML_SCALAR_NODE:
		return scalarnode::create(d, n.type, tag);

	case YAML_SEQUENCE_NODE:
		return sequencenode::create(d, n.type, tag);

	case YAML_MAPPING_NODE:
		return mappingnode::create(d, n.type, tag);
	default:
		break;
	}

	return node::create(n.type, tag); // ???
}

#if 0
{
	{
#endif
	}
}
