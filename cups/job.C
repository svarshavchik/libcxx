/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/cups/job.H"
#include "x/fd.H"
#include "x/mmap.H"
#include <cups/cups.h>
#include <sys/stat.h>

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

jobObj::jobObj()=default;

jobObj::~jobObj()=default;

static std::tuple<std::string,
		  std::function<std::optional<std::string>()>>
create_document_contents(const std::string &mime_type,
			 const fd &descriptor,
			 const mmap<char> &contents,
			 const struct ::stat &s)
{
	return {
		mime_type,
			[descriptor, contents, size=s.st_size, first=true]
			()
			mutable
			{
				std::optional<std::string> r;
				if (!first)
					return r;

				first=false;

				const char *str=contents->object();

				r.emplace(str, str+size);

				return r;
			}};
}

void jobObj::add_document_file(const std::string &name,
			       const std::string &filename,
			       const std::string &mime_type)
{
	add_document
		(name,
		 [=]
		 {
			 auto fd=fd::base::open(filename, O_RDONLY);
			 auto s=fd->stat();
			 auto contents=mmap<char>::create(fd, PROT_READ,
							  MAP_SHARED,
							  0, s.st_size);

			 return create_document_contents(mime_type, fd,
							 contents, s);
		     });
}

void jobObj::add_document_file(const std::string &name,
			       const std::string &filename)
{
	add_document_file(name, filename, CUPS_FORMAT_AUTO);
}

#if 0
{
#endif
}
