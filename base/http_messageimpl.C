/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http/messageimpl.H"
#include "http/responseimpl.H"
#include "gettext_in.h"
#include <cstring>
#include <cctype>
#include <set>
#include <functional>
#include <streambuf>

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif


const char messageimpl::content_length[]="Content-Length";

const char messageimpl::transfer_encoding[]="Transfer-Encoding";

const char messageimpl::transfer_encoding_chunked[]="chunked";

const char messageimpl::connection[]="Connection";

const char messageimpl::expect[]="Expect";

const char messageimpl::expect_100_continue[]="100-continue";

messageimpl::messageimpl() noexcept
{
}

messageimpl::messageimpl(const headersbase &headers)
	: headersimpl<headersbase::crlf_endl>(headers)
{
}

messageimpl::~messageimpl() noexcept
{
}

bool messageimpl::connection_closed() const
{
	std::set<std::string, chrcasecmp::str_less> connset;

	getCommaValues(connection,
		       std::insert_iterator<std::set<std::string,
						     chrcasecmp::str_less> >
		       (connset, connset.end()));

	return connset.find("close") != connset.end();
}

void messageimpl::getCommaValuesToSet(const char *headername,
				      keywordseti_t &ks) const
{
	ks.clear();

	getCommaValues(headername,
		       std::insert_iterator<keywordseti_t>(ks, ks.end()));
}

template std::istreambuf_iterator<char>
messageimpl::parse(std::istreambuf_iterator<char>,
		   std::istreambuf_iterator<char>, size_t);

template std::istreambuf_iterator<char>
messageimpl::httpver_parse(std::istreambuf_iterator<char>,
			   std::istreambuf_iterator<char>,
			   httpver_t &);

template std::ostreambuf_iterator<char>
messageimpl::httpver_toString(std::ostreambuf_iterator<char>, httpver_t)
;

template std::insert_iterator<messageimpl::keywordseti_t>
messageimpl::getCommaValues(const char *headername,
			    std::insert_iterator<keywordseti_t>
			    outputIterator) const;

template std::back_insert_iterator<std::list<std::string> >
messageimpl::getCommaValues(const char *headername,
			    std::back_insert_iterator<std::list<std::string> >
			    outputIterator) const;

template std::back_insert_iterator<std::vector<std::string> >
messageimpl::getCommaValues(const char *headername,
			    std::back_insert_iterator<std::vector<std::string> >
			    outputIterator) const;

#if 0
{
	{
#endif
	}
}
