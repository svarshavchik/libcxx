/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#define LIBCXX_TEMPLATE_ITER std::istreambuf_iterator<char>
#include <x/http/form_to.H>
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER fdinputiter
#include <x/http/form_to.H>
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER2 std::string::const_iterator
#include <x/http/form_to2.H>
#undef LIBCXX_TEMPLATE_ITER2

#define LIBCXX_TEMPLATE_ITER2 std::string::iterator
#include <x/http/form_to2.H>
#undef LIBCXX_TEMPLATE_ITER2

#ifndef DOXYGEN

LIBCXX_TEMPLATE_DECL template
parametersObj::parametersObj(const std::string &str);

LIBCXX_TEMPLATE_DECL template
parametersObj::parametersObj(receiverimpl<requestimpl,
			     std::istreambuf_iterator<char> > &,
			     const requestimpl &req, bool);

LIBCXX_TEMPLATE_DECL template
parametersObj::parametersObj(receiverimpl<requestimpl,
			     std::istreambuf_iterator<char> > &,
			     const requestimpl &req, bool,
			     bool &);

LIBCXX_TEMPLATE_DECL template
bool parametersObj::process_received_request(receiverimpl<requestimpl,
					     std::istreambuf_iterator<char> > &,
					     const requestimpl &req, bool);
#endif
