/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mime/sectionprocessor.H"
#include "x/weakptr.H"
#include <sstream>

namespace LIBCXX_NAMESPACE::mime {
#if 0
}
#endif

sectioninfoObj::sectioninfoObj()
	: starting_pos(0), header_char_cnt(0), body_char_cnt(0),
	  header_line_cnt(0), body_line_cnt(0),
	  index(0), no_trailing_newline(false)
{
}

sectioninfo sectioninfoObj::create_subsection()
{
	return sectioninfo::create(sectioninfo(this));
}

sectioninfoObj::sectioninfoObj(const sectioninfo &parentArg)
	: sectioninfoObj()
{
	parent=parentArg;

	starting_pos=parentArg->starting_pos + parentArg->header_char_cnt +
		parentArg->body_char_cnt;

	index=parentArg->children.size();
}

sectioninfoObj::~sectioninfoObj()
{
}

std::string sectioninfoObj::index_name() const
{
	std::ostringstream o;
	index_name(o);
	return o.str();
}

void sectioninfoObj::index_name(std::ostream &o) const
{
	auto p=parent.getptr();

	if (!p.null())
	{
		p->index_name(o);
		o << ".";
	}

	o << (index+1);
}

#if 0
{
#endif
}
