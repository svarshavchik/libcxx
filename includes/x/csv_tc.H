/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#define LIBCXX_TEMPLATE_ITER std::ostreambuf_iterator<LIBCXX_TEMPLATE_CHAR>
#include <x/csvto_to.H>
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::back_insert_iterator<std::basic_string<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvto_to.H>
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::back_insert_iterator<std::vector<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvto_to.H>
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::istreambuf_iterator<LIBCXX_TEMPLATE_CHAR>
#include <x/csvheader_to.H>
#define LIBCXX_TEMPLATE_CONT std::vector<std::basic_string<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvfrom_to.H>
#undef LIBCXX_TEMPLATE_CONT
#define LIBCXX_TEMPLATE_CONT std::list<std::basic_string<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvfrom_to.H>
#undef LIBCXX_TEMPLATE_CONT
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::basic_string<LIBCXX_TEMPLATE_CHAR>::iterator
#include <x/csvheader_to.H>
#define LIBCXX_TEMPLATE_CONT std::vector<std::basic_string<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvfrom_to.H>
#undef LIBCXX_TEMPLATE_CONT
#define LIBCXX_TEMPLATE_CONT std::list<std::basic_string<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvfrom_to.H>
#undef LIBCXX_TEMPLATE_CONT
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::basic_string<LIBCXX_TEMPLATE_CHAR>::const_iterator
#include <x/csvheader_to.H>
#define LIBCXX_TEMPLATE_CONT std::vector<std::basic_string<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvfrom_to.H>
#undef LIBCXX_TEMPLATE_CONT
#define LIBCXX_TEMPLATE_CONT std::list<std::basic_string<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvfrom_to.H>
#undef LIBCXX_TEMPLATE_CONT
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::vector<LIBCXX_TEMPLATE_CHAR>::iterator
#include <x/csvheader_to.H>
#define LIBCXX_TEMPLATE_CONT std::vector<std::basic_string<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvfrom_to.H>
#undef LIBCXX_TEMPLATE_CONT
#define LIBCXX_TEMPLATE_CONT std::list<std::basic_string<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvfrom_to.H>
#undef LIBCXX_TEMPLATE_CONT
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::vector<LIBCXX_TEMPLATE_CHAR>::const_iterator
#include <x/csvheader_to.H>
#define LIBCXX_TEMPLATE_CONT std::vector<std::basic_string<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvfrom_to.H>
#undef LIBCXX_TEMPLATE_CONT
#define LIBCXX_TEMPLATE_CONT std::list<std::basic_string<LIBCXX_TEMPLATE_CHAR> >
#include <x/csvfrom_to.H>
#undef LIBCXX_TEMPLATE_CONT
#undef LIBCXX_TEMPLATE_ITER

#ifndef DOXYGEN
LIBCXX_TEMPLATE_DECL template class csvheaderrow_iter<LIBCXX_TEMPLATE_CHAR>;
#endif
