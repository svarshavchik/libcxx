/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/writelock.H"
#include "x/xml/newdtd.H"
#include "x/to_string.H"
#include "x/uriimpl.H"

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

writelockObj::writelockObj()=default;

writelockObj::~writelockObj()=default;

newdtd writelockObj::create_external_dtd(const std::string &external_id,
					 const char *system_id)
{
	return create_external_dtd(external_id, std::string(system_id));
}

newdtd writelockObj::create_external_dtd(const std::string &external_id,
					 const uriimpl &system_id)
{
	return create_external_dtd(external_id, to_string(system_id));
}

newdtd writelockObj::create_internal_dtd(const std::string &external_id,
					 const char *system_id)
{
	return create_internal_dtd(external_id, std::string(system_id));
}

newdtd writelockObj::create_internal_dtd(const std::string &external_id,
					 const uriimpl &system_id)
{
	return create_internal_dtd(external_id, to_string(system_id));
}

#if 0
{
	{
#endif
	}
}
