/*
** Copyright 2015-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/strtok.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template void trim(const char *&, const char *&);
template void trim(const char32_t *&, const char32_t *&);
template void trim(std::string::iterator &, std::string::iterator &);
template void trim(std::string::const_iterator &,
		   std::string::const_iterator &);
template void trim(std::u32string::iterator &, std::u32string::iterator &);
template void trim(std::u32string::const_iterator &,
		   std::u32string::const_iterator &);

template std::string trim(const std::string &);
template std::u32string trim(const std::u32string &);

#if 0
{
#endif
}
