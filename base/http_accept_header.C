/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/accept_header.H"
#include "x/http/requestimpl.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

const char accept_header::name[]="Accept";

class accept_header::parser {

	accept_header &h;

	accept_entry_t newentry;

public:

	void (accept_header::parser::*next_func)(const std::string &,
						 char);

	parser(accept_header &hArg) noexcept;

	void operator()(const std::string &name,
			char terminator);

	void parse_typesubtype(const std::string &name,
			       char terminator)
		LIBCXX_HIDDEN;
	void parse_media_params(const std::string &name,
				char terminator)
		LIBCXX_HIDDEN;
	void parse_accept_params(const std::string &name,
				 char terminator)
		LIBCXX_HIDDEN;
};

accept_header::parser::parser(accept_header &hArg) noexcept
	: h(hArg), next_func(&parser::parse_typesubtype)
{
}

void accept_header::parser::operator()(const std::string &name,
				       char terminator)
{
	(this->*next_func)(name, terminator);
}

void accept_header::parser::parse_typesubtype(const std::string &name,
					      char terminator)
{
	newentry=accept_entry_t();

	size_t p=name.find('/');

	if (p != std::string::npos)
	{
		newentry.subtype=name.substr(p+1);
		newentry.type=name.substr(0, p);
	}
	else
	{
		newentry.type=name;
		newentry.subtype="*";
	}

	if (terminator != ',')
		next_func=&parser::parse_media_params;
	else
		h.list.insert(newentry);
}

void accept_header::parser::parse_media_params(const std::string &name,
					       char terminator)
{
	mime::parameter_t newparam;

	newparam.name=name;

	size_t p=newparam.name.find('=');

	if (p != std::string::npos)
	{
		newparam.value=newparam.name.substr(p+1);
		newparam.name=newparam.name.substr(0, p);
	}

	if (newparam.name == "q")
	{
		int qv=0;
		int dec_pt= -1;

		for (std::string::iterator
			     b(newparam.value.begin()),
			     e(newparam.value.end()); b != e;
		     ++b)
		{
			if (*b == ' ')
				continue;

			if (*b == '.')
			{
				if (dec_pt >= 0)
				{
					qv=0;
					break;
				}
				dec_pt=0;
				continue;
			}

			if (*b < '0' || *b > '9')
			{
				qv=0;
				break;
			}

			if (dec_pt >= 3)
				continue;

			qv=qv * 10 + (*b - '0');
			if (dec_pt >= 0)
				++dec_pt;
		}

		if (dec_pt < 0)
			dec_pt=0;
		while (dec_pt < 3)
		{
			qv *= 10;
			++dec_pt;
		}
		newentry.qvalue=qv;
		next_func=&parser::parse_accept_params;
	}
	else
	{
		newentry.media_parameters.insert(newparam);
	}

	if (terminator == ',')
	{
		h.list.insert(newentry);
		next_func=&parser::parse_typesubtype;
	}
}

void accept_header::parser::parse_accept_params(const std::string &name,
						char terminator)

{
	mime::parameter_t newparam;

	newparam.name=name;

	size_t p=newparam.name.find('=');

	if (p != std::string::npos)
	{
		newparam.value=newparam.name.substr(p+1);
		newparam.name=newparam.name.substr(0, p);
	}

	newentry.accept_params.insert(newparam);
	if (terminator == ',')
	{
		h.list.insert(newentry);
		next_func=&parser::parse_typesubtype;
	}
}

accept_header::accept_header() noexcept
{
}

accept_header::~accept_header()
{
}

accept_header::accept_header(const headersbase &req)
{
	operator=(req);
}

accept_header &accept_header::operator=(const headersbase &req)
{
	list.clear();

	parser p(*this);

	req.process
		(name,
		 [&]
		 (std::string::const_iterator b,
		  std::string::const_iterator e)
		 {
			 headersbase::parse_structured_header
				 (headersbase::comma_or_semicolon,
				  [&p]
				  (char sep,
				   const std::string &str)
				  {
					  if (sep == 0)
						  sep=',';

					  p(str, sep);
				  }, b, e);
		 });

	return *this;
}

accept_header::accept_header(const std::string &str)
{
	operator=(str);
}

accept_header::accept_header(const char *str)
{
	operator=(str);
}

accept_header &accept_header::operator=(const char *str)
{
	return operator=(std::string(str));
}

accept_header &accept_header::operator=(const std::string &str)
{
	headersimpl<headersbase::crlf_endl> headers;

	headers.replace(name, str);
	return operator=(headers);
}

accept_header::media_type_t::media_type_t(const std::string &value)

{
	init(value);
}

accept_header::media_type_t::media_type_t(const char *value)

{
	init(value);
}

void accept_header::media_type_t::init(const std::string &value)

{
	headersimpl<headersbase::crlf_endl> headers;

	headers[name]=value;

	accept_header parsed(headers);

	accept_header::list_t::const_iterator
		b(parsed.list.begin()), e(parsed.list.end()), p=b;

	if (b == e || ++b != e)
		throw EXCEPTION(libmsg(_txt("Cannot parse media type")));

	*this= *p;
}


#if 0
{
#endif
}
