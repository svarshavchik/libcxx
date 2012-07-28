/*
** Copyright 2012 Double Precision, Inc.
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

template<typename char_type,
	 typename traits_type=std::char_traits<char_type>,
	 typename alloc_type=std::allocator<char_type> >
class basic_messagesObj;

//! Narrow character message catalog implementation.

typedef basic_messagesObj<char> messagesObj;

//! Wide character message catalog implementation.

typedef basic_messagesObj<wchar_t> wmessagesObj;

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
//! \ref basic_messagesObj<char, traits_type, alloc_type>::get() "get()"
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

//! Wide character message catalogs

//! \code
//! locale l;
//!
//! wmessages msgcat(wmessages::create(l, "libx"));
//!
//! std::wstring txt=msgcat->get(L"Yes");
//! \endcode
//!
//! This is the wide character version of the #messages catalog. get()
//! get() returns wide character strings.
//!
//! The underlying gettext() API still uses narrow characters. This wide
//! character implementation converts wide character strings to narrow
//! character strings using the given locale (the keys should contain Latin
//! characters only, anyway), and the locallized strings get converted back
//! to wide character strings, using the same locale. As such, this is
//! slower than the narrow characters messages implementation, but exists
//! for convenience when the application primarily handles wide character
//! strings.
//!
//! \see gettextmsg()

typedef ref<wmessagesObj> wmessages;

//! A constant messages reference

//! \see #messages

typedef const_ref<messagesObj> const_messages;

//! A constant wmessages reference

//! \see #messages

typedef const_ref<wmessagesObj> const_wmessages;

//! A pointer to a reference-counted locale messages object.

//! \see #messages

typedef ptr<messagesObj> messagesptr;

//! A pointer to a reference-counted locale messages object.

//! \see #messages

typedef ptr<wmessagesObj> wmessagesptr;

//! A pointer to a reference-counted locale messages object.

//! \see #messages
typedef const_ptr<messagesObj> const_messagesptr;

//! A pointer to a reference-counted locale messages object.

//! \see #messages

typedef const_ptr<wmessagesObj> const_wmessagesptr;

//! Retrieve internal catalog

//! \internal

messages libmsg();

//! Retrieve internal catalog

//! \internal

wmessages wlibmsg();


//! Retrieve a message from a catalog

//! \internal
//!
std::string libmsg(const char *str) LIBCXX_HIDDEN;

//! Retrieve a message from a catalog

//! \internal
//!
std::wstring wlibmsg(const char *str) LIBCXX_HIDDEN;

#if 0
{
#endif
}
#endif
