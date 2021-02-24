/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/obj.H"
#include "x/basicstreamobj.H"
#include "x/fdstreambufobj.H"
#include "x/messages.H"
#include "x/gnutls/exceptions.H"
#include "x/gnutls/session.H"
#include "x/gnutls/sessionobj.H"
#include "x/gnutls/credentials.H"
#include "x/gnutls/init.H"
#include "x/gnutls/sessioncache.H"
#include "x/logger.H"
#include "x/exception.H"
#include "x/timerfd.H"
#include "x/property_value.H"
#include "x/hms.H"
#include "x/strtok.H"
#include "gettext_in.h"

#include <sys/poll.h>

#include <cstring>

#define AGAIN(e) \
	((e) == GNUTLS_E_INTERRUPTED || (e) == GNUTLS_E_AGAIN)

// GnuTLS can now return GNUTLS_E_AGAIN for its own reasons, not realted
// to non-blocking I/O. Detect this situation and try again ourselves,
// because our caller may not be prepared to deal with this.

#define AGAIN_SHOULD_BE_HANDLED_BY_GNUTLS			 \
	(transport_errno != EAGAIN && \
	 transport_errno != EWOULDBLOCK)

// Once an errno is encounted on a socket, we remember it and fail all
// subsequent requests.
#define REAL_ERROR_IN_TRANSPORT				 \
	(transport_errno && transport_errno != EAGAIN && \
	 transport_errno != EWOULDBLOCK)

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

#define LOCK_SESSION \
	std::unique_lock<std::recursive_mutex> session_mutex_lock{session_mutex}

static property::value<hms> session_cache_expiration
( LIBCXX_NAMESPACE_STR "::gnutls::session_cache::expiration",
  hms(1,0,0));

static property::value<bool> ignore_premature_termination
( LIBCXX_NAMESPACE_STR "::gnutls::ignore_premature_termination_error", false);

gnutls::sessionObj::factoryObj::factoryObj()
{
}

gnutls::sessionObj::factoryObj::factoryObj(const gnutls::credentials::certificate &cert)
	: cred(cert)
{
}

gnutls::sessionObj::factoryObj::~factoryObj()
{
}

gnutls::session gnutls::sessionObj::factoryObj
::create(unsigned mode,
	 const fdbase &transportArg)
{
	gnutls::session sess(session::create(mode, transportArg));

	sess->set_default_priority();
	sess->set_private_extensions();

	auto cred_copy=({
			std::unique_lock<std::mutex> lock(mutex);

			cred;
		});
	sess->credentials_set(cred_copy);

	return sess;
}

gnutls::sessionObj::sessionObj(unsigned modeArg,
			       const fdbase &transportArg)
	: mode(modeArg),
	  transport(transportArg)
{
	gnutls::init::gnutls_init();

	chkerr(gnutls_init(&sess, mode), "gnutls_init");
	gnutls_session_set_ptr(sess, this);
	gnutls_transport_set_ptr2(sess,
				  reinterpret_cast<gnutls_transport_ptr_t>
				  (this),
				  reinterpret_cast<gnutls_transport_ptr_t>
				  (this));

	gnutls_transport_set_pull_function(sess, (gnutls_pull_func)
					   &gnutls::sessionObj::pull_func);
	gnutls_transport_set_pull_timeout_function
		(sess,
		 (gnutls_pull_timeout_func)
		 &gnutls::sessionObj::pull_timeout_func);
	gnutls_transport_set_push_function(sess, (gnutls_push_func)
					   &gnutls::sessionObj::push_func);
}

gnutls::session gnutls::sessionBase::client(const fd &conn,
					    const std::string &server)

{
	gnutls::credentials::certificate
		cred(gnutls::credentials::certificate::create());

	cred->set_x509_trust_default();

	gnutls::session s(gnutls::session::create(GNUTLS_CLIENT, conn));

	s->credentials_set(cred);
	s->set_default_priority();

	int dummy;

	s->handshake(dummy);
	s->verify_peer(server);

	return s;
}

extern "C" {
#if 0
}
#endif

ssize_t gnutls::sessionObj::pull_func(gnutls_transport_ptr_t ptr,
				      void *buf,
				      size_t buf_s) noexcept
{
	sessionObj *me=reinterpret_cast<sessionObj *>(ptr);

	return me->pull_func(buf, buf_s);
}

#if 0
{
#endif
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::gnutls::sessionObj, debugLog);

ssize_t gnutls::sessionObj::pull_func(void *buf,
				      size_t buf_s) noexcept
{
	LOG_FUNC_SCOPE(debugLog);

	// If an error already occured here, reset errno, and do nothing.

	{
		LOCK_SESSION;

		if (REAL_ERROR_IN_TRANSPORT)
		{
			errno=transport_errno;
			LOG_TRACE(this << ": pull: preserving existing error "
				  << strerror(errno));
			return (ssize_t)-1;
		}
	}

	size_t n;

	errno=0;
	try {
		n=transport->pubread( (char *)buf, buf_s);
	} catch (const sysexception &e)
	{
		errno=e.getErrorCode();
		n=0;
	} catch (...)
	{
		errno=EIO;
		n=0;
	}

	LOG_TRACE(this << ": pull: pubread() returned " << n
		  << ", errno=" << (errno ? strerror(errno):"0"));

	{
		LOCK_SESSION;

		transport_errno=errno;
	}

	if (n == 0)
	{
		if (errno == 0)
			return 0; // End of file

		return -1; // EOF
	}

	return n;
}

extern "C" {
#if 0
}
#endif
int gnutls::sessionObj::pull_timeout_func(gnutls_transport_ptr_t ptr,
					  unsigned int ms) noexcept
{
	sessionObj *me=reinterpret_cast<sessionObj *>(ptr);

	return me->pull_timeout_func(ms);
}

#if 0
{
#endif
}

int gnutls::sessionObj::pull_timeout_func(unsigned int ms) noexcept
{
	LOG_FUNC_SCOPE(debugLog);

	// If an error already occured here, reset errno, and do nothing.

	{
		LOCK_SESSION;

		if (REAL_ERROR_IN_TRANSPORT)
		{
			errno=transport_errno;
			LOG_TRACE(this << ": pull_timeout:"
				  " preserving existing error "
				  << strerror(errno));
			return -1;
		}
	}

	int n;

	try {
		n=transport->pubpoll(ms == GNUTLS_INDEFINITE_TIMEOUT
				     ? -1
				     : (int)ms);
	} catch (const sysexception &e)
	{
		errno=e.getErrorCode();
		n=-1;
	} catch (...)
	{
		errno=EIO;
		n=-1;
	}

	{
		LOCK_SESSION;

		transport_errno=errno;

		if (n >= 0)
			transport_errno=0;
	}

	LOG_TRACE(this << ": pull: pubpoll() returned " << n
		  << ", errno=" << (n ? strerror(transport_errno):"0"));

	return n;
}

extern "C" {
#if 0
}
#endif
ssize_t gnutls::sessionObj::push_func(gnutls_transport_ptr_t ptr,
				      const void *buf,
				      size_t buf_s) noexcept
{
	sessionObj *me=reinterpret_cast<sessionObj *>(ptr);

	return me->push_func(buf, buf_s);
}

#if 0
{
#endif
}

ssize_t gnutls::sessionObj::push_func(const void *buf,
				      size_t buf_s) noexcept
{
	LOG_FUNC_SCOPE(debugLog);

	size_t n;

	// If an error already occured here, reset errno, and do nothing.

	{
		LOCK_SESSION;

		if (REAL_ERROR_IN_TRANSPORT)
		{
			errno=transport_errno;
			LOG_TRACE(this << ": push: preserving existing error "
				  << strerror(errno));
			return (ssize_t)-1;
		}
	}

	errno=0;
	try {
		n=transport->pubwrite( (const char *)buf, buf_s);
	} catch (const sysexception &e)
	{
		errno=e.getErrorCode();
		n=0;
	} catch (...)
	{
		errno=EIO;
		n=0;
	}

	{
		LOCK_SESSION;

		if (n == 0 && !errno)
			errno=ECONNRESET; // Something must've happened

		transport_errno=errno;
	}

	LOG_TRACE(this << ": push: pubwrite() returned " << n
		  << ", errno=" << (errno ? strerror(errno):"0"));

	if (n == 0)
	{
		return (ssize_t)-1;
	}

	return n;
}

gnutls::sessionObj::~sessionObj()
{
	if (session_remove_needed && !cache.null())
		gnutls_db_remove_session(sess);
	gnutls_deinit(sess);
}

void gnutls::sessionObj::credentials_set(const credentials::certificate &cert)

{
	LOCK_SESSION;

	chkerr(gnutls_credentials_set(sess, GNUTLS_CRD_CERTIFICATE,
				      cert->cred),
	       "gnutls_credentials_set");

	certificateCred=cert;
}

void gnutls::sessionObj::credentials_clear()
{
	LOCK_SESSION;

	certificateCred=credentials::certificateptr();
	gnutls_credentials_clear(sess);
}

void gnutls::sessionObj::set_private_extensions(bool flag)
{
	LOCK_SESSION;

	gnutls_handshake_set_private_extensions(sess, flag ? 1:0);
}

void gnutls::sessionObj::set_server_name(const std::string &hostname)

{
	LOCK_SESSION;

	chkerr(gnutls_server_name_set(sess, GNUTLS_NAME_DNS, hostname.c_str(),
				      hostname.size()),
	       "gnutls_server_name_set");
}

void gnutls::sessionObj::get_server_names(gnutls_session_t sess,
					  std::list<std::string> &hostname_list)

{
	unsigned int idx=0;

	size_t name_len;
	unsigned int type;

	while (gnutls_server_name_get(sess, NULL, &(name_len=0), &type, idx)
	       != GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
	{
		char buf[++name_len];

		chkerr(gnutls_server_name_get(sess, buf, &name_len, &type, idx),
		       "gnutls_server_name_get");

		hostname_list.push_back(std::string(buf, buf+name_len));
		++idx;
	}
}

void gnutls::sessionObj::set_alpn(const std::string_view &protocols,
				  gnutls_alpn_flags_t flags)
{
	std::vector<std::string_view> protocols_vec;

	strtok_str(protocols, ", \t\r\n", protocols_vec);

	std::vector<gnutls_datum_t> datums;

	datums.reserve(protocols_vec.size());

	for (auto &p:protocols_vec)
		datums.push_back(gnutls_datum_t{reinterpret_cast<unsigned
								 char *>
						(const_cast<char *>(p.data())),
						(unsigned int)p.size()});

	const gnutls_datum_t *datums_p=nullptr;

	if (!datums.empty())
		datums_p=&datums[0];

	LOCK_SESSION;

	chkerr(gnutls_alpn_set_protocols(sess, datums_p, datums.size(),
					 flags),
	       "gnutls_alpn_set_protocols");
}

std::optional<std::string> gnutls::sessionObj::get_alpn() const
{
	std::optional<std::string> ret;

	LOCK_SESSION;

	gnutls_datum_t protocol;

	auto e=gnutls_alpn_get_selected_protocol(sess, &protocol);

	if (e == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
		return ret;

	chkerr(e, "gnutls_alpn_get_selected_protocol");

	if (!protocol.size)
		ret.emplace();
	else
	{
		ret.emplace(protocol.data, protocol.data+protocol.size);
	}
	return ret;
}

void gnutls::sessionObj::set_session_data(const datum_t &session_dataArg)
{
	LOCK_SESSION;

	chkerr(gnutls_session_set_data(sess,
				       &*session_dataArg->begin(),
				       session_dataArg->size()),
	       "gnutls_session_set_data");

	session_data=session_dataArg;
}

void gnutls::sessionObj::session_cache(const sessioncache &cacheArg)
{
	session_cache(cacheArg, session_cache_expiration.get().seconds());
}

void gnutls::sessionObj::session_cache(const sessioncache &cacheArg,
				       time_t expiration)
{
	LOCK_SESSION;

	gnutls_db_set_cache_expiration(sess, expiration);

	tempdatum temp_datum(cacheArg->ticket);

	chkerr(gnutls_session_ticket_enable_server(sess, &temp_datum.datum),
	       "gnutls_session_ticket_enable_server");

	cache=cacheArg;

	auto voidp=reinterpret_cast<void *>(&*cache);
	gnutls_db_set_ptr(sess, voidp);
	gnutls_db_set_remove_function(sess, &sessioncacheObj::remove_func);
	gnutls_db_set_retrieve_function(sess, &sessioncacheObj::retr_func);
	gnutls_db_set_store_function(sess, &sessioncacheObj::store_func);
}

bool gnutls::sessionObj::handshake(int &direction)
{
	LOG_FUNC_SCOPE(debugLog);

	LOCK_SESSION;

	if (!handshake_needed)
		return true; // Already done.

 again:
	direction=0;
	errno=0;

	int err=gnutls_handshake(sess);

	LOG_TRACE(this << ": gnutls_handshake returned "
		  << gnutls_strerror_name(err));

	if (!certificateCred.null())
	{
		credentials::callbackptr
			cb(certificateCred->certificateCallback);

		if (!cb.null() && cb->captured_exception)
			cb->saved_exception->rethrow();
	}

	if (err != GNUTLS_E_SUCCESS)
	{
		// Propagate ETIMEDOUT, because gnutls complains about packet length instead

		LOG_TRACE(this << ": errno is "
			  << strerror(transport_errno));

		if (transport_errno == ETIMEDOUT)
			throw SYSEXCEPTION("gnutls_handshake");

		if (AGAIN(err))
		{
			if (AGAIN_SHOULD_BE_HANDLED_BY_GNUTLS)
				goto again; // GNUTLS_E_AGAIN from GnuTLS itself

			direction=compute_direction();
			return (false);
		}

		if (err == GNUTLS_E_WARNING_ALERT_RECEIVED)
			throw alertexception(false, gnutls_alert_get(sess));

		if (err == GNUTLS_E_FATAL_ALERT_RECEIVED)
			throw alertexception(true, gnutls_alert_get(sess));

		chkerr(err, "gnutls_handshake");
	}
	LOG_TRACE(this << ": handshake complete");

	handshake_needed=false;
	session_remove_needed=true; // Removed after a bye()
	return true;
}

bool gnutls::sessionObj::rehandshake(int &direction)
{
	LOG_FUNC_SCOPE(debugLog);
	LOCK_SESSION;

 again:
	direction=0;

	int err=gnutls_rehandshake(sess);

	LOG_TRACE(this << ": gnutls_rehandshake returned "
		  << gnutls_strerror_name(err));

	if (err != GNUTLS_E_SUCCESS)
	{
		LOG_TRACE(this << ": errno is "
			  << strerror(transport_errno));

		if (AGAIN(err))
		{
			if (AGAIN_SHOULD_BE_HANDLED_BY_GNUTLS)
				goto again; // GNUTLS_E_AGAIN from GnuTLS itself

			direction=compute_direction();
			return false;
		}

		// Propagate ETIMEDOUT, because gnutls complains about packet length instead

		if (transport_errno == ETIMEDOUT)
			throw SYSEXCEPTION("gnutls_rehandshake");
	}
	chkerr(err, "gnutls_rehandshake");
	return true;
}

void gnutls::sessionObj::verify_peer_internal(const std::string *hostname)

{
	LOCK_SESSION;

	unsigned int flags=0;

	chkerr(gnutls_certificate_verify_peers2(sess, &flags),
	       "gnutls_certificate_verify_peers2");

	if (flags)
	{

		throw EXCEPTION
			((hostname ?
			  *hostname:std::string("Peer certificate"))
			 << ": " <<
			 libmsg(flags & GNUTLS_CERT_REVOKED ?
				_txt("Revoked certificate"):
				flags & GNUTLS_CERT_SIGNER_NOT_FOUND ?
				_txt("Unknown certificate authority"):
				flags & GNUTLS_CERT_SIGNER_NOT_CA ?
				_txt("Certificate signer is not an authority"):
				flags & GNUTLS_CERT_INSECURE_ALGORITHM ?
				_txt("Certificate signed by a weak hash"):
				flags & GNUTLS_CERT_NOT_ACTIVATED ?
				_txt("Certificate not yet active"):
				flags & GNUTLS_CERT_EXPIRED ?
				_txt("Certificate expired"):
				flags & GNUTLS_CERT_INVALID ?
				_txt("Invalid certificate"):
				_txt("Certificate verification failed")));
	}

	if (certificate_type_get() == GNUTLS_CRT_X509)
		verify_peers_x509(hostname);
}

void gnutls::sessionObj::verify_peers_x509(const std::string *hostname)

{
	LOCK_SESSION;

	std::list<x509::crt> peercerts;

	get_peer_certificates(peercerts);

	if (peercerts.empty())
		chkerr(GNUTLS_E_X509_CERTIFICATE_ERROR,
		       _("Peer did not provide a certificate"));

	x509::crt &c=peercerts.front();

	if (hostname && !c->check_hostname(*hostname))
		chkerr(GNUTLS_E_X509_CERTIFICATE_ERROR,
		       ((std::string)
			gettextmsg(_("Peer certificate is not %1%"),
				   *hostname)).c_str());
}

const_fdbase gnutls::sessionObj::get_transport()
{
	LOCK_SESSION;

	return transport;
}

void gnutls::sessionObj::set_transport(const fdbase &transportArg)
{
	LOCK_SESSION;

	transport=transportArg;
}

gnutls_cipher_algorithm_t gnutls::sessionObj::get_cipher() const
{
	LOCK_SESSION;

	return gnutls_cipher_get(sess);
}

gnutls_kx_algorithm_t gnutls::sessionObj::get_kx() const
{
	LOCK_SESSION;

	return gnutls_kx_get(sess);
}

gnutls_mac_algorithm_t gnutls::sessionObj::get_mac() const
{
	LOCK_SESSION;

	return gnutls_mac_get(sess);
}

std::string gnutls::sessionObj::get_suite() const
{
	LOCK_SESSION;

	return gnutls::session::base::get_cipher_name(get_cipher()) + "/"
		+ gnutls::session::base::get_kx_name(get_kx()) + "/"
		+ gnutls::session::base::get_mac_name(get_mac());
}

bool gnutls::sessionObj::bye(int &direction,
			     gnutls_close_request_t how)
{
	LOG_FUNC_SCOPE(debugLog);

	LOCK_SESSION;

	if (REAL_ERROR_IN_TRANSPORT)
	{
		errno=transport_errno;
		LOG_TRACE(this << ": bye: preserving existing error "
			  << strerror(errno));
		throw SYSEXCEPTION(transport_errno);
	}

 again:
	direction=0;

	int err=gnutls_bye(sess, how);

	LOG_TRACE(this << ": bye: gnutls_bye() returned "
		  << gnutls_strerror_name(err)
		  << ", errno="
		  << (transport_errno ? strerror(transport_errno):"0"));
	if (AGAIN(err))
	{
		if (AGAIN_SHOULD_BE_HANDLED_BY_GNUTLS)
			goto again; // GNUTLS_E_AGAIN from GnuTLS itself
		direction=compute_direction();
		return false;
	}

	errno=transport_errno;
	// Propagate ETIMEDOUT, because gnutls complains about packet length instead

	if (err && errno == ETIMEDOUT)
		throw SYSEXCEPTION("gnutls_bye");

	chkerr(err, "gnutls_bye");
	session_remove_needed=false;
	return true;
}

size_t gnutls::sessionObj::read_pending() const
{
	LOCK_SESSION;

	return gnutls_record_check_pending(sess);
}

void gnutls::sessionObj::set_max_size(size_t recsize)
{
	LOCK_SESSION;

	chkerr(gnutls_record_set_max_size(sess, recsize),
	       "gnutls_record_set_max_size");
}

size_t gnutls::sessionObj::get_max_size() const noexcept
{
	LOCK_SESSION;

	return gnutls_record_get_max_size(sess);
}

size_t gnutls::sessionObj::send(const void *data, size_t cnt, int &direction)

{
	LOG_FUNC_SCOPE(debugLog);

	{
		LOCK_SESSION;

		if (REAL_ERROR_IN_TRANSPORT)
		{
			// An error already occured

			errno=transport_errno;
			direction=0;
			LOG_TRACE(this << ": send: preserving existing error "
				  << strerror(errno));
			return 0;
		}
	}

 again:

	if (!handshake(direction))
	{
		LOG_TRACE(this << ": send: incomplete handshake");
		return 0;
	}
	direction=0;
	errno=0;

	ssize_t n=gnutls_record_send(sess, data, cnt);

	if (n >= 0)
	{

		LOG_TRACE(this << ": push: gnutls_record_send() returned "
			  << n);
		return n;
	}

	LOG_TRACE(this << ": push: gnutls_record_send() returned "
		  << gnutls_strerror_name(n)
		  << ", errno="
		  << (transport_errno ? strerror(transport_errno):"0"));

	if (AGAIN(n))
	{
		LOCK_SESSION;

		if (AGAIN_SHOULD_BE_HANDLED_BY_GNUTLS)
			goto again; // GNUTLS_E_AGAIN from GnuTLS itself

		direction=POLLOUT;
		// Always for gnutls_record_send().

		// gnutls_record_get_direction() reads a global that's set by
		// both gnutls_record_send() and gnutls_record_recv(). Although
		// gnutls_record_send() always set it to 1, if another thread
		// invoked gnutls_record_recv() and we call
		// gnutls_record_get_direction() here, we'll get wrong
		// results.
		return 0;
	}

	LOCK_SESSION;

	errno=transport_errno;

	// Ignore GnuTLS's gripes, if the transport broke because of our own
	// doing (read/write limits, time outs, etc...)

	switch (errno) {
	case ETIMEDOUT:
	case EPIPE:
		direction=0;
		return 0;
	}

	if (n == GNUTLS_E_PREMATURE_TERMINATION &&
	    ignore_premature_termination.get())
	{
		errno=0;
		direction=0;
		return 0;
	}

	chkerr(n, "gnutls_record_send");

	// Shouldn't get here

	LOG_ERROR("gnutls_record_send: default error handler invoked");
	errno=EIO;
	throw EXCEPTION("gnutls_record_send");
}

size_t gnutls::sessionObj::recv(void *data, size_t cnt, int &direction)

{
	LOG_FUNC_SCOPE(debugLog);

	{
		LOCK_SESSION;

		if (REAL_ERROR_IN_TRANSPORT)
		{
			// An error already occured

			errno=transport_errno;
			direction=0;
			LOG_TRACE(this << ": recv: preserving existing error "
				  << strerror(errno));
			return 0;
		}

	}

 again:
	if (!handshake(direction))
	{
		LOG_TRACE(this << ": recv: incomplete handshake");
		return 0;
	}

	ssize_t n=gnutls_record_recv(sess, data, cnt);

	LOCK_SESSION;

	if (n >= 0)
	{
		LOG_TRACE(this << ": recv: gnutls_record_recv() returned "
			  << n);
		direction=0;
		return n;
	}
	LOG_TRACE(this << ": recv: gnutls_record_recv() returned "
		  << gnutls_strerror_name(n)
		  << ", errno="
		  << (transport_errno ? strerror(transport_errno):"0"));

	if (n == GNUTLS_E_REHANDSHAKE)
	{
		handshake_needed=true;
		goto again;
	}

	if (AGAIN(n))
	{
		if (AGAIN_SHOULD_BE_HANDLED_BY_GNUTLS)
			goto again; // GNUTLS_E_AGAIN from GnuTLS itself

		direction=POLLIN;
		// Always for gnutls_record_recv()

		// gnutls_record_get_direction() reads a global that's set by
		// both gnutls_record_send() and gnutls_record_recv(). Although
		// gnutls_record_recv() always set it to 0, if another thread
		// invoked gnutls_record_send() and we call
		// gnutls_record_get_direction() here, we'll get wrong
		// results.

		return 0;
	}

	errno=transport_errno;

	// Ignore GnuTLS's gripes, if the transport broke because of our own
	// doing (read/write limits, time outs, etc...)

	switch (errno) {
	case ETIMEDOUT:
	case EOVERFLOW:
		direction=0;
		return 0;
	}

	if (n == GNUTLS_E_PREMATURE_TERMINATION &&
	    ignore_premature_termination.get())
	{
		errno=0;
		direction=0;
		return 0;
	}

	if (n == GNUTLS_E_WARNING_ALERT_RECEIVED)
		throw alertexception(false, gnutls_alert_get(sess));

	if (n == GNUTLS_E_FATAL_ALERT_RECEIVED)
		throw alertexception(true, gnutls_alert_get(sess));

	chkerr(n, "gnutls_record_recv");
	LOG_ERROR("gnutls_record_recv: default error handler invoked");

	errno=EIO;
	throw EXCEPTION("gnutls_record_send");
}

gnutls_certificate_type_t gnutls::sessionObj::certificate_type_get()
	const
{
	LOCK_SESSION;

	gnutls_certificate_type_t t;

	chkerr((t=gnutls_certificate_type_get(sess)),
	       "gnutls_certificate_type_get");
	return t;
}

void gnutls::sessionObj
::get_peer_certificates(std::list<gnutls::x509::crt> &certList)
	const
{
	LOCK_SESSION;

	if (certificate_type_get() != GNUTLS_CRT_X509)
		throw EXCEPTION("Unknown certificate type");

	unsigned int list_size;
	const gnutls_datum_t *certs=gnutls_certificate_get_peers(sess,
								 &list_size);

	unsigned int i;

	for (i=0; i<list_size; i++)
	{
		std::list<gnutls::x509::crt> cert;

		try {
			datum_t dt(datumwrapper::convert(certs[i]));

			gnutls::x509::crt::base::import_cert_list(cert,
								  dt,
								  GNUTLS_X509_FMT_DER);
			if (cert.size() != 1)
				chkerr(GNUTLS_E_X509_CERTIFICATE_ERROR,
				       "get_peer_certificates: unexpected chain decoded from a DER cert");
		} catch (...)
		{
			while (i)
			{
				certList.pop_back();
				--i;
			}
			throw;
		}

		certList.push_back(cert.front());
	}
}

void gnutls::sessionObj
::server_set_certificate_request(gnutls_certificate_request_t req)

{
	LOCK_SESSION;

	gnutls_certificate_server_set_request(sess, req);
}

void gnutls::sessionObj::set_default_priority()
{
	set_priority("NORMAL");
}

void gnutls::sessionObj::set_priority(const std::string &priorityStr)
{
	LOCK_SESSION;

	chkerr(gnutls_priority_set_direct(sess, priorityStr.c_str(), NULL),
	       "gnutls_priority_set_direct");
}

bool gnutls::sessionObj::alert_send(gnutls_alert_level_t level,
				    gnutls_alert_description_t desc,
				    int &direction)
{
	LOCK_SESSION;

	direction=0;

 again:
	int rc=gnutls_alert_send(sess, level, desc);

	if (rc != GNUTLS_E_SUCCESS)
	{
		if (AGAIN(rc))
		{
			if (AGAIN_SHOULD_BE_HANDLED_BY_GNUTLS)
				goto again; // GNUTLS_E_AGAIN from GnuTLS itself

			direction=compute_direction();
			return false;
		}
		// Propagate ETIMEDOUT, because gnutls complains about packet length instead

		if (rc && errno == ETIMEDOUT)
			throw SYSEXCEPTION("gnutls_alert_send");

		chkerr(rc, "gnutls_alert_send");
	}
	return true;
}

ref<fdstreambufObj> gnutls::sessionObj::getStreamBuffer()
{
	LOCK_SESSION;

	size_t s=get_max_size();

	if (s < BUFSIZ)
		s=BUFSIZ;

	return ref<fdstreambufObj>::create(fdbase(this), s*2, false);
}


gnutls::datum_t gnutls::sessionObj::get_session_id() const
{
	LOCK_SESSION;

	gnutls_datum_t session_id;

	chkerr(gnutls_session_get_id2(sess, &session_id),
	       "gnutls_session_get_id2");

	return datumwrapper(session_id);
}

gnutls::datum_t gnutls::sessionObj::get_session_data() const
{
	LOCK_SESSION;

	gnutls_datum_t session_data;

	chkerr(gnutls_session_get_data2(sess, &session_data),
	       "gnutls_session_get_data2");

	return datumwrapper(session_data);
}

bool gnutls::sessionObj::session_resumed() const
{
	LOCK_SESSION;

	return gnutls_session_is_resumed(sess) ? true:false;
}

iostream gnutls::sessionObj::getiostream()
{
	LOCK_SESSION;

	return ref<basic_streamObj<std::iostream> >::create(getStreamBuffer());
}

int gnutls::sessionObj::get_fd() const noexcept
{
	LOCK_SESSION;

	return transport->get_fd();
}

size_t gnutls::sessionObj::pubread(char *buffer, size_t cnt)
{
	LOG_FUNC_SCOPE(debugLog);

	int direction;

	size_t n=recv(buffer, cnt, direction);

	LOG_TRACE(this << ": pubread: recv() returned " << n);

	if (n == 0)
	{
		if (direction)
		{
			if (direction & POLLOUT)
			{
				LOG_ERROR("pubread: wants to write to the socket");
				errno=EWOULDBLOCK;
				throw SYSEXCEPTION("gnutls_record_recv");
			}
			errno=EAGAIN;
		}
	}

	return n;
}

int gnutls::sessionObj::pubpoll(int timeout_ms)
{
	LOCK_SESSION;

	if (read_pending())
		return 1;

	return transport->pubpoll(timeout_ms);
}

size_t gnutls::sessionObj::pubread_pending() const
{
	LOCK_SESSION;

	return read_pending();
}

size_t gnutls::sessionObj::pubwrite(const char *buffer, size_t cnt)

{
	LOG_FUNC_SCOPE(debugLog);

	int direction;

	size_t n=send(buffer, cnt, direction);

	LOG_TRACE(this << ": pubwrite: send() returned " << n);

	if (n == 0)
	{
		if (direction)
		{
			if (direction & POLLIN)
			{
				LOG_ERROR("pubwrite: wants to read from the socket");
				errno=EWOULDBLOCK;
				throw SYSEXCEPTION("gnutls_record_send");
			}
			errno=EAGAIN;
		}
	}
	return n;
}

off64_t gnutls::sessionObj::pubseek(off64_t offset, int whence)
{
	throw EXCEPTION(strerror(ESPIPE));
}

void gnutls::sessionObj::pubconnect(const struct ::sockaddr *serv_addr,
				    socklen_t addrlen)
{
	errno=EINVAL;
	throw SYSEXCEPTION("connect");
}

void gnutls::sessionObj::pubclose()
{
	errno=EINVAL;
	throw SYSEXCEPTION("close");
}

fdptr gnutls::sessionObj::pubaccept(//! Connected peer's address
				    sockaddrptr &peername)
{
	errno=EINVAL;
	throw SYSEXCEPTION("accept");
}

#if 0
{
#endif
};
