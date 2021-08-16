/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_credentials_callbackobj_H
#define x_tls_credentials_callbackobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/sysexception.H>

#include <x/gnutls/init.H>
#include <x/gnutls/x509_privkey.H>
#include <x/gnutls/x509_crt.H>

#include <gnutls/gnutls.h>
#include <x/gnutls/sessionfwd.H>

#include <list>
#include <vector>
#include <string>

namespace LIBCXX_NAMESPACE::gnutls::credentials {
	class callbackObj;
	class certificateObj;
}

//! A base class for the %credentials callback object

//! \see gnutls::credentials::callback
//!

class LIBCXX_NAMESPACE::gnutls::credentials::callbackObj : virtual public obj {

public:
	friend class LIBCXX_NAMESPACE::gnutls::credentials::certificateObj;
	friend class LIBCXX_NAMESPACE::gnutls::sessionObj;

	//! Default constructor
	callbackObj() noexcept;

	//! Default destructor
	~callbackObj();

	//! get() returns a nullable pointer reference to this object

	class keycertObj : virtual public obj {

	public:
		//! Certificate's private list
		x509::privkey key;

		//! The certificate, and any intermediate certs
		std::list<x509::crt> cert;

		//! Constructor

		//! The constructor takes the key up front, then the caller
		//! can load the cert list, manually.

		keycertObj(const x509::privkey &keyArg=
			   x509::privkey::create());

		//! Destructor

		~keycertObj();
	};

	//! A reference to a key and a cert chain returned by get().

	typedef ref<keycertObj> keycert;

	//! A nullable pointer to a key and a cert chain, returned by get().

	typedef ptr<keycertObj> keycertptr;

	//! Retrieve credentials

	//! Must be implemented by the subclass. The subclass returns a
	//! keycertptr. The return value may be null() for a client connection,
	//! but a server connection is required to return a non-null() value.

	virtual keycertptr
	get(//! Session being authenticated

	    const gnutls::sessionObj *session,

	    //! Server callback: optional list of hostnames

	    //! The server must provide a certificate for any
	    //! given hostname. An empty list indicates that
	    //! the client did not ask for any specific hostname.
	    const std::list<std::string> &hostname_list,

	    //! Client callback: Allowed algorithms
	    const std::vector<gnutls_pk_algorithm_t> &algos,

	    //! Client callback: client certificate requested by the server
	    const gnutls_datum_t *req_ca_dn,

	    //! Client callback: client certificate requested by the server
	    size_t n_req_ca_dn)=0;

private:

	//! Callback handler
	static int callback(gnutls_session_t sess,
			    const gnutls_datum_t* req_ca_dn,
			    int nreqs,
			    const gnutls_pk_algorithm_t* pk_algos,
			    int pk_algos_length, gnutls_retr2_st* st)
		LIBCXX_INTERNAL;

	//! Assemble certificates

	//! \internal
	//!
	static gnutls_x509_crt_t *init_certs(std::list<x509::crt> &)
		LIBCXX_INTERNAL;

	//! Captured exception, to be rethrown

	exception saved_exception;

	//! Whether an exception was captured in the callback from gnutls

	bool captured_exception;
};
#endif
