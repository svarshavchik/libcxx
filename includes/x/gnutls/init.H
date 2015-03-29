/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_init_H
#define x_tls_init_H

#include <x/namespace.h>
#include <x/exception.H>
#include <x/option_list.H>
#include <x/property_value.H>

#include <thread>

namespace LIBCXX_NAMESPACE {

	//! Security and encryption

	//! Set <code>INSERT_LIBX_NAMESPACE::gnutls::random</code> property
	//! to "high" to enable high quality random number generation.
	//! The default "low" values uses the quick random number generator.
	//! Without an entropy gathering daemon running, the high quality
	//! random number generator results in excessive initialization delays,
	//! that may last minutes, while sufficient entropy is gathered.
	//!
	//! Set <code>INSERT_LIBX_NAMESPACE::gnutls::loglevel</code> to increase
	//! GnuTLS diagnostic output on standard error
	//! (the gnutls_global_set_log_level() function).
	//!
	//! GnuTLS methods generally throw
	//! \ref errexception "gnutls::errexception" objects when GnuTLS
	//! library calls fail.
	//!
	//! Set <code>INSERT_LIBX_NAMESPACE::gnutls::logerrors</code> to
	//! \c true to also report GnutTLS errors with the
	//! \ref logger "logging facility".

	namespace gnutls {
#if 0
	};
};
#endif

class progress_notifier;

//! Initialize \c %gnutls and \c %gcrypt libraries

//! \internal
//! A single static instance of this class initializes \c %gnutls and \c %gcrypt
//! libraries when this library is loaded.
class init {

	//! Singleton static instance

	static init dummy LIBCXX_INTERNAL;

	//! The \c gnutls::random property

	static property::value<std::string> high_random_prop LIBCXX_INTERNAL;

	//! The \c gnutls::loglevel property

	static property::value<int> loglevel LIBCXX_INTERNAL;

public:
	//! The \c gnutls::logerrors property

	static property::value<bool> logerrors LIBCXX_INTERNAL;
private:
	//! The GnuTLS log callback function

	static void log_func(int loglevel, const char *msg);

	//! Callback progress functions from %gnutls

	//! \internal
	//!
	static void int_progress_handler_cb(void *dummy, const char *method,
					    int printchar,
					    int current, int total) noexcept
		LIBCXX_HIDDEN;

	//! Thread-local function pointer to the current progress indication function

	//! internal
	static __thread progress_notifier *progress_cb_func LIBCXX_INTERNAL;

public:
	~init() noexcept;

	//! Initialize gnutls

	//! \internal
	//!

	static void gnutls_init() noexcept
	{
		std::call_once(once_init, &init_impl);
	}


private:
	//! Initialization flag for gnutls

	static std::once_flag once_init;

	//! gnutls initialized flag

	static int init_flag;

	//! Random seed file initialized flag

	static int randseed_flag LIBCXX_INTERNAL;

	//! Implementation of gnutls initialization

	//! \internal
	//!
	static void init_impl() noexcept;
public:
	//! Load the random number generator seed

	//! This function should be called before invoking any function that
	//! initializes the random number entropy pool.
	static void set_random_seed_file(std::string filename) noexcept;

	//! Save the random number entropy pool

	static void save_random_seed_file() noexcept;

	friend class progress_notifier;

};

//! Progress notifier

//! Create an instance of this object on the stack to receive progress
//! notification of long-running operations. Such as key generation, etc...
//!
//! Only one instance of this object may exist, per thread.

class progress_notifier {

public:
	//! Default constructor
	progress_notifier();

	//! Default destructor
	~progress_notifier();

	//! The subclass must define this function

	//! The default implementation writes printchar to stderr, if
	//! printchar is > 0.
	//!
	//! The parameters are passed through from the gcrypt library.
	//! The current description is as follows:
	//!
	//! - method is "need_enthropy": waiting for more entropy, need "total"
	//! more bytes of entropy.
	//!
	//! - method is "primegen": "printchar" reports the status of the
	//! private key generation.

	virtual void progress(std::string method,
			      int printchar,
			      int current,
			      int total) noexcept;

private:
	//! Flag -- whether the default progress() method generated output

	//! \internal
	//!
	char default_notifier_char;
};


//! Throw an %exception after a GnuTLS function failed

//! \internal
//!
void chkerr_throw(//! Return code from a GnuTLS function
		  int errcode,

		  //! The GnuTLS function's name
		  const char *funcname) LIBCXX_HIDDEN;

//! Throw an %exception if the GnuTLS function failed

//! \internal
//!
inline void chkerr(//! Return code from a GnuTLS function
		   int errcode,

		   //! The GnuTLS function's name
		   const char *funcname)
{
	if (errcode < 0)
		chkerr_throw(errcode, funcname);
}

#if 0
{
	{
#endif
	}
}
#endif
