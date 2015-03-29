/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_messages_H
#define x_messages_H

#include <x/exception.H>
#include <x/locale.H>
#include <x/localeobj.H>
#include <x/tostring.H>
#include <x/messagesfwd.H>
#include <libintl.h>
#include <string>
#include <map>
#include <algorithm>
#include <cstring>
#include <tuple>
#include <sstream>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class localeObj;

template<typename string_type, typename ...Args> class gettext_helper_ref;

//! Interface to dgettext()

template<typename char_type>
class basic_messagescommonObj : virtual public obj {

protected:

	//! The %messages' locale.

	const const_locale l;

	//! The text domain
	std::string domain;

	//! The constructor

	basic_messagescommonObj(//! The %messages' locale
			      const const_locale &lArg,

			      //! The %messages' domain
			      const std::string &domainArg)

		: l(lArg),
		  domain(domainArg)
	{
	}

	//! Destructor

	virtual ~basic_messagescommonObj() noexcept
	{
	}

	//! Internal call to dgettext()

	const char *call_dgettext(const char *s) const
	{
		thread_locale thread_locale(*l);
		const char *msg =
			const_cast<const char*>(dgettext(domain.c_str(), s));
		return msg;
	}

	//! Internal call to dngettext

	const char *call_dngettext(//! Singular
				   const char *s,

				   //! Plural
				   const char *p,

				   //! The numeric value
				   unsigned long n) const
	{
		thread_locale thread_locale(*l);

		const char *msg=
			const_cast<const char*>(dngettext(domain.c_str(),
							  s, p, n));
		return msg;
	}


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

	//! Retrieve this catalog's locale

	const_locale getLocale() const noexcept
	{
		return l;
	}
};

extern template class basic_messagescommonObj<char>;
extern template class basic_messagescommonObj<wchar_t>;

//! Helper class for converting basic_strings of different traits (but same char)

template<typename to_string_t, typename from_string_t>
class basic_messages_conv {
public:

	//! Default, manual, conversion

	static inline to_string_t doconv(const from_string_t &arg)
	{
		return arg.c_str();
	}
};

//! Specialization for no-op char conversion, same traits.

template<typename string_t>
class basic_messages_conv<string_t, string_t> {
public:

	//! This is a no-op

	static inline const string_t &doconv(const string_t &arg)
	{
		return arg;
	}
};

//! Specialization for narrow character strings

template<typename traits_type,
	 typename alloc_type>
class basic_messagesObj<char, traits_type, alloc_type> :
	public basic_messagescommonObj<char> {

	//! Locale
	const const_locale l;

	//! The string type being returned

	typedef std::basic_string<char, traits_type, alloc_type> string_t;

public:
	//! The constructor

	basic_messagesObj(//! The %messages' locale
			  const const_locale &lArg,

			  //! The %messages' domain
			  const std::string &domainArg)
		: basic_messagescommonObj<char>(lArg, domainArg),
		  l(lArg)
	{
	}

	//! Constructor, set a directory's text domain

	basic_messagesObj(//! The %messages' locale
			  const const_locale &lArg,

			  //! The %messages' domain
			  const std::string &domainArg,

			  //! The message catalog directory
			  const std::string &directory)
		: basic_messagescommonObj<char>(lArg, domainArg),
		  l(lArg)
	{
		bindtextdomain(domainArg.c_str(), directory.c_str());
	}

	//! Destructor

	virtual ~basic_messagesObj() noexcept
	{
	}

	//! Retrieve a message from the catalog

	string_t get(//! The message to retrieve

		     const char *s) const
	{
		return call_dgettext(s);
	}

	//! Retrieve a message from the catalog

	//! \overload

	template<typename arg_traits_type,
		 typename arg_alloc_type>
	string_t get(//! The message to retrieve

		     const std::basic_string<char, arg_traits_type,
					     arg_alloc_type> &s) const
	{
		return get(s.c_str());
	}

	//! Retrieve a message from the catalog

	//! \overload

	string_t get(//! The message to retrieve
		     const wchar_t *s) const
	{
		return get(tostring(s, l).c_str());
	}


	//! Retrieve a message from the catalog

	//! This is a version that retrieves singular or plural form, as
	//! appropriate for a given int value.

	template<typename int_type>
	string_t get(//! The singular form

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
	template<typename int_type,
		 typename arg_traits_type, typename arg_alloc_type>
	string_t get(//! The singular form

		     const std::basic_string<char, arg_traits_type,
					     arg_alloc_type> &s,

		     //! The plural form

		     const std::basic_string<char, arg_traits_type,
					     arg_alloc_type> &p,

		     //! The numeric value

		     int_type n) const
	{
		return get(s.c_str(), p.c_str(), n);
	}

	//! Retrieve a message from the catalog

	//! \overload

	template<typename int_type>
	string_t get(//! The singular form

		     const wchar_t *s,

		     //! The plural form

		     const wchar_t *p,

		     //! The numeric value

		     int_type n
		     ) const
	{
		return get( tostring(s, l).c_str(), tostring(p, l).c_str(), n);
	}

	//! Retrieve a message from the catalog

	//! \overload
	template<typename int_type,
		 typename arg_traits_type, typename arg_alloc_type>
	string_t get(//! The singular form

		     const std::basic_string<wchar_t, arg_traits_type,
					     arg_alloc_type> &s,

		     //! The plural form

		     const std::basic_string<wchar_t, arg_traits_type,
					     arg_alloc_type> &p,

		     //! The numeric value

		     int_type n) const
	{
		return get(s.c_str(), p.c_str(), n);
	}

	//! Retrieve a localized message, then use gettextmsg() to format it.

	template<typename formatstr_type, typename ...Args>
	gettext_helper_ref<string_t, Args...>
	format(//! Formatting string
	       const formatstr_type &str,

	       //! Format string parameters
	       Args && ...args)
	{
		return gettext_helper_ref<string_t, Args...>
			(get(str), std::forward<Args>(args)...);
	}

	//! Retrieve a localized message, then use gettextmsg() to format it.

	template<typename formatstr_type, typename int_type,
		 typename ...Args>
	gettext_helper_ref<string_t, Args...>
	formatn(//! Formatting string, singular form
		const formatstr_type &singular,

		//! Formatting string, plural form
		const formatstr_type &plural,

		//! Numerical value for the localized string
		int_type n,

		//! Format string parameters
		Args && ...args)
	{
		return gettext_helper_ref<string_t, Args...>
			(get(singular, plural, n), std::forward<Args>(args)...);
	}

	//! Retrieve a localized message, then use gettextmsg() to format it.

	template<typename char_type, typename int_type,
		 typename ...Args>
	gettext_helper_ref<string_t, Args...>
	formatn(//! Formatting string, singular form
		const char_type *singular,

		//! Formatting string, plural form
		const char_type *plural,

		//! Numerical value for the localized string
		int_type n,
		
		//! Format string parameters
		Args && ...args)
	{
		return gettext_helper_ref<string_t, Args...>
			(get(singular, plural, n), std::forward<Args>(args)...);
	}
};

//! Specialization for wide character strings

template<typename traits_type,
	 typename alloc_type>
class basic_messagesObj<wchar_t, traits_type, alloc_type> :
	public basic_messagescommonObj<wchar_t> {

	//! The locale

	const const_locale l;

	//! The string type being returned

	typedef std::basic_string<wchar_t, traits_type, alloc_type> string_t;

public:
	//! The constructor

	basic_messagesObj(//! The %messages' locale
			  const const_locale &lArg,

			  //! The %messages' domain
			  const std::string &domainArg)
		: basic_messagescommonObj<wchar_t>(lArg, domainArg),
		  l(lArg)
	{
	}


	//! Constructor, set a directory's text domain

	basic_messagesObj(//! The %messages' locale
			  const const_locale &lArg,

			  //! The %messages' domain
			  const std::string &domainArg,

			  //! The message catalog directory
			  const std::string &directory)
		: basic_messagescommonObj<wchar_t>(lArg, domainArg),
		  l(lArg)
	{
		bindtextdomain(domainArg.c_str(), directory.c_str());
	}

	//! Destructor

	virtual ~basic_messagesObj() noexcept
	{
	}

	//! Retrieve a message from the catalog

	string_t get(//! The message to retrieve

		     const char *s) const
	{
		return basic_messages_conv<string_t, std::wstring>
			::doconv(towstring(call_dgettext(s), l));
	}

	//! Retrieve a message from the catalog

	//! \overload

	template<typename arg_traits_type,
		 typename arg_alloc_type>
	string_t get(//! The message to retrieve

		     const std::basic_string<char, arg_traits_type,
					     arg_alloc_type> &s) const
	{
		return get(s.c_str());
	}

	//! Retrieve a message from the catalog

	//! \overload

	template<typename arg_traits_type,
		 typename arg_alloc_type>
	string_t get(//! The message to retrieve

		     const std::basic_string<wchar_t, arg_traits_type,
					     arg_alloc_type> &s) const
	{
		typedef std::basic_string<wchar_t, arg_traits_type,
					  arg_alloc_type> arg_t;

		std::string sNarrow(tostring(basic_messages_conv<std::wstring,
								 arg_t>::doconv(s),
					     l));

		const char *c=sNarrow.c_str();

		const char *p=call_dgettext(c);

		if (strcmp(c, p) == 0) // No change, return original string
			return basic_messages_conv<string_t, arg_t>::doconv(s);

		return basic_messages_conv<string_t, std::wstring>
			::doconv(towstring(p, l));
	}

	//! Retrieve a message from the catalog

	//! \overload

	string_t get(//! The message to retrieve
		     const wchar_t *s) const
	{
		return get(string_t(s));
	}


	//! Retrieve a message from the catalog

	//! This is a version that retrieves singular or plural form, as
	//! appropriate for a given int value.

	template<typename int_type>
	string_t get(//! The singular form

		     const char *s,

		     //! The plural form

		     const char *p,

		     //! The numeric value

		     int_type n
		     ) const
	{
		return basic_messages_conv<string_t, std::wstring>
			::doconv(towstring(call_dngettext_wrapper(s, p, n), l));
	}

	//! Retrieve a message from the catalog

	//! \overload

	template<typename int_type,
		 typename arg_traits_type, typename arg_alloc_type>
	string_t get(//! The singular form

		     const std::basic_string<char, arg_traits_type,
					     arg_alloc_type> &s,

		     //! The plural form

		     const std::basic_string<char, arg_traits_type,
					     arg_alloc_type> &p,

		     //! The numeric value

		     int_type n) const
	{
		return get(s.c_str(), p.c_str(), n);
	}

	//! Retrieve a message from the catalog

	//! \overload

	template<typename int_type,
		 typename arg_traits_type, typename arg_alloc_type>
	string_t get(//! The singular form

		     const std::basic_string<wchar_t, arg_traits_type,
					     arg_alloc_type> &s,

		     //! The plural form

		     const std::basic_string<wchar_t, arg_traits_type,
					     arg_alloc_type> &p,

		     //! The numeric value

		     int_type n) const
	{
		typedef std::basic_string<wchar_t, arg_traits_type,
					  arg_alloc_type> arg_t;

		std::string sNarrow(tostring(basic_messages_conv<std::wstring, arg_t>::doconv(s), l));
		std::string pNarrow(tostring(basic_messages_conv<std::wstring, arg_t>::doconv(s), l));

		const char *sc=sNarrow.c_str();
		const char *pc=pNarrow.c_str();

		const char *ret=call_dngettext_wrapper(sc, pc, n);

		if (strcmp(ret, sc) == 0)
			return basic_messages_conv<string_t, arg_t>::doconv(s);

		if (strcmp(ret, pc) == 0)
			return basic_messages_conv<string_t, arg_t>::doconv(p);

		return basic_messages_conv<string_t, std::wstring>
			::doconv(towstring(ret, l));
	}

	//! Retrieve a message from the catalog

	//! \overload

	template<typename int_type>
	string_t get(//! The singular form

		     const wchar_t *s,

		     //! The plural form

		     const wchar_t *p,

		     //! The numeric value

		     int_type n) const
	{
		return get(string_t(s), string_t(p), n);
	}

	//! Retrieve a localized message, then use gettextmsg() to format it.

	template<typename formatstr_type, typename ...Args>
	gettext_helper_ref<string_t, Args...>
	format(//! Formatting string
	       const formatstr_type &str,

	       //! Format string parameters
	       Args && ...args)
	{
		return gettext_helper_ref<string_t, Args...>
			(get(str), std::forward<Args>(args)...);
	}

	//! Retrieve a localized message, then use gettextmsg() to format it.

	template<typename formatstr_type, typename int_type,
		 typename ...Args>
	gettext_helper_ref<string_t, Args...>
	formatn(//! Formatting string, singular form
		const formatstr_type &singular,

		//! Formatting string, plural form
		const formatstr_type &plural,

		//! Numerical value for the localized string
		int_type n,

		//! Format string parameters
		Args && ...args)
	{
		return gettext_helper_ref<string_t, Args...>
			(get(singular, plural, n), std::forward<Args>(args)...);
	}

	//! Retrieve a localized message, then use gettextmsg() to format it.

	template<typename char_type, typename int_type,
		 typename ...Args>
	gettext_helper_ref<string_t, Args...>
	formatn(//! Formatting string, singular form
		const char_type *singular,

		//! Formatting string, plural form
		const char_type *plural,

		//! Numerical value for the localized string
		int_type n,

		//! Format string parameters
		Args && ...args)
	{
		return gettext_helper_ref<string_t, Args...>
			(get(singular, plural, n), std::forward<Args>(args)...);
	}
};

//! Advance literal text in a formatting string in gettextmsg()

//! \internal
//!
template<typename char_type>
void gettext_fmt_advance(std::basic_ostream<char_type> &stream,
			 const char_type *&str)
{
	const char_type *start=str;

	while (*str && *str != '%')
		++str;
	stream.write(start, str-start);
}

//! Parse parameter number in a formatting string in gettextmsg()

//! \internal
//!
template<typename char_type>
size_t gettext_fmt_getn(const char_type *&str)
{
	size_t n=0;

	while (++str, *str >= '0' && *str <= '9')
	{
		n = n * 10 + (*str - '0');
	}

	if (*str == '%')
		++str;
	return n;
}

//! Helper object used by gettextmsg()

//! \internal
//!
//! gettextmsg() returns a reference to this reference-counted object, which
//! stores the formatting parameters and formats the string into an output
//! stream.

template<typename string_type, typename ...Args>
class gettext_helper : virtual public obj {

public:
	//! Character type
	typedef typename string_type::value_type char_type;

	//! The formatting string
	string_type fmt;

	//! Formatting string arguments
	std::tuple<Args...> args;

	//! The constructor
	gettext_helper(const string_type &fmtArg,
		       Args && ...given_args) noexcept
	: fmt(fmtArg), args(std::forward<Args>(given_args)...) {}

	//! Destructor
	~gettext_helper() noexcept {}

	//! Format the nth parameter

	template<size_t n>
	inline void format_arg(//! Output stream
			       std::basic_ostream<char_type> &o,

			       //! If \c n is \c i then format the parameter
			       size_t i) const
	{
		if (i == n)
			o << std::get<n>(args);
	}
};

//! Helper object for formatting parameters into an output stream

//! \internal

template<typename helper_type, typename ostream_type, size_t n>
class gettext_helper_format_args {

public:

	//! Iterate over formatting parameters

	//! Iterate to parameter n-1, then format the nth parameter.

	static inline void format(//! Formatting parameters
				  const helper_type &h,

				  //! Output stream
				  ostream_type &o,

				  //! Which parameter to format.
				  size_t i)
	{
		gettext_helper_format_args<helper_type, ostream_type, n-1>
			::format(h, o, i);
		h.template format_arg<n-1>(o, i);
	}
};

//! Specialization stops iteration at parameter #0

//! \internal
//! 
template<typename helper_type, typename ostream_type>
class gettext_helper_format_args<helper_type, ostream_type, 0> {

public:

	//! No more.

	static inline void format(const helper_type &h,
				  ostream_type &o, size_t i)
	{
	}
};

//! Format a specific formatting parameter, in gettextmsg()

//! \internal
//! Iterate over all formatting parameters, selecting the one to be formatted
//! next.

template<typename string_type, typename ...Args>
inline void gettext_format_args(//! Parameters to gettextmsg()
				const gettext_helper<string_type, Args...> &h,

				//! Output stream
				std::basic_ostream< typename
						    string_type::value_type
						    > &o,

				//! Which parameter number to format.
				size_t i)
{
	gettext_helper_format_args< gettext_helper<string_type, Args...>,
				    std::basic_ostream< typename
							string_type::value_type
							>,
				    sizeof...(Args)>::format(h, o, i);
}

//! Helper object returned by gettextmsg()

//! \internal
//! It's a subclass of a reference to \ref gettext_helper "gettext_helper",
//! which defined a string operator, and the << operator.
//!

template<typename string_type, typename ...Args>
class gettext_helper_ref : public ref<gettext_helper<string_type, Args...> > {

	//! The superclass type
	typedef ref<gettext_helper<string_type, Args...> > ref_t;

public:

	//! Constructor
	gettext_helper_ref(//! Formatting string
			   const string_type &msg,

			   //! Arguments
			   Args && ...args)
		: ref_t(ref_t::create(msg, std::forward<Args>(args)...))
	{
	}

	//! Destructor
	~gettext_helper_ref() noexcept {}

	using ref_t::operator*;
	using ref_t::operator->;

	//! Casting operator
	operator string_type() const;
};

//! Overload the << operator on the return value of gettextmsg()

//! \internal

template<typename string_type, typename ...Args>
std::basic_ostream<typename string_type::value_type>
&operator<<(std::basic_ostream<typename string_type::value_type> &o,
	    const gettext_helper_ref<string_type, Args...> &h)
{
	const typename string_type::value_type *str=h->fmt.c_str();

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
			o << (typename string_type::value_type)'%';
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
//! The object returned by gettextmsg() implements the \c << operator, or
//! is convertible to a string:
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
//!
//! \par Wide characters
//!
//! \code
//!
//! INSERT_LIBX_NAMESPACE::wmessages wmsgcat;
//!
//! std::wcout << INSERT_LIBX_NAMESPACE::gettextmsg(wmsgcat->get(L"Window size: %1% rows, %2% columns"),
//!                            nrows, ncols) << std::endl;
//! \endcode
//!
//! gettextmsg() handles wide or narrow characters.
//! \ref messages "INSERT_LIBX_NAMESPACE::messages" returns narrow
//! strings, and
//! \ref wmessages "INSERT_LIBX_NAMESPACE::wmessages" returns wide strings from
//! the catalog. Both
//! \ref messages "INSERT_LIBX_NAMESPACE::messages" and
//! \ref wmessages "INSERT_LIBX_NAMESPACE::wmessages"
//! look up either narrow or wide character strings. Message
//! catalogs store strings as narrow characters, so if the string being looked
//! up is a wide string, it gets converted to a narrow string, first, in the
//! specified locale. msgcat returns the looked up string directly, wmsgcat
//! converts the looked up string to a wide character string, and returns it.
//!
//! With gettextmsg(), a wide
//! character formatting string produces a result that can be written, via
//! the \c << operator, to a wide character stream, or assigned to a wide
//! character string.

template<typename char_type, typename traits_type,
	 typename alloc_type, typename ...Args>
gettext_helper_ref<std::basic_string<char_type, traits_type, alloc_type>,
		   Args...>
gettextmsg(const std::basic_string<char_type, traits_type, alloc_type> &msg,
	   Args && ...args)
{
	return gettext_helper_ref<std::basic_string<char_type, traits_type,
						    alloc_type>, Args...>
		(msg, std::forward<Args>(args)...);
}

//! Overloaded gettextmsg() from a literal formatting string.

//! \overload
template<typename char_type, typename ...Args>
gettext_helper_ref<std::basic_string<char_type>, Args...>
gettextmsg(const char_type *msg, Args && ...args)
{
	return gettext_helper_ref<std::basic_string<char_type>, Args...>
		(msg, std::forward<Args>(args)...);
}

template<typename string_type, typename ...Args>
gettext_helper_ref<string_type, Args...>::operator string_type() const
{
	std::basic_ostringstream<typename string_type::value_type> o;

	o << *this;

	return o.str();
}

extern template class basic_messagesObj<char>;
extern template class basic_messagesObj<wchar_t>;

#ifndef DOXYGEN
extern template void gettext_fmt_advance(std::ostream &, const char *&);
extern template size_t gettext_fmt_getn(const char *&);

extern template void gettext_fmt_advance(std::wostream &, const wchar_t *&);
extern template size_t gettext_fmt_getn(const wchar_t *&);
#endif

#if 0
{
#endif
}
#endif
