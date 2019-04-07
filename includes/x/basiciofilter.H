/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basiciofilter_H
#define x_basiciofilter_H

#include <x/exception.H>
#include <x/obj.H>
#include <algorithm>
#include <vector>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Template definition of an IO filter

//! An IO filter transforms an input stream to an output stream.
//! The input to the IO filter is provided in \c next_in and \c avail_in
//! member objects, and the output buffer is provided in \c next_out
//! and \c avail_out fields. The filter() method consumes some portion of the
//! input and/or produces some output.
//!
//! The end of the input stream is signaled
//! with a 0 value in \c avail_in.
//! filter() may still produce output with \c avail_in set to 0, so it
//! should be repeatedly
//! invoked as long as it continues to update \c next_out and \c avail_out.
//!
//! \note
//! The actual filtering implementation must virtually derive from
//! iofilter.

template<typename input_type, typename output_type>
class iofilter {

public:
	//! The input stream's type
	typedef input_type input_t;

	//! The output stream's type
	typedef output_type output_t;

	//! Input buffer pointer
	const input_t *next_in;

	//! How much input remains
	size_t avail_in;

	//! Output buffer pointer
	output_t *next_out;

	//! Remaining available output buffer type
	size_t avail_out;

	//! The constructor initializes all pointers to 0

	iofilter() noexcept : next_in(0), avail_in(0),
		next_out(0), avail_out(0)
	{
	}

	//! The destructor

	~iofilter()
	{
	}

	//! Filter IO

	//! Consume input and/or produce output. \c avail_out must not be 0.
	//! filter() updates \c next_in and \c avail_in, and/or updates
	//! \c next_out and \c avail_out.
	//!
	//! When no more input is available, or any "in-flight" output
	//! should be produced, filter() should be repeatedly invoked with
	//! avail_in set to 0, until filter() stops updating \c next_out and
	//! \c avail_out.
	virtual void filter()=0;
};

extern template class iofilter<char, char>;

//! Default buffer size used by iofilteradapterObj

extern size_t iofilter_bufsize();

//! Provide and manage buffers for an iofilter.

//! An iofilter subclass. Provides input and output buffers. This is a
//! reference-counted object.

template<typename input_type, typename output_type>
class iofilteradapterObj : virtual public obj,
			   virtual public iofilter<input_type, output_type> {

public:
	//! Input buffer allocation
	std::vector<input_type> inbuf;

	//! Output buffer allocation
	std::vector<output_type> outbuf;

	//! Used by consumers of output
	output_type *outp;

	//! Constructor
	iofilteradapterObj(//! Default input and output buffer size
			   size_t n=iofilter_bufsize() )

	{
		if (n < 256)
			n=256;
		inbuf.resize(n);
		outbuf.resize(n);
	}

	//! Destructor
	~iofilteradapterObj()
	{
	}

	//! Reset iofilter input and output pointers.

	//! The following actions are taken:
	//!
	//! - The chunk specifies by next_in and avail_in, if not empty, gets
	//! moved to the beginning of inbuf, and next_in adjusted accordingly.
	//! This gives maximum room to grow for whatever's putting more stuff
	//! into the input buffer.
	//!
	//! - next_out and avail_out are reset to point to the entire output
	//! buffer.
	//!
	//! The iofilter constructor sets avail_in to 0. Invoke reset() after
	//! constructing an iofilter to ready the buffers for the first call
	//! to filter(). After each call to filter(), the user is expected to
	//! consume everything in the output buffer, and calling reset()
	//! prepares for the next call to filter(), preserving any input
	//! that the previous call to filter() hasn't consumed, and leaving
	//! the maximum space left to add to the input.

	void reset() noexcept
	{
		const input_type *cp=this->next_in;
		input_type *p=&inbuf[0];

		this->next_in=p;

		std::copy(cp, cp+this->avail_in, p);

		this->outp=this->next_out=&outbuf[0];
		this->avail_out=outbuf.size();
	}
};

extern template class iofilteradapterObj<char, char>;

#if 0
{
#endif
}
#endif
