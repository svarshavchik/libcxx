/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/upload.H"

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

uploadObj::uploadObj()
{
}

uploadObj::~uploadObj()
{
}

uploadObj::filebaseObj::filebaseObj()
{
}

uploadObj::filebaseObj::~filebaseObj()
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
#endif
}
