/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/createnode.H"

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

#define LIBCXX_TEMPLATE_DECL
#include "x/xml/createnode_to.H"

createnodeObj::createnodeObj()=default;

createnodeObj::~createnodeObj()=default;


createnode createnodeObj::attribute(const new_attribute &attr)
{
	do_attribute(attr);
	return createnode{this};
}

createnode createnodeObj::create_namespace(const std::string &prefix,
					   const std::string &uri)
{
	do_create_namespace(prefix, uri);
	return createnode{this};
}

createnode createnodeObj::create_namespace(const std::string &prefix,
					   const uriimpl &uri)
{
	do_create_namespace(prefix, uri);
	return createnode{this};
}

createnode createnodeObj::create_namespace(const std::string &prefix,
					   const char *uri)
{
	do_create_namespace(prefix, uri);

	return createnode{this};
}

createnode createnodeObj::element(const new_element &e,
				  const std::vector<new_attribute> &a)
{
	element(e);

	for (const auto &aa:a)
	{
		do_attribute(aa);
	}
	return createnode{this};
}

createnode createnodeObj::comment(const std::string &comment)
{
	do_comment(comment);
	return createnode{this};
}

createnode createnodeObj::processing_instruction(const std::string &name,
						 const std::string &content)
{
	do_processing_instruction(name, content);
	return createnode{this};
}

createnode createnodeObj::set_base(const std::string &uri)
{
	do_set_base(uri);
	return createnode{this};
}

createnode createnodeObj::set_base(const char *uri)
{
	do_set_base(uri);
	return createnode{this};
}

createnode createnodeObj::set_base(const uriimpl &uri)
{
	do_set_base(uri);
	return createnode{this};
}


createnode createnodeObj::set_lang(const std::string &lang)
{
	do_set_lang(lang);
	return createnode{this};
}

createnode createnodeObj::set_space_preserve(bool flag)
{
	do_set_space_preserve(flag);
	return createnode{this};
}

createnode createnodeObj::parent()
{
	do_parent();
	return createnode{this};
}

#if 0
{
#endif
}
