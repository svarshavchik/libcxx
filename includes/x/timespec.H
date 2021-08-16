/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

/*! \file

This file defines timespec, a wrapper for \c struct \c %timespec that provides
a few value-added conveniences.

*/
#ifndef x_timespec_H
#define x_timespec_H

#include <x/namespace.h>
#include <x/exception.H>
#include <x/locale.H>

#include <iostream>
#include <limits>
#include <cstdlib>
#include <chrono>
#include <sys/time.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A wrapper for a \c struct \c %timespec \anchor timespecAnchor

//! The wrapper provides several convenient constructors for initializing
//! the \c %timespec structure -  a time value with nanosecond precision.

class timespec : public ::timespec {

	//! Number of nanoseconds in a second

	static constexpr long billion=1000000000;

public:
	//! Default constructor -- initialize timespec to 0 seconds

	timespec(//! Number of seconds
		 time_t seconds=0) noexcept
	{
		tv_sec=seconds;
		tv_nsec=0;
	}

	//! Initialize timespec to the given seconds and nanoseconds

	//! \overload
	//!
	timespec(//! Number of seconds
		 time_t seconds,

		 //! Nanoseconds: 0 - 999999999

		 long nanoseconds) noexcept
	{
		tv_sec=seconds + nanoseconds / billion;
		tv_nsec=nanoseconds % billion;
	}

	//! Initialize timespec to the given seconds and nanoseconds expressed as a fraction

	//! \overload
	//!
	timespec(//! Number of seconds
		 time_t seconds,

		 //! Nominator

		 int nano_nominator,

		 //! Denominator, must be greater than \c nano_nominator

		 int nano_denominator) noexcept
	{
		long nanoseconds= nano_denominator ? billion / nano_denominator
			* nano_nominator:0;

		tv_sec=seconds + nanoseconds / billion;
		tv_nsec=nanoseconds % billion;
	}

	//! Return a timespec that holds the current clock time

	static timespec getclock(//! Clock ID

				 //! Possible values are
				 //! \c CLOCK_REALTIME,
				 //! \c CLOCK_MONOTONIC,
				 //! \c CLOCK_PROCESS_CPUTIME_ID,
				 //! and \c CLOCK_THREAD_CPUTIME_ID.

				 clockid_t clock_id=CLOCK_REALTIME);

	//! Comparison operator

	bool operator<(const timespec &o) const noexcept
	{
		return tv_sec < o.tv_sec ||
			(tv_sec == o.tv_sec &&
			 tv_nsec < o.tv_nsec);
	}

	//! Comparison operator

	bool operator>(const timespec &o) const noexcept
	{
		return tv_sec > o.tv_sec ||
			(tv_sec == o.tv_sec &&
			 tv_nsec > o.tv_nsec);
	}

	//! Comparison operator

	bool operator==(const timespec &o) const noexcept
	{
		return tv_sec == o.tv_sec &&
			tv_nsec == o.tv_nsec;
	}

	//! Comparison operator

	bool operator!=(const timespec &o) const noexcept
	{
		return !operator==(o);
	}
	//! Comparison operator

	bool operator<=(const timespec &o) const noexcept
	{
		return !operator>(o);
	}

	//! Comparison operator

	bool operator>=(const timespec &o) const noexcept
	{
		return !operator<(o);
	}

	//! Compute a difference between this timespec and another one.

	//! \return the difference between this and another timespec

	timespec operator-(//! The other timespec
			   const timespec &o) const noexcept
	{
		timespec t(*this);

		t -= o;

		return t;
	}

	//! Subtract another timespec from this one.

	//! \return a reference to this timespec object.

	timespec &operator-=(const timespec &o) noexcept
	{
		tv_sec -= o.tv_sec;

		if (tv_nsec < o.tv_nsec)
		{
			tv_nsec -= o.tv_nsec;
			tv_nsec += billion;
			--tv_sec;
		}
		else
			tv_nsec = tv_nsec - o.tv_nsec;

		return *this;
	}

	//! Add two timespecs together.

	//! \return the sum of this and the other timespec

	timespec operator+(//! Another timespec
			   const timespec &o) const noexcept
	{
		timespec t(*this);

		t += o;
		return t;
	}

	//! Add another timespec to this one

	//! \return a reference to this timespec

	timespec &operator+=(const timespec &o) noexcept
	{
		tv_sec = tv_sec+o.tv_sec;
		tv_nsec = tv_nsec+o.tv_nsec;

		if (tv_nsec >= billion)
		{
			tv_nsec -= billion;
			++tv_sec;
		}
		return *this;
	}

	//! Convert this timespec class to a string

	template<typename OutputIterator>
	OutputIterator to_string(//! Output iterator
				OutputIterator iter,

				//! Locale for the conversion
				const const_locale &localeRef=locale::base::global())
		const
	{
		char buf[std::numeric_limits<time_t>::digits10 +
			 std::numeric_limits<long>::digits10 + 1];

		char *p=buf+sizeof(buf);

		long n=tv_nsec;

		for (size_t i=0; i<9; ++i)
		{
			*--p='0' + (n%10);
			n /= 10;
		}

		*--p='.';

		time_t s=tv_sec;

		do
		{
			if (p == buf)
				abort();
			*--p = '0' + (s%10);
			s /= 10;
		} while (s > 0);

		return std::copy(p, buf+sizeof(buf), iter);
	}

	//! Create an interval from a string

	template<typename InputIterator>
	static timespec from_string(//! Beginning iterator
				   InputIterator beg_iter,

				   //! Ending iterator
				   InputIterator end_iter,

			      //! The locale
			      const const_locale &localeArg=locale::base::global())

	{
		timespec ts;

		while (beg_iter != end_iter &&
		       *beg_iter >= '0' && *beg_iter <= '9')
		{
			ts.tv_sec = ts.tv_sec * 10 + *beg_iter - '0';
			++beg_iter;
		}

		if (beg_iter != end_iter && *beg_iter == '.')
		{
			while (++beg_iter != end_iter &&
			       *beg_iter >= '0' && *beg_iter <= '9')
			{
				ts.tv_nsec = ts.tv_nsec * 10 + *beg_iter - '0';
			}
		}

		return ts;
	}

	//! Convert to a timeval

	operator timeval() const noexcept
	{
		timeval tv;

		tv.tv_sec=tv_sec;
		tv.tv_usec=tv_nsec / 1000;
		return tv;
	}

	//! Convert to a std::chrono duration

	template<typename duration_type>
	duration_type to_duration() const
	{
		return std::chrono::duration_cast<duration_type>
			(std::chrono::nanoseconds(tv_nsec)) +
			std::chrono::duration_cast<duration_type>
			(std::chrono::seconds(tv_sec));
	}
};

#if 0
{
#endif
}
#endif
