/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mime/structured_content_header.H"
#include "x/mime/rfc2047.H"
#include "x/http/form.H"
#include "x/iconviofilter.H"
#include "x/tokens.H"
#include "x/qp.H"
#include "x/base64.H"
#include "x/chrcasecmp.H"
#include <algorithm>
#include <map>
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE::mime {
#if 0
}
#endif

const char structured_content_header::content_type[]="Content-Type";

const char structured_content_header::content_disposition[]="Content-Disposition";
const char structured_content_header::content_transfer_encoding[]=
	"Content-Transfer-Encoding";

const char structured_content_header::application_x_www_form_urlencoded[]=
	"application/x-www-form-urlencoded";

const char structured_content_header::multipart_form_data[]=
	"multipart/form-data";

const char structured_content_header::message_rfc822[]=
	"message/rfc822";

const char structured_content_header::message_global[]=
	"message/global";

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

//! Callback used by format()

//! \internal
class LIBCXX_HIDDEN structured_content_header::formatwords_cb {

public:
	//! Return the next formatted word/parameter
	virtual void operator()(const std::string &s)=0;
};

template<typename header_type>
struct LIBCXX_HIDDEN structured_content_header::format_words_wrap :
	public formatwords_cb {

 public:

	headersimpl<header_type> &headers;
	std::string name;
	bool first;
	bool firstline;

	size_t maxwidth;
	size_t curwidth;

	std::string h;

	format_words_wrap(headersimpl<header_type> &headersArg,
			  const std::string &nameArg,
			  size_t maxwidthArg)
		: headers(headersArg), name(nameArg), first(true),
		firstline(true),
		maxwidth(maxwidthArg), curwidth(nameArg.size()+2)
	{
	}

	void operator()(const std::string &s) override
	{
		if (!first && maxwidth && curwidth+s.size()+1 > maxwidth)
			emit();

		if (!first)
		{
			h += " ";
			++curwidth;
		}
		first=false;
		h += s;
		curwidth += s.size();
	}

	void emit()
	{
		if (firstline)
			headers.append(name, h);
		else
			headers.append(h);
		firstline=false;

		h="   ";
		curwidth=3;
	}
};

template class LIBCXX_HIDDEN structured_content_header
::format_words_wrap<headersbase::crlf_endl>;

template class LIBCXX_HIDDEN structured_content_header
::format_words_wrap<headersbase::lf_endl>;



template<typename header_type>
void structured_content_header::append(headersimpl<header_type> &headers,
				       const std::string &name,
				       size_t maxwidth)
{
	format_words_wrap<header_type> cb(headers, name, maxwidth);
	format(cb);
	cb.emit();
}

// Instantiate for the two cases

template
void structured_content_header::append(headersimpl<headersbase::crlf_endl>
				       &headers,
				       const std::string &name,
				       size_t maxwidth);

template
void structured_content_header::append(headersimpl<headersbase::lf_endl>
				       &headers,
				       const std::string &name,
				       size_t maxwidth);

structured_content_header::structured_content_header() noexcept
{
}

structured_content_header::~structured_content_header()
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

structured_content_header &
structured_content_header::operator+=(const parameter_t &param)
{
	parameters.insert(std::make_pair(param.name, param));
	return *this;
}

structured_content_header &
structured_content_header::operator+=(parameter_t &&param)
{
	parameters.insert(std::make_pair(param.name, std::move(param)));
	return *this;
}

structured_content_header &
structured_content_header::operator()(const std::string &name,
				      const std::string &value)
{
	parameter_t param;

	param.name=name;
	param.value=value;
	return operator+=(param);
}

structured_content_header &
structured_content_header::rfc2231(const std::string &name,
				   const std::string &value,
				   const std::string &charset,
				   size_t width)
{
	return rfc2231(name, value, charset, "EN", width);
}

structured_content_header &
structured_content_header::rfc2231(const std::string &name,
				   const std::string &value,
				   const std::string &charset,
				   const std::string &language,
				   size_t width)
{
	if (std::find_if(value.begin(), value.end(),
			 []
			 (unsigned char c)
			 {
				 return c < ' ' || c > 0x7F || c == '"' || c == '\\';
			 }) == value.end() &&
	    (width == 0 || name.size()+value.size()+5 /* est */ < width))
		return operator()(name, value);

	std::string prefix=charset + "'" + language + "'";

	size_t s=prefix.size()+name.size()+12; // Estimate
	bool first=true;
	size_t cnt=0;

	std::map<int, std::string> newparams;

	for (unsigned char c:value)
	{
		bool escape=c < ' ' || c > 0x7F || c == '"' || c == '\\';

		size_t n=escape ? 3:1;

		if (width && (!first && s+n > width))
		{
			newparams[cnt++]=prefix;
			prefix.clear();
			s=name.size()+12;
		}

		if (escape)
		{
			prefix += (char)'%';
			prefix += http::form::hex[c / 16];
			prefix += http::form::hex[c % 16];
		}
		else
		{
			prefix += (char)c;
		}
		s += n;
		first=false;
	}

	if (cnt == 0)
		return operator()(name + "*", prefix);

	newparams[cnt]=prefix;

	for (const auto &p:newparams)
	{
		std::ostringstream o;

		o << name << "*" << p.first << "*";
		operator()(o.str(), p.second);
	}
	return *this;
}

structured_content_header &
structured_content_header::rfc2047(const std::string &name,
				   const std::string &value,
				   const std::string &charset)
{
	return rfc2047(name, value, charset, "");
}

struct LIBCXX_HIDDEN structured_rfc2047_encode {

	static bool encode(unsigned char c)
	{
		return c < ' ' || c > 0x7F || c == '"'
			|| c == '\\' || c == '=' || c == '?';
	}
};

structured_content_header &
structured_content_header::rfc2047(const std::string &name,
				   const std::string &value,
				   const std::string &charset,
				   const std::string &language)
{
	size_t nescapes;

	nescapes=0;

	for (char c:value)
	{
		if (structured_rfc2047_encode::encode(c))
			++nescapes;
	}

	if (nescapes == 0)
		return operator()(name, value); // No encoding necessary.

	std::ostringstream o;

	o << "=?" << charset;

	if (language.size())
		o << "*" << language;

	size_t qp_size=value.size()+nescapes*2;
	size_t b64_size=(value.size()+2)/3*4;

	if (qp_size < b64_size)
	{
		o << "?Q?";

		std::copy(value.begin(),
			  value.end(),
			  qp_encoder<std::ostreambuf_iterator<char>,
			  structured_rfc2047_encode>
			  (std::ostreambuf_iterator<char>(o), 0)).eof();
	}
	else
	{
		o << "?B?";

		std::copy(value.begin(),
			  value.end(),
			  base64<>::encoder<std::ostreambuf_iterator<char>>
			  (std::ostreambuf_iterator<char>(o), 0)).eof();
	}
	o << "?=";

	return operator()(name, o.str());
}

void structured_content_header::format(formatwords_cb &callback) const
{
	std::string word=value;

	for (const auto &parameter:parameters)
	{
		word += ";";
		callback(word);
		word.clear();
		parameter.second.to_string(std::back_insert_iterator<std::string>
					  (word));
	}
	callback(word);
}

//! Callback assembles the formatted value as a single string.

struct LIBCXX_HIDDEN structured_content_header::format_cb_string
	: public formatwords_cb {

	//! Header value collected here.

	std::ostringstream o;

	//! Flag, first callback.
	bool first;

	//! Constructor
	format_cb_string();

	//! Destructor
	~format_cb_string();

	//! Callback.
	void operator()(const std::string &s) override;
};

structured_content_header::format_cb_string::format_cb_string()
	: first(true)
{
}

structured_content_header::format_cb_string::~format_cb_string()
{
}

structured_content_header::operator std::string() const
{
	format_cb_string fcs;

	format(fcs);

	return fcs.o.str();
}

void structured_content_header::format_cb_string
::operator()(const std::string &s)
{
	if (!first)
		o << ' ';
	first=false;

	o << s;
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
		       chrcasecmp::tolower);
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
		       chrcasecmp::tolower);
	return s;
}

bool structured_content_header::is_message() const
{
	std::string s=value;

	std::transform(s.begin(),
		       s.end(),
		       s.begin(),
		       chrcasecmp::tolower);
	return s == message_rfc822 || s == message_global;
}

bool structured_content_header::is_multipart() const
{
	return mime_content_type() == "multipart";
}

std::string structured_content_header::charset(const std::string &def) const
{
	auto p=parameters.find("charset");

	return p==parameters.end() ? def:p->second.value;
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

		value=unicode::iconvert::convert(std::string(b, e),
						 charset,
						 unicode::utf_8);
	}

	return value;
}


#if 0
{
#endif
}
