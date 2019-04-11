/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/headersbase.H"
#include "x/messages.H"
#include "gettext_in.h"
#include <cstring>
#include <algorithm>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

const char headersbase::crlf_endl::eol_str[]="\r\n";

const char headersbase::lf_endl::eol_str[]="\n";

void headersbase::parse_endofstream()
{
	throw EXCEPTION(libmsg(_txt("End of stream while parsing headers")));
}

void headersbase::parse_toomanyheaders()
{
	throw EXCEPTION(libmsg(_txt("Number of headers exceeds maximum allowed")));
}

void headersbase::invalid_char_in_token()
{
	throw EXCEPTION(libmsg(_txt("Invalid character in token")));
}

headersbase::headersbase() noexcept
{
}

headersbase::~headersbase()
{
}

std::string headersbase::header::name() const noexcept
{
	std::string::const_iterator b=std::string::begin();

	return std::string(b, std::find(b, std::string::end(), ':'));
}

std::string::const_iterator headersbase::header::begin() const noexcept
{
	std::string::const_iterator b=std::string::begin(),
		e=std::string::end();

	// To avoid end() calling begin(), trim the tail first.

	while (b != e && isspace(e[-1]))
		--e;

	b=std::find(b, e, ':');

	if (b != e)
		++b;

	while (b != e && isspace(*b))
		++b;

	return b;
}

std::string::const_iterator headersbase::header::end() const noexcept
{
	std::string::const_iterator b=std::string::begin(),
		e=std::string::end();

	while (b != e && isspace(e[-1]))
		--e;

	return e;
}

headersbase::iterator
headersbase::new_header(const std::string &line)
{
	headerlist.push_back(line);

	header_map_val_t mv(--headerlist.end());
	return headermap.insert(std::make_pair(mv.name(), mv));
}

headersbase::headersbase(const headersbase &o)
{
	*this=o;
}

headersbase &headersbase::operator=(const headersbase &o)
{
	headermap.clear();
	headerlist.clear();

	for (auto hdr: o.headerlist)
	{
		new_header(hdr);
	}
	return *this;
}

char headersbase::comma_or_semicolon(char c)
{
	return c == ',' || c == ';' ? c:0;
}

std::pair<std::string, std::string>
headersbase::name_and_value(//! The parameter
			    const std::string &word)
{
	std::pair<std::string, std::string> ret;

	size_t p=word.find('=');

	if (p == std::string::npos)
	{
		ret.first=word;
	}
	else
	{
		size_t q=p;

		while (q > 0)
		{
			switch (word[q-1]) {
			case ' ':
			case '\r':
			case '\n':
			case '\t':
				--q;
				continue;
			default:
				break;
			}
			break;
		}
		ret.first=word.substr(0, q);

		while (++p < word.size())
		{
			switch (word[q-1]) {
			case ' ':
			case '\r':
			case '\n':
			case '\t':
				continue;
			default:
				break;
			}
			break;
		}
		ret.second=word.substr(p);
	}

	return ret;
}

template
std::ostreambuf_iterator<char>
headersbase::to_string<std::ostreambuf_iterator<char>
		      >(std::ostreambuf_iterator<char>, const char *, bool)
	const;

template std::string headersbase::quoted_string(const std::string &);

#if 0
{
#endif
}
