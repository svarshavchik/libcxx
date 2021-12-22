/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_appid_H
#define x_appid_H

#include <x/namespace.h>
#include <string>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Application identifier

//! \code
//! std::string INSERT_LIBX_NAMESPACE::appid() noexcept
//! {
//!     return "example@example.com";
//! }
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::appid() must be defined as a global function
//! in the application code, and it
//! returns a string of the form "domain" or "identifier@domain",
//! where "domain" is an internet domain that's owned by the application.
//!
//! The library provides a default implementation of appid() when it is not
//! defined in the application. This default implementation constructs an
//! appid based on the executable's name and localhost.localdomain.
//!
//! \see appver

std::string appid() noexcept;

//! Application version

//! \code
//! std::string INSERT_LIBX_NAMESPACE::appver() noexcept
//! {
//!     return "1.0";
//! }
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::appver() must be defined as a global function
//! in the application code, and it declares the application's version,
//! expressed as a string containing some kind of a dotted-decimal identifier.
//!
//! The library provides a default implementation of appver() when it is not
//! defined in the application. This default implementation constructs an
//! version string based on the application's executable's timestamp.
//!
//! \see appid

std::string appver() noexcept;

#if 0
{
#endif
}
#endif
