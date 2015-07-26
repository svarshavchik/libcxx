/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_messagesfwd_H
#define x_messagesfwd_H

#include <x/exceptionfwd.H>
#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class messagesObj;

//! Message catalogs

//! This class implements access to message catalogs.
//!
//! \code
//! locale l;
//!
//! messages msgcat(messages::create(l, "libx"));
//! \endcode
//!
//! create() opens a new message catalog. The first parameter is a
//! locale, the second parameter is the message text domain.
//!
//! \code
//! locale l;
//!
//! messages msgcat(messages::create(l, "libx", "/usr/share/libx/msgs"));
//! \endcode
//!
//! An optional third parameter to create() specifies a non-standard
//! directory for the given message text domain's catalog file. The directory
//! must be an absolute path, and it must be specified only once, the first
//! time a message for the given domain is opened.
//!
//! \c messages is a reference to a reference-counted object, which may be
//! passed around freely.
//!
//! \code
//! std::string txt=msgcat->get("Yes");
//! \endcode
//!
//! \ref messagesObj::get() "get()"
//! retrieves the message
//! from the catalog, using the usual semantics of gettext().
//!
//! \par Handling plural forms
//!
//! \code
//! std::string txt=msgcat->get("%d record", "%d records", 2);
//! \endcode
//!
//! A three parameter version of get() implement the plural feature of message
//! catalogs. It retrieves the appropriate plural form of a message, from the
//! catalog, for the integral numerical value that's passed as a third
//! parameter. If a plural form is not available in the message catalog, the
//! first parameter gets returned if the numerical value is 1, otherwise
//! the second parameter gets returned.
//!
//! \see gettextmsg()

typedef ref<messagesObj> messages;

//! A constant messages reference

//! \see #messages

typedef const_ref<messagesObj> const_messages;

//! A pointer to a reference-counted locale messages object.

//! \see #messages

typedef ptr<messagesObj> messagesptr;

//! A pointer to a reference-counted locale messages object.

//! \see #messages
typedef const_ptr<messagesObj> const_messagesptr;

//! Retrieve internal catalog

//! \internal

messages libmsg();

//! Retrieve a message from a catalog

//! \internal
//!
std::string libmsg(const char *str) LIBCXX_HIDDEN;

#if 0
{
#endif
}
#endif
