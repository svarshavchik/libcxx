/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_messages_H
#define x_messages_H

#include <x/exception.H>
#include <x/locale.H>
#include <x/localeobj.H>
#include <x/to_string.H>
#include <x/messagesfwd.H>
#include <string>
#include <map>
#include <algorithm>
#include <cstring>
#include <tuple>
#include <sstream>
#include <x/namespace.h>
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename ...Args>
class gettext_helper_ref;

//! Specialization for narrow character strings

class messagesObj : virtual public obj {

	//! Locale
	const const_locale l;

	//! The text domain
	std::string domain;

	//! Internal call to dngettext

	//! \internal
	const char *call_dngettext(//! Singular
				   const char *s,

				   //! Plural
				   const char *p,

				   //! The numeric value
				   unsigned long n) const;

	//! Internal call to dngettext

	template<typename int_type>
	const char *call_dngettext_wrapper(//! Singular
					   const char *s,

					   //! Plural
					   const char *p,

					   //! The numeric value
					   int_type n) const
	{
		if (n < 0)
			n= -n;

		if (n > 1000000)
			n=(n % 1000000)+1000000;

		return call_dngettext(s, p, n);
	}

public:
	//! Return the locale

	inline const_locale getLocale() const noexcept
	{
		return l;
	}

	//! The constructor

	messagesObj(//! The %messages' locale
		    const const_locale &lArg,

		    //! The %messages' domain
		    const std::string &domainArg);

	//! Constructor, set a directory's text domain

	messagesObj(//! The %messages' locale
		    const const_locale &lArg,

		    //! The %messages' domain
		    const std::string &domainArg,

		    //! The message catalog directory
		    const std::string &directory);

	//! Destructor

	~messagesObj();

	//! Retrieve a message from the catalog

	std::string get(//! The message to retrieve

			const char *s) const;

	//! Retrieve a message from the catalog

	//! \overload

	std::string get(//! The message to retrieve

			const std::string &s) const;

	//! Retrieve a message from the catalog

	//! This is a version that retrieves singular or plural form, as
	//! appropriate for a given int value.

	template<typename int_type>
	std::string get(//! The singular form

		     const char *s,

		     //! The plural form

		     const char *p,

		     //! The numeric value

		     int_type n
		     ) const
	{
		return call_dngettext_wrapper(s, p, n);
	}

	//! Retrieve a message from the catalog

	//! \overload
	template<typename int_type>
	std::string get(//! The singular form

			const std::string &s,

		     //! The plural form

			const std::string &p,

		     //! The numeric value

		     int_type n) const
	{
		return get(s.c_str(), p.c_str(), n);
	}

	//! Retrieve a message from the catalog, as unicode.

	template<typename ...Args>
	inline std::u32string get_unicode(Args && ...args) const
	{
		return to_unicode(get(std::forward<Args>(args)...));
	}

	//! Convert just retrieved text string to unicode

	//! Used by get_unicode().
	//! \internal

	std::u32string to_unicode(const std::string &s) const;

	//! Retrieve a localized message, then use gettextmsg() to format it.

	template<typename formatstr_type, typename ...Args>
	gettext_helper_ref<Args...>
	format(//! Formatting string
	       const formatstr_type &str,

	       //! Format string parameters
	       Args && ...args)
	{
		return gettext_helper_ref<Args...>
			(get(str), std::forward<Args>(args)...);
	}

	//! Retrieve a localized message, then use gettextmsg() to format it.

	template<typename formatstr_type, typename int_type,
		 typename ...Args>
	gettext_helper_ref<Args...>
	formatn(//! Formatting string, singular form
		const formatstr_type &singular,

		//! Formatting string, plural form
		const formatstr_type &plural,

		//! Numerical value for the localized string
		int_type n,

		//! Format string parameters
		Args && ...args)
	{
		return gettext_helper_ref<Args...>
			(get(singular, plural, n), std::forward<Args>(args)...);
	}

	//! Retrieve a localized message, then use gettextmsg() to format it.

	template<typename char_type, typename int_type,
		 typename ...Args>
	gettext_helper_ref<Args...>
	formatn(//! Formatting string, singular form
		const char_type *singular,

		//! Formatting string, plural form
		const char_type *plural,

		//! Numerical value for the localized string
		int_type n,

		//! Format string parameters
		Args && ...args)
	{
		return gettext_helper_ref<Args...>
			(get(singular, plural, n), std::forward<Args>(args)...);
	}
};

//! Advance literal text in a formatting string in gettextmsg()

//! \internal
//!
void gettext_fmt_advance(std::ostream &stream,
			 const char *&str);

//! Parse parameter number in a formatting string in gettextmsg()

//! \internal
//!
size_t gettext_fmt_getn(const char *&str);

//! Helper object used by gettextmsg()

//! \internal
//!
//! gettextmsg() returns a reference to this reference-counted object, which
//! stores the formatting parameters and formats the string into an output
//! stream.

template<typename ...Args>
class gettext_helper : virtual public obj {

public:
	//! The formatting string
	std::string fmt;

	//! Formatting string arguments
	std::tuple<Args...> args;

	//! The constructor
	gettext_helper(const std::string &fmtArg,
		       Args && ...given_args) noexcept
	: fmt(fmtArg), args(std::forward<Args>(given_args)...) {}

	//! Destructor
	~gettext_helper() {}

	//! Format the nth parameter

	template<size_t n>
	inline void format_arg(//! Output stream
			       std::ostream &o,

			       //! If \c n is \c i then format the parameter
			       size_t i) const
	{
		if (i == n)
			o << std::get<n>(args);
	}
};

//! Helper object for formatting parameters into an output stream

//! \internal

template<typename helper_type, size_t n>
class gettext_helper_format_args {

public:

	//! Iterate over formatting parameters

	//! Iterate to parameter n-1, then format the nth parameter.

	static inline void format(//! Formatting parameters
				  const helper_type &h,

				  //! Output stream
				  std::ostream &o,

				  //! Which parameter to format.
				  size_t i)
	{
		gettext_helper_format_args<helper_type, n-1>
			::format(h, o, i);
		h.template format_arg<n-1>(o, i);
	}
};

//! Specialization stops iteration at parameter #0

//! \internal
//!
template<typename helper_type>
class gettext_helper_format_args<helper_type, 0> {

public:

	//! No more.

	static inline void format(const helper_type &h,
				  std::ostream &o, size_t i)
	{
	}
};

//! Format a specific formatting parameter, in gettextmsg()

//! \internal
//! Iterate over all formatting parameters, selecting the one to be formatted
//! next.

template<typename ...Args>
inline void gettext_format_args(//! Parameters to gettextmsg()
				const gettext_helper<Args...> &h,

				//! Output stream
				std::ostream &o,

				//! Which parameter number to format.
				size_t i)
{
	gettext_helper_format_args< gettext_helper<Args...>,
				    sizeof...(Args)>::format(h, o, i);
}

//! Helper object returned by gettextmsg()

//! \internal
//! It's a subclass of a reference to \ref gettext_helper "gettext_helper",
//! which defined a string operator, and the << operator.
//!

template<typename ...Args>
class gettext_helper_ref : public ref<gettext_helper<Args...> > {

	//! The superclass type
	typedef ref<gettext_helper<Args...> > ref_t;

public:

	//! Constructor
	gettext_helper_ref(//! Formatting string
			   const std::string &msg,

			   //! Arguments
			   Args && ...args)
		: ref_t(ref_t::create(msg, std::forward<Args>(args)...))
	{
	}

	//! Destructor
	~gettext_helper_ref() {}

	using ref_t::operator*;
	using ref_t::operator->;

	//! Casting operator
	operator std::string() const;
};

//! Overload the << operator on the return value of gettextmsg()

//! \internal

template<typename ...Args>
std::ostream
&operator<<(std::ostream &o, const gettext_helper_ref<Args...> &h)
{
	const char *str=h->fmt.c_str();

	while (*str)
	{
		if (*str != '%')
		{
			gettext_fmt_advance(o, str);
			continue;
		}

		size_t n=gettext_fmt_getn(str);

		if (n == 0)
		{
			o << '%';
			continue;
		}

		gettext_format_args(*h, o, n-1);
	}
	return o;
}

//! Construct a localized message, the C++ way

//! \code
//!
//! INSERT_LIBX_NAMESPACE::messages msgcat;
//!
//! // ...
//!
//! std::cout << INSERT_LIBX_NAMESPACE::gettextmsg(msgcat->get("Window size: %1% rows, %2% columns"),
//!                           nrows, ncols) << std::endl;
//! \endcode
//!
//! gettextmsg() formats localized messages for output streams. The first
//! parameter is a formatting string, which usually comes from a message
//! catalog, permitting localization.
//!
//! Sequences of "%n%" in the formatting string get replaced with the nth
//! additional parameter that's passed to gettextmsg(). The above example
//! passes two more parameters, which replace "%1%" and "%2%" in the formatting
//! string.
//!
//! The numerical parameters do not have to appear in numerical order in the
//! formatting string, so a localized string may specify %2% prior to %1%:
//!
//! \code
//!
//! std::cout << INSERT_LIBX_NAMESPACE::gettextmsg(msgcat->get("Window size: %2% columns, %1% rows"),
//!                           nrows, ncols) << std::endl;
//! \endcode
//!
//! In this example, \c ncols and \c nrows still appear where they should be.
//!
//! \code
//!
//! std::string s=INSERT_LIBX_NAMESPACE::gettextmsg(msgcat->get("Window size: %2% columns, %1% rows"),
//!                            nrows, ncols);
//! \endcode
//!
//! The additional parameters to gettextmsg() are inserted in lieu of their
//! place holders as if by the \c << operator on an output stream. The
//! additional parameters may be any type that can be written to a stream by
//! the \c << operator. This includes manipulators:
//!
//! \code
//!
//! std::cout << INSERT_LIBX_NAMESPACE::gettextmsg(msgcat
//!                         ->get("Window size: %1%%2% rows, %3% columns%4%"),
//!                         std::setw(3), nrows, ncols, std::setw(0))
//!      << std::endl;
//! \endcode
//!
//! Manipulators are handled as ordinary formatting parameters, and get
//! referenced by the appropriate %%n% placeholder in the formatting string.

template<typename ...Args>
std::string gettextmsg(const std::string &msg,
		       Args && ...args)
{
	return gettext_helper_ref<Args...>
		(msg, std::forward<Args>(args)...);
}

//! Overloaded gettextmsg() from a literal formatting string.

//! \overload
template<typename ...Args>
std::string gettextmsg(const char *msg, Args && ...args)
{
	return gettext_helper_ref<Args...>
		(msg, std::forward<Args>(args)...);
}

template<typename ...Args>
inline gettext_helper_ref<Args...>::operator std::string() const
{
	std::ostringstream o;

	o << *this;

	return o.str();
}

#if 0
{
#endif
}
#endif
