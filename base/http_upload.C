/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/upload.H"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

uploadObj::uploadObj()
{
}

uploadObj::~uploadObj() noexcept
{
}

uploadObj::filebaseObj::filebaseObj()
{
}

uploadObj::filebaseObj::~filebaseObj() noexcept
{
}

void uploadObj::add_file_upload(useragentObj &ua,
				const std::string &charset,
				std::list<mime::encoder> &list)
{
	for (const auto &file:files)
	{
		file->add_file_upload(ua, charset, list);
	}
}

#if 0
{
	{
#endif
	}
}