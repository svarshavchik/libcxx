/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/init.H"
#include "x/gnutls/exceptions.H"
#include "x/gcrypt/errors.H"
#include "x/property_value.H"
#include "x/logger.H"

#include <gcrypt.h>
#include <gnutls/gnutls.h>
#include <pthread.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <iostream>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

gnutls::init gnutls::init::dummy;

std::once_flag gnutls::init::once_init;

int gnutls::init::init_flag=0, gnutls::init::randseed_flag=0;

property::value<std::string> gnutls::init::high_random_prop(LIBCXX_NAMESPACE_STR
							    "::gnutls::random",
							    "low");
property::value<int> gnutls::init::loglevel(LIBCXX_NAMESPACE_STR
					     "::gnutls::loglevel", 0);

property::value<bool> gnutls::init::logerrors(LIBCXX_NAMESPACE_STR
					      "::gnutls::logerrors", false);

void gnutls::init::init_impl() noexcept
{
	__sync_fetch_and_add(&init_flag, 1);

	if (!gnutls_check_version(LIBGNUTLS_VERSION))
	{
		std::cout << "Failed to initialize GnuTLS" << std::endl;
		abort();
	}
	gnutls_global_init();
	gcry_set_progress_handler(int_progress_handler_cb, 0);

	if (high_random_prop.getValue() != "high")
		gcry_control(GCRYCTL_ENABLE_QUICK_RANDOM);

	int loglevelValue=loglevel.getValue();

	if (loglevelValue > 0)
	{
		gnutls_global_set_log_function(&log_func);
		gnutls_global_set_log_level(loglevelValue);
	}
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::gnutls, gnutls_logger);

void gnutls::init::log_func(int loglevel, const char *msg)
{
	LOG_FUNC_SCOPE(gnutls_logger);

	logger(msg, LOGLEVEL_INFO);
}

__thread gnutls::progress_notifier *gnutls::init::progress_cb_func;

void gnutls::init::int_progress_handler_cb(void *dummy, const char *method,
					   int printchar,
					   int current, int total) noexcept
{
	if (progress_cb_func)
		progress_cb_func->progress(method, printchar, current, total);
}

gnutls::progress_notifier::progress_notifier()
	: default_notifier_char('\n')
{
	gnutls::init::gnutls_init();

	if (gnutls::init::progress_cb_func)
		throw EXCEPTION("Multiple progress_notifier objects defined in the same thread");

	gnutls::init::progress_cb_func=this;
}

gnutls::progress_notifier::~progress_notifier()
{
	gnutls::init::progress_cb_func=0;
	if (default_notifier_char != '\n')
		if (write(2, "\n", 1))
			;
}

void gnutls::progress_notifier::progress(std::string method,
					 int printchar,
					 int current,
					 int total) noexcept
{
	char c=default_notifier_char=printchar;

	if (printchar > 0)
		if (write(2, &c, 1))
			;
}

gnutls::init::~init() noexcept
{
	if (__sync_fetch_and_add(&init_flag, 0))
		gnutls_global_deinit();
}

void gnutls::chkerr_throw(int errcode,
			  const char *funcname)
{
	LOG_FUNC_SCOPE(gnutls_logger);

	if (init::logerrors.getValue())
		LOG_ERROR(funcname << ": " << gnutls_strerror(errcode));
	throw errexception(errcode, funcname);
}

void gcrypt::chkerr_throw(gcry_error_t code)
{
	throw EXCEPTION(std::string(gcry_strsource(code)) + ": "
			+ gcry_strerror(code));
}

void gnutls::init::set_random_seed_file(std::string filename) noexcept
{
	if (__sync_fetch_and_add(&randseed_flag, 1) == 0)
	{
		// Call it only once.

		gcry_control(GCRYCTL_SET_RANDOM_SEED_FILE, filename.c_str());
	}
	else
		__sync_fetch_and_add(&randseed_flag, -1);
}

void gnutls::init::save_random_seed_file() noexcept
{
	gcry_control(GCRYCTL_UPDATE_RANDOM_SEED_FILE);
}

#if 0
{
#endif
};
