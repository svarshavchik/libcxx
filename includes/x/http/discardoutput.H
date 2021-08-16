/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_discardoutput_H
#define x_http_discardoutput_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! A dummy output iterator that discards all output

class discardoutput {

public:
	//! Iterator trait
	typedef std::output_iterator_tag iterator_category;

	//! Iterator trait
	typedef void value_type;

	//! Iterator trait
	typedef void difference_type;

	//! Iterator trait
	typedef void pointer;

	//! Iterator trait
	typedef void reference;

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
#endif
}
#endif
