/*
** Copyright 2012-2021 Double Precision, Inc.
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
class messagesBase;

template<typename Msg, typename ...Args>
auto gettextmsg(Msg &&msg, Args && ...args);

//! Message catalogs

//! This class implements access to message catalogs using
//! the \c gettext(3) family of library calls.
//!
//! \code
//! auto msgcat=INSERT_LIBX_NAMESPACE::messages::create("libx"));
//!
//! auto l=INSERT_LIBX_NAMESPACE::locale::base::environment();
//!
//! auto msgcat=INSERT_LIBX_NAMESPACE::messages::create("libx", "/usr/share/libx/messages", l);
//! \endcode
//!
//! create()'s sole required parameter is the name of the text domain.
//! Each of the following parameters is optional, but any that appear
//! must in the following order:
//!
//! - the name of the directory with the message catalog files. See
//! bindtextdomain(3). An exception gets thrown if the domain is already bound
//! to a different directory
//!
//! - the \ref locale "locale" that specifies the messages' encoding. This
//! defaults to \c INSERT_LIBX_NAMESPACE::locale::base::global().
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
//! \par Unicode
//!
//! \code
//! std::u32string txt=msgcat->get_unicode("%d record", "%d records", 2);
//! \endcode
//!
//! get_unicode() is a version of the overloaded get() that converts the
//! message to a Unicode string. Note that the localized message text is
//! still a \c char string. Also, the underlying gettext() library still
//! transcodes the localized message to the locale's character set, before
//! converting it to unicode. Use UTF-8 message catalogs to make this a
//! less painful process.
//!
//! \see gettextmsg

typedef ref<messagesObj, messagesBase> messages;

//! A constant messages reference

//! \see #messages

typedef const_ref<messagesObj, messagesBase> const_messages;

//! A pointer to a reference-counted locale messages object.

//! \see #messages

typedef ptr<messagesObj, messagesBase> messagesptr;

//! A pointer to a reference-counted locale messages object.

//! \see #messages
typedef const_ptr<messagesObj, messagesBase> const_messagesptr;

//! Retrieve internal catalog

//! \internal

messages libmsg();

//! Retrieve a message from a catalog

//! \internal
//!
std::string libmsg(const char *str);

#if 0
{
#endif
}
#endif
