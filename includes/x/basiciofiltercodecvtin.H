/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basiciofiltercodecvtin_H
#define x_basiciofiltercodecvtin_H

#include <x/basiciofiltercodecvtcommon.H>
#include <x/namespace.h>

#include <vector>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Implementation class for basic_codecvtin

//! basic_codecvtin_impl::inout() gets invoked by the
//! basic_iofiltercodecvtcommon subclass of  basic_codecvtin

template<typename x_codecvt_T> class basic_codecvtin_impl {

public:
	//! Convenience typedef
	typedef typename x_codecvt_T::extern_type extern_type;

	//! Convenience typedef
	typedef typename x_codecvt_T::intern_type intern_type;

	//! The requested conversion is effected by invoking the locale's in() method

	static inline std::codecvt_base::result
	inout(const x_codecvt_T &codecvt,
	      typename x_codecvt_T::state_type &state,
	      const extern_type *from,
	      const extern_type *from_end,
	      const extern_type *&from_next,
	      intern_type *to,
	      intern_type *to_limit,
	      intern_type *&to_next)
		LIBCXX_INTERNAL
	{
		return codecvt.in(state, from, from_end, from_next,
				  to, to_limit, to_next);
	}
};

//! An IO filter that converts characters to wide characters

//! This is a subclass of the \c iofilter template that implements
//! character-to-wide-character conversion.
//!
//! This is defined as a template class, with the localization library's
//! \c codecvt facet as the only template parameter. The \c ::ctow_codecvt
//! typedef instantiates the sole implementation using the sole defined
//! \c codecvt instance.
//!
//! The heavy lifting is done in the basic_iofiltercodecvtcommon superclass,
//! which contains the logic that's common with basic_codecvtout.

template<typename x_codecvt_T>
class basic_codecvtin :
	public basic_iofiltercodecvtcommon<x_codecvt_T,
					   typename x_codecvt_T::extern_type,
					   typename x_codecvt_T::intern_type,
					   basic_codecvtin_impl<x_codecvt_T> > {

public:
	//! Default constructor

	basic_codecvtin(//! The locale for conversions.
			const const_locale &localeArg=locale::base::global()) noexcept
		: basic_iofiltercodecvtcommon<x_codecvt_T,
					      typename x_codecvt_T::extern_type,
					      typename x_codecvt_T::intern_type,
					      basic_codecvtin_impl<x_codecvt_T>
					      >(localeArg)
	{
	}

	//! Default destructor
	~basic_codecvtin() noexcept
	{
	}
};

//! An IO filter that converts characters to wide characters

typedef basic_codecvtin< std::codecvt<wchar_t, char, mbstate_t> > ctow_codecvt;

extern template class basic_codecvtin< std::codecvt<wchar_t, char, mbstate_t> >;
extern template class basic_iofiltercodecvtcommon<std::codecvt<wchar_t, char,
							       mbstate_t>,
						  char, wchar_t,
						  basic_codecvtin_impl
						  <std::codecvt<wchar_t, char,
								mbstate_t> >
						  >;
#if 0
{
#endif
}
#endif
