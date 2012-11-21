/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http/content_type_header.H"
#include "chrcasecmp.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

const char content_type_header::name[]="Content-Type";

const char content_type_header::application_x_www_form_urlencoded[]=
	"application/x-www-form-urlencoded";

class content_type_header::parser {

	content_type_header &h;

public:

	void (content_type_header::parser::*next_func)(const std::string &,
						       char);

	parser(content_type_header &hArg) noexcept;

	void operator()(const std::string &name,
			char terminator);

	void parse_typesubtype(const std::string &name,
			       char terminator)
		LIBCXX_HIDDEN;
	void parse_param(const std::string &name,
			 char terminator)
		LIBCXX_HIDDEN;
};

content_type_header::parser::parser(content_type_header &hArg) noexcept
	: h(hArg), next_func(&parser::parse_typesubtype)
{
}

void content_type_header::parser::operator()(const std::string &name,
			char terminator)
{
	(this->*next_func)(name, terminator);
}

void content_type_header::parser::parse_typesubtype(const std::string &name,
					      char terminator)
{
	size_t p=name.find('/');

	if (p != std::string::npos)
	{
		h.subtype=name.substr(p+1);
		h.type=name.substr(0, p);
	}
	else
	{
		h.type=name;
		h.subtype="*";
	}

	next_func=&parser::parse_param;
}

void content_type_header::parser::parse_param(const std::string &name,
					      char terminator)
{
	parameter_t newparam;

	newparam.name=name;

	size_t p=newparam.name.find('=');

	if (p != std::string::npos)
	{
		newparam.value=newparam.name.substr(p+1);
		newparam.name=newparam.name.substr(0, p);
	}

	h.parameters.insert(std::make_pair(newparam.name, newparam));
}

content_type_header::content_type_header() noexcept
{
}

content_type_header::~content_type_header() noexcept
{
}

content_type_header::content_type_header(const headersbase &req)

{
	operator=(req);
}

content_type_header &content_type_header::operator=(const headersbase &req)

{
	parameters.clear();

	parser p(*this);

	req.process(name,
		    [&]
		    (std::string::const_iterator b,
		     std::string::const_iterator e)
		    {
			    req.parse_structured_header(headersbase::
							comma_or_semicolon,
							[&]
							(char ignore,
							 const std::string &w)
							{
								p(w, ignore);
							}, b, e);
		    });
	return *this;
}

content_type_header::content_type_header(const std::string &str)
{
	operator=(str);
}

content_type_header::content_type_header(const char *str)
{
	operator=(str);
}

content_type_header &content_type_header::operator=(const char *str)
{
	return operator=(std::string(str));
}

content_type_header &content_type_header::operator=(const std::string &str)

{
	headersimpl<headersbase::crlf_endl> headers;

	headers.replace(name, str);
	return operator=(headers);
}

bool content_type_header::operator==(const std::string &mimetype) const

{
	return chrcasecmp::str_equal_to()(type + "/" + subtype, mimetype);
}

#if 0
{
	{
#endif
	}
}
