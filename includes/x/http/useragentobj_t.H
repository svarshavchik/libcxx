/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN

#ifdef x_http_useragentobj_H

LIBCXX_TEMPLATE_DECL template
LIBCXX_NAMESPACE::http::useragentObj::response
LIBCXX_NAMESPACE::http::useragentObj::request(const LIBCXX_NAMESPACE::fd &,
					    LIBCXX_NAMESPACE::http::requestimpl &)
;

#if 0
LIBCXX_TEMPLATE_DECL template
LIBCXX_NAMESPACE::http::useragentObj::response
LIBCXX_NAMESPACE::http::useragentObj::request(LIBCXX_NAMESPACE::http::requestimpl &)
;
#endif

#define LIBCXX_TEMPLATE_ITER std::string::iterator
#include <x/http/useragentobj_to.H>
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::string::const_iterator
#include <x/http/useragentobj_to.H>
#undef LIBCXX_TEMPLATE_ITER

#endif

#ifdef x_http_useragentobj_H
#ifdef x_http_form_H

#define LIBCXX_TEMPLATE_ITER LIBCXX_NAMESPACE::http::form::parametersObj::encode_iter
#include <x/http/useragentobj_to.H>
#undef LIBCXX_TEMPLATE_ITER

#endif
#endif

#endif
