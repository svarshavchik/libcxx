/*
** Copyright 2012 Double Precision, Inc.
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
#include "x/logger.H"
#include "x/exception.H"
#include "x/timerfd.H"
#include "gettext_in.h"

#include <sys/poll.h>

#include <cstring>

#define AGAIN(e) \
	((e) == GNUTLS_E_INTERRUPTED || (e) == GNUTLS_E_AGAIN)

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

gnutls::sessionObj::factoryObj::factoryObj()
{
}

gnutls::sessionObj::factoryObj::factoryObj(const gnutls::credentials::certificate &cert)
	: cred(cert)
{
}

gnutls::sessionObj::factoryObj::~factoryObj() noexcept
{
}

gnutls::session gnutls::sessionObj::factoryObj
::create(unsigned mode,
	 const fdbase &transportArg)
{
	gnutls::session sess(session::create(mode, transportArg));

	sess->set_default_priority();
	sess->set_private_extensions();

	{
		std::lock_guard<std::mutex> lock(mutex);

		sess->credentials_set(cred);
	}
	return sess;
}

gnutls::sessionObj::sessionObj(unsigned modeArg,
			       const fdbase &transportArg)
	: mode(modeArg),
	  transport(transportArg), handshake_needed(true)
{
	gnutls::init::gnutls_init();

	chkerr(gnutls_init(&sess, mode), "gnutls_init");
	gnutls_session_set_ptr(sess, this);
	gnutls_transport_set_ptr2(sess,
				  (gnutls_transport_ptr_t) &transport,
				  (gnutls_transport_ptr_t) &transport);

	gnutls_transport_set_pull_function(sess, (gnutls_pull_func)
					   &gnutls::sessionObj::pull_func);
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

ssize_t gnutls::sessionObj::pull_func(gnutls_transport_ptr_t ptr,
				      void *buf,
				      size_t buf_s) noexcept
{
	size_t n;

	try {
		n=(*(fdbase *)ptr)->pubread( (char *)buf, buf_s);
	} catch (const sysexception &e)
	{
		errno=e.getErrorCode();
		return (ssize_t)-1;
	} catch (...)
	{
		errno=EIO;
		return (ssize_t)-1;
	}

	if (n == 0)
	{
		if (!errno)
		{
			return 0; // EOF
		}

		return (ssize_t)-1; // EAGAIN or EWOULDBLOCK
	}

	return n;
}

ssize_t gnutls::sessionObj::push_func(gnutls_transport_ptr_t ptr,
				      const void *buf,
				      size_t buf_s) noexcept
{
	std::streamsize n;

	try {
		n=(*(fdbase *)ptr)->pubwrite( (const char *)buf, buf_s);
	} catch (const sysexception &e)
	{
		errno=e.getErrorCode();
		return (ssize_t)-1;
	} catch (...)
	{
		errno=EIO;
		return (ssize_t)-1;
	}

	if (n == 0)
	{
		if (!errno)
			errno=ENOSPC;

		return (ssize_t)-1;
	}

	return n;
}

gnutls::sessionObj::~sessionObj() noexcept
{
	gnutls_deinit(sess);
}

void gnutls::sessionObj::credentials_set(const credentials::certificate &cert)

{
	chkerr(gnutls_credentials_set(sess, GNUTLS_CRD_CERTIFICATE,
				      cert->cred),
	       "gnutls_credentials_set");

	certificateCred=cert;
}

void gnutls::sessionObj::credentials_clear()
{
	certificateCred=credentials::certificateptr();
	gnutls_credentials_clear(sess);
}

void gnutls::sessionObj::set_private_extensions(bool flag)
{
	gnutls_handshake_set_private_extensions(sess, flag ? 1:0);
}

void gnutls::sessionObj::set_server_name(const std::string &hostname)

{
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

bool gnutls::sessionObj::handshake(int &direction)
{
	return handshake(true, direction);
}

bool gnutls::sessionObj::handshake(bool force,
				   int &direction)
{
	std::lock_guard<std::mutex> lock(objmutex);

	if (force)
		handshake_needed=true;

	if (!handshake_needed)
		return true; // Already done.

	direction=0;
	errno=0;

	int err=gnutls_handshake(sess);

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

		if (errno == ETIMEDOUT)
			throw SYSEXCEPTION("gnutls_handshake");

		if (AGAIN(err))
		{
			direction=compute_direction();
			return (false);
		}

		if (err == GNUTLS_E_WARNING_ALERT_RECEIVED)
			throw alertexception(false, gnutls_alert_get(sess));

		if (err == GNUTLS_E_FATAL_ALERT_RECEIVED)
			throw alertexception(true, gnutls_alert_get(sess));

		chkerr(err, "gnutls_handshake");
	}
	handshake_needed=false;
	return true;
}

bool gnutls::sessionObj::rehandshake(int &direction)
{
	direction=0;
	errno=0;
	int err=gnutls_rehandshake(sess);

	if (err != GNUTLS_E_SUCCESS)
	{
		if (AGAIN(err))
		{
			direction=compute_direction();
			return false;
		}

		// Propagate ETIMEDOUT, because gnutls complains about packet length instead

		if (errno == ETIMEDOUT)
			throw SYSEXCEPTION("gnutls_rehandshake");
	}
	chkerr(err, "gnutls_rehandshake");
	return true;
}

void gnutls::sessionObj::verify_peer_internal(const std::string *hostname)

{
	unsigned int flags=0;

	chkerr(gnutls_certificate_verify_peers2(sess, &flags),
	       "gnutls_certificate_verify_peers2");

	if (flags)
	{
		messages msgcat(messages::create(locale::base::environment(),
						 LIBCXX_DOMAIN));

		throw EXCEPTION((hostname ?
				 *hostname:std::string("Peer certificate"))
				+ ": " + msgcat->
				get(flags & GNUTLS_CERT_REVOKED ?
				    _("Revoked certificate"):
				    flags & GNUTLS_CERT_SIGNER_NOT_FOUND ?
				    _("Unknown certificate authority"):
				    flags & GNUTLS_CERT_SIGNER_NOT_CA ?
				    _("Certificate signer is not an authority"):
				    flags & GNUTLS_CERT_INSECURE_ALGORITHM ?
				    _("Certificate signed by a weak hash"):
				    flags & GNUTLS_CERT_NOT_ACTIVATED ?
				    _("Certificate not yet active"):
				    flags & GNUTLS_CERT_EXPIRED ?
				    _("Certificate expired"):
				    flags & GNUTLS_CERT_INVALID ?
				    _("Invalid certificate"):
				    _("Certificate verification failed")));
	}

	if (certificate_type_get() == GNUTLS_CRT_X509)
		verify_peers_x509(hostname);
}

void gnutls::sessionObj::verify_peers_x509(const std::string *hostname)

{
	std::list<x509::crt> peercerts;

	get_peer_certificates(peercerts);

	if (peercerts.empty())
		chkerr(GNUTLS_E_X509_CERTIFICATE_ERROR,
		       "verify_peers: empty X.509 certificate list");

	x509::crt &c=peercerts.front();

	if (hostname && !c->check_hostname(*hostname))
		chkerr(GNUTLS_E_X509_CERTIFICATE_ERROR,
		       "verify_peers: certificate name mismatch");
}

gnutls_cipher_algorithm_t gnutls::sessionObj::getCipher() const
{
	return gnutls_cipher_get(sess);
}

gnutls_kx_algorithm_t gnutls::sessionObj::getKx() const
{
	return gnutls_kx_get(sess);
}

gnutls_mac_algorithm_t gnutls::sessionObj::getMac() const
{
	return gnutls_mac_get(sess);
}

gnutls_compression_method_t gnutls::sessionObj::getCompression()
	const
{
	return gnutls_compression_get(sess);
}

std::string gnutls::sessionObj::getSuite() const
{
	gnutls_compression_method_t comp=getCompression();

	return gnutls::session::base::get_cipher_name(getCipher()) + "/"
		+ gnutls::session::base::get_kx_name(getKx()) + "/"
		+ gnutls::session::base::get_mac_name(getMac())
		+ (comp == GNUTLS_COMP_UNKNOWN || comp == GNUTLS_COMP_NULL
		   ? "": "/" + gnutls::session::base::get_compression_name(comp));
}

bool gnutls::sessionObj::bye(int &direction,
			     gnutls_close_request_t how)
{
	direction=0;

	errno=0;
	int err=gnutls_bye(sess, how);

	if (AGAIN(err))
	{
		direction=compute_direction();
		return false;
	}

	// Propagate ETIMEDOUT, because gnutls complains about packet length instead

	if (err && errno == ETIMEDOUT)
		throw SYSEXCEPTION("gnutls_rehandshake");


	chkerr(err, "gnutls_bye");
	return true;
}

size_t gnutls::sessionObj::read_pending() const
{
	return gnutls_record_check_pending(sess);
}

void gnutls::sessionObj::set_max_size(size_t recsize)
{
	chkerr(gnutls_record_set_max_size(sess, recsize),
	       "gnutls_record_set_max_size");
}

size_t gnutls::sessionObj::get_max_size() const noexcept
{
	return gnutls_record_get_max_size(sess);
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::gnutls::sessionObj::recv,sendLog);

size_t gnutls::sessionObj::send(const void *data, size_t cnt, int &direction)

{
	if (!handshake(false, direction))
	{
		errno=EAGAIN;
		return 0;
	}

	LOG_FUNC_SCOPE(sendLog);

	direction=0;
	errno=0;
	ssize_t n=gnutls_record_send(sess, data, cnt);

	LOG_TRACE("gnutls_record_send: " << n);

	if (n >= 0)
		return n;

	if (AGAIN(n))
	{
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

	// Propagate ETIMEDOUT, because gnutls complains about packet length instead

	if (errno == ETIMEDOUT)
		throw SYSEXCEPTION("gnutls_record_send");

	chkerr(n, "gnutls_record_send");

	// Shouldn't get here

	LOG_ERROR("gnutls_record_send: default error handler invoked");
	errno=EIO;
	throw EXCEPTION("gnutls_record_send");
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::gnutls::sessionObj::recv,recvLog);

size_t gnutls::sessionObj::recv(void *data, size_t cnt, int &direction)

{
	LOG_FUNC_SCOPE(recvLog);

	if (!handshake(false, direction))
	{
		errno=EAGAIN;
		return 0;
	}

	ssize_t n=gnutls_record_recv(sess, data, cnt);

	LOG_TRACE("gnutls_record_recv: " << n);

	if (n >= 0)
	{
		direction=0;
		return n;
	}

	if (AGAIN(n))
	{
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

	if (n == GNUTLS_E_WARNING_ALERT_RECEIVED)
		throw alertexception(false, gnutls_alert_get(sess));

	if (n == GNUTLS_E_FATAL_ALERT_RECEIVED)
		throw alertexception(true, gnutls_alert_get(sess));

	// Propagate ETIMEDOUT, because gnutls complains about packet length instead

	if (errno == ETIMEDOUT)
		throw SYSEXCEPTION("gnutls_record_recv");

	chkerr(n, "gnutls_record_recv");
	LOG_ERROR("gnutls_record_recv: default error handler invoked");

	errno=EIO;
	throw EXCEPTION("gnutls_record_send");
}

gnutls_certificate_type_t gnutls::sessionObj::certificate_type_get()
	const
{
	gnutls_certificate_type_t t;

	chkerr((t=gnutls_certificate_type_get(sess)),
	       "gnutls_certificate_type_get");
	return t;
}

void gnutls::sessionObj
::get_peer_certificates(std::list<gnutls::x509::crt> &certList)
	const
{
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
	gnutls_certificate_server_set_request(sess, req);
}

void gnutls::sessionObj::set_default_priority()
{
	set_priority("NORMAL:-CTYPE-OPENPGP");
}

void gnutls::sessionObj::set_priority(const std::string &priorityStr)
{
	chkerr(gnutls_priority_set_direct(sess, priorityStr.c_str(), NULL),
	       "gnutls_priority_set_direct");
}

bool gnutls::sessionObj::alert_send(gnutls_alert_level_t level,
				    gnutls_alert_description_t desc,
				    int &direction)
{
	direction=0;
	errno=0;

	int rc=gnutls_alert_send(sess, level, desc);

	if (rc != GNUTLS_E_SUCCESS)
	{
		if (AGAIN(rc))
		{
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
	size_t s=get_max_size();

	if (s < BUFSIZ)
		s=BUFSIZ;

	return ref<fdstreambufObj>::create(fdbase(this), s*2, false);
}

iostream gnutls::sessionObj::getiostream()
{
	return ref<basic_streamObj<std::iostream> >::create(getStreamBuffer());
}

int gnutls::sessionObj::getFd() const noexcept
{
	return transport->getFd();
}

size_t gnutls::sessionObj::pubread(char *buffer, size_t cnt)
{
	LOG_FUNC_SCOPE(recvLog);

	int direction;

	size_t n=recv(buffer, cnt, direction);

	if (n == 0)
	{
		errno=0;

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

size_t gnutls::sessionObj::pubread_pending() const
{
	return read_pending();
}

size_t gnutls::sessionObj::pubwrite(const char *buffer, size_t cnt)

{
	LOG_FUNC_SCOPE(recvLog);

	int direction;

	size_t n=send(buffer, cnt, direction);

	if (n == 0)
	{
		errno=0;

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

