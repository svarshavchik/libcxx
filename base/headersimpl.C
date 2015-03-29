/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/headersimpl.H"
#include "gettext_in.h"
#include <cstring>
#include <algorithm>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename endl_type>
headersimpl<endl_type>::headersimpl() noexcept
{
}

template<typename endl_type>
headersimpl<endl_type>::headersimpl(const headersbase &headers)
	: headersbase(headers)
{
}

template<typename endl_type>
headersimpl<endl_type>::~headersimpl() noexcept
{
}

template<typename endl_type>
headersbase::iterator headersimpl<endl_type>::append(const std::string &name,
						     const std::string &value)

{
	return append(name + ": " + value);
}

template<typename endl_type>
headersbase::iterator headersimpl<endl_type>::append(const std::string &header)

{
	std::string::const_iterator b(header.begin()), e(header.end());

	if (b == e)
		return headermap.end();

	if (std::search(b, e, endl_type::eol_str,
			endl_type::eol_str + strlen(endl_type::eol_str)) != e)
		goto bad;

	if ((*b == ' ' || *b == '\t' || *b == '\r') && !headerlist.empty())
	{
		fold_header(header);
		return headermap.end();
	}

	while (b != e)
	{
		unsigned char c= *b;

		if (c <= ' ' || strchr("()<>@,;:\\\"/[]?={}", c))
			break;
		++b;
	}

	if (b == e || b == header.begin() || *b != ':')
	{
	bad:
		throw EXCEPTION(_("Bad header format"));
	}

	return new_header(header);
}

template<typename endl_type>
headersbase::iterator headersimpl<endl_type>::replace(const std::string &name,
						      const std::string &value)

{
	erase(equal_range(name));
	return append(name, value);
}

template<typename endl_type>
void headersimpl<endl_type>::fold_header(const std::string &line)

{
	std::string &s(*--headerlist.end());

	s += endl_type::eol_str;
	s += line;
}

template class headersimpl<headersbase::crlf_endl>;
template class headersimpl<headersbase::lf_endl>;

template
std::istreambuf_iterator<char>
headersimpl<headersbase::crlf_endl>::parse<std::istreambuf_iterator<char>
					   >(std::istreambuf_iterator<char>,
					     std::istreambuf_iterator<char>,
					     size_t);

template
std::ostreambuf_iterator<char>
headersimpl<headersbase::crlf_endl>::toString<std::ostreambuf_iterator<char>
					      >(std::ostreambuf_iterator<char>,
						bool)
	const;

template
std::istream &
headersimpl<headersbase::crlf_endl>::parse(std::istream &, size_t);

template
std::istreambuf_iterator<char>
headersimpl<headersbase::lf_endl>::parse<std::istreambuf_iterator<char>
					   >(std::istreambuf_iterator<char>,
					     std::istreambuf_iterator<char>,
					     size_t);

template
std::istream &
headersimpl<headersbase::lf_endl>::parse(std::istream &, size_t);

template
std::ostreambuf_iterator<char>
headersimpl<headersbase::lf_endl>::toString<std::ostreambuf_iterator<char>
					    >(std::ostreambuf_iterator<char>,
					      bool)
	const;

#if 0
{
#endif
}
