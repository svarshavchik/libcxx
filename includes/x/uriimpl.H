/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_uriimpl_H
#define x_uriimpl_H

#include <cstring>
#include <string>
#include <algorithm>
#include <iterator>
#include <functional>
#include <map>
#include <compare>
#include <x/uriimplfwd.H>
#include <x/http/formfwd.H>
#include <x/exception.H>
#include <x/locale.H>
#include <x/idn.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A non reference-counted URI implementation

//! \c uriimpl implements URI semantics as described in RFC 3986.
//! A URI is decomposed into a scheme, authority, path, query, and fragment.

class uriimpl {

	//! URI delimiter characters

	//! \internal

	static const char gen_delims[];

	//! Verify that the collected URI character is not a control character

	//! \internal

	inline static void validate_char(unsigned char c)
	{
		if (c <= ' ')
			invalid_url_char();
	}

public:
	//! This object is returned by get_authority()

	class authority_t {

	public:
		//! The "userinfo" portion of the URI's authority, if provided.

		std::string userinfo;

		//! Whether the userinfo portion of the URI authority is provided.

		//! The RFC 3986 syntax allows the specification of an
		//! empty userinfo part. This \c bool distinguishes between
		//! the userinfo portion not being specified, or being specified
		//! as empty.

		bool has_userinfo;

		//! The hostname/regname portion of the authority.

		//! This includes the optional :port stuff
		//!
		std::string hostport;

		//! Constructor
		authority_t() noexcept;

		//! Construct from an authority string
		authority_t(const std::string &authorityStr);

		//! Construct from an authority string
		authority_t(const std::string &authorityStr,
			    const const_locale &locale,
			    int flags=0);

		//! Construct from a string defined by a beginning and ending input iterator

		template<typename iter_type>
		authority_t(iter_type beg_iter,
			    iter_type end_iter,
			    int flags=0)
			: has_userinfo(false)
		{
			if (parse(beg_iter, end_iter, flags) != end_iter)
				invalid_authority();
		}

		//! Destructor
		~authority_t();

		//! Indicate if the authority component was provided
		operator bool() const
		{
			return has_userinfo || hostport.size() > 0;
		}

		friend class uriimpl;

	private:

		//! Reset and clear

		void clear() noexcept
		{
			userinfo.clear();
			has_userinfo=false;
			hostport.clear();
		}

		//! Internal parser

		//! \internal
		//!

		template<typename iter_type>
		iter_type parse(iter_type beg_iter,
				iter_type end_iter,
				int flags)
		{
			clear();

			if (beg_iter == end_iter)
				return beg_iter;

			if (*beg_iter != '[')
			{
				beg_iter=parse_nonipliteral(beg_iter, end_iter,
							    flags);

				if (beg_iter == end_iter || *beg_iter != '@')
				{
					validate_port();
					return beg_iter;
				}
				++beg_iter;
				has_userinfo=true;
				userinfo=hostport;
			}

			hostport.clear();
			if (beg_iter == end_iter)
				return beg_iter;

			if (*beg_iter == '[')
			{
				hostport.push_back(*beg_iter);

				++beg_iter;

				for (;;)
				{
					char c;

					if (beg_iter == end_iter)
						invalid_authority();

					c= *beg_iter;

					hostport.push_back(c);

					if (c == ']')
						break;

					if (c != ':' && strchr(gen_delims, c))
						invalid_authority();

					if (c & 0x80)
						invalid_authority();
					// Do not allow non-ASCII chars.
					++beg_iter;
				}

				++beg_iter;
				if (beg_iter != end_iter && *beg_iter == ':')
				{
					hostport.push_back(*beg_iter++);

					while (beg_iter != end_iter &&
					       strchr(gen_delims, *beg_iter)
					       == NULL)
					{
						hostport.push_back(*beg_iter);
						++beg_iter;
					}

					validate_port();
				}
				return beg_iter;
			}

			beg_iter=parse_nonipliteral(beg_iter, end_iter,
						    flags);

			validate_port();

			return beg_iter;
		}

		//! If the port is specify, make sure it's numeric

		//! \internal
		//!
		void validate_port();

		//! Internal parser

		//! \internal
		//!

		template<typename iter_type>
		iter_type parse_nonipliteral(iter_type beg_iter,
					     iter_type end_iter,
					     int flags)
		{
			std::string s;

			char c;

                        while (beg_iter != end_iter &&
                               ((c=*beg_iter) == ':' ||
                                strchr(gen_delims, c) == NULL))
                        {
                                s.push_back(c);
                                ++beg_iter;
                        }

			auto lastcolon=s.rfind(':');

			if (lastcolon == std::string::npos)
				lastcolon=s.size();

			hostport=idn::to_ascii(s.substr(0, lastcolon),
					       flags)
				+ s.substr(lastcolon);

			return beg_iter;
		}

		//! Called by to_string() to emit userinfo.

		template<typename OutputIterator>
		OutputIterator emit_userinfo(//! The output iterator
					    OutputIterator iter) const
		{
			if (has_userinfo)
			{
				iter=std::copy(userinfo.begin(), userinfo.end(),
					       iter);
				*iter++ = '@';
			}
			return iter;
		}

	public:
		//! Convert this authority to a string representation

		//! Convert this authority to a string, which is sent to the
		//! given output iterator.

		template<typename OutputIterator>
		OutputIterator to_string(//! The output iterator
					OutputIterator iter)
			const
		{
			iter=emit_userinfo(iter);

			return std::copy(hostport.begin(), hostport.end(),
					 iter);
		}

		//! Convert this URI to a string representation

		//! \overload
		//! Compatibility with value_string. Converts ACE domain
		//! name to the current locale.

		template<typename OutputIterator>
		OutputIterator to_string(//! The output iterator
					OutputIterator iter,

					//! The locale
					const const_locale &localeRef,

					//! libidn flags
					int flags=0)
			const
		{
			auto b=hostport.begin(), e=hostport.end();

			if (b == e || *b == '[')
				return to_string(iter); // IP literal

			auto p=std::find(b, e, ':');

			iter=emit_userinfo(iter);
			auto s=idn::from_ascii(std::string(b, p), flags);
			iter=std::copy(s.begin(), s.end(), iter);

			return std::copy(p, e, iter);
		}

		//! Convert this URI to a string representation

		//! \overload
		//! Returns a string.

		std::string to_string() const;

		//! Comparison operator

		std::strong_ordering operator<=>(const authority_t &o) const
			noexcept;

		//! Comparison operator

		inline bool operator==(const authority_t &o) const noexcept
		{
			return operator<=>(o) == std::strong_ordering::equal;
		}
	};

private:

	//! Valid first character in a URI scheme

	//! \internal
	//!
	static inline bool valid_scheme_1st_char(char c) noexcept
	{
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	}

	//! Valid second and subsequent characters in a URI scheme

	//! \internal
	//!
	static inline bool valid_scheme_char(char c) noexcept
	{
		return (c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			(c >= '0' && c <= '9') ||
			c == '+' || c == '-' || c == '.';
	}


public:
	//! Default constructor

	uriimpl() noexcept;

	//! Constructor

	//! The URI is constructed from a beginning and an ending input
	//! iterator that defines a sequence of an URI string. The URI
	//! string is decomposed into a scheme, authority, path, query, and
	//! fragment.

	template<typename InputIterator>
	uriimpl(//! Beginning iterator
		InputIterator beg_iter,

		//! Ending iterator
		InputIterator end_iter,

		//! Locale used to parse an international domain name, if given

		const const_locale &locale=locale::base::environment(),

		//! Flags for libidn
		int flags=0)
	{
		parse(beg_iter, end_iter, flags);
	}

private:

	//! Report a control character in a URI

	//! \internal

	static void invalid_url_char() __attribute__((noreturn));

	//! Report an invalid authority component

	//! \internal

	static void invalid_authority() __attribute__((noreturn));

public:
	//! Initialize

	template<typename InputIterator>
	void parse(InputIterator beg_iter,
		   InputIterator end_iter,
		   int flags)
	{
		scheme.clear();
		authority.clear();
		path.clear();
		query.clear();
		fragment.clear();

		if (beg_iter == end_iter)
			return;

		std::string n;

		char c= *beg_iter;

		if (c == '/')
			goto parse_path;

		if (valid_scheme_char(c))
		{
			do
			{
				n.push_back(c);
				++beg_iter;

			} while (beg_iter != end_iter &&
				 valid_scheme_char(c=*beg_iter));

			if (beg_iter != end_iter && c == ':')
			{
				scheme=n;
				++beg_iter;

				if (beg_iter == end_iter)
					return;

				if ((c=*beg_iter) != '/')
					goto relative_path;

			parse_path:

				++beg_iter;

				n="/";
				if (beg_iter != end_iter && *beg_iter == '/')
				{
					++beg_iter;

					beg_iter=authority.parse(beg_iter,
								 end_iter,
								 flags);

					if (beg_iter == end_iter)
						return;

					if (*beg_iter == '?')
						goto collect_query;

					if (*beg_iter == '#')
						goto collect_fragment;

					if (*beg_iter != '/')
						invalid_authority();

				relative_path:
					n="";
				}
			}
		}

		path=n;

		while (beg_iter != end_iter &&
		       (c=*beg_iter) != '?' && c != '#')
		{
			validate_char(c);
			path.push_back(c);
			++beg_iter;
		}

		if (beg_iter != end_iter && *beg_iter == '?')
		{
		collect_query:

			++beg_iter;

			while (beg_iter != end_iter && *beg_iter != '#')
			{
				c= *beg_iter++;

				validate_char(c);
				if (c == '?')
					invalid_url_char();

				query.push_back(c);
			}
		}

	collect_fragment:

		if (beg_iter != end_iter)
		{
			++beg_iter;

			while (beg_iter != end_iter)
			{
				c=*beg_iter++;
				validate_char(c);
				if (c == '?' || c == '#')
					invalid_url_char();
				fragment.push_back(c);
			}
		}
	}
public:
	//! Constructor

	uriimpl(const char *urlString);

	//! Constructor

	uriimpl(//! URI string
		const std::string &urlString,

		//! Optional \ref idn "libidn flags".
		int flags=0);
	//! Destructor

	~uriimpl();

	//! Return the URI's scheme part.

	const std::string &get_scheme() const noexcept
	{
		return scheme;
	}

	//! Return the URI's authority part

	//! This method returns a reference to a class that specifies the
	//! authority component of the URI, if it was provided.

	const authority_t &get_authority() const noexcept
	{
		return authority;
	}

	//! Compute the host and the port

	//! If the authority does not specify a port, retrieve the port
	//! associated with the given scheme.

	std::pair<std::string, int> get_host_port(const std::string &protocol
						  ="tcp")
		const;

	//! Get the scheme's default port

	int get_scheme_port(const std::string &protocol="tcp")
		const;

	//! Remove an explicit :port in the authority if it's the scheme's default port

	void fix_scheme_port(const std::string &protocol="tcp");

	//! Return the URI's path part

	const std::string &get_path() const noexcept
	{
		return path;
	}

	//! Return the URI's query part

	const std::string &get_query() const noexcept
	{
		return query;
	}

	//! Return the URI's query part as form parameters
	http::form::parameters get_form() const;

	//! Return the URI's fragment part

	const std::string &get_fragment() const noexcept
	{
		return fragment;
	}

	//! Set the URI's scheme part.

	void set_scheme(const std::string &value);

	//! Set the URI's authority part
	void set_authority(const authority_t &authorityArg)
	{
		authority=authorityArg;
	}

	//! Set the URI's authority part from a string

	//! The authority gets parsed into an optional userinfo, and
	//! a "host[:port]" component.

	void set_authority(const std::string &value);

	//! Set the URI's path part

	void set_path(const std::string &value);

	//! Set the URI's query part

	void set_query(const std::string &value);

	//! Set the URI's query part using form parameters

	void set_query(//! Form parameters
		      const http::form::const_parameters &value);

	//! Set the URI's fragment part

	void set_fragment(const std::string &value);

	//! Modify this URI with a relative URI

	//! \return a reference to \c this. This operator applies
	//! the authority, path, query, and fragment parts to this URI
	//! object.

	uriimpl &operator+=(//! Relative URI

			    const uriimpl &r);

	//! Combine this absolute URI with a relative URI.

	//! This is like operator +=(), however this URI object is not
	//! modified. A new URI object gets returned by this function.

	uriimpl operator+(const uriimpl &r) const
	{
		uriimpl cpy(*this);

		cpy += r;

		return cpy;
	}

	//! Convert this URI to a string representation

	//! Convert this URI to a string, which is sent to the given
	//! output iterator.

	template<typename OutputIterator>
	OutputIterator to_string(//! The output iterator
				OutputIterator iter,

				//! Flag - omit scheme and authority
				bool noSchemeAuth=false,

				//! The locale version of to_string() sets this.

				//! Convert hostname portion to this locale, if not nullptr
				const const_locale *localeArg=nullptr,

				//! Flags for libidn
				int flags=0) const
	{
		bool has_path=false;

		if (!noSchemeAuth)
		{
			if (scheme.size() > 0)
			{
				iter=std::copy(scheme.begin(), scheme.end(),
					       iter);

				*iter++ = ':';
				has_path=true;
			}

			if (authority)
			{
				*iter++ = '/';
				*iter++ = '/';

				iter=localeArg ?
					authority.to_string(iter, *localeArg,
							   flags)
					: authority.to_string(iter);
				has_path=true;
			}
		}

		iter=std::copy(path.begin(), path.end(), iter);

		if (path.size() == 0 && !has_path)
			*iter++ = '/';

		if (query.size() > 0)
		{
			*iter++ = '?';
			iter=std::copy(query.begin(), query.end(), iter);
		}

		if (fragment.size() > 0)
		{
			*iter++ = '#';
			iter=std::copy(fragment.begin(), fragment.end(), iter);
		}
		return iter;
	}

	//! Convert this URI to a string representation

	//! \overload
	//! Compatibility with value_string.

	template<typename OutputIterator>
	OutputIterator to_string(//! The output iterator
				OutputIterator iter,

				//! The locale
				const const_locale &localeRef,

				//! Flag - omit scheme and authority
				bool noSchemeAuth=false)
		const
	{
		return to_string(iter, noSchemeAuth, nullptr, 0);
	}

	//! Convert this URI to a string representation, possibly using an i18n domain name

	//! \overload
	//! If the domain portion of this URI is an ASCII-encoded domain
	//! name, convert it to the encoding used by the given locale
	std::string to_string_i18n(const const_locale &localeRef, int flags=0)
		const;

	//! \overload
	//! Compatibility with value_string.

	//! The from_string() method

	//! \see value_string

	template<typename InputIterator>
	static uriimpl from_string(//! The beginning iterator
				  InputIterator beg_iter,

				  //! The ending iterator
				  InputIterator end_iter,

				  //! Locale for international domain names
				  const const_locale &localeArg,

				  //! Flags for libidn
				  int flags=0)

	{
		return uriimpl(beg_iter, end_iter, localeArg, flags);
	}

	//! Comparison operator

	std::strong_ordering operator<=>(const uriimpl &o) const noexcept;

	//! Comparison operator

	inline bool operator==(const uriimpl &o) const noexcept
	{
		return operator<=>(o) == std::strong_ordering::equal;
	}

	//! Return \c true if this URI encloses another URI.

	//! Returns \c true if this URI does not have a query or a fragment;
	//! if it does not have a userinfo; if it does not have a scheme or
	//! the scheme is equal to the other URI's scheme; if it does not have
	//! an authority or if the authority is equal to the other URI's
	//! authority; and if the path is either equal to the other URI's path,
	//! or if the concatenation of the path and "/" is a prefix of the
	//! other URI's path.

	bool operator<<(const uriimpl &o) const;

private:
	//! The schema part of a URI.
	std::string scheme;

	//! The authority part of a URI.
	authority_t authority;

	//! The path part of a URI.
	std::string path;

	//! The query part of a URI.
	std::string query;

	//! The fragment part of a URI

	std::string fragment;

};

#ifndef DOXYGEN
extern template void uriimpl::parse(std::string::const_iterator,
				    std::string::const_iterator,
				    int);

extern template std::ostreambuf_iterator<char>
uriimpl::to_string(std::ostreambuf_iterator<char>, bool,
		  const const_locale *, int)
	const;

extern template std::ostreambuf_iterator<char>
uriimpl::to_string(std::ostreambuf_iterator<char>, const const_locale &, bool)
	const;

extern template
uriimpl::authority_t::authority_t(std::string::const_iterator,
				  std::string::const_iterator,
				  int);
extern template
std::string::const_iterator
uriimpl::authority_t::parse(std::string::const_iterator,
			    std::string::const_iterator,
			    int);

extern template std::ostreambuf_iterator<char>
uriimpl::authority_t::to_string(std::ostreambuf_iterator<char>)
	const;

extern template std::ostreambuf_iterator<char>
uriimpl::authority_t::to_string(std::ostreambuf_iterator<char>,
			       const const_locale &, int)
	const;

#endif

#if 0
{
#endif
}
#endif
