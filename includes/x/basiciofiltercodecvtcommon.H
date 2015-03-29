/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basiciofiltercodecvtcommon_H
#define x_basiciofiltercodecvtcommon_H

#include <x/basiciofilter.H>
#include <x/basiccodecvtcommon.H>
#include <x/locale.H>
#include <x/imbue.H>
#include <x/namespace.h>

#include <vector>
#include <iostream>
#include <algorithm>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! An IO filter that converts between narrow and wide characters

//! This is a common superclass to basic_codecvtin and basic_codecvtout.
//! The template parameters are:
//!
//! - a std::codecvt class used to invoke in() or out().
//!
//! - the input type for the conversion, either std::codecvt<>::intern_type or
//! std::codecvt<>::extern_type
//!
//! - the output type for the conversion, either std::codecvt<>::intern_type or
//! std::codecvt<>::extern_type
//!
//! - the implementation class. The implementation class defines a single
//! static function, inout() that invokes either in our out()+unshift().
//! The implementation class is supplied by
//! basic_codecvtin and basic_codecvtout.

template<typename x_codecvt_T,
	 typename codecvt_input_type,
	 typename codecvt_output_type,
	 typename impl_type>
class basic_iofiltercodecvtcommon
	: public basic_codecvt_common,
	  virtual public iofilter<codecvt_input_type, codecvt_output_type>
{
	//! Current character mapping object.

	//! This is the current character mapping
	//! object.
	container<x_codecvt_T> current_codecvt;

	//! The locale
	const_locale localeRef;

public:
	//! The type of the input sequence.

	//! This is the type that serves as the input to
	//! the filter.
	typedef codecvt_input_type input_type;

	//! The type of the output sequence.

	//! This is the type that becomes the output of
	//! the character conversion
	typedef codecvt_output_type output_type;

	//! The type of the character conversion state.

	//! In most cases, this is
	//! \c std::mbstate_t.

	typedef typename x_codecvt_T::state_type state_type;

	//! The default constructor.

	//! The default constructor resets the conversion to its initial
	//! state.
	basic_iofiltercodecvtcommon(//! The locale for conversions.
				    const const_locale &localeArg)
		noexcept
		: current_codecvt(localeArg),
		  localeRef(localeArg),
		  tmp_out_start(0), tmp_out_end(0), current_state(state_type())
	{
	}

	//! The default destructor

	//! \note
	//! The output does not get flushed by the destructor.
	~basic_iofiltercodecvtcommon() noexcept
	{
	}

private:

	//! Convenience typedef

	typedef iofilter<codecvt_input_type, codecvt_output_type> filter_t;

	//! Temporary input buffer

	//! If the provided input buffer is too small, use this one
	//! temporarily.

	std::vector<input_type> tmp_in_buffer;

	//! Next buffer input

	const input_type *tmp_next_in;

	//! Size of the buffered input

	size_t tmp_avail_in;

	//! Temporary output buffer

	//! If the provided output buffer is too small, use this one
	//! temporarily.

	std::vector<output_type> tmp_out_buffer;

	//! Next buffered output
	output_type *tmp_out_start;

	//! End of buffered output

	output_type *tmp_out_end;

	//! Current conversion state

	state_type current_state;

	//! Flush out temporarily-buffered output

	bool flushtmp()
	{
		bool flushed=false;

		while (filter_t::avail_out && tmp_out_start < tmp_out_end)
		{
			*filter_t::next_out++ = *tmp_out_start++;

			--filter_t::avail_out;
			flushed=true;
		}

		if (tmp_out_start == tmp_out_end)
			tmp_out_start=0;

		return flushed;
	}

public:
	//! Perform filtering

	//! Verify that the output buffer is big enough. If not, allocate
	//! an internal buffer, and filter to the output buffer. Copy buffered
	//! output to the supplied output buffer, on a best-effort basis.

	void filter();

private:
	//! Accumulate sufficient input, then filter it

	//! Make sure std::codecvt has sufficient input to perform
	//! conversion. If not, accumulate input in an internal buffer, until
	//! there's enough of it.

	void do_filter_in(output_type *&next_out,
			  output_type *end_out)
	{
		if (tmp_in_buffer.size() > 0)
		{
			// Previously-collected input: move to start of
			// tmp_in_buiffer.

			if (tmp_avail_in)
				std::copy(tmp_next_in,
					  tmp_next_in + tmp_avail_in,
					  &tmp_in_buffer[0]);
			tmp_in_buffer.resize(tmp_avail_in);
		}

		if ((tmp_avail_in=tmp_in_buffer.size()) > 0)
			tmp_next_in=&tmp_in_buffer[0];
		else
			tmp_next_in=0;

		if (filter_t::avail_in == 0) // EOF
		{
			while (tmp_avail_in)
			{
				output_type *old_next_out=next_out;

				do_filter_inout(tmp_next_in, tmp_avail_in,
						next_out, end_out);

				if (next_out == old_next_out &&
				    tmp_avail_in == tmp_in_buffer.size())
				{
					tmp_avail_in=0;
					in_failed(localeRef->name());
					return;
				}

				if (old_next_out != next_out)
					return;
			}
			do_filter_inout(filter_t::next_in, filter_t::avail_in,
					next_out, end_out);
			return;
		}

		// If there was previously buffered input, we have no choice but
		// continue to append supplied input into the buffer. Or,
		// if there is no previously buffered input, but the supplied
		// input is too small, then begin buffering it.

		if (tmp_avail_in ||
		    filter_t::avail_in < (size_t)(current_codecvt.max_length * 2))
		{
			size_t consumed=0;

			// Keep buffering for some reasonable period of time.

			size_t max_length=current_codecvt.max_length*100;

			while (filter_t::avail_in > 0)
			{
				tmp_in_buffer.push_back(*filter_t::next_in++);
				--filter_t::avail_in;
				++consumed;

				if (tmp_in_buffer.size() >= max_length)
				{
					tmp_next_in=&tmp_in_buffer[0];
					tmp_avail_in=tmp_in_buffer.size();

					do_filter_inout(tmp_next_in,
							tmp_avail_in,
							next_out, end_out);

					if (tmp_avail_in < consumed)
					{
						filter_t::next_in-=tmp_avail_in;
						filter_t::avail_in+=tmp_avail_in;
						tmp_avail_in=0;
					}
					return;
				}
			}
			tmp_next_in=&tmp_in_buffer[0];
			tmp_avail_in=tmp_in_buffer.size();
			return;
		}

		do_filter_inout(filter_t::next_in, filter_t::avail_in,
				next_out, end_out);
	}

	//! Filter function implementation.

	//! Converts available input sequence from \c next_in and
	//! \c avail_in into the output buffer \c next_out and
	//! \c avail_out.
	void do_filter_inout(const input_type *&next_in, size_t &avail_in,
			     output_type *&next_out,
			     output_type *&end_out)
	{
		const input_type *next_in_out;
		output_type *next_out_out;

		std::codecvt_base::result res;

		{
			globlocale switch_locale(localeRef);

			res=impl_type::inout(*current_codecvt.cptr,
					     current_state, next_in,
					     next_in + avail_in,
					     next_in_out,
					     next_out, end_out,
					     next_out_out);
		}

		size_t in_processed = next_in_out - next_in;

		avail_in -= in_processed;
		next_in += in_processed;

		next_out=next_out_out;

		if (res == current_codecvt.cptr->noconv)
		{
			while (avail_in && next_out < end_out)
			{
				*next_out++ = *next_in++;
				--avail_in;
				--next_in;
			}
			return;
		}

		if (res == current_codecvt.cptr->error)
		{
			current_state=state_type();
			if (avail_in) // Should always be the case.
			{
				++next_in;
				--avail_in;
			}
			in_failed(localeRef->name());
			return;
		}
	}
};

/* BUG: gcc 4.6 barfs if this is declared inline in the class */

template<typename x_codecvt_T,
	 typename codecvt_input_type,
	 typename codecvt_output_type,
	 typename impl_type>
void basic_iofiltercodecvtcommon<x_codecvt_T, codecvt_input_type,
				  codecvt_output_type,
				  impl_type>::filter()
{
	if (tmp_out_start && flushtmp())
		return; // Flushed previously buffered output.

	size_t min_length=current_codecvt.max_length*2;

	if (filter_t::avail_out < min_length)
	{
		// Supplied output buffer too small, redirect.

		tmp_out_buffer.resize(min_length);
		tmp_out_end=(tmp_out_start=&tmp_out_buffer[0])
			+ min_length;

		do_filter_in(tmp_out_start, tmp_out_end);
		tmp_out_end=tmp_out_start;
		tmp_out_start=&tmp_out_buffer[0];
		flushtmp();
		return;
	}

	output_type *out=filter_t::next_out;

	do_filter_in(out, out+filter_t::avail_out);

	filter_t::avail_out -= out - filter_t::next_out;
	filter_t::next_out=out;
}

#if 0
{
#endif
}
#endif
