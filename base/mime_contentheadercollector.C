/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mime/contentheadercollector.H"
#include "x/mime/tokens.H"

#include <sstream>

namespace LIBCXX_NAMESPACE {
	namespace mime {
#if 0
	};
};
#endif

contentheader_collectorObj::contentheader_collectorObj(bool mime_10_required)
	: mime_version_seen(!mime_10_required)
{
}

contentheader_collectorObj::~contentheader_collectorObj() noexcept
{
}

void contentheader_collectorObj::operator=(int c)
{
	header_collectorObj::operator=(c);

	if ((c == body_start || c == eof) && !mime_version_seen)
		content_headers.clear();
}

void contentheader_collectorObj::header()
{
	if (name_lc == "mime-version")
	{
		int major=0;
		char sep=0;
		int minor=0;

		std::istringstream i(contents);

		i >> major >> sep >> minor;

		if (major == 1 && minor == 0)
			mime_version_seen=1;
		return;
	}

	if (name_lc.substr(0, 8) == "content-")
		content_headers.insert(std::make_pair(name, contents));
}

#if 0
{
	{
#endif
	}
}
