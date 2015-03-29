/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basiciofiltercodecvtout_H
#define x_basiciofiltercodecvtout_H

#include <x/basiciofiltercodecvtcommon.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Implementation class for basic_codecvtout

//! basic_codecvtin_impl::inout() gets invoked by the
//! basic_iofiltercodecvtcommon subclass of  basic_codecvtout

template<typename x_codecvt_T> class basic_codecvtout_impl {

public:
	//! Convenience typedef
	typedef typename x_codecvt_T::extern_type extern_type;

	//! Convenience typedef
	typedef typename x_codecvt_T::intern_type intern_type;

	//! The requested conversion is effected by invoking the locale's out() or unshift()

	static inline std::codecvt_base::result
	inout(const x_codecvt_T &codecvt,
	      typename x_codecvt_T::state_type &state,
	      const intern_type *from,
	      const intern_type *from_end,
	      const intern_type *&from_next,
	      extern_type *to,
	      extern_type *to_limit,
	      extern_type *&to_next)
		LIBCXX_INTERNAL
	{
		if (from != from_end)
			return codecvt.out(state, from, from_end, from_next,
					   to, to_limit, to_next);

		from_next=from_end;
		return codecvt.unshift(state, to, to_limit, to_next);
	}
};


//! An IO filter that converts wide characters to characters

//! This is a subclass of the \c iofilter template that implements
//! wide-character-to-character conversion.
//!
//! This is defined as a template class, with the localization library's
//! \c codecvt facet as the only template parameter. The \c ::wtoc_codecvt
//! typedef instantiates the sole implementation using the sole defined
//! \c codecvt instance.
//!
//! The heavy lifting is done in the basic_iofiltercodecvtcommon superclass,
//! which contains the logic that's common with basic_codecvtin.

template<typename x_codecvt_T>
class basic_codecvtout :
	public basic_iofiltercodecvtcommon<x_codecvt_T,
					   typename x_codecvt_T::intern_type,
					   typename x_codecvt_T::extern_type,
					   basic_codecvtout_impl<x_codecvt_T> >{
public:
	//! The default constructor.

	//! The default constructor resets the conversion to its initial
	//! state.
	basic_codecvtout(//! The locale for conversions.
			const const_locale &localeArg=locale::base::global()) noexcept
								     
		: basic_iofiltercodecvtcommon<x_codecvt_T,
					      typename x_codecvt_T::intern_type,
					      typename x_codecvt_T::extern_type,
					      basic_codecvtout_impl<x_codecvt_T>
					      >(localeArg)
	{
	}

	//! The default destructor

	//! \note
	//! The output does not get flushed by the destructor.
	~basic_codecvtout() noexcept
	{
	}
};

//! An IO filter that converts wide characters to char

typedef basic_codecvtout< std::codecvt<wchar_t, char, mbstate_t> > wtoc_codecvt;

extern template class basic_codecvtout<std::codecvt<wchar_t, char, mbstate_t> >;

extern template class basic_iofiltercodecvtcommon<std::codecvt<wchar_t, char,
							       mbstate_t>,
						  wchar_t, char,
						  basic_codecvtout_impl
						  <std::codecvt<wchar_t, char,
								mbstate_t> > >;


#if 0
{
#endif
}
#endif
