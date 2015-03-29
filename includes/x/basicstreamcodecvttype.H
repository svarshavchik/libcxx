/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basicstreamcodecvttype_H
#define x_basicstreamcodecvttype_H

#include <x/basiciofiltercodecvtin.H>
#include <x/basiciofiltercodecvtout.H>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename input_type, typename output_type,
	 typename state_type=std::mbstate_t> class basic_streamcodecvt_type;

//! Specify an iofilter subclass for a widechar to char mapping

//! The \c basic_streamcodecvt_type template maps the desired character
//! conversion, specified as the "input" character type and an "output"
//! character type, into an implementing \c iofilter-based class.
//!
//! This, the \c wchar_t to \c char conversion, is implemented by
//! basic_codecvtout.
//!
//! This template's sole contents is a typedef called \c basicstreamcodecvt_t
//! that is a typedef for an \c iofilter subclass that implements the
//! specified conversion.

template<typename state_type>
class basic_streamcodecvt_type<wchar_t, char, state_type> {

public:

	//! This conversion is implemented by \c basic_codecvtin

	typedef basic_codecvtout< std::codecvt<wchar_t, char, state_type>
				  > basicstreamcodecvt_t;
};

//! Specify an iofilter subclass for a char to widechar mapping

//! The \c basic_streamcodecvt_type template maps the desired character
//! conversion, specified as the "input" character type and an "output"
//! character type, into an implementing \c iofilter-based class.
//!
//! This, the \c char to \c wchar_t conversion, is implemented by
//! basic_codecvtout.
//!
//! This template's sole contents is a typedef called \c basicstreamcodecvt_t
//! that is a typedef for an \c iofilter subclass that implements the
//! specified conversion.

template<typename state_type>
class basic_streamcodecvt_type<char, wchar_t, state_type> {

public:
	//! This conversion is implemented by \c basic_codecvtout

	typedef basic_codecvtin< std::codecvt<wchar_t, char, state_type>
				 > basicstreamcodecvt_t;
};

#if 0
{
#endif
}
#endif
