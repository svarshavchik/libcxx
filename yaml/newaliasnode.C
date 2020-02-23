/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/yaml/newaliasnodeobj.H"
#include "x/yaml/writer.H"
#include "x/ref.H"
#include "x/ptr.H"

namespace LIBCXX_NAMESPACE {
	namespace yaml {
#if 0
	}
};
#endif

newaliasnodeObj::newaliasnodeObj(const std::string &anchorArg)
	: anchor(anchorArg)
{
}

newaliasnodeObj::~newaliasnodeObj()
{
}

void newaliasnodeObj::write(writerObj &obj) const
{
	obj.write_alias(*this);
}

#if 0
{
	{
#endif
	};
};
