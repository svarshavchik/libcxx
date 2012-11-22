/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mime/structured_content_header.H"
#include "x/mime/rfc2047.H"
#include "x/http/form.H"
#include "x/iconviofilter.H"
#include "chrcasecmp.H"
#include <algorithm>
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
	namespace mime {
#if 0
	};
};
#endif

const char structured_content_header::content_type[]="Content-Type";

const char structured_content_header::content_disposition[]="Content-Disposition";
const char structured_content_header::content_transfer_encoding[]=
	"Content-Transfer-Encoding";

const char structured_content_header::application_x_www_form_urlencoded[]=
	"application/x-www-form-urlencoded";

const char structured_content_header::multipart_form_data[]=
	"multipart/form-data";

class structured_content_header::parser {

	structured_content_header &h;

public:

	void (structured_content_header::parser::*next_func)(const std::string &,
						       char);

	parser(structured_content_header &hArg) noexcept;

	void operator()(const std::string &name,
			char terminator);

	void parse_value(const std::string &name,
			 char terminator)
		LIBCXX_HIDDEN;
	void parse_param(const std::string &name,
			 char terminator)
		LIBCXX_HIDDEN;
};

structured_content_header::parser::parser(structured_content_header &hArg) noexcept
	: h(hArg), next_func(&parser::parse_value)
{
}

void structured_content_header::parser::operator()(const std::string &name,
			char terminator)
{
	(this->*next_func)(name, terminator);
}

void structured_content_header::parser::parse_value(const std::string &name,
						    char terminator)
{
	h.value=name;
	next_func=&parser::parse_param;
}

void structured_content_header::parser::parse_param(const std::string &name,
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

structured_content_header::structured_content_header() noexcept
{
}

structured_content_header::~structured_content_header() noexcept
{
}

structured_content_header::structured_content_header(const headersbase &req,
						     const std::string &name)

{
	req.process(name,
		    [this]
		    (std::string::const_iterator b,
		     std::string::const_iterator e)
		    {
			    this->operator=(std::string(b, e));
		    });
}

structured_content_header::structured_content_header(const std::string &str)
{
	operator=(str);
}

structured_content_header::structured_content_header(const char *str)
{
	operator=(str);
}

structured_content_header &structured_content_header::operator=(const char *str)
{
	return operator=(std::string(str));
}

structured_content_header &structured_content_header::operator=(const std::string &str)
{
	parameters.clear();

	parser p(*this);

	headersbase::parse_structured_header(headersbase::comma_or_semicolon,
					     [&]
					     (char ignore,
					      const std::string &w)
					     {
						     p(w, ignore);
					     }, str.begin(), str.end());
	return *this;
}

bool structured_content_header::operator==(const std::string &mimetype) const

{
	return chrcasecmp::str_equal_to()(value, mimetype);
}

std::string structured_content_header::mime_content_type() const
{
	auto b=value.begin(), e=value.end(), p=std::find(b, e, '/');

	std::string s(b, p);

	if (s.empty())
		s="text";

	std::transform(s.begin(),
		       s.end(),
		       s.begin(),
		       std::ptr_fun(chrcasecmp::tolower));
	return s;
}

std::string structured_content_header::mime_content_subtype() const
{
	auto b=value.begin(), e=value.end(), p=std::find(b, e, '/');

	if (p != e)
		++p;

	std::string s(p, e);

	if (s.empty())
		s="plain";

	std::transform(s.begin(),
		       s.end(),
		       s.begin(),
		       std::ptr_fun(chrcasecmp::tolower));
	return s;
}

std::string structured_content_header::charset() const
{
	auto p=parameters.find("charset");

	return p==parameters.end() ? "iso-8859-1":p->second.value;
}

std::string structured_content_header::boundary() const
{
	auto p=parameters.find("boundary");

	return p==parameters.end() ? "":p->second.value;
}

std::string structured_content_header::decode_utf8(const std::string &name,
						   const std::string &native_charset)
	const
{
	std::string value;

	// Just scan the parameters for RFC 2231-encoded parameter
	// fragments.

	std::map<int, std::string> rfc2231_strings;

	std::string rfc2231_prefix=name + "*";

	for (const auto &param:parameters)
	{
		if (param.second.name.substr(0, rfc2231_prefix.size())
		    != rfc2231_prefix)
			continue;

		bool encoded=true;
		int n=0;
			
		std::string suffix=
			param.second.name.substr(rfc2231_prefix.size());

		if (!suffix.empty()) // Else: just name*
		{
			// <n> or <n>*

			if (suffix.find('*') == std::string::npos)
				encoded=false; // Not an encoded one.

			if ((std::istringstream(suffix) >> n).fail())
				continue; // Garbage
		}

		std::string value=param.second.value;

		if (encoded)
		{
			std::ostringstream o;

			for (std::string::iterator b=value.begin(),
				     e=value.end(); b != e; )
			{
				if (*b != '%')
				{
					o << *b++;
					continue;
				}
				auto res=http::form::decode_nybble(++b,
								   e);

				b=res.first;
				o << (char)res.second;
			}
			value=o.str();
		}
		rfc2231_strings[n]=value;
	}

	// If no RFC 2231 strings were found, look for a plain parameter.

	if (rfc2231_strings.empty())
	{
		auto param=parameters.find(name);

		if (param != parameters.end())
			value=mime::from_rfc2047_as_utf8(param->second.value,
							 native_charset);
	}
	else
	{
		// Continue with the RFC 2231 decoding.

		std::string s;

		for (const auto &string:rfc2231_strings)
		{
			s += string.second;
		}

		std::string::iterator b=s.begin(), e=s.end(),
			p=std::find(b, e, '\'');

		std::string charset(b, p);

		if (p != e) ++p;

		b=std::find(p, e, '\'');

		std::string language(p, b); // TODO

		if (b != e) ++b;

		value=iconviofilter::from_u32string
			(iconviofilter::to_u32string(std::string(b, e),
						     charset),
			 "UTF-8");
	}
			
	return value;
}


#if 0
{
	{
#endif
	}
}
