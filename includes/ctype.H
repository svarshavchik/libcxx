/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ctype_H
#define x_ctype_H

#include <x/exceptionfwd.H>
#include <x/locale.H>
#include <x/facet.H>
#include <x/namespace.h>

#include <string>
#include <locale>
#include <vector>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Encapsulate character type functions

//! This template class definition provides locale-specific character type
//! functions.
//!
//! This is, internally, a reference to a wrapper of the \c std::ctype facet,
//! and may be freely passed around, once constructed, with minimal cost.
//!
//! This template is typically instantiated using \c char or \c wchar_t
//! for the template paramer, giving the implementation for the given
//! character class.
//!
//! \see ctype, wctype

template<typename char_type,
	 typename traits_type=std::char_traits<char_type> >
class basic_ctype : private ptr< facetObj<std::ctype<char_type> > > {

	//! The facet type

	//! \internal
	//!
	typedef facetObj<std::ctype<char_type> > facet_t;

	//! The internal facet reference pointer type

	//! \internal
	typedef ptr<facet_t> ptr_type;

public:
	//! Default constructor

	//! The default constructor creates an empty placeholder. None of
	//! the functions can be used until this instance is assigned from
	//! another populated instance.
	basic_ctype() noexcept
	{
	}

	//! Initialize the type conversion object.

	//! Initialize the character conversion functions for the given locale.
	//!
	basic_ctype(//! The locale used for character conversion functions

		    const const_locale &l)
		: ptr_type( l->get_facet<ptr_type> ())
	{
	}

	//! Default destructor
	~basic_ctype() noexcept
	{
	}

	//! Convert a string to uppercase

	//! \return the uppercase version of the requested string.
	//!
	template<typename alloc_type>
	std::basic_string<char_type, traits_type, alloc_type>
	toupper(//! Original string
		const std::basic_string<char_type, traits_type, alloc_type>
		&str)
		const
	{
		facet_t &f(*ptr_type::operator->());
		globlocale g(f.getLocale());

		std::basic_string<char_type, traits_type, alloc_type>
			strcopy(str);

		f.getFacetConstRef()
			.toupper(&strcopy[0], &strcopy[strcopy.size()]);

		return strcopy;
	}

	//! Convert a string to uppercase

	//! \overload
	//!
	std::basic_string<char_type>
	toupper (//! Original string
		 const char_type *str) const noexcept
	{
		return toupper(std::basic_string<char_type>(str));
	}

	//! Convert a string to lowercase

	//! \return the lowercase version of the requested string.
	//!

	template<typename alloc_type>
	std::basic_string<char_type, traits_type, alloc_type>
	tolower (//! Original string
		 const std::basic_string<char_type, traits_type, alloc_type>
		 &str) const
	{
		facet_t &f(*ptr_type::operator->());
		globlocale g(f.getLocale());

		std::basic_string<char_type, traits_type, alloc_type>
			strcopy(str);

		f.getFacetConstRef()
			.tolower(&strcopy[0], &strcopy[strcopy.size()]);

		return strcopy;
	}

	//! Convert a string to lowercase

	//! \overload
	//!
	std::basic_string<char_type>
	tolower (//! Original string
		 const char_type *str) const noexcept
	{
		return tolower(std::basic_string<char_type>(str));
	}

	//! Scan for the first occurence of a character class

	//! Locate the first character in the range of character specified by
	//! low-high that meets the classification specified by Mask.
	//!
	//! The low and high iterators may be plain character pointers, or
	//! string iterators.

	template<typename iter_type>
	iter_type scan_is(//! Character classification
			  std::ctype_base::mask Mask,

			  //! Starting search position
			  iter_type low,

			  //! Ending search position
			  iter_type high) const noexcept
	{
		facet_t &f(*ptr_type::operator->());
		globlocale g(f.getLocale());

		return f.getFacetConstRef()
			.scan_is(Mask, &*low, &*high) - &*low + low;
	}

	//! Scan for the first occurence not of the given character class

	//! Locate the first character in the range of character specified by
	//! low-high that does not meet the classification specified by Mask.
	//!
	//! The low and high iterators may be plain character pointers, or
	//! string iterators.
	template<typename iter_type>
	iter_type scan_not(//! Character classification
			  std::ctype_base::mask Mask,

			  //! Starting search position
			  iter_type low,

			  //! Ending search position
			  iter_type high) const noexcept
	{
		facet_t &f(*ptr_type::operator->());
		globlocale g(f.getLocale());

		return f.getFacetConstRef()
			.scan_not(Mask, &*low, &*high) - &*low + low;
	}

	//! Extract character sequences from a string

	//! strtok_is() locates consecutive character sequences from a string
	//! that do not belong to the specified character class. Each
	//! consecutive character sequence is formed into a separate string
	//! and added to the specified container.
	//!
	//! \note
	//! The container does not get cleared, its existing contents remain
	//! the place. If the string consists solely of characters in the
	//! specified character class, nothing gets added to the container.

	template<typename container_type>
	void strtok_is(//! String to scan
		       const char_type *b,

		       //! Individual character sequence are added to this container
		       container_type &Container,

		       //! Character classification.
		       //! With the default value, whitespace-delimited words
		       //! get extracted and placed into the container.
		       std::ctype_base::mask mask=std::ctype_base::space,

		       //! Optional character that quotes strings that
		       //! contain delimiters.

		       typename traits_type::int_type
		       quote_char=traits_type::eof()) const
	{
		const char_type *e, *p;

		for (e=b; *e; ++e)
			;

		while ((b=scan_not(mask, b, e)) != e)
		{
			if (traits_type::eq_int_type(traits_type
						     ::to_int_type(*b),
						     quote_char))
			{
				typename container_type::value_type val;

				b=skip_quoted(++b, e, val, quote_char);

				Container.push_back(val);
				continue;
			}

			p=scan_is(mask, b, e);
			Container.push_back(typename
					    container_type::value_type(b, p));

			b=p;
		}
	}

	//! Extract character sequences from a string

	//! \overload
	template<typename alloc_type,
		 typename container_type>
	void strtok_is( const std::basic_string<char_type, traits_type,
			alloc_type> &str,
			container_type &Container,
			std::ctype_base::mask mask=std::ctype_base::space,
			typename traits_type::int_type quote_char=
			traits_type::eof()) const
	{
		return strtok_is(str.c_str(), Container, mask, quote_char);
	}

	//! Extract character sequences from a string

	//! strtok_not() locates consecutive character sequences from a string
	//! that belong to the specified character class. Each
	//! consecutive character sequence is formed into a separate string
	//! and added to the specified container.
	//!
	//! \note
	//! The container does not get cleared, its existing contents remain
	//! the place. If the string consists solely of characters not in the
	//! specified character class, nothing gets added to the container.

	template<typename container_type>
	void strtok_not(//! String to scan
			const char_type *b,
			
			//! Individual character sequence are added to this container
			container_type &Container,

			//! Character classification.
			//! If, for example, this is std::ctype_base::digit,
			//! strtok_not() extracts integers and places them
			//! into the container.
			std::ctype_base::mask mask,

			//! Optional character that quotes strigs that
			//! contain delimiters.

			typename traits_type::int_type quote_char=
			traits_type::eof()) const
	{
		const char_type *e, *p;

		for (e=b; *e; ++e)
			;

		while ((b=scan_is(mask, b, e)) != e)
		{
			p=scan_not(mask, b, e);

			if (traits_type::eq_int_type(traits_type
						     ::to_int_type(*b),
						     quote_char))
			{
				typename container_type::value_type val;

				b=skip_quoted(++b, e, val, quote_char);

				Container.push_back(val);
				continue;
			}

			Container.push_back(typename
					    container_type::value_type(b, p));

			b=p;
		}
	}

	//! Extract character sequences from a string

	//! \overload
	template<typename alloc_type,
		 typename container_type>
	void strtok_not( const std::basic_string<char_type, traits_type,
			 alloc_type> &str,
			 container_type &Container,
			 std::ctype_base::mask mask=std::ctype_base::space,
			 typename traits_type::int_type quote_char=
			 traits_type::eof()) const
	{
		strtok_not(str.c_str(), Container, mask, quote_char);
	}

	//! Test if given character nelongs to the given character class

	bool is(//! Character
		char_type c,

		//! Character class
		std::ctype_base::mask m) const
	{
		facet_t &f(*ptr_type::operator->());
		globlocale g(f.getLocale());

		return f.getFacetConstRef().is(m, c);
	}

	//! Widen a character

	char_type widen(//! Character
			char c) const
	{
		facet_t &f(*ptr_type::operator->());
		globlocale g(f.getLocale());

		return f.getFacetConstRef().widen(c);
	}

	//! Widen characters

	std::basic_string<char_type> widen(//! Starting position
					   const char *start,

					   //! Ending position,
					   const char *end=0)
		const
	{
		facet_t &f(*ptr_type::operator->());
		globlocale g(f.getLocale());

		if (!end)
			for (end=start; *end; ++end)
				;

		std::vector<char_type> buffer;
		buffer.resize(end-start);

		f.getFacetConstRef().
			widen(start, end, &buffer[0]);

		return std::basic_string<char_type>(&buffer[0],
						    &buffer[end-start]);
	}

	//! Widen characters

	std::basic_string<char_type> widen(//! The string
					   const std::string &str)
		const
	{
		return widen(str.c_str());
	}

	//! Narrow a character

	char narrow(//! Character
		   char_type c,

		   //! Default character
		   char dfault) const
	{
		facet_t &f(*ptr_type::operator->());
		globlocale g(f.getLocale());

		return f.getFacetConstRef().narrow(c, dfault);
	}

	//! Narrow characters

	std::string narrow(//! Starting position
			   const char_type *start,

			   //! Default character
			   char dfault,

			   //! Ending position,
			   const char_type *end=0)
		const
	{
		facet_t &f(*ptr_type::operator->());
		globlocale g(f.getLocale());

		if (!end)
			for (end=start; *end; ++end)
				;

		std::vector<char> buffer;
		buffer.resize(end-start);

		f.getFacetConstRef().narrow(start, end, dfault, &buffer[0]);

		return std::string(&buffer[0], &buffer[end-start]);
	}

	//! Narrow characters

	std::string narrow(//! The string
			   const std::basic_string<char_type> &str,

			   //! The default character
			   char dfault)
		const
	{
		return narrow(str.c_str(), dfault);
	}

private:
	//! Extract quoted word

	//! \internal
	//!
	template<typename container_type> static
	const char_type *skip_quoted(const char_type *p,
				     const char_type *e,
				     container_type &val,
				     typename traits_type::int_type quote_char)

	{
		while (p != e)
		{
			const char_type *q=p;

			p=traits_type::find(p, e-p, quote_char);

			val.insert(val.end(), q, p);

			if (p != e)
				++p;

			if ( !traits_type::eq_int_type(traits_type
						       ::to_int_type(*p),
						       quote_char))
				break;
			q=p;
			val.insert(val.end(), q, ++p);
		}

		return p;
	}

};

//! Character conversion functions - narrow characters

//! \code
//! locale utf8("en_us.UTF8");
//!
//! ctype ct(utf8);
//!
//! std::string str1=ct.toupper("Hello world");
//!
//! std::string str2=ct.tolower("Hello world");
//! \endcode
//!
//! The \c ::ctype object provides character conversion functions for a given
//! locale. The constructor specifies the %locale.
//!
//! The available character conversion functions:
//!
//! - \c toupper() - convert a string to uppercase
//!
//! - \c tolower() - convert a string to lowercase
//!
//! \par A fancier strtok()
//!
//! \code
//! std::list<std::string> words;
//! std::string str;
//!
//! ct.strtok_is(str, words, std::ctype_base::space, '"');
//! \endcode
//!
//! strtok_is() splits a string, \c str, into individual words (the \c words
//! parameter may be any container type). The optional fourth parameter
//! (\c std::ctype_base::space by default)
//! is a \c ctype mask that defines the word delimiters (default is the
//! whitespace). The optional fifth parameter defined a character that's
//! recognized as the escape character, which delimits an entire word that
//! may contain character that may otherwise be interpreted as delimiters.
//! Additionally, the doubled escape character results in a single escape
//! character inserted into the word.
//!
//! If the fifth parameter is not specified, no escape character is recognized.
//!
//! There's a complementary strtok_not() function that inverts the word
//! delimiter parameter. The third parameter to strtok_not() does not have
//! a defined default, since \c std::ctype_base::space would be meaningless
//! in this context. For example, \c std::ctype_base::digit ends up extracting
//! all consecutive digits from the string into the container, with all
//! other characters serving as delimiters.

typedef basic_ctype<char> ctype;

//! Character conversion functions - wide characters

//! This is the wide character version of ::ctype.
//!
//! \code
//! locale utf8("en_us.UTF8");
//!
//! wctype ct(utf8);
//!
//! std::wstring str1=ct.toupper(L"Hello world");
//!
//! std::wstring str2=ct.tolower(L"Hello world");
//!
//! std::list<std::wstring> words;
//! std::wstring str;
//!
//! ct.strtok_is(str, words, std::ctype_base::space, L'"');
//! \endcode
//!
//! \see ::ctype

typedef basic_ctype<wchar_t> wctype;

#if 0
{
#endif
}
#endif
