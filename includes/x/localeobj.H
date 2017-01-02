/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_localeobj_H
#define x_localeobj_H

#include <locale>
#include <string>
#include <exception>
#include <x/exceptionfwd.H>
#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <x/messagesfwd.H>
#include <x/obj.H>
#include <x/xlocale.H>
#include <x/globlocale.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class thread_locale;

template<class FacetType> class facetObj;

template<typename obj_type> class imbue;

//! Implement a reference-counted wrapper for a \c std::locale.

//! \c std::locale returns constant references to facets that, as the C++
//! standard specifies, remain visible as long as some locale object that
//! references the same locale, exists.
//!
//! This class wraps a \c std::locale in an obj, so that we can properly track
//! the references ourselves.

class localeObj : virtual public obj {

	//! The wrapped \c std::locale object.
	std::locale locale;

	//! We need xlocale to reimplement the messages facet

	//! The extended handle gets created in parallel to the C++ library
	//! locale object. Unfortunately, std::messages is insufficient for
	//! our needs, so we must reimplement it.

	xlocale x;

	//! Helper function for creating new locales.

	//! \internal This function creates a \c std::locale, and traps and
	//! converts a \c std::exception to our-style exception.
	static std::locale create_locale(const char *explicit_name);

	//! Cached character set.

	std::string locale_charset;

	//! A locale was deserialized.
	void deserialized(const std::string &n);
public:
	//! Instantiate a locale object.

	//! Once constructed, the locale member remains hands
	//! off.
	localeObj(const char *localeName=nullptr);

	//! Instantiate a locale object.

	//! \overload
	//!

	localeObj(const std::string &localeName);

	//! The default destructor.

	//! Deallocate the \c std::locale
	//! object.
	~localeObj();

	//! Return the name of this locale

	//! \return this locale's
	//! name.

	inline const std::string &name() const noexcept
	{
		return x.n;
	}

	//! Return the character set used by this locale

	inline const std::string &charset() const noexcept
	{
		return locale_charset;
	}

	//! Convert text string in this locale's charset() to lowercase.

	std::string tolower(const std::string &text) const;

	//! Convert text string in this locale's charset() to uppercase.

	std::string toupper(const std::string &text) const;

	//! Make this locale the global locale.

	//! The global locale is the default locale used by the C
	//! library.
	void global() const noexcept;

	template<typename imbue_obj> friend class imbue;

	//! Serialization function

	//! Serializing a locale object or a reference serializes the
	//! locale's name. Deserializing a locale object recreates a locale
	//! object using the serialized locale name.

	template<typename iter_type>
	void serialize(iter_type &i)
	{
		if (i.serializing)
		{
			std::string n=name();

			i(n);
		}
		else
		{
			std::string n;

			i(n);

			deserialized(n);
		}
	}

	friend class thread_locale;

	//! Helper function that reports a \c std::exception.

	//! \internal
	//! This function throws an exception using the given message.

	static void throw_locale_exception(//! Error message
					   const std::string &errmsg)

		__attribute__((noreturn)) LIBCXX_INTERNAL;

private:
	//! Helper function reports a nonexistent facet.

	//! \internal
	//! This function throws an exception.

	static void throw_unknown_facet()

		__attribute__((noreturn));

public:

	//! Obtain a \c std::locale facet.

	//! This template function provides an interface to the
	//! std::use_facet() function. The std::bad_cast exception thrown
	//! by a non-existent facet gets translated to our kind of an
	//! exception.
	//!
	//! Instantiate this template using one of the references in the
	//! facets namespace. Example:
	//!
	//! \code
	//! facets::time_get_facetrs time_get
	//!     myLocale->get_facet<time_get_facet>();
	//! \endcode
	//! \return a reference to a facetObj instance.

	template<typename ref_type> ref_type
	get_facet() const
	{
		try {
			globlocale switch_locale(const_ptr<localeObj>(this));

			const typename ref_type::obj_type::facet_type
				&f=std::use_facet<typename ref_type::obj_type::facet_type>(locale);

			return ref<facetObj<typename ref_type::obj_type
					    ::facet_type> >
				::create(this, f);
		} catch(std::exception &e)
		{
			throw_unknown_facet();
			/* NOTREACHED */
		}
	}
};

#if 0
{
#endif
}
#endif
