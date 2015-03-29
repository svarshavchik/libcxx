/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN

LIBCXX_TEMPLATE_DECL template
void parametersObj::decode_params(LIBCXX_TEMPLATE_ITER2, LIBCXX_TEMPLATE_ITER2);

LIBCXX_TEMPLATE_DECL template
LIBCXX_TEMPLATE_ITER2 parametersObj::decode_octet(LIBCXX_TEMPLATE_ITER2,
						  LIBCXX_TEMPLATE_ITER2,
						  std::string &);

LIBCXX_TEMPLATE_DECL template
std::pair<LIBCXX_TEMPLATE_ITER2, unsigned char>
decode_nybble(LIBCXX_TEMPLATE_ITER2, LIBCXX_TEMPLATE_ITER2);

LIBCXX_TEMPLATE_DECL template
std::string parametersObj::decode_octets(LIBCXX_TEMPLATE_ITER2,
					 LIBCXX_TEMPLATE_ITER2);

#endif
