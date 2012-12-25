/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN

#define LIBCXX_TEMPLATE_CHAR_T wchar_t
#define LIBCXX_TEMPLATE_ITER std::ostreambuf_iterator<wchar_t>
#include <x/property_save_to.H>
#undef LIBCXX_TEMPLATE_ITER
#undef LIBCXX_TEMPLATE_CHAR_T

#define LIBCXX_TEMPLATE_CHAR_T char
#define LIBCXX_TEMPLATE_ITER std::ostreambuf_iterator<char>
#include <x/property_save_to.H>
#undef LIBCXX_TEMPLATE_ITER
#undef LIBCXX_TEMPLATE_CHAR_T

#define LIBCXX_TEMPLATE_CHAR_T wchar_t
#define LIBCXX_TEMPLATE_ITER ocodecvtiter_bytype<std::ostreambuf_iterator<char>, wchar_t, char>::iter_t::iter
#include <x/property_save_to.H>
#undef LIBCXX_TEMPLATE_ITER
#undef LIBCXX_TEMPLATE_CHAR_T

LIBCXX_TEMPLATE_DECL template std::ostreambuf_iterator<char>
save_properties<char>(const std::map<propvalue, propvalue> &,
		      std::ostreambuf_iterator<char>,
		      const const_locale &);

LIBCXX_TEMPLATE_DECL template std::ostreambuf_iterator<wchar_t>
save_properties<wchar_t>(const std::map<propvalue, propvalue> &,
			 std::ostreambuf_iterator<wchar_t>,
			 const const_locale &);
#endif
