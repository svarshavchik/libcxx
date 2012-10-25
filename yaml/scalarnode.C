/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/yaml/scalarnodeobj.H"
#include "x/yaml/docnode.H"
#include "x/yaml/document.H"

namespace LIBCXX_NAMESPACE {
	namespace yaml {
#if 0
	}
};
#endif

scalarnodeObj::scalarnodeObj(const docnode &nArg,
			     yaml_node_type_t nodetypeArg,
			     const std::string &tagArg)
	: nodeObj(nodetypeArg, tagArg),
	  value ( ({
				  documentObj::readlock_t lock(nArg->doc->doc);

				  auto ptr=nArg->getNode(lock);

				  std::string(ptr->data.scalar.value,
					      ptr->data.scalar.value
					      + ptr->data.scalar.length);
			  }))
{
}

scalarnodeObj::~scalarnodeObj() noexcept
{
}

#if 0
{
	{
#endif
	};
};
