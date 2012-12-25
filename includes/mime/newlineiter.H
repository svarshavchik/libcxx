/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_newlineiter_H
#define x_mime_newlineiter_H

#include <x/namespace.h>
#include <x/mime/tokens.H>
#include <x/refiterator.H>
#include <x/ref.H>
#include <x/ptr.H>
#include <x/obj.H>
#include <iterator>

namespace LIBCXX_NAMESPACE {

	//! Classes and templates for parsing MIME-formatted messages.

	namespace mime {
#if 0
	}
}
#endif

//! Implement a reference-counted newline-labeling iterator

//! \see newline_iter
//!
template<typename output_iter_type>
class newline_iterObj : virtual public obj {

	//! Whether the newline sequence is crlf

	bool crlf;

	//! Seen a lone CR, when the newline sequence is crlf

	bool seen_cr;

	//! At start of line.
	bool sol;

public:

	//! My iterator traits

	typedef std::iterator<std::output_iterator_tag, void,
			      void, void, void> iterator_traits;

	//! Underlying output iterator over ints.
	output_iter_type iter;

	//! Constructor
	newline_iterObj(const output_iter_type &iterArg,
			bool crlfArg=false) : crlf(crlfArg),
					      seen_cr(false),
					      sol(true),
					      iter(iterArg)
	{
	}

	//! Move constructor
	newline_iterObj(output_iter_type && iterArg,
			bool crlfArg=false) : crlf(crlfArg),
					      seen_cr(false),
					      sol(true),
					      iter(std::move(iterArg))
	{
	}

	//! Iterator operator

	newline_iterObj<output_iter_type> &operator*() { return *this; }

	//! Iterator operator

	newline_iterObj<output_iter_type> &operator++() { return *this; }

	//! Iterator operator

	newline_iterObj<output_iter_type> *before_postoper() { return this; }

	//! Iterator operator

	void operator=(unsigned char c)
	{
		sol=false;

		if (crlf)
		{
			if (seen_cr && c != '\n')
			{
				*iter++='\r';
				seen_cr=false;
			}

			if (c == '\r')
			{
				seen_cr=true;
				return;
			}

			if (seen_cr && c == '\n')
			{
				*iter++=newline_start;
				*iter++='\r';
				*iter++='\n';
				*iter++=newline_end;
				seen_cr=false;
				sol=true;
				return;
			}
		}
		else if (c == '\n')
		{
			*iter++=newline_start;
			*iter++='\n';
			*iter++=newline_end;
			sol=true;
			return;
		}

		*iter++=c;
		return;
	}

	//! Emits \c INSERT_LIBX_NAMESPACE::mime::eof into the output sequence.

	void eof()
	{
		// If the output sequence didn't end with a newline,
		// emit one.

		if (seen_cr)
		{
			*iter++='\r';
			seen_cr=false;
		}

		if (!sol)
		{
			*iter++=newline_start;
			*iter++=newline_end;
			sol=true;
		}
		*iter++=mime::eof;
	}
};

//! A reference-counted output iterator that labels newline sequences.

//! This is a \ref refiterator "reference-counted iterator" that implements
//! an output iterator.
//! The output iterator takes ints, however it really takes only chars and
//! eof. The template parameter is an output
//! iterator that takes ints. The constructor takes the instance of the
//! template parameter.
//!
//! The chars that this iterator iterates over get passed through to the
//! underlying int iterator, after being promoted to ints. Additionally,
//! each newline sequence gets preceded by \c newline_start, and finished by
//! \c newline_end.
//!
//! Iterating over eof the end of the output sequence. If the output
//! sequence does not end in a newline, the output iterator emits a
//! \c newline_start immediately followed by \c newline_end.
//! The \c eof element gets forwarded
//! to the underlying iterator.

template<typename output_iter>
using newline_iter=refiterator<newline_iterObj<output_iter> >;

//! A nullable pointer reference to a \ref newline_iterObj "newline_iterObj".

//! \see newline_iter

template<typename output_iter>
using newline_iterptr=ptriterator<newline_iterObj<output_iter> >;

#if 0
{
	{
#endif
	}
}
#endif
