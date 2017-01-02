/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "xml_internal.h"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

dtdObj::dtdObj()
{
}

dtdObj::~dtdObj()
{
}

const dtdimplObj::subset_impl_t dtdimplObj::impl_external = {
	&dtdimplObj::get_external_dtd,
	&xmlAddDtdEntity,
};

const dtdimplObj::subset_impl_t dtdimplObj::impl_internal = {
	&dtdimplObj::get_internal_dtd,
	&xmlAddDocEntity,
};

dtdimplObj::dtdimplObj(const subset_impl_t &subset_implArg,
		       const ref<impldocObj> &implArg,
		       const doc::base::readlock lockArg)
	: subset_impl(subset_implArg),
	  impl(implArg),
	  lock(lockArg)
{
}

dtdimplObj::~dtdimplObj()
{
}

xmlDtdPtr dtdimplObj::get_external_dtd()
{
	return impl->p->extSubset;
}

xmlDtdPtr dtdimplObj::get_internal_dtd()
{
	return xmlGetIntSubset(impl->p);
}

xmlDtdPtr dtdimplObj::get_dtd_not_null()
{
	auto dtd=(this->*(subset_impl.get_dtd))();

	if (!dtd)
		throw EXCEPTION(libmsg(_txt("Document type is not defined")));
	return dtd;
}

bool dtdimplObj::exists()
{
	return !!(this->*(subset_impl.get_dtd))();
}

std::string dtdimplObj::name()
{
	return get_str(get_dtd_not_null()->name);
}

std::string dtdimplObj::external_id()
{
	return get_str(get_dtd_not_null()->ExternalID);
}

std::string dtdimplObj::system_id()
{
	return get_str(get_dtd_not_null()->SystemID);
}

void dtdimplObj::notwrite()
{
	throw EXCEPTION(libmsg(_txt("Somehow you ended up calling a virtual write method on a read object. Virtual objects are not for playing.")));
}

void dtdimplObj::create_entity(const std::string &name,
			       int type,
			       const std::string &external_id,
			       const std::string &system_id,
			       const std::string &content)
{
	notwrite();
}

void dtdimplObj::include_parameter_entity(const std::string &name)
{
	notwrite();
}

#if 0
{
	{
#endif
	}
}
