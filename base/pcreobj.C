/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/pcre.H"
#include "x/exception.H"
#include "x/tostring.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

pcreObj::pcreObj(const std::string &pattern, int options)
	: compiled(nullptr), studied(nullptr)
{
	const char *errptr;
	int erroffset;

	if (options & PCRE_UTF8)
	{
		int flag;

		pcre_config(PCRE_CONFIG_UTF8, &flag);

		if (!flag)
			options &= ~PCRE_UTF8;
	}

	if ((compiled=pcre_compile(pattern.c_str(), options, &errptr,
				   &erroffset, NULL)) == NULL)
		throw EXCEPTION(errptr);

	studied=pcre_study(compiled, PCRE_STUDY_JIT_COMPILE, &errptr);

	if (errptr)
		throw EXCEPTION(errptr);
}

pcreObj::~pcreObj()
{
	if (studied)
		pcre_free_study(studied);
	if (compiled)
		pcre_free(compiled);
}

void pcreObj::set_match_limit(unsigned long match_limit)
{
	if (studied)
	{
		studied->match_limit=match_limit;
		studied->flags |= PCRE_EXTRA_MATCH_LIMIT;
	}
}

void pcreObj::set_match_limit_recursion(unsigned long match_limit_recursion)
{
	if (studied)
	{
		studied->match_limit_recursion=match_limit_recursion;
		studied->flags |= PCRE_EXTRA_MATCH_LIMIT_RECURSION;
	}
}

bool pcreObj::match(const std::string &string, int options)
{
	int cnt;

	if (pcre_fullinfo(compiled, studied, PCRE_INFO_CAPTURECOUNT, &cnt) < 0)
		throw EXCEPTION("pcre_fullinfo failed");

	int buf_size=(cnt+1)*3;
	int captured[buf_size];

	int n=pcre_exec(compiled, studied, string.c_str(),
			string.size(), 0, options, captured, buf_size);

	subpatterns.clear();

	if (n == PCRE_ERROR_NOMATCH || n == 0)
		return false;

	if (n < 0)
		throw EXCEPTION("pcre_exec failed ("
				<< tostring(n)
				<< ")");

	subpatterns.reserve(n);

	for (int i=0; i<n; ++i)
		subpatterns.push_back(string.substr(captured[i*2],
						    captured[i*2+1]
						    -captured[i*2]));
	return true;
}

#if 0
{
#endif
}
