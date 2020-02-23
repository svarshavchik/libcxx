/*
** Copyright 2015-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ltdl.H"
#include "x/exception.H"
#include <ltdl.h>
#include <iostream>
#include <mutex>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class LIBCXX_HIDDEN initializer {

 public:

	initializer()
	{
		if (lt_dlinit())
		{
			std::cerr << "lt_dlinit failed!" << std::endl;
		}
	}

	~initializer()
	{
		lt_dlexit();
	}
};

static initializer initializer_instance;

static std::mutex dl_mutex;

static void throw_error() __attribute__((noreturn));

static void throw_error()
{
	throw EXCEPTION(lt_dlerror());
}

class LIBCXX_HIDDEN ltdlObj::optionsObj::implObj : virtual public obj {

 public:
	lt_dladvise opts;

	implObj()
	{
		if (lt_dladvise_init(&opts))
			throw_error();
	}

	~implObj()
	{
		lt_dladvise_destroy(&opts);
	}
};

ltdlObj::optionsObj::optionsObj() : impl(ref<implObj>::create())
{
}

ltdlObj::optionsObj::~optionsObj()
{
}

void ltdlObj::optionsObj::set_ext()
{
	std::unique_lock<std::mutex> lock(dl_mutex);

	if (lt_dladvise_ext(&impl->opts))
		throw_error();
}

void ltdlObj::optionsObj::set_global()
{
	std::unique_lock<std::mutex> lock(dl_mutex);

	if (lt_dladvise_global(&impl->opts))
		throw_error();
}

void ltdlObj::optionsObj::set_local()
{
	std::unique_lock<std::mutex> lock(dl_mutex);

	if (lt_dladvise_local(&impl->opts))
		throw_error();
}

void ltdlObj::optionsObj::set_resident()
{
	std::unique_lock<std::mutex> lock(dl_mutex);

	if (lt_dladvise_resident(&impl->opts))
		throw_error();
}

void ltdlObj::optionsObj::set_preload()
{
	std::unique_lock<std::mutex> lock(dl_mutex);

	if (lt_dladvise_preload(&impl->opts))
		throw_error();
}

///////////////////////////////////////////////////////////////////////////////

class LIBCXX_HIDDEN ltdlObj::implObj : virtual public obj {

 public:
	lt_dlhandle handle;

	implObj(const std::string &filename,
		const ltdl::base::options &opts)
		: handle(
			 ({
				 std::unique_lock<std::mutex> lock(dl_mutex);

				 lt_dlopenadvise(filename.c_str(),
						 opts->impl->opts);
			 }))
	{
		if (!handle)
			throw_error();
	}

	~implObj()
	{
		std::unique_lock<std::mutex> lock(dl_mutex);
		lt_dlclose(handle);
	}
};

static inline ltdl::base::options default_options()
{
	auto opts=ltdl::base::options::create();

	opts->set_ext();

	return opts;
}

ltdlObj::ltdlObj(const std::string &filename)
	: ltdlObj(filename, default_options())
{
}

ltdlObj::ltdlObj(const std::string &filename, const ref<optionsObj> &opts)
	: impl(ref<implObj>::create(filename, opts))
{
}

ltdlObj::~ltdlObj()
{
}

bool ltdlObj::isresident() const
{
	std::unique_lock<std::mutex> lock(dl_mutex);

	auto ret=lt_dlisresident(impl->handle);

	if (ret < 0)
		throw_error();

	return ret != 0;
}

void ltdlObj::makeresident()
{
	std::unique_lock<std::mutex> lock(dl_mutex);

	if (lt_dlmakeresident(impl->handle))
		throw_error();
}

void *ltdlObj::get_sym(const std::string &name) const
{
	return lt_dlsym(impl->handle, name.c_str());
}

/////////////////////////////////////////////////////////////////////////////

void ltdlBase::addsearchdir(const std::string &dir)
{
	std::unique_lock<std::mutex> lock(dl_mutex);

	if (lt_dladdsearchdir(dir.c_str()))
	    throw_error();
}

void ltdlBase::insertsearchdir(const std::string &before,
			       const std::string &dir)
{
	std::unique_lock<std::mutex> lock(dl_mutex);

	if (lt_dlinsertsearchdir(before.c_str(), dir.c_str()))
		throw_error();
}

void ltdlBase::setsearchpath(const std::string &dir)
{
	std::unique_lock<std::mutex> lock(dl_mutex);

	if (lt_dlsetsearchpath(dir.c_str()))
		throw_error();
}

std::string ltdlBase::getsearchpath()
{
	std::unique_lock<std::mutex> lock(dl_mutex);

	auto s=lt_dlgetsearchpath();

	return std::string(s ? s:"");
}

static int for_each(const char *filename, void *ptr)
{
	reinterpret_cast<std::set<std::string> *>(ptr)->insert(filename);
	return 0;
}

std::set<std::string> ltdlBase::enumerate()
{
	std::set<std::string> ret;

	std::unique_lock<std::mutex> lock(dl_mutex);

	if (lt_dlforeachfile(NULL, &for_each,
			     reinterpret_cast<void *>(&ret)))
		throw_error();
	return ret;
}

std::set<std::string> ltdlBase::enumerate(const std::string &search_path)
{
	std::set<std::string> ret;

	std::unique_lock<std::mutex> lock(dl_mutex);

	if (lt_dlforeachfile(search_path.c_str(), &for_each,
			     reinterpret_cast<void *>(&ret)))
		throw_error();
	return ret;
}

#if 0
{
#endif
}
