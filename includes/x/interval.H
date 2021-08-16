/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_interval_H
#define x_interval_H

#include <x/exception.H>
#include <x/namespace.h>
#include <x/locale.H>
#include <x/messages.H>
#include <x/to_string.H>

#include <string>
#include <vector>
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for the interval parser

//! \internal
//!
class interval_base {

protected:
	//! Default constructor

	interval_base() noexcept {}

	//! Default destructor
	~interval_base() {}

	//! Throw an exception - invalid parameters to interval constructor

	static void invalid_constructor()
		__attribute__((noreturn));

	//! Throw an exception - syntax error

	static void syntax_error()
		__attribute__((noreturn));
};

//! An interval parser

//! This template is used by the library to parse localized
//! date and time intervals,
//! but is generic enough to be used for parsing strings with some arbitrary
//! names and values.
//!
//! The parser takes a string, defined by a beginning and an ending iterator,
//! that contains localized names and values. A name
//! may appear before or after the corresponding value. Each localized name is
//! matched to a list of specified names. The list of names is provided in a
//! given order, and the parser returns a list of values in the same order.
//!
//! So, when parsing hours, minutes, and seconds, for example, the string
//! may specify the values in any order, such as "2 minutes, 1 hour", and
//! the parser returns the values in the correct order.
//!
//! This parser tries to be loose in matching up value names. When attempting
//! to interpret a word, the parser tries to match it up against:
//!
//! * The entire localized name, using a given message catalog
//!
//! * The first character of the localized name
//!
//! * The first character of the unlocalized name

template<typename int_type>
class interval : public interval_base {

	//! Localized string array

	const char *const *sp;

	//! Default value index.

	size_t default_value;

	//! Parsed values

	std::vector<int_type> values;

	//! Message catalog
	const_messages catalog;

public:
	//! The parser constructor
	interval(//! An array of alternate singular and plurals words.
		 //! The array must be of even size, and followed by a single
		 //! NULL character pointer.
		 //!
		 //! Example: {"minute", "minutes", "hour", "hours",
		 //! "second", "seconds", NULL}
		 const char * const * spArg,

		 //! The default value. The parser will parse a string that
		 //! consists of only a single value, and assign it to the
		 //! corresponding name. So, in this example, specifying \c 0
		 //! will parse the nameless value as an hour, \c 1 would make
		 //! it a minute, and \c 2 a second.
		 size_t default_valueArg,

		 //! The message catalog for localized names.
		 const const_messages &catalogArg)

		: sp(spArg),
		  default_value(default_valueArg),
		  catalog(catalogArg)
	{
		size_t i=0;

		// Array must be even

		while (sp[i])
		{
			++i;
			if (!sp[i])
				invalid_constructor();
			++i;
		}

		values.resize(i/2);

		if (default_value >= values.size())
			invalid_constructor();
	}

	//! Parse a character string

	//! \return a reference to a vector that contains the values for each
	//! associated name. If the value was not specified, it gets set to 0.

	const std::vector<int_type> &parse(const std::u32string &uc)
	{
		std::fill(values.begin(), values.end(), int_type(0));

		bool has_word=false;
		bool has_value=false;

		std::u32string word;
		int_type value;

		std::vector<std::u32string> lookup; // Cache localized words

		std::vector<bool> lookup_flag; // Seen this word

		lookup.resize(values.size());
		lookup_flag.resize(values.size());
		std::fill(lookup_flag.begin(), lookup_flag.end(), false);

		for (auto b=uc.begin(), e=uc.end(); b != e; )
		{
			if (unicode_isspace(*b))
			{
				++b;
				continue;
			}

			if (*b != '-' && unicode_ispunct(*b))
			{
				++b;
				continue;
			}

			if (unicode_isalpha(*b))
			{
				// Grab a word

				if (has_word)
					syntax_error(); // Already did

				word.clear();

				while (b != e && unicode_isalpha(*b))
				{
					word.push_back(*b++);
				}
				unicode::tolower(word);
				has_word=true;
			}
			else
			{
				// Grab a number

				if (has_value)
					syntax_error(); // Already did

				std::string w;

				if (*b == '-')
					w.push_back(*b++);

				if (b == e || !unicode_isdigit(*b))
					syntax_error();

				while (b != e && unicode_isdigit(*b))
				{
					w.push_back(((*b++) & 0x0F) + '0');
				}

				std::istringstream i(w);

				i >> value;

				if (i.fail())
					syntax_error();

				has_value=true;
			}

			if (!has_word || !has_value)
				continue;

			size_t i;

			for (i=0; i<values.size(); i++)
			{
				lookup[i]=
					unicode::tolower
					(catalog->get_unicode(sp[i*2],
							      sp[i*2+1],
							      value));
				if (lookup[i] == word)
					break;
			}

			if (i >= values.size() && !word.empty())
			{
				// Maybe match the first character.

				for (i=0; i<values.size(); i++)
				{
					if (lookup[i].empty())
						continue;

					if (lookup[i][0] == word[0])
						break;
				}
			}

			if (i >= values.size())
				syntax_error();

			lookup_flag[i]=true;
			values[i]=value;
			has_word=false;
			has_value=false;

		}

		if (has_word)
			syntax_error();

		if (has_value)
			values[default_value]=value;

		return values;
	}

	//! Parse a character string

	//! \overload
	//!
	const std::vector<int_type> &parse(const std::string &str,
					   const const_locale &l)
	{
		std::u32string uc;

		unicode::iconvert::convert(str, l->charset(), uc);
		return parse(uc);
	}
};

extern template class interval<int>;
extern template class interval<unsigned>;

extern template class interval<long>;
extern template class interval<unsigned long>;

#if 0
{
#endif
}
#endif
