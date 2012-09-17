/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "headersbase.H"
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
	throw EXCEPTION("End of stream while parsing headers");
}

void headersbase::parse_toomanyheaders()
{
	throw EXCEPTION("Number of headers exceeds maximum allowed");
}

headersbase::headersbase() noexcept
{
}

headersbase::~headersbase() noexcept
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

headersbase::tokenhdrparser::tokenhdrparser(const std::string &headername,
					    const headersbase &headers)

{
	std::pair<const_iterator,
		  const_iterator> values(headers.equal_range(headername));

	hdr_start=values.first;
	hdr_end=values.second;
	newhdr();
}

headersbase::tokenhdrparser::~tokenhdrparser() noexcept
{
}

void headersbase::tokenhdrparser::newhdr()
{
	while (hdr_start != hdr_end)
	{
		val_start = hdr_start->second.begin();
		val_end = hdr_start->second.end();

		if (val_start != val_end)
			break;
		++hdr_start;
	}
}

char headersbase::tokenhdrparser::operator()()
{
	while (hdr_start != hdr_end)
	{
		// Skip leading spaces and spurious separators

		switch (*val_start) {
		case ',':
		case ';':
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			if (++val_start == val_end)
			{
				++hdr_start;
				newhdr();
			}
			continue;
		default:
			break;
		}
		break;
	}

	if (hdr_start == hdr_end)
		return 0;

	value="";
	// Collect value until , or ; is seen outside of any
	// quotes.
	bool inquote=false;

	while (val_start != val_end &&
	       (inquote || (*val_start != ',' && *val_start != ';')))
	{
		if (*val_start == '"')
		{
			inquote=!inquote;
			++val_start;
			continue;
		}

		std::string::const_iterator p=val_start;

		// If a space outside of a quote, scan ahead
		// and skip all the spaces.

		if (!inquote)
		{
			while (p != val_end &&
			       (*p == ' '
				|| *p == '\t'
				|| *p == '\r'
				|| *p == '\n'))
				++p;

			// If the next nonspace
			// character is a terminator, stop,
			// thus skipping the trailing
			// whitespace.

			if (p == val_end || *p == ','
			    || *p == ';')
			{
				val_start=p;
				break;
			}

			if (p != val_start) // skipped some spc
				--p;
		}
		else
		{
			if (inquote && *p == '\\')
			{
				if (++p == val_end)
					break;
			}
			val_start=p;
		}

		++p;

		while (val_start != p)
		{
			char c=*val_start++;

			if (c == '\t' || c == '\n' || c == '\r')
				c=' ';

			value.push_back(c);
		}
	}

	if (val_start == val_end)
	{
		++hdr_start;
		newhdr();
		return ',';
	}

	return *val_start;
}

template
std::ostreambuf_iterator<char>
headersbase::toString<std::ostreambuf_iterator<char>
		      >(std::ostreambuf_iterator<char>, const char *)
	const;


#if 0
{
#endif
}
