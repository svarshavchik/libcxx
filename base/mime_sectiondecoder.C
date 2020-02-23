/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mime/sectiondecoder.H"
#include "x/chrcasecmp.H"
#include "x/exception.H"

#include <sstream>

namespace LIBCXX_NAMESPACE {
	namespace mime {
#if 0
	};
};
#endif

bool section_decoderBase::is_quoted_printable(const std::string &te)
{
	return chrcasecmp::str_equal_to()(te, "quoted-printable");
}

bool section_decoderBase::is_base64(const std::string &te)
{
	return chrcasecmp::str_equal_to()(te, "base64");
}

#if 0
{
	{
#endif
	}
}
