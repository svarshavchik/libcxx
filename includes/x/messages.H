/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_messages_H
#define x_messages_H

#include <x/locale.H>
#include <x/messagesfwd.H>
#include <x/messagesobj.H>
#include <x/refptr_traits.H>
#include <x/optional_args.H>
#include <tuple>
#include <type_traits>
#include <sstream>
#include <string_view>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Advance literal text in a formatting string in gettextmsg()

//! \internal
//!
void gettext_fmt_advance(std::ostream &stream,
			 std::string_view::const_iterator &b,
			 std::string_view::const_iterator &e);

//! Parse parameter number in a formatting string in gettextmsg()

//! \internal
//!
size_t gettext_fmt_getn(std::string_view::const_iterator &b,
			std::string_view::const_iterator &e) noexcept;

//! Helper object used by gettextmsg()

//! \internal
//!
//! gettextmsg() returns a reference to this reference-counted object, which
//! stores the formatting parameters and formats the string into an output
//! stream.

template<typename ...Args> class gettext_helper {

public:
	//! Copy constructor is deleted
	gettext_helper(const gettext_helper &)=delete;

	//! Assignment operator is deleted.
	gettext_helper &operator=(const gettext_helper &)=delete;

	//! The formatting string

	std::string_view fmt;

	//! Formatting string arguments
	std::tuple<Args...> args;

	//! The constructor
	template<typename Arg, typename ...constructorArgs>
	gettext_helper(Arg &&fmtArg,
		       constructorArgs && ...given_args) noexcept
		: fmt{std::forward<Arg>(fmtArg)},
		  args{std::forward<constructorArgs>(given_args)...} {}

	//! Destructor
	~gettext_helper()=default;

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

	//! Convert to a string.
	operator std::string() const;
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

//! Overload the << operator on the return value of gettextmsg()

//! \internal

template<typename ...Args>
std::ostream
&operator<<(std::ostream &o, const gettext_helper<Args...> &h)
{
	auto b=h.fmt.begin();
	auto e=h.fmt.end();

	while (b != e)
	{
		if (*b != '%')
		{
			gettext_fmt_advance(o, b, e);
			continue;
		}

		size_t n=gettext_fmt_getn(b, e);

		if (n == 0)
		{
			o << '%';
			continue;
		}

		gettext_format_args(h, o, n-1);
	}
	return o;
}

template<typename ...Args>
inline gettext_helper<Args...>::operator std::string() const
{
	std::ostringstream o;

	o << *this;

	return o.str();
}

//! Construct a localized message, the C++ way

//! \code
//!
//! auto msgcat=INSERT_LIBX_NAMESPACE::messages::create("domain");
//!
//! // ...
//!
//! std::cout << INSERT_LIBX_NAMESPACE::gettextmsg(msgcat->get("Window size: %1% rows, %2% columns"),
//!                           nrows, ncols) << std::endl;
//! \endcode
//!
//! gettextmsg() formats (presumably) localized messages for output streams.
//! gettextmsg() returns a template object with an overloaded \c <<
//! operator that writes the formatted localized message directly to a
//! \c std::ostream. It also implements an conversion operator to
//! \c std::string, which is just a shortcut for constructing
//! a \c std::ostringstream, using the same \c << operator with it, and
//! then returning the resulting string.
//!
//! The internal template object has a deleted
//! copy constructor and an assignment operator, in order to encourage it to
//! be used only in temporary contexts where it gets processed by the
//! overloaded \c << operator and then destroyed, or converted to a single
//! \c std::string.
//!
//! \c gettextmsg()'s first
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
//!
//! \see messages

template<typename Msg, typename ...Args>
auto gettextmsg(Msg &&msg, Args && ...args)
{
	return gettext_helper<std::decay_t<Args>...>{
		std::forward<Msg>(msg), std::forward<Args>(args)...
			};
}

//! Base class of messages.

//! Refer to this class as messages::base
//!
//! create()ing a messages object with a locale and a domain parameters
//! contructs the object normally.
//!
//! A custom create() with an additional directory parameter internally
//! enforces the same directory for multiple instances for the same
//! domain, because this is implemented in terms of gettext's
//! bindtextdomain() global.

class messagesBase {

public:

	typedef optional_args<std::string,
			      explicit_refptr<const_locale>> constructor_args;

	static messages create(const std::string_view &domain,
			       const constructor_args &args);

	template<typename ptrrefType> class objfactory {
	public:

		template<typename ...Args>
		static inline ptrrefType create(const std::string_view &domain,
						Args && ...args)
		{
			return messagesBase::create(domain,
						    {std::forward<Args>
						     (args)...});
		}
	};
};

#if 0
{
#endif
}
#endif
