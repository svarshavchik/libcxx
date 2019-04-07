/*
** Copyright 2013-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_idn_H
#define x_idn_H

#include <x/namespace.h>
#include <x/locale.H>

#include <string>
#include <idna.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Functions for convert international domain names to/from ASCII compatible encoding used by the Domain Name System.

//! Use to_ascii() to convert an international domain into ASCII compatible
//! encoding used by DNS. If the input string does not contain an international
//! domain name, it is an ASCII string already, to_ascii() has no effect
//! (except waste a bunch of cycles doing needless conversions to/from UTF-8,
//! etc...)
//!
//! Use from_ascii() to convert a domain name from ASCII compatible encoding
//! used by DNS. If the input string does not use the ASCII compatible encoding
//! scheme used in DNS, from_ascii() has no effect too.

class idn {

public:

	//! Convert domain name to ASCII compatible encoding used by DNS

	static std::string to_ascii(//! i18n domain in the UTF-8 locale.
				    const std::string &str,

				    //! libidn flags, \c IDNA_ALLOW_UNASSIGNED or \c IDNA_USE_STD3_ASCII_RULES
				    int flags=0);



	//! Convert domain name from ASCII compatible encoding used by DNS to UTF-8

	static std::string from_ascii(//! Domain name, using ASCII compatible encoding

				      const std::string &str,

				      //! libidn flags, \c IDNA_ALLOW_UNASSIGNED or \c IDNA_USE_STD3_ASCII_RULES
				      int flags=0);
};

#if 0
{
#endif
}
#endif
