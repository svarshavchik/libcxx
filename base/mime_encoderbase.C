/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mime/encoderbase.H"

namespace LIBCXX_NAMESPACE {
	namespace mime {
#if 0
	};
};
#endif

encoderbaseObj::encoderbaseObj()
{
}

encoderbaseObj::~encoderbaseObj() noexcept
{
}

inputrefiterator<char> encoderbaseObj::end() const
{
	return inputrefiterator<char>::create();
}

#if 0
{
	{
#endif
	}
}
