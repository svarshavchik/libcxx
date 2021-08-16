/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/wordexp.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void wordexp(const std::string &str,
	     std::vector<std::string> &words,
	     int flags)
{
	wordexp_t t;

	int rc=::wordexp(str.c_str(), &t, flags);

	switch (rc) {
	case 0:
		break;

	case WRDE_BADCHAR:
		throw EXCEPTION(_("Bad character"));
	case WRDE_BADVAL:
		throw EXCEPTION(_("Undefined shell variable"));
	case WRDE_CMDSUB:
		throw EXCEPTION(_("Command substitution disabled"));
	case WRDE_NOSPACE:
		throw EXCEPTION(_("Out of memory"));
	default:
		throw EXCEPTION(_("Syntax error"));
	}

	try {
		words.insert(words.end(), t.we_wordv, t.we_wordv + t.we_wordc);

		wordfree(&t);
	} catch (...) {
		wordfree(&t);
		throw;
	}
}

#if 0
{
#endif
}
