/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_mime_bodystartiter_H
#define x_mime_bodystartiter_H

#include <x/namespace.h>
#include <x/mime/newlineiter.H>

namespace LIBCXX_NAMESPACE::mime {
#if 0
}
#endif

//! Implement a reference-counted iterator that finds the start of a message body.

//! \see bodystart_iter
//!

template<typename output_iter_type>
class bodystart_iterObj : public outputrefiteratorObj<int> {

	//! Whether the previous element was newline_end

	bool was_newlineend;

	//! Next newline_end will start a body

	bool will_body_start;

	//! Whether body_start was seen

	bool seen_body_start;

public:

	//! Underlying output iterator over ints.

	output_iter_type iter;

	//! Constructor
	bodystart_iterObj(const output_iter_type &iterArg)
		: was_newlineend(true), // Make-believe
		  will_body_start(false),
		  seen_body_start(false),
		  iter(iterArg)
	{
	}

	//! Move constructor
	bodystart_iterObj(output_iter_type && iterArg)
		: was_newlineend(true), // Make-believe
		  will_body_start(false),
		  seen_body_start(false),
		  iter(std::move(iterArg))
	{
	}

	//! Iterator operator

	void operator=(int c) final override
	{
		if (c == eof)
		{
			// If the output sequence didn't end with a body_start,
			// add one.

			if (!seen_body_start)
				*iter++=body_start;
			seen_body_start=true;
			*iter++=c;
			return;
		}

		if (!seen_body_start)
		{
			if (c == newline_start && was_newlineend)
			{
				*iter++=separator_line_start;
				will_body_start=true;
			}
			was_newlineend=false;
			if (c == newline_end)
			{
				was_newlineend=true;

				if (will_body_start)
				{
					seen_body_start=true;
					*iter++=c;
					*iter++=body_start;
					return;
				}
			}
		}
		*iter++=c;
		return;
	}
};

//! A reference-counter output iterator that finds the start of a message body.

//! This is a \ref refiterator "reference-counted iterator" that implements
//! an output iterator.
//! This output iterator typically receives the output from
//! \ref newline_iter "newline_iter", and
//! the template parameter is an output
//! iterator that takes ints. The constructor takes the instance of the
//! template parameter.
//!
//! The ints that this iterator receives get passed through to the underlying
//! int operator. Additionally, the first time two newline sequences are seen
//! in a row, the second newline sequence gets preceded by a
//! \c separator_line_start value, and followed by a \c body_start value.
//!
//! An eof element checks if a body has been seen, if not \c body_start gets
//! emitted. However \c separator_line_start does not get emitted in that
//! situation.

template<typename output_iter>
using bodystart_iter=refiterator<bodystart_iterObj<output_iter> >;

//! A nullable pointer reference to a \ref bodystart_iterObj "bodystart_iterObj".

//! \see bodystart_iter

template<typename output_iter>
using bodystart_iterptr=ptriterator<bodystart_iterObj<output_iter> >;

#if 0
{
#endif
}
#endif
