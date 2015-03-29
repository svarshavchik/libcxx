/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/escape.H"

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

std::string escapestr(const std::string &str,
		      bool escapeqac)
{
	std::string s;

	escape(str.begin(), str.end(),
	       std::back_insert_iterator<std::string>(s), escapeqac);

	return s;
}

template std::back_insert_iterator<std::string>
escape(std::string::const_iterator, std::string::const_iterator,
       std::back_insert_iterator<std::string>, bool);

template std::ostreambuf_iterator<char>
escape(std::string::const_iterator, std::string::const_iterator,
       std::ostreambuf_iterator<char>, bool);

template std::back_insert_iterator<std::string>
escape(std::vector<int16_t>::const_iterator,
       std::vector<int16_t>::const_iterator,
       std::back_insert_iterator<std::string>, bool);

template std::ostreambuf_iterator<char>
escape(std::vector<int16_t>::const_iterator,
       std::vector<int16_t>::const_iterator,
       std::ostreambuf_iterator<char>, bool);

template std::back_insert_iterator<std::string>
escape(std::vector<uint16_t>::const_iterator,
       std::vector<uint16_t>::const_iterator,
       std::back_insert_iterator<std::string>, bool);

template std::ostreambuf_iterator<char>
escape(std::vector<uint16_t>::const_iterator,
       std::vector<uint16_t>::const_iterator,
       std::ostreambuf_iterator<char>, bool);

template std::back_insert_iterator<std::string>
escape(std::vector<int32_t>::const_iterator,
       std::vector<int32_t>::const_iterator,
       std::back_insert_iterator<std::string>, bool);

template std::ostreambuf_iterator<char>
escape(std::vector<int32_t>::const_iterator,
       std::vector<int32_t>::const_iterator,
       std::ostreambuf_iterator<char>, bool);

template std::back_insert_iterator<std::string>
escape(std::vector<uint32_t>::const_iterator,
       std::vector<uint32_t>::const_iterator,
       std::back_insert_iterator<std::string>, bool);

template std::ostreambuf_iterator<char>
escape(std::vector<uint32_t>::const_iterator,
       std::vector<uint32_t>::const_iterator,
       std::ostreambuf_iterator<char>, bool);

template std::string
escapestr(std::string::const_iterator, std::string::const_iterator, bool);

template std::string
escapestr(std::vector<int16_t>::const_iterator,
	  std::vector<int16_t>::const_iterator, bool);

template std::string
escapestr(std::vector<uint16_t>::const_iterator,
	  std::vector<uint16_t>::const_iterator, bool);

template std::string
escapestr(std::vector<int32_t>::const_iterator,
	  std::vector<int32_t>::const_iterator, bool);

template std::string
escapestr(std::vector<uint32_t>::const_iterator,
	  std::vector<uint32_t>::const_iterator, bool);

#if 0
{
	{
#endif
	}
}
