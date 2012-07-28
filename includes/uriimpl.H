/*
** Copyright 2012 Double Precision, Inc.
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
#include <x/http/formfwd.H>
#include <x/exception.H>
#include <x/locale.H>
#include <x/value_stringable.H>
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
	//! This object is returned by getAuthority()

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

		//! Construct from a string defined by a beginning and ending input iterator

		template<typename iter_type>
		authority_t(iter_type beg_iter,
			    iter_type end_iter)
			: has_userinfo(false)
		{
			if (parse(beg_iter, end_iter) != end_iter)
				invalid_authority();
		}

		//! Destructor
		~authority_t() noexcept;

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
				iter_type end_iter)
		{
			clear();

			if (beg_iter == end_iter)
				return beg_iter;

			if (*beg_iter != '[')
			{
				beg_iter=parse_nonipliteral(beg_iter, end_iter);

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

					++beg_iter;
				}

				++beg_iter;
				if (beg_iter != end_iter && *beg_iter == ':')
				{
					do
					{
						hostport.push_back(*beg_iter);
						++beg_iter;
					} while (beg_iter != end_iter &&
						 strchr(gen_delims, *beg_iter)
						 == NULL);

					validate_port();
				}
				return beg_iter;
			}

			beg_iter=parse_nonipliteral(beg_iter, end_iter);

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
					     iter_type end_iter)

		{
			hostport.clear();

			char c;

			while (beg_iter != end_iter &&
			       ((c=*beg_iter) == ':' ||
				strchr(gen_delims, c) == NULL))
			{
				hostport.push_back(c);
				++beg_iter;
			}

			return beg_iter;
		}
	public:
		//! Convert this authority to a string representation

		//! Convert this authority to a string, which is sent to the
		//! given output iterator.

		template<typename OutputIterator>
		OutputIterator toString(//! The output iterator
					OutputIterator iter)
			const
		{
			if (has_userinfo)
			{
				iter=std::copy(userinfo.begin(), userinfo.end(),
					       iter);
				*iter++ = '@';
			}

			return std::copy(hostport.begin(), hostport.end(),
					 iter);
		}

		//! Convert this URI to a string representation

		//! \overload
		//! Compatibility with value_string.
		
		template<typename OutputIterator>
		OutputIterator toString(//! The output iterator
					OutputIterator iter,

					//! The locale
					const const_locale &localeRef)
			const
		{
			return toString(iter);
		}

		//! Convert this URI to a string representation

		//! \overload
		//! Returns a string.

		std::string toString() const;

		//! Comparison operator

		//! \return 0 if this authority is the same as the other
		//! authority, or a negative or a positive value, based on
		//! an arbitrary comparison function's notion of which one
		//! is lexicographically before the other one.

		int compare(const authority_t &o) const noexcept;

		//! Comparison operator

		//! \return \c true - if this authority should be
		//! lexicographically less than the other one, \c false
		//! otherwise.
		bool operator<(const authority_t &o) const noexcept
		{
			return compare(o) < 0;
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
		InputIterator end_iter)
	{
		parse(beg_iter, end_iter);
	}

private:

	//! Report a control character in a URI

	//! \internal

	static void invalid_url_char()
 __attribute__((noreturn));

	//! Report an invalid authority component

	//! \internal

	static void invalid_authority()
 __attribute__((noreturn));

public:
	//! Initialize

	template<typename InputIterator>
	void parse(InputIterator beg_iter,
		   InputIterator end_iter)
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
								 end_iter);

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

	uriimpl(const std::string &urlString);

	//! Constructor

	uriimpl(const char *urlString);

	//! Destructor

	~uriimpl() noexcept;

	//! Return the URI's scheme part.

	const std::string &getScheme() const noexcept
	{
		return scheme;
	}

	//! Return the URI's authority part

	//! This method returns a reference to a class that specifies the
	//! authority component of the URI, if it was provided.

	const authority_t &getAuthority() const noexcept
	{
		return authority;
	}

	//! Compute the host and the port

	//! If the authority does not specify a port, retrieve the port
	//! associated with the given scheme.

	std::pair<std::string, int> getHostPort(const std::string &protocol
						="tcp")
		const;

	//! Get the scheme's default port

	int getSchemePort(const std::string &protocol="tcp")
		const;

	//! Remove an explicit :port in the authority if it's the scheme's default port

	void fixSchemePort(const std::string &protocol="tcp")
;

	//! Return the URI's path part

	const std::string &getPath() const noexcept
	{
		return path;
	}

	//! Return the URI's query part

	const std::string &getQuery() const noexcept
	{
		return query;
	}

	//! Return the URI's fragment part

	const std::string &getFragment() const noexcept
	{
		return fragment;
	}

	//! Set the URI's scheme part.

	void setScheme(const std::string &value);

	//! Set the URI's authority part
	void setAuthority(const authority_t &authorityArg)
	{
		authority=authorityArg;
	}

	//! Set the URI's authority part from a string

	//! The authority gets parsed into an optional userinfo, and
	//! a "host[:port]" component.

	void setAuthority(const std::string &value);

	//! Set the URI's path part

	void setPath(const std::string &value);

	//! Set the URI's query part

	void setQuery(const std::string &value);

	//! Set the URI's query part using form parameters

	void setQuery(//! Form parameters

		      const http::form::parameters &value)
;

	//! Set the URI's fragment part

	void setFragment(const std::string &value);

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
	OutputIterator toString(//! The output iterator
				OutputIterator iter,

				//! Flag - omit scheme and authority
				bool noSchemeAuth=false) const
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

				iter=authority.toString(iter);
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


	//! This class implements toString() and fromString()
	static const stringable_t stringable=class_tostring;

	//! Convert this URI to a string representation

	//! \overload
	//! Compatibility with value_string.

	template<typename OutputIterator>
	OutputIterator toString(//! The output iterator
				OutputIterator iter,

				//! The locale
				const const_locale &localeRef,

				//! Flag - omit scheme and authority
				bool noSchemeAuth=false)
		const
	{
		return toString(iter, noSchemeAuth);
	}

	//! The fromString() method

	//! \see value_string

	template<typename InputIterator>
	static uriimpl fromString(//! The beginning iterator
				  InputIterator beg_iter,

				  //! The ending iterator
				  InputIterator end_iter,

				  //! Ignored
				  const const_locale &localeArg)

	{
		return uriimpl(beg_iter, end_iter);
	}

	//! Comparison operator

	//! \return 0 if this URI is the same as the other
	//! authority, or a negative or a positive value, based on
	//! an arbitrary comparison function's notion of which one
	//! is lexicographically before the other one.

	int compare(const uriimpl &o) const noexcept;

	//! Comparison operator

	//! \return \c true - if both URIs are the same.

	bool operator==(const uriimpl &o) const noexcept
	{
		return compare(o) == 0;
	}

	//! Comparison operator

	//! \return \c true - if both URIs are not the same.

	bool operator!=(const uriimpl &o) const noexcept
	{
		return compare(o) != 0;
	}

	//! Comparison operator

	//! \return \c true - if this URI is
	//! lexicographically less than the other one, \c false
	//! otherwise.

	bool operator<(const uriimpl &o) const noexcept
	{
		return compare(o) < 0;
	}

	//! Comparison operator

	//! \return \c true - if this URI should be
	//! lexicographically less than or equal to the other one, \c false
	//! otherwise.

	bool operator<=(const uriimpl &o) const noexcept
	{
		return compare(o) <= 0;
	}

	//! Comparison operator

	//! \return \c true - if this URI is
	//! lexicographically greater than the other one, \c false
	//! otherwise.

	bool operator>(const uriimpl &o) const noexcept
	{
		return compare(o) > 0;
	}

	//! Comparison operator

	//! \return \c true - if this URI should be
	//! lexicographically greater than or equal to the other one, \c false
	//! otherwise.

	bool operator>=(const uriimpl &o) const noexcept
	{
		return compare(o) >= 0;
	}

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
				    std::string::const_iterator);

extern template std::ostreambuf_iterator<char>
uriimpl::toString(std::ostreambuf_iterator<char>, bool)
	const;

extern template std::ostreambuf_iterator<char>
uriimpl::toString(std::ostreambuf_iterator<char>, const const_locale &, bool)
	const;

extern template
uriimpl::authority_t::authority_t(std::string::const_iterator,
				  std::string::const_iterator);

extern template
std::string::const_iterator
uriimpl::authority_t::parse(std::string::const_iterator,
			    std::string::const_iterator);

extern template std::ostreambuf_iterator<char>
uriimpl::authority_t::toString(std::ostreambuf_iterator<char>)
	const;

extern template std::ostreambuf_iterator<char>
uriimpl::authority_t::toString(std::ostreambuf_iterator<char>,
			       const const_locale &)
	const;

#endif

#if 0
{
#endif
}
#endif
