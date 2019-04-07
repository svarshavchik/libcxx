/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/glob.H"
#include "x/ref.H"
#include "x/messages.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

globObj::globObj() : glflag(false)
{
}

globObj::~globObj()
{
	if (glflag)
		globfree(&gl);
}

ref<globObj> globObj::expand(const std::string &pattern, int flag)

{
	int err=::glob(pattern.c_str(),
		       flag | (glflag ? GLOB_APPEND : 0), NULL, &gl);
	glflag=true;

	switch (err) {
	case GLOB_NOSPACE:
		throw EXCEPTION(gettextmsg(_("glob() ran out of memory processing %1%"), pattern));
	case GLOB_ABORTED:
		throw EXCEPTION(gettextmsg(_("read error expanding %1%"),
					   pattern));
	case GLOB_NOMATCH:
		throw EXCEPTION(gettextmsg(_("%1% not found"), pattern));
	};

	return ref<globObj>(this);
}

void globObj::get(std::vector<std::string> &vec)
{
	if (!glflag)
		return;

	vec.reserve(vec.size()+gl.gl_pathc);
	get(std::back_insert_iterator<std::vector<std::string> >(vec));
}

void globObj::get(std::set<std::string> &vec)
{
	if (!glflag)
		return;

	get(std::insert_iterator<std::set<std::string> >(vec, vec.end()));
}

#if 0
{
#endif
}
