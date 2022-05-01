/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_pcreobj_H
#define x_pcreobj_H

#define PCRE2_CODE_UNIT_WIDTH	8
#include <pcre2.h>
#include <vector>
#include <tuple>
#include <string_view>
#include <x/obj.H>
#include <x/localefwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A wrapper for the pcre library.

class pcreObj : virtual public obj {

	//! PCRE library object
	pcre2_code *compiled;

	//! PCRE library object
	pcre2_match_data *match_data;

public:

	//! Constructor

	//! Note: the PCRE_UTF flag gets always specified.
	//!
	pcreObj(//! Pattern
		const std::string_view &pattern,

		//! Options for pcre2_compile. See pcre2_compile(3)
		uint32_t options=0);

	//! Destructor
	~pcreObj();

	//! Match a string against the pattern.

	//! Returns an empty vector if the pattern was found, or a non-empty
	//! vector if the pattern was found.
	//!
	//! An error from the underlying PCRE library can result in a thrown
	//! exception. The caller should catch any thrown exceptions, and
	//! decide what to do with them.
	//!
	//! The return vector is a tuple of starting+one past the ending
	//! offset in the string where a match was found.
	//!
	//! The first value in the vector gives the position of the entire
	//! matching string, the remaining values are any sub-patterns.
	//!
	//! The returned vector comes from pcre2_get_ovector_pointer(3).


	template<typename T, typename=decltype(std::string_view{
			std::declval<const T &>()})>
	std::vector<std::string_view> match(
		//! String to match
		const T &string,

		//! Starting offset to match
		size_t starting_offset=0,

		//! Matching option to pcre2_match().
		uint32_t options=0) const
	{
		return match_impl(string, starting_offset, options);
	}

	//! Disallow temporary string values getting passed to match().
	template<typename T>
	std::vector<std::string_view> match(
		T &&string,
		size_t starting_offset=0,
		uint32_t options=0) const=delete;
private:
	//! Internal implementation

	std::vector<std::string_view> match_impl(
		const std::string_view &string,
		size_t starting_offset,
		uint32_t options) const;
public:

	//! Find all matches

	//! Repeatedly calls match() until all matches are found.

	template<typename T, typename=decltype(std::string_view{
			std::declval<const T &>()})>
	std::vector<std::vector<std::string_view>> match_all(
		//! String to match
		const T &string,

		//! Matching option to pcre2_match().
		uint32_t options=0) const
	{
		return match_all_impl(string, options);
	}

	//! Disallow temporary string values getting passed to match_all().

	template<typename T>
	std::vector<std::vector<std::string_view>> match_all(
		T &&string,
		uint32_t options=0) const=delete;

private:
	//! Internal implementation
	std::vector<std::vector<std::string_view>> match_all_impl(
		const std::string_view &string,
		uint32_t options) const;
};

#if 0
{
#endif
}
#endif
