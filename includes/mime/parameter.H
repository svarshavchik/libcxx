/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_parameters_H
#define x_http_parameters_H

#include <string>
#include <algorithm>
#include <x/chrcasecmp.H>
#include <x/http/messageimplfwd.H>
#include <x/tokens.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace mime {

#if 0
	};
};
#endif

//! A name=value parameter that appears in many headers

//! The name part is case-insensitive, the value is case sensitive
//!

struct parameter_t {

	//! Parameter name
	std::string name;

	//! Parameter value
	std::string value;

	//! Comparison function for associative containers
	bool operator<(const parameter_t &o) const noexcept
	{
		int rc=chrcasecmp::compare(name, o.name);

		if (rc == 0)
			rc=value.compare(o.value);

		return rc < 0;
	}

	//! Convert this name=value pair to a string

	template<typename iter_type>
	iter_type toString(//! Output iterator
			   iter_type iter) const
	{
		iter=std::copy(name.begin(), name.end(), iter);
		*iter++='=';

		if (tokenizer<is_http_token>::is_token(value))
			iter=std::copy(value.begin(), value.end(), iter);
		else
		{
			*iter++ = '"';

			for (std::string::const_iterator
				     b(value.begin()), e(value.end());
			     b != e; ++b)
			{
				if (*b == '\\' || *b == '"')
					*iter++='\\';
				*iter++=*b;
			}

			*iter++ = '"';
		}
		
		return iter;
	}
};

#if 0
{
	{
#endif
	}
}
#endif
