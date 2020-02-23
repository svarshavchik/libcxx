/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN
LIBCXX_TEMPLATE_DECL template
LIBCXX_TEMPLATE_ITER tocsv(std::vector<std::basic_string<LIBCXX_TEMPLATE_CHAR> >::const_iterator,
			 std::vector<std::basic_string<LIBCXX_TEMPLATE_CHAR> >::const_iterator,
			 LIBCXX_TEMPLATE_ITER);

LIBCXX_TEMPLATE_DECL template
LIBCXX_TEMPLATE_ITER tocsv_value(LIBCXX_TEMPLATE_ITER,
			       const std::basic_string<LIBCXX_TEMPLATE_CHAR> &);

LIBCXX_TEMPLATE_DECL template
LIBCXX_TEMPLATE_ITER tocsv_value(LIBCXX_TEMPLATE_ITER,
			       const LIBCXX_TEMPLATE_CHAR *);


#ifndef LIBCXX_TEMPLATE_INST
template<>
inline LIBCXX_TEMPLATE_ITER tocsv(std::vector<std::basic_string<LIBCXX_TEMPLATE_CHAR> >::iterator b,
				std::vector<std::basic_string<LIBCXX_TEMPLATE_CHAR> >::iterator e,
				LIBCXX_TEMPLATE_ITER iter)
{
	std::vector<std::basic_string<LIBCXX_TEMPLATE_CHAR> >::const_iterator cb=b;
	std::vector<std::basic_string<LIBCXX_TEMPLATE_CHAR> >::const_iterator ce=e;

	return tocsv(cb, ce, iter);
}

#endif
#endif
