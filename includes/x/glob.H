/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_glob_H
#define x_glob_H

#include <x/exception.H>
#include <x/ref.H>
#include <x/namespace.h>
#include <set>
#include <string>
#include <vector>
#include <glob.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class globObj;

//! Filename pattern expansion

//! \code
//! std::set<std::string> filenames;
//!
//! glob::create()->expand("/etc/rc?.d/*")->get(filenames);
//! \endcode
//!
//! This reference-counted object implements \c glob(3) filename expansion.
//! Invoking expand() expands the given filename pattern. Multiple invocation
//! of expand() accumulates all expanded filenames in an internal buffer.
//! get() retrieves all expanded filenames

typedef ref<globObj> glob;

//! A pointer to a filename pattern expansion object

//! \see glob

typedef ptr<globObj> globptr;

//! A wrapper for \c glob(3)

class globObj : virtual public obj {

	//! The library glob structure

	glob_t gl;

	//! Flag: gl has been allocated

	bool glflag;

public:
	//! Default constructor
	globObj();

	//! Default destructor
	~globObj();

	//! Recomment flags for expand()

	static constexpr int default_flags=
		GLOB_ERR
#ifdef GLOB_BRACE
		| GLOB_BRACE
#endif
#ifdef GLOB_TILDE_CHECK
		| GLOB_TILDE_CHECK
#endif
		;

	//! Perform filename expansion

	//! \return a reference to this object, which can be used to chain
	//! consecutive calls. An error results in a thrown %exception.

	ref<globObj> expand(//! The pattern to match
			    const std::string &pattern,


			    //! %glob(3) flags.

			    //! Possible flags are: \c GLOB_ERR,
			    //! \c GLOB_MARK, \c GLOB_NOSORT, \c GLOB_NOCHECK,
			    //! \c GLOB_NOESCAPE, \c GLOB_PERIOD,
			    //! \c GLOB_BRACE, \c GLOB_NOMAGIC,
			    //! \c GLOB_TILDE, \c GLOB_TILDE_CHECK,
			    //! or \c GLOB_ONLYDIR. Do not use other flags
			    //! documented in the %glob(3) man page.

			    int flags=default_flags
			    );

	//! Retrieve the expanded filename list

	//! get() takes an output iterator, and feeds it the list of expanded
	//! names, so far.

	template<typename OutputIterator>
	OutputIterator get(OutputIterator iter)
	{
		if (glflag)
			for (size_t i=0; i<gl.gl_pathc; ++i)
				if (gl.gl_pathv[i])
					*iter++ = gl.gl_pathv[i];
		return iter;
	}

	//! Retrieve the expanded filename list

	//! Specialization for std::vector&lt;std::string&gt;.
	//! This specialization appends the matched filenames into a vector.

	void get(std::vector<std::string> &);

	//! Retrieve the expanded filename list

	//! Specialization for std::set&lt;std::string&gt;.
	//! This specialization appends the matched filenames into a set.

	void get(std::set<std::string> &);
};


#if 0
{
#endif
}
#endif
