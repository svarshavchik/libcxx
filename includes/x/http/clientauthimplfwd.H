/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef http_clientauthimplfwd_H
#define http_clientauthimplfwd_H

#include <x/ref.H>
#include <x/to_string.H>
#include <x/namespace.h>
#include <unordered_map>
#include <string>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! Known authorization schemes, in order from least to most preferred

enum class auth { unknown, basic, digest };

class clientauthimplBase;
class clientauthimplObj;

//! Convert authorization scheme to a string

std::string auth_tostring(auth a);

//! Convert authorization scheme from a string

auth auth_fromstring(const std::string &s);

//! A reference-counted client-side authorization module.

//! \internal

typedef ref<clientauthimplObj, clientauthimplBase> clientauthimpl;

//! A nullable reference pointer to aclient-side authorization module.

//! \internal

typedef ptr<clientauthimplObj, clientauthimplBase> clientauthimplptr;


//! Container for an internal authorization map.

typedef std::unordered_map<std::string, clientauthimpl> authorization_map_t;

#if 0
{
#endif
}
#endif
