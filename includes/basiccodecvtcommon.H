/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basiccodecvtcommon_H
#define x_basiccodecvtcommon_H

#include <x/namespace.h>
#include <x/facet.H>
#include <x/imbue.H>
#include <x/locale.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Base class of common functions for basic_codecvtreadseq and basic_codecvtwriteseq.

//! This class provides some common functions that are used by the
//! basic_codecvtreadseq and basic_codecvtwriteseq templates.

class basic_codecvt_common {

public:

	//! Internal error - std::codecvt::in() failed.

	//! This function throws an error -- std::codecvt::in() did not
	//! convert the requested range.

	void in_failed(const std::string &localeName);

	//! A holding container for a constant reference to a \c std::codecvt instance.

	//! The C++ language definition specifies an unusual implementation of
	//! \c std::codecvt objects, objects that implement character
	//! conversion. These objects are instantiated indirectly, as constant
	//! references.
	//!
	//! The constant reference cannot be a direct member of a class,
	//! since the reference itself cannot be updated, but we do
	//! want to support
	//! the functionality of switching character conversion objects.
	//! This container class takes care of the messy details, by
	//! turning the instantiated class reference into a pointer.
	//! A separate reference is maintained to the underlying facet class,
	//! thus ensuring the validity of the reference.

	template<typename x_codecvt_T>
	class container {

		//! Reference to the underlying facet wrapper.

		ptr< facetObj<x_codecvt_T> > codecvt_ref;

	public:
		//! The character conversion object.
		const x_codecvt_T *cptr;

		//! Cache ptr->always_conv().

		//! Rather than check ptr.always_conv() each time, do it
		//! once, and cache it. \c always_conv is set when this
		//! object is initialized.
		bool always_noconv;

		//! Cache ptr->max_length()

		//! Another codecvt property that's worth
		//! caching.

		int max_length;

		//! The default constructor.

		//! Construct a container for a reference to a constant
		//! instance of a \c x_codecvt_T.

		container( //! The current locale
			  const const_locale &localeArg) :
			codecvt_ref(localeArg->get_facet<ptr< facetObj<x_codecvt_T> > >()),
			cptr(&codecvt_ref->getFacetConstRef())
		{
			globlocale switch_locale(localeArg);

			always_noconv=cptr->always_noconv();
			max_length=cptr->max_length();
		}
	};

};

#if 0
{
#endif
}
#endif
