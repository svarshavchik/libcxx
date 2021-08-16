/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_sessioncacheobj_H
#define x_tls_sessioncacheobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/exception.H>
#include <x/gnutls/datumwrapper.H>
#include <x/gnutls/init.H>
#include <x/logger.H>

namespace LIBCXX_NAMESPACE::gnutls {
	class sessioncacheObj;
#if 0
};
#endif

//! An object that implement TLS/SSL session caching

class sessioncacheObj : virtual public obj {

	//! Logger

	LOG_CLASS_SCOPE;
public:
	friend class sessionObj;

	//! Constructor
	sessioncacheObj();

	//! Destructor
	~sessioncacheObj();

private:
	//! Session ticket
	datum_t ticket;

	//! Store a session

	//! store() must be thread-safe in the implementing subclass.
	virtual void store(const gnutls::datum_t &key,
			   const gnutls::datum_t &data)=0;

	//! Remove a session

	//! remove() must be thread-safe in the implementing subclass.
	virtual void remove(const gnutls::datum_t &key)=0;

	//! Retrieve a session

	//! retr() must be thread-safe in the implementing subclass.
	virtual gnutls::datumptr_t retr(const gnutls::datum_t &key)=0;

public:

	class LIBCXX_HIDDEN basic_implObj;

	//! Callback installed by gnutls_db_set_store_function

	//! \internal
	//!
	//! ptr is a sessioncacheObj, invokes ptr->store()

	static int store_func(void *ptr,
			      gnutls_datum_t key, gnutls_datum_t data)
		LIBCXX_HIDDEN;

	//! Callback installed by gnutls_db_set_remove_function

	//! \internal
	//!
	//! ptr is a sessioncacheObj, invokes ptr->store()
	static int remove_func(void *ptr, gnutls_datum_t key)
		LIBCXX_HIDDEN;

	//! Callback installed by gnutls_db_set_retrieve_function

	//! \internal
	//!
	//! ptr is a sessioncacheObj, invokes ptr->store()

	static gnutls_datum_t retr_func(void *ptr, gnutls_datum_t key)
		LIBCXX_HIDDEN;
};

#if 0
{
#endif
}
#endif
