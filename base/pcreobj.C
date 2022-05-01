/*
** Copyright 2012-2022 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/pcre.H"
#include "x/exception.H"
#include "x/to_string.H"
#include "x/messages.H"
#include "x/locale.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

pcreObj::pcreObj(const std::string_view &pattern, uint32_t options)
{
	int errcode;
	PCRE2_SIZE errindex;

	compiled=pcre2_compile(
		reinterpret_cast<PCRE2_SPTR8>(pattern.data()),
		pattern.size(),
		options,
		&errcode,
		&errindex,
		nullptr);

	if (!compiled)
	{
		throw EXCEPTION(
			gettextmsg(
				libmsg(_txt("Invalid regular expression "
					    "offset %1% of: %s")),
				errindex,
				pattern));
	}

	match_data=pcre2_match_data_create_from_pattern(compiled, nullptr);

	if (!match_data)
	{
		pcre2_code_free(compiled);

		throw EXCEPTION(
			gettextmsg(
				libmsg(_txt("Failed to create match data "
					    "for: ")),
				pattern));
	}
}

pcreObj::~pcreObj()
{
	if (match_data)
		pcre2_match_data_free(match_data);
	if (compiled)
		pcre2_code_free(compiled);
}

std::vector<std::string_view> pcreObj::match_impl(
	const std::string_view &string,
	size_t starting_offset,
	uint32_t options) const
{
	if (starting_offset >= string.size())
		return {};

	std::unique_lock lock{objmutex};

	std::vector<std::string_view> ret;

	int rc=pcre2_match(compiled,
			   reinterpret_cast<PCRE2_SPTR8>(string.data()),
			   string.size(),
			   starting_offset,
			   options,
			   match_data,
			   nullptr);

	if (rc < 0)
		return ret;

	PCRE2_SIZE *ovector=pcre2_get_ovector_pointer(match_data);
	uint32_t ovector_count=pcre2_get_ovector_count(match_data);

	ret.reserve(ovector_count);

	if (ovector)
	{
		for (uint32_t cnt=0; cnt < ovector_count; ++cnt)
			ret.emplace_back(&string.data()[ovector[cnt*2]],
					 ovector[cnt*2+1]-ovector[cnt*2]);
	}

	return ret;
}

std::vector<std::vector<std::string_view>> pcreObj::match_all_impl(
	const std::string_view &string,
	uint32_t options) const
{
	size_t starting_offset=0;

	std::vector<std::vector<std::string_view>> ret;

	while (1)
	{
		auto matches=match(string, starting_offset, options);

		if (matches.empty())
			break;

		auto &first_match=matches[0];

		if (first_match.size() == 0)
			++starting_offset;
		else
			starting_offset=
				first_match.data()+first_match.size()-
				string.data();

		ret.push_back(std::move(matches));
	}

	return ret;
}

#if 0
{
#endif
}
