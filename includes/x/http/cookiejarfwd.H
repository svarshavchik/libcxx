/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_cookiejarfwd_H
#define x_http_cookiejarfwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

class cookiejarObj;
class cookiejarBase;

//! A reference to a cookie jar

typedef ref<cookiejarObj, cookiejarBase> cookiejar;

//! A nullable pointer reference to a cookie jar

//! \see cookiejar

typedef ptr<cookiejarObj, cookiejarBase> cookiejarptr;

//! A nullable reference to a const cookie jar

//! \see cookiejar

typedef const_ref<cookiejarObj, cookiejarBase> const_cookiejar;

//! A nullable pointer reference to a const cookie jar

//! \see cookiejar

typedef const_ptr<cookiejarObj, cookiejarBase> const_cookiejarptr;
#if 0
{
#endif
}
#endif
