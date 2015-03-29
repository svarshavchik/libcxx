/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_facetobj_H
#define x_facetobj_H

#include <x/localeobj.H>
#include <x/ptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A wrapper for a \c std::Facet.

//! This template implements a class that holds a reference to a
//! \c std::Facet. A reference to this reference-counted object
//! gets returned by get_facet().

template<class FacetType> class facetObj : virtual public obj {

	//! Reference to this facet's locale.

	const_ptr<localeObj> locale;

	//! The facet itself
	const FacetType &facet;

public:

	//! Define the facet type implemented by this wrapper

	typedef FacetType facet_type;

	//! The default constructor.
	facetObj(
		 //! This facet's locale.

		 //! This class keeps a reference to this locale,
		 //! to make sure that the reference to the std::Facet
		 //! remains valid.
		 const localeObj *localeArg,

		 //! The facet itself.
		 const FacetType &facetArg) noexcept
		: locale(const_cast<localeObj *>(localeArg)),
		  facet(facetArg)
	{
	}

	//! The default destructor

	~facetObj() noexcept
	{
	}

	//! Return the std::Facet handle.
	const FacetType &getFacetConstRef() const { return facet; }

	//! Return the locale
	const const_ptr<localeObj> &getLocale() const { return locale; }
};

#if 0
{
#endif
}
#endif
