/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "xml_internal.h"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

newdtdObj::newdtdObj()
{
}

newdtdObj::~newdtdObj()
{
}

void newdtdObj::create_general_entity(const std::string &name,
				      const std::string &value)
{
	create_entity(name, (int)XML_INTERNAL_GENERAL_ENTITY, "", "", value);
}

void newdtdObj::create_parsed_entity(const std::string &name,
				     const std::string &external_id,
				     const std::string &system_id)
{
	create_entity(name, (int)XML_EXTERNAL_GENERAL_PARSED_ENTITY,
		      external_id, system_id, "");
}

void newdtdObj::create_unparsed_entity(const std::string &name,
				       const std::string &external_id,
				       const std::string &system_id,
				       const std::string &ndata)
{
	create_entity(name, (int)XML_EXTERNAL_GENERAL_UNPARSED_ENTITY,
		      external_id, system_id, ndata);
}

void newdtdObj::create_internal_parameter_entity(const std::string &name,
						 const std::string &value)
{
	create_entity(name, (int)XML_INTERNAL_PARAMETER_ENTITY,
		      "", "", value);
}

void newdtdObj::create_external_parameter_entity(const std::string &name,
						 const std::string &external_id,
						 const std::string &system_id)
{
	create_entity(name, (int)XML_EXTERNAL_PARAMETER_ENTITY,
		      external_id, system_id, "");
}

newdtdimplObj::newdtdimplObj(const subset_impl_t &subset_implArg,
			     const ref<impldocObj> &implArg,
			     const writelock &lockArg)
	: dtdimplObj(subset_implArg, implArg, lockArg), lock(lockArg)
{
}

newdtdimplObj::~newdtdimplObj()
{
}

void newdtdimplObj::create_entity(const std::string &name,
				  int type,
				  const std::string &external_id,
				  const std::string &system_id,
				  const std::string &content)
{
	xmlEntityPtr ptr;

	{
		error_handler::error trap_errors;

		ptr=(*subset_impl.add_entity)
			(impl->p,
			 to_xml_char{name, *this},
			 type,
			 to_xml_char_or_null{external_id, *this},
			 to_xml_char_or_null{system_id, *this},
			 to_xml_char_or_null{content, *this});
		trap_errors.check();
	}
	if (!ptr)
		throw EXCEPTION(libmsg(_txt("Cannot create a new entity")));
}

void newdtdimplObj::include_parameter_entity(const std::string &name)
{
	auto dtd=get_dtd_not_null();

	auto str= "%" + name + ";";

	auto nodeptr=xmlNewText(to_xml_char{str, *this});
	if (!nodeptr)
		throw EXCEPTION(libmsg(_txt("xmlNewText failed")));

	if (!xmlAddChild(reinterpret_cast<xmlNodePtr>(dtd), nodeptr))
	{
		xmlFreeNode(nodeptr);
		throw EXCEPTION(libmsg(_txt("xmlAddChild failed")));
	}
}

#if 0
{
#endif
}
