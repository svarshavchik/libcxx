/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_facet_H
#define x_facet_H

#include <x/localeobj.H>
#include <x/facetobj.H>
#include <x/ptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<class FacetType> class facetObj;

//! Facet object specializations

//! This namespace defines certain common facetObj specializations --
//! specializations for some common \c std::locale %facets. This namespace
//! defines the reference-counted object implementations. These objects
//! are normally not accessed directly, but rather using the references
//! defined in the facets namespace.

namespace facetObjs {

	//! A \c std::ctype&lt;char&gt; facet implementation

	//! \internal
	//! \see facets::ctype

	typedef facetObj< std::ctype<char> > ctype;

	//! A \c std::time_put facet implementation

	//! \internal
	//! \see facets::time_put

	template<typename charT,
		 typename OutputIterator=std::ostreambuf_iterator<charT> >
	class time_put {

	public:
		//! The actual type

		//! C++ doesn't have template typedefs, yet.
		//!
		typedef facetObj< std::time_put<charT, OutputIterator> > type_t;
	};

	//! A \c std::time_get facet implementation

	//! \internal
	//! \see facets::time_get

	template<typename charT,
		 typename InputIterator=std::istreambuf_iterator<charT> >
	class time_get {

	public:
		//! The actual type

		//! C++ doesn't have template typedefs, yet.
		//!

		typedef facetObj< std::time_get<charT, InputIterator> > type_t;
	};

}

//! Facet reference definitions.

//! This namespace defines references to common facetObj specializations,
//! wrappers for some common facets.
//!
//! Typical usage:
//! \code
//! locale myLocale("en_US.utf-8");
//!
//! facets::time_get_facet time_get=myLocale->get_facet<facets::time_get_facet>();
//! \endcode
//!
//! The above implementation if for informational purposes only. There are
//! other classes that operate on %facet references directly. There's usually
//! no need to access the underlying \c std::locale %facet object directly.

namespace facets {


	//! Define a wrapper for the \c std::ctype&lt;char&gt; facet.

	//! See also: \ref ctype "ctype".

	typedef ref< facetObjs::ctype> ctype_facet;

	//! A pointer to a reference-counted ctype facet

	//! \see ctype_facet
	typedef ptr< facetObjs::ctype> ctype_facetptr;

	//! Define a wrapper for the \c std::time_put facet.

	//! See also: \ref strftime "strftime".

	template<typename charT,
		 typename OutputIterator=std::ostreambuf_iterator<charT> >
	using time_put_facet=ref<typename facetObjs::time_put<charT,
							      OutputIterator>
				 ::type_t>;

	//! A nullable reference pointer to a time_put facet.

	//! \see time_put_facet;

	template<typename charT,
		 typename OutputIterator=std::ostreambuf_iterator<charT> >
	using time_put_facetptr=ptr<typename facetObjs::time_put<charT,
								 OutputIterator>
				    ::type_t>;
	//! Define a wrapper for the \c std::time_get facet.

	template<typename charT,
		 typename InputIterator=std::istreambuf_iterator<charT> >
	using time_get_facet=ref<typename facetObjs::time_get<charT,
							      InputIterator>
				 ::type_t>;

	//! A nullable reference pointer to a time_get facet.

	//! \see time_get_facet;

	template<typename charT,
		 typename InputIterator=std::istreambuf_iterator<charT> >
	using time_get_facetptr=ptr<typename facetObjs::time_get<charT,
								 InputIterator>
				    ::type_t>;
}

#if 0
{
#endif
}
#endif
