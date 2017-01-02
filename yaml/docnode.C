/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/yaml/docnode.H"

namespace LIBCXX_NAMESPACE {
	namespace yaml {
#if 0
	}
};
#endif

#include "yaml_internal.H"

docnodeObj::rootnodeObj::rootnodeObj(const document &docArg) : docnodeObj(docArg)
{
}

docnodeObj::rootnodeObj::~rootnodeObj()
{
}

const yaml_node_t *docnodeObj::rootnodeObj::getNode(const documentObj::readlock_t
						 &lock)
{
	return yaml_document_get_root_node(*lock);
}

yaml_node_t *docnodeObj::rootnodeObj::getNode(const documentObj::writelock_t
					   &lock)
{
	return yaml_document_get_root_node(*lock);
}

docnodeObj::nonrootnodeObj::nonrootnodeObj(const document &docArg,
					yaml_node_item_t itemArg)
	: docnodeObj(docArg), item(itemArg)
{
}

docnodeObj::nonrootnodeObj::~nonrootnodeObj()
{
}

const yaml_node_t *docnodeObj::nonrootnodeObj::getNode(const
						    documentObj::readlock_t
						    &lock)
{
	return yaml_document_get_node(*lock, item);
}

yaml_node_t *docnodeObj::nonrootnodeObj::getNode(const documentObj::writelock_t
					      &lock)
{
	return yaml_document_get_node(*lock, item);
}

docnodeObj::docnodeObj(const document &docArg) : doc(docArg)
{
}

docnodeObj::~docnodeObj()
{
}

#if 0
{
	{
#endif
	};
};
