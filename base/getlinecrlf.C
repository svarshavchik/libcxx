/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "getlinecrlf.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template std::istream &getlinecrlf(std::istream &, std::string &)
;

template std::pair<std::istreambuf_iterator<char>, bool>
getlinecrlf(std::istreambuf_iterator<char>,
	    std::istreambuf_iterator<char>,
	    std::string &);

template std::istream &getlinelf(std::istream &, std::string &)
;

template std::pair<std::istreambuf_iterator<char>, bool>
getlinelf(std::istreambuf_iterator<char>,
	    std::istreambuf_iterator<char>,
	    std::string &);

#if 0
{
#endif
}
