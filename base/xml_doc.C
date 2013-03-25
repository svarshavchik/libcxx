/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/doc.H"
#include "x/xml/parser.H"
#include "x/fditer.H"
#include "x/fd.H"
#include "xml_internal.h"

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

impldocObj::impldocObj(xmlDocPtr pArg) : p(pArg)
{
}

impldocObj::~impldocObj() noexcept
{
	if (p)
		xmlFreeDoc(p);
}

docObj::docObj()
{
}

docObj::~docObj() noexcept
{
}

doc docBase::create()
{
	return ref<impldocObj>::create(xmlNewDoc((const xmlChar *)"1.0"));
}

doc docBase::create(const std::string &filename)
{
	return create(filename, "");
}

doc docBase::create(const std::string &filename,
		    const std::string &options)
{
	auto file=fd::base::open(filename, O_RDONLY);

	return create(fdinputiter(file), fdinputiter(), filename, options);
}

#if 0
{
	{
#endif
	}
}
