/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fmtsize_H
#define x_fmtsize_H

#include <x/locale.H>
#include <x/namespace.h>
#include <string>
#include <stdint.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Format memory or file size

//! Formats a string like "x Kb" or "x Mb". The logic is:
//!
//! - < 1024 bytes, formatted as "nnnn bytes"
//!
//! - < 10 Kb, formatted as "n.n Kb"
//!
//! - < 1 Mb, formatted as "n Kb"
//!
//! - < 10 Mb, formatted as "n.n Mb"
//!
//! - < 1 Gb, formatted as "n Mb"
//!
//! - < 10 Gb, formatted as "n.n Gb"
//!
//! - < 1 Tb, formatted as "n Gb"
//!
//! - < 10 Tb, formatted as "n.n Tb"
//!
//! - >= 10Tb, formatted s "n Tb"

std::string fmtsize(uint64_t bytes) noexcept;

//! The opposite of fmtsize()

//! Given a string, compute the value, in bytes.
//!
//! The value return by this function, if passed to fmtsize(), will result
//! in the same string, provided that the string passed here follows one of
//! the formats described in fmtsize(), with
//! formatting differences are limited to whitespace, and casing.
//!
//! \note
//! parsesize("5000") returns 4916. This is because fmtsize() formats values
//! between 1kb and 10kb as "N.N Kb"; 4916 gets formatted as "4.8kb", and
//! 5017 gets formatted as "4.9kb". "5000" cannot be represented exactly,
//! and parsesize() return value remains consistent values with what fmtsize()
//! would return.

uint64_t parsesize(const std::string &s,
		   const const_locale &localeArg=locale::base::global()) noexcept;

//! A %property or an %option that represent a definition of a memory/file size

//! This is a simple class that stores a numeric %value that gets formatted and
//! parsed with fmtsize() and parsesize(), and may be used with the
//! \ref value_string "value_string" template class, This allows some memory
//! or file size value to be specified as %property or an %option.
//!
//! \c property::value<memsize> allows a memory or file size be specified as
//! a property. Use the resulting object's getValue(), as usual, to retrieve
//! the property value.
//!
//! \par Properties
//!
//! Specifying memory or file sizes as properties, see \ref property "property".
//!
//! \code
//! property::value<memsize> memsize_prop(L"memsize", memsize(4096));
//!
//! // ...
//!
//! uint64_t bytes=memsize_prop.getValue().bytes;
//! \endcode
//!
//! In the property file:
//!
//! \code
//! memsize=1 Mb
//! \endcode
//!
//! This example set \c bytes to 10485760, ten megabytes.
//!
//! \par A command line option
//!
//! Specifying memory or file sizes as a command line option, see
//! \ref option "option".
//!
//! \code
//!
//! option::memsize_value memsize=option::memsize_value::create();
//!
//! // ...
//!
//! options->add(memsize, L's', L"memsize",
//!              option::list::hasvalue, L"Memory size", L"size");
//!
//! // ...
//!
//! uint64_t bytes= memsize->value.bytes;
//! \endcode
//!
//! The option "--memsize=1Mb" will result in \c bytes getting set to
//! 10485760.

class memsize {

public:
	//! Memory size value
	uint64_t bytes;

	//! Constructor

	//! The constructor normalizes the value to what can be represented
	//! by parsesize() and fmtsize().
	//!
	//! Note, however, that the \c bytes member can always be set directly,
	//! to any value.
	memsize(uint64_t bytesArg=0,
		const const_locale &localeArg=locale::base::global()) noexcept;

	//! Destructor
	~memsize() {}

	//! Format the memory size via fmtstring

	template<typename OutputIterator>
	OutputIterator toString(//! Output iterator
				OutputIterator iter,

				//! Locale for the conversion
				const const_locale &localeRef=locale::base::global())
		const
	{
		std::string s=fmtsize(bytes);

		return std::copy(s.begin(), s.end(), iter);
	}

	//! Create an interval from a string

	template<typename InputIterator>
	static memsize fromString(//! Beginning iterator
				  InputIterator beg_iter,

				  //! Ending iterator
				  InputIterator end_iter,

				  //! The locale
				  const const_locale &localeArg=locale::base::global())

	{
		return memsize(parsesize(std::string(beg_iter, end_iter), localeArg));
	}
};
#if 0
{
#endif
}
#endif
