/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_sectioniter_H
#define x_mime_sectioniter_H

#include <x/namespace.h>
#include <x/refiterator.H>
#include <x/mime/newlineiter.H>
#include <vector>

namespace LIBCXX_NAMESPACE {

	//! Classes and templates for parsing MIME-formatted messages.

	namespace mime {
#if 0
	}
}
#endif

//! Implement a reference-counted iterator that finds the start and end of sections inside a multipart section.

//! \see section_iter
//!

template<typename output_iter_type>
class section_iterObj : public outputrefiteratorObj<int> {

	//! The boundary delimiter, with the leading --.
	std::string delimiter;

	//! When scanning for a delimiter, the newline sequence that preceded it
	std::vector<int> newline_before_delimiter;

	//! Current position in delimiter we're scanning for
	size_t delimiter_pos;

	//! Used to check for last boundary delimiter
	int last_boundary;

	//! Whether scanning for the first boundary delimiter
	bool first;

	//! State machine
	void (section_iterObj<output_iter_type>::*next)(int);

public:
	//! Underlying output iterator over ints.

	output_iter_type iter;

	//! Constructor
	section_iterObj(const output_iter_type &iterArg,
			const std::string &delimiterArg)
		: delimiter("--" + delimiterArg),
		  delimiter_pos(0),
		  first(true),
		  next(&section_iterObj<output_iter_type>::scan_delimiter),
		  iter(iterArg)
	{
	}

	//! Move constructor
	section_iterObj(output_iter_type && iterArg,
			const std::string &delimiterArg)
		: delimiter("--" + delimiterArg),
		  delimiter_pos(0),
		  first(true),
		  next(&section_iterObj<output_iter_type>::scan_delimiter),
		  iter(std::move(iterArg))
	{
	}

	//! Iterator operator

	void operator=(int c) final override
	{
		(this->*next)(c);
	}

private:
	//! At the beginning of line, scanning for the delimiter string.

	void scan_delimiter(int c)
	{
		if (c == eof)
		{
			emit_newline_and_delimiter();
			do_eof();
			return;
		}

		if (c == delimiter[delimiter_pos])
		{
			if (++delimiter_pos == delimiter.size())
			{
				// See NOTE TO IMPLEMENTORS in RFC 2046!

				if (!first)
					*iter++=section_end; // End of prev sect

				// Emit what we buffered
				emit_newline_and_delimiter();
				last_boundary=0;
				next=&section_iterObj<output_iter_type>
					::found_delimiter;
			}
			return;
		}

		// This wasn't it.

		next=&section_iterObj<output_iter_type>::not_found_delimiter;

		// Emit what we buffered
		emit_newline_and_delimiter();

		// Look for the end of the line, and try again.
		not_found_delimiter(c);
	}

	//! Pass through newline sequence+delimiter prefix found up to now.

	void emit_newline_and_delimiter()
	{
		iter=std::copy(delimiter.begin(),
			       delimiter.begin()+delimiter_pos,
			       std::copy(newline_before_delimiter.begin(),
					 newline_before_delimiter.end(),
					 iter));
		delimiter_pos=0;
	}

	//! Found an EOF, make sure that we close the last section.

	void do_eof()
	{
		if (!first)
			*iter++=section_end;
		*iter++=eof;
	}

	//! Delimiter not found, looking for newline, next line.

	void not_found_delimiter(int c)
	{
		if (c == eof)
		{
			do_eof();
			return;
		}

		if (c != newline_start)
		{
			*iter++=c;
			return;
		}

		newline_before_delimiter.clear();
		next=&section_iterObj<output_iter_type>::not_found_delimiter_newline;
		not_found_delimiter_newline(c);
	}

	//! Collect the newline sequence before a potential delimiter

	void not_found_delimiter_newline(int c)
	{
		if (c == eof) // Shouldn't happen, according to newlineiter.
		{
			emit_newline_and_delimiter();
			do_eof();
			return;
		}

		newline_before_delimiter.push_back(c);
		if (c == newline_end)
			next=&section_iterObj<output_iter_type>::scan_delimiter;
	}

	//! Found the delimiter, start a new section on the next line.

	void found_delimiter(int c)
	{

		// Check if it's the last boundary delimiter.
		if (last_boundary >= 0)
		{
			if (c != '-')
				last_boundary=-1;
			else if (++last_boundary == 2)
			{
				next=&section_iterObj<output_iter_type>::nomore;
				nomore(c);
				return;
			}
		}
				
		*iter++=c;
		if (c != newline_end)
			return;

		// Start of a brand new line. Here we go.
		first=false;
		*iter++=section_start;
		next=&section_iterObj<output_iter_type>::not_found_delimiter;
	}

	//! Seen last boundary delimiter, we have nothing to do.
	void nomore(int c)
	{
		*iter++=c;
	}
};

//! A reference-counter output iterator that marks sections in a multipart section.

//! This is a \ref refiterator "reference-counted iterator" that implements
//! an output iterator.
//! This output iterator typically receives the output after bodystart_iter
//! emits a \c body_start element. The constructor takes the instance of the
//! template parameter, and the value of the "boundary" attribute of the
//! Content-Type: header in the preceding headers, that specify a multipart
//! content type.
//!
//! The ints that this iterator receives get passed through to the underlying
//! int operator. Additionally, this iterator inserts \c section_start and
//! \c section end for each MIME section that's demarkated by the boundary
//! delimiter string.
//!
//! Each \c section_start is inserted at the start of the line of the first
//! header in each MIME section. Each \c section_end is inserted before
//! the \c newline_start that precedes the terminating boundary string,
//! as specified in RFC 2046.
//!
//! This is a template alias for a
//! \ref refiterator "refiterator" to a
//! \ref section_iterObj "section_iterObj".

template<typename output_iter>
using section_iter=refiterator<section_iterObj<output_iter> >;

//! A nullable pointer reference to a \ref section_iterObj "section_iterObj".

//! \see section_iter

template<typename output_iter>
using section_iterptr=ptriterator<section_iterObj<output_iter> >;

#if 0
{
	{
#endif
	}
}
#endif
