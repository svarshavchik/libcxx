/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cups_job_H
#define x_cups_job_H

#include <x/namespace.h>
#include <x/cups/jobfwd.H>
#include <x/cups/jobobj.H>
#include <x/ref.H>
#include <optional>
#include <vector>
#include <string_view>
#include <string>

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	};
};
#endif

//! Parse a string into an IPP range type.

//! Validates that the range is consecutive, and valid. Returns no value
//! if the string is not valid.

std::optional<std::vector<std::tuple<int, int>
			  >> parse_range_string(const std::string_view &);

//! And convert an IPP range type to string.

std::string range_to_string(const std::vector<std::tuple<int, int>> &);

#if 0
{
	{
#endif
	}
}
#endif
