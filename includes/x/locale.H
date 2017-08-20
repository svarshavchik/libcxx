/*
** Copyright 2012 Double Precision, Inc.
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

	static locale global();

	//! A UTF-8 locale

	//! \c locale::base::utf8() returns the en_US.UTF-8 locale.

	static locale utf8();

	//! The global system environment locale

	static locale environment();
};

#if 0
{
#endif
}
#endif
