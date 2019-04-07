/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mime/headeriter.H"
#include "x/mime/headercollector.H"
#include "x/chrcasecmp.H"

namespace LIBCXX_NAMESPACE {
	namespace mime {
#if 0
	};
};
#endif

#define SWITCH state=&header_collectorObj::

header_collectorObj::header_collectorObj()
	: state(&header_collectorObj::wait_header_start)
{
}

void header_collectorObj::operator=(int c)
{
	(this->*state)(c);
}

void header_collectorObj::wait_header_start(int c)
{
	if (c == header_name_start)
	{
		name.clear();
		name_lc.clear();
		contents.clear();
		spacecount=0;

		SWITCH wait_header_end;
	}
}

void header_collectorObj::wait_header_end(int c)
{
	if (c == header_name_end)
	{
		SWITCH wait_contents_start;
		return;
	}

	if (!header_iterbase::space(c))
	{
		name.push_back(c);
		name_lc.push_back(chrcasecmp::tolower(c));
	}
}

void header_collectorObj::wait_contents_start(int c)
{
	if (c == header_contents_start)
		SWITCH wait_contents_end;
}

void header_collectorObj::wait_contents_end(int c)
{
	if (c == header_contents_end)
	{
		SWITCH wait_header_start;
		header();
		return;
	}

	if (c == newline_start || c == header_fold_start)
	{
		SWITCH wait_folded_end;
		return;
	}

	if (!nontoken(c))
		return; // What's this?

	if (header_iterbase::space(c))
	{
		++spacecount;
		return;
	}

	contents.append(spacecount, ' ');
	spacecount=0;
	contents.push_back(c);
}

void header_collectorObj::wait_folded_end(int c)
{
	switch (c) {
	case header_fold_end:
		++spacecount;

		/* FALLTHRU */

	case newline_end:
		SWITCH wait_contents_end;
	}
}

#if 0
{
	{
#endif
	}
}
