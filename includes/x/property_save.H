/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_property_save_H
#define x_property_save_H

#include <x/namespace.h>
#include <x/property_properties.H>
#include <type_traits>
#include <iterator>

namespace LIBCXX_NAMESPACE::property {
#if 0
};
#endif

//! Format properties into an output iterator

//! The properties are formatted and written into the specified output iterator,
//! so that load_properties() can restore them, later.

template<typename iter_type>
iter_type save_properties(//! Properties
			  const std::map<std::string, std::string> &properties,

			  //! Output iterator
			  iter_type iter,
			  //! Locale for parsing
			  const const_locale &locale)
{
	std::string name=locale->name();

	if (name.size() != 0)
	{
		name="#LOCALE " + name + "\n";

		iter=std::copy(name.begin(), name.end(), iter);
	}

	for (const auto &prop:properties)
	{
		iter=std::copy(prop.first.begin(), prop.first.end(), iter);
		*iter++='=';

		for (auto ch:prop.second)
		{
			if (ch == '#' || ch == '\\' || (ch >= 0 && ch <= 0x20))
			{
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
			else
			{
				*iter++=ch;
			}
		}
		*iter++='\n';
	}
	return iter;
}

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/property_save_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
#endif
}
#endif
