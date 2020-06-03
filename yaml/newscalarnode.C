/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/yaml/newscalarnodeobj.H"
#include "x/yaml/writer.H"
#include "x/ref.H"
#include "x/ptr.H"

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

newscalarnodeObj::newscalarnodeObj(const std::string &valueArg,
				   yaml_scalar_style_t styleArg,
				   bool plain_implicitArg,
				   bool quoted_implicitArg)
	: newscalarnodeObj("", valueArg, styleArg, plain_implicitArg,
			   quoted_implicitArg)
{
}

newscalarnodeObj::newscalarnodeObj(const std::string &tagArg,
				   const std::string &valueArg,
				   yaml_scalar_style_t styleArg,
				   bool plain_implicitArg,
				   bool quoted_implicitArg)
	: tag(tagArg), value(valueArg), style(styleArg),
	  plain_implicit(plain_implicitArg),
	  quoted_implicit(quoted_implicitArg)
{
}

newscalarnodeObj::~newscalarnodeObj()
{
}

void newscalarnodeObj::write(writerObj &obj) const
{
	obj.write_scalar(*this);
}

#if 0
{
#endif
}
