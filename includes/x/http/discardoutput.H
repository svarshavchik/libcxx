/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_discardoutput_H
#define x_http_discardoutput_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! A dummy output iterator that discards all output

class discardoutput : public std::iterator<std::output_iterator_tag, char> {

public:
	//! Dummy constructor
	discardoutput() noexcept {}

	//! Dummy destructor
	~discardoutput() {}

	//! Dummy output iterator
	discardoutput &operator*() noexcept { return *this; }

	//! Dummy output iterator
	discardoutput &operator++() noexcept { return *this; }

	//! Dummy output iterator
	discardoutput &operator++(int) noexcept { return *this; }
	//! Dummy output iterator
	discardoutput &operator=(char) noexcept { return *this; }
};

#if 0
{
	{
#endif
	}
}
#endif
