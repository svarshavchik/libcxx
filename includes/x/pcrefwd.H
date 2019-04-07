/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_pcrefwd_H
#define x_pcrefwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class pcreObj;

//! Perl-compatible regular expression.

//! Implement regular expression matching using the pcre library. This is
//! a reference to an object that represents a pattern. Invoke
//! \ref pcreObj "pcre->match()" to match the pattern against a string.

typedef ref<pcreObj> pcre;

//! A nullable pointer reference to a \ref pcre "pcre" object.

typedef ptr<pcreObj> pcreptr;

//! A reference to a constant \ref pcre "pcre" object.

typedef const_ref<pcreObj> const_pcre;

//! A nullable pointer reference to a constant \ref pcre "pcre" object.

typedef const_ptr<pcreObj> const_pcreptr;

#if 0
{
#endif
}
#endif

