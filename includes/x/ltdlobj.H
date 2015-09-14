/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ltdlobj_H
#define x_ltdlobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A loaded shared library

//! \see ltdl

class ltdlObj : virtual public obj {

public:

	//! Options for loading shared libraries

	//! \see ltdlbase::options

	class optionsObj : virtual public obj {

	public:
		class implObj;

		//! Internal implementation
		const ref<implObj> impl;

		//! Constructor
		optionsObj();

		//! Destructor
		~optionsObj() noexcept;

		//! Use system standard extensions
		void set_ext();

		//! Export symbols in the loaded library
		void set_global();

		//! Do not export symbols in the loaded library
		void set_local();

		//! Make this library resident
		void set_resident();

		//! Only open a preloaded module (if none available, throw an exception)
		void set_preload();
	};

	class implObj;

	//! Internal implementation
	const ref<implObj> impl;

	//! Constructor
	ltdlObj(const std::string &filename);

	//! Constructor
	ltdlObj(const std::string &filename, const ref<optionsObj> &opts);

	//! Destructor
	~ltdlObj();

	//! Indicate whether this library is permanently loaded.
	bool isresident() const;

	//! Make this loaded library permanently loaded.
	void makeresident();

	//! Search for a symbol in the loaded library
	template<typename typeObj>
	inline auto sym(const std::string &name) const
	{
		return reinterpret_cast<typeObj>(get_sym(name));
	}

	//! Retrieve a symbol from a shared library.
	void *get_sym(const std::string &name) const;
};

#if 0
{
#endif
}
#endif
