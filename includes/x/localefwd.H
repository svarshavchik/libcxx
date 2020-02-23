/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_localefwd_H
#define x_localefwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class localeBase;
class localeObj;

//! A wrapper for a \c std::locale.

//! This is a reference to a wrapper for a \c std::locale.
//! \code
//! INSERT_LIBX_NAMESPACE::locale myLocale=INSERT_LIBX_NAMESPACE::locale::base::global();          // Instantiate the default, current locale
//!
//! INSERT_LIBX_NAMESPACE::locale myLocale=INSERT_LIBX_NAMESPACE::locale::base::utf8();            // Instantiate the C.UTF-8 locale
//!
//! INSERT_LIBX_NAMESPACE::locale myLocale=INSERT_LIBX_NAMESPACE::locale::base::environment();     // Instantiate the system environment locale.
//!
//! INSERT_LIBX_NAMESPACE::locale myLocale=INSERT_LIBX_NAMESPACE::locale::base::c();               // Instantiate the C locale
//!
//! INSERT_LIBX_NAMESPACE::locale myLocale(INSERT_LIBX_NAMESPACE::locale::create("ru_RU.UTF-8"));  // Instantiate a specific locale
//! \endcode
//!
//! Typical usage:
//!
//! \code
//! const char *name=myLocale->name(); // Return the name of the locale
//!
//! myLocale->global();    // Make this locale the default locale
//! \endcode
//!
//! \see facets, for additional usage examples.

typedef ref<localeObj, localeBase> locale;

//! A constant reference to a \c std::locale

//! \see #locale

typedef const_ref<localeObj, localeBase> const_locale;

//! A pointer  to a \c std::locale

//! \see #locale

typedef ptr<localeObj, localeBase> localeptr;

//! A constant pointer to a \c std::locale

//! \see #locale

typedef const_ptr<localeObj, localeBase> const_localeptr;

#if 0
{
#endif
}
#endif
