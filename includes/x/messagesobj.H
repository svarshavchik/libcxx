/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_messagesobj_H
#define x_messagesobj_H

#include <x/messagesfwd.H>
#include <x/localefwd.H>
#include <x/localeobj.H>
#include <string>
#include <string_view>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Specialization for narrow character strings

class messagesObj : virtual public obj {

	//! Locale
	const const_locale l;

	//! The text domain
	const std::string domain;

	//! Internal call to dngettext

	//! \internal
	std::string call_dngettext(//! Singular
				   const std::string_view &s,

				   //! Plural
				   const std::string_view &p,

				   //! The numeric value
				   unsigned long n) const noexcept;

	//! Internal call to dngettext

	template<typename int_type>
	std::string call_dngettext_wrapper(//! Singular
					   const std::string_view &s,

					   //! Plural
					   const std::string_view &p,

					   //! The numeric value
					   int_type n) const noexcept
	{
		if (n < 0)
			n= -n;

		if (n > 1000000)
			n=(n % 1000000)+1000000;

		return call_dngettext(s, p, n);
	}

public:
	//! Return the locale

	inline const_locale get_locale() const noexcept
	{
		return l;
	}

	//! The constructor

	messagesObj(//! The %messages' locale
		    const const_locale &lArg,

		    //! The %messages' domain
		    const std::string_view &domainArg) noexcept;

	//! Destructor

	~messagesObj();

	//! Retrieve a message from the catalog

	std::string get(//! The message to retrieve

			const std::string_view &s) const noexcept;

	//! Retrieve a message from the catalog

	//! This is a version that retrieves singular or plural form, as
	//! appropriate for a given int value.

	template<typename int_type>
	std::string get(//! The singular form

			const std::string_view &s,

		     //! The plural form

			const std::string_view &p,

			//! The numeric value

			int_type n
			) const noexcept
	{
		return call_dngettext_wrapper(s, p, n);
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
};

#if 0
{
#endif
}
#endif
