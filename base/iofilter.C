/*
** Copyright 2016-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/basiciofilter.H"
#include "x/basiciofilteriter.H"
#include "x/property_value.H"
#include "x/exception.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif


property::value<size_t> iofilter_bufsizeprop LIBCXX_INTERNAL
(LIBCXX_NAMESPACE_STR "::iofilter::defaultsize", 1024);

size_t iofilter_bufsize()
{
	return iofilter_bufsizeprop.get();
}

template class iofilter<char, char>;
template class iofilteradapterObj<char, char>;
template class obasiciofilteriterbase<char, char>;


#if 0
{
#endif
}
