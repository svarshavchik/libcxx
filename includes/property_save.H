/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_property_save_H
#define x_property_save_H

#include <x/namespace.h>
#include <x/property_properties.H>
#include <x/codecvtiter.H>
#include <x/iter_value.H>
#include <iterator>

namespace LIBCXX_NAMESPACE {
	namespace property {
#if 0
	};
};
#endif
//! Helper implementation class for saving property map to an output stream

//! The template value is the character type of the output stream. The template
//! defines a static template method, save_impl(), that implements
//! the save functionality.
//!
//! save_properties() uses the value that the output iterator iterates over
//! to instantiate this template class, then uses the full iterator class
//! to invoke save_impl().

template<typename char_type>
class save_properties_impl {

public:
	template<typename iter_type> static
	iter_type save_impl(const std::map<propvalue, propvalue> &properties,
			    iter_type iter,
			    const const_locale &locale);
};

//! Helper implementation specialization

//! This specialization for the property value character class defines the
//! specialized save_impl() method that actually saves the properties into
//! the output stream
template<>
class save_properties_impl<propvalue::value_type> {

public:
	template<typename iter_type> static
	iter_type save_impl(const std::map<propvalue, propvalue> &properties,
			    iter_type iter,
			    const const_locale &locale);
};

//! Save properties into an output iterator

//! Constructs a temporary output iterator that converts the property values
//! to the output character class and writes the result to the real output
//! iterator, using the parsing locale, then invokes the
//! specialization that saves the property values into the temporary iterator.

template<typename char_type>
template<typename iter_type>
iter_type save_properties_impl<char_type>
::save_impl(//! Properties to save
	    const std::map<propvalue, propvalue> &properties,

	    //! Output iterator
	    iter_type iter,

	    //! Locale for parsing
	    const const_locale &locale)
{
	typedef typename ocodecvtiter_bytype<iter_type,
					     propvalue::value_type,
					     char_type>::iter_t iter_t;

	return save_properties_impl<propvalue::value_type>
		::save_impl(properties, iter_t::create(iter, locale),
			    locale).flush();
}

//! Save property values into an output iterator

//! The properties are formatted as "name=value\n" pairs, into the output
//! iterator, prepended by "#LOCALE name\n" if the parsing locale has a name.
//!
//! Property values are escaped, if necessary, so that load_properties()
//! can restore the original values.

template<typename iter_type>
iter_type save_properties_impl<propvalue::value_type>
::save_impl(//! Properites to save
	    const std::map<propvalue, propvalue> &properties,
	    //! Output iterator
	    iter_type iter,
	    //! Locale for parsing
	    const const_locale &locale)
{
	basic_ctype<propvalue::value_type> ct(locale);

	std::string name=locale->name();

	if (name.size() != 0)
	{
		name="#LOCALE " + name + "\n";

		iter=std::copy(name.begin(), name.end(), iter);
	}

	for (std::map<propvalue, propvalue>::const_iterator
		     b=properties.begin(), e=properties.end(); b != e; ++b)
	{
		iter=std::copy(b->first.begin(), b->first.end(), iter);
		*iter++='=';

		for (propvalue::const_iterator vb=b->second.begin(),
			     ve=b->second.end(), p=vb; p != ve; )
		{
			propvalue::value_type ch= *p;
			bool esc=p == vb && ct.is(ch, std::ctype_base::space);

			if (++p == ve && ct.is(ch, std::ctype_base::space))
				esc=true;

			if (ch == '#' || ch == '\\' || (ch >= 0 && ch < 0x20))
				esc=true;

			if (!esc)
			{
				*iter++=ch;
				continue;
			}

			*iter++='\\';

			switch (ch) {
			case '\n':
				*iter++ = 'n';
				break;
			case '\r':
				*iter++ = 'r';
				break;
			case '\t':
				*iter++ = 't';
				break;
			default:
				if (ch >= 0 && ch < 0x20)
				{
					*iter++ = (ch/8) + '0';
					*iter++ = (ch & 7) + '0';
				}
				else *iter++=ch;
			}
		}
		*iter++='\n';
	}
	return iter;
}

//! Format properties into an output iterator

//! The properties are formatted and written into the specified output iterator,
//! so that load_properties() can restore them, later.
template<typename iter_type>
iter_type save_properties(//! Properties
			  const std::map<propvalue, propvalue> &properties,

			  //! Output iterator
			  iter_type iter,
			  //! Locale for parsing
			  const const_locale &locale=
			  locale::base::environment())
{
	typedef typename iter_value_type<iter_type>::value_t value_t;

	return save_properties_impl<value_t>
		::save_impl(properties, iter, locale);
}

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/property_save_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
	{
#endif
	}
}
#endif
