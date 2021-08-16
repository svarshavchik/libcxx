/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_pcreobj_H
#define x_pcreobj_H

#include <pcre.h>
#include <vector>
#include <x/obj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A wrapper for the pcre library.

class pcreObj : virtual public obj {

	//! PCRE library object
	::pcre *compiled;

	//! PCRE library object
	::pcre_extra *studied;

public:

	//! Constructor

	//! Note: the PCRE_STUDY_JIT_COMPILE flag gets always specified.
	//!
	pcreObj(//! Pattern
		const std::string &pattern,

		//! Options for pcre_compile. See pcreapi(3).

		//! Note: PCRE_UTF8 gets automatically turned off, if pcre does
		//! not have UTF-8 support compiled in.
		int options=0);

	//! Destructor
	~pcreObj();

	//! Match a string against the pattern.

	//! Returns true if the pattern was found, and the matching string and
	//! any subpatterns gets placed into
	//! \ref subpatterns "subpatterns".
	//!
	//! An error from the underlying PCRE library can result in a thrown
	//! exception. Specifically, PCRE_ERROR_MATCHLIMIT. The caller
	//! should catch any thrown exceptions, and decide what to do with them.

	bool match(//! String to match
		   const std::string &string,

		   //! Matching option to pcre_exec().
		   int options=0);

	//! Matched subpatterns.

	//! Note that subpatterns[0] is the entire matched string.
	//!
	std::vector<std::string> subpatterns;

	//! Set the match_limit value, and enable the PCRE_EXTRA_MATCH_LIMIT flag.

	void set_match_limit(unsigned long match_limit);

	//! Set the match_limit_recursion value, and enable the PCRE_EXTRA_MATCH_LIMIT_RECURSION flag.
	void set_match_limit_recursion(unsigned long match_limit_recursion);

};

#if 0
{
#endif
}
#endif
