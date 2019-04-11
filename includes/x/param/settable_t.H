/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN

LIBCXX_TEMPLATE_DECL
template LIBCXX_SETTABLE_CLASS
LIBCXX_SETTABLE_CLASS::from_string(std::string::const_iterator,
				std::string::const_iterator,
				const const_locale &);

template<>
template<>
LIBCXX_SETTABLE_CLASS
LIBCXX_SETTABLE_CLASS::from_string<std::string::iterator
				  >(std::string::iterator b,
				    std::string::iterator e,
				    const const_locale &l)
#ifdef LIBCXX_TEMPLATE_DECLARE
{
	return from_string(std::string::const_iterator(b),
			  std::string::const_iterator(e),
			  l);
}
#else
;
#endif

#endif
