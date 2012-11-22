/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_formfwd_H
#define x_http_formfwd_H

#include <x/namespace.h>
#include <x/obj.H>

#include <map>
#include <string>

namespace LIBCXX_NAMESPACE {

	namespace http {

		//! Encode and decode HTML forms for HTTP

		//! This namespace defines \ref parameters "parameters",
		//! a map that
		//! holds parameters submitted from a web form.
		//!
		//! The parameter names and values are specified in UTF-8.

		namespace form {
#if 0
		};
	};
};
#endif

//! The type of a map that holds form parameters.

//! By fiat, the character set is UTF-8.

typedef std::multimap<std::string, std::string> map_t;

//! The reference-counted form parameters object

class parametersObj;
class parametersBase;

//! A reference to a form parameter object

typedef ref<parametersObj, parametersBase> parameters;

//! A nullable pointer reference to a form parameter object

typedef ptr<parametersObj, parametersBase> parametersptr;

//! A const reference to a form parameter object

typedef const_ref<parametersObj, parametersBase> const_parameters;

//! A nullable const pointer reference to a form parameter object

typedef const_ptr<parametersObj, parametersBase> const_parametersptr;

#if 0
{
	{
		{
#endif
		}
	}
}
#endif
