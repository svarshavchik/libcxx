/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_locale_H
#define x_locale_H

#include <x/localefwd.H>
#include <x/localeobj.H>
#include <x/ref.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Refer to this class as \c locale::base or \c localeptr::base

class localeBase : public ptrref_base {
public:

	//! The global locale

	static const_locale global();

	//! A UTF-8 locale

	//! \c locale::base::utf8() returns the C.UTF-8 locale.

	static const_locale utf8();

	//! A C locale

	//! \c locale::base::c() return the "C" locale.
	static const_locale c();

	//! The global system environment locale

	static const_locale environment();
};

#if 0
{
#endif
}
#endif
