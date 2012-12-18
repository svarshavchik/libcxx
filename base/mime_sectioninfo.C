/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mime/sectionprocessor.H"
#include "x/weakptr.H"
#include <sstream>

namespace LIBCXX_NAMESPACE {
	namespace mime {
#if 0
	};
};
#endif

sectioninfoObj::sectioninfoObj()
	: starting_pos(0), header_char_cnt(0), body_char_cnt(0),
	  header_line_cnt(0), body_line_cnt(0),
	  index(0), no_trailing_newline(false)
{
}

sectioninfo sectioninfoObj::new_child()
{
	sectioninfo c=sectioninfo::create();

	c->starting_pos=starting_pos + header_char_cnt + body_char_cnt;
	c->index=children.size();
	c->parent=sectioninfo(this);
	children.push_back(c);
	return c;
}

sectioninfoObj::~sectioninfoObj() noexcept
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
	{
#endif
	}
}
