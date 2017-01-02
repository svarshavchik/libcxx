/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_sessionobj_H
#define x_tls_sessionobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/ptr.H>
#include <x/exception.H>
#include <x/gnutls/init.H>
#include <x/gnutls/sessionfwd.H>
#include <x/gnutls/credentials.H>
#include <x/gnutls/x509_crt.H>
#include <x/gnutls/datumwrapper.H>
#include <x/gnutls/sessioncachefwd.H>
#include <gnutls/gnutls.h>

#include <string>
#include <mutex>

namespace LIBCXX_NAMESPACE {

	class fdstreambufObj;

	template<class streamType> class basic_streamObj;

	namespace gnutls {
		class sessionObj;
	}
#if 0
};
#endif

//! A wrapper for a \c gnutls_session_t

class gnutls::sessionObj : virtual public obj, public fdbaseObj {

	//! Internal mutex

	mutable std::recursive_mutex session_mutex;

	//! \c GNUTLS_CLIENT or \c GNUTLS_SERVER
	unsigned mode;

	//! The session handle

	gnutls_session_t sess;

	//! The TLS session cache used by this handle, for servers.

	sessioncacheptr cache;

	//! The underlying transport

	fdbase transport;

	//! Error when reading from the transport.

	int transport_errno=0;

	//! Whether a handshake is needed

	bool handshake_needed=true;

	//! Whether the TLS session should be uncached by the destructor
	bool session_remove_needed=false;

	//! Reference to the currently-set %credentials

	credentials::certificateptr certificateCred;

public:
	//! Construct a new session object

	sessionObj(//! \c GNUTLS_CLIENT or \c GNUTLS_SERVER
		   unsigned modeArg,

		   //! The underlying sequence

		   const fdbase &transportArg);

	//! Default destructor
	~sessionObj();

	//! Return the underlying transport object

	const_fdbase getTransport()
	{
		return transport;
	}

	//! Update the underlying transport object

	void setTransport(const fdbase &transportArg)
	{
		transport=transportArg;
	}

	//! Return the negotiated session cipher algorithm

	gnutls_cipher_algorithm_t getCipher() const;

	//! Return the negotiated key exchange algorithm

	gnutls_kx_algorithm_t getKx() const;

	//! Return the negotiated hash algorithm

	gnutls_mac_algorithm_t getMac() const;

	//! Return the negotiated compression algorithm

	gnutls_compression_method_t getCompression() const;

	//! Return a generic description of this session's encryption suite

	//! \return a string of the form "cipher/kx/mac(/compression)?"
	//!
	std::string getSuite() const;

private:

	//! Stub for GnuTLS's session pull function

	static ssize_t pull_func(gnutls_transport_ptr_t ptr,
				 void *buf,
				 size_t buf_s) noexcept LIBCXX_HIDDEN;

	//! Stub for GnuTLS's session push function

	static ssize_t push_func(gnutls_transport_ptr_t ptr,
				 const void *buf,
				 size_t buf_s) noexcept LIBCXX_HIDDEN;

	//! Stub for GnuTLS's session pull function

	ssize_t pull_func(void *buf,
			  size_t buf_s) noexcept LIBCXX_HIDDEN;

	//! Stub for GnuTLS's session push function

	ssize_t push_func(const void *buf,
			  size_t buf_s) noexcept LIBCXX_HIDDEN;

public:
	//! Return the session identifier
	datum_t get_session_id() const;

	//! Return the session data
	datum_t get_session_data() const;

	//! Set the session resumption data
	void set_session_data(const datum_t &session_dataArg);

	//! Set the TLS session cache for a server
	void session_cache(const sessioncache &cacheArg);

	//! Set the TLS session cache for a server with a custom expiration
	void session_cache(const sessioncache &cacheArg,
			   time_t expiration);
private:
	//! Saved session data.
	datumptr_t session_data;
public:
	//! Whether this is a resumed session
	bool session_resumed() const;

	//! Define the %credentials object for this session

	void credentials_set(//! Certificate-based %credentials
			     const credentials::certificate &cert);

	//! Clear all %credentials

	void credentials_clear();

	friend class credentials::callbackObj;
	friend class credentials::certificateObj;

	//! Set default configuration of allowed protocols

	//! This must be done before the handshake.
	//! This is equivalent to
	//! \c set_priority("NORMAL:-CTYPE-OPENPGP").
	//! \note
	//! This default configuration
	//! disables OpenPGP certs, not yet fully implemented
	//! by this class.
	void set_default_priority();

	//! Manually set the protocol priorities, as a string

	//! See gnutls_priority_init(3) documentation for a description of
	//! what this string is.

	void set_priority(const std::string &priorityStr);

	//! Set maximum record size

	//! This must be done before the handshake.
	//!
	void set_max_size(//! Record size: 512, 1024, 2048, or 4096
			  size_t recsize);

	//! Set certificate request flag

	//! This must be done before the handshake.
	//! Only defined for servers.

	void server_set_certificate_request(//! If \c GNUTLS_CERT_REQUIRE, handshake fails if client does not send the certificate, if \c GNUTLS_CERT_REQUEST, it's still allowed

				     gnutls_certificate_request_t req=
				     GNUTLS_CERT_REQUIRE);

	//! Enable/disable private extensions

	//! This must be done before the handshake.
	//!
	void set_private_extensions(//! \c true - enable extensions
				    bool flag=true);

	//! Set server's name

	//! Request the certificate for the given server name, from the
	//! server. Only valid for clients, must be done before the handshake.

	void set_server_name(//! The name of the server
			     const std::string &hostname);

private:

	//! Retrieve the list of server names acceptable to the client

	//! Only valid for servers.
	//! \internal
	static void get_server_names(//! GnuTLS session handle
				     gnutls_session_t sess,

				     //! Server names requested by the client
				     std::list<std::string> &hostname_list)
		LIBCXX_HIDDEN;

	//! Internal method

	//! \internal
	//!
	int compute_direction() noexcept
	{
		return gnutls_record_get_direction(sess) == 0 ? POLLIN:POLLOUT;
	}

public:
	//! Perform handshake

	//! \return \c true if the handshake succeeded, \c false if
	//! further I/O is required
	//! (when the underlying transport is non-blocking). A failed
	//! handshake results in a thrown %exception.
	//!
	//! Returns \c true immediately if a handshake was already completed.
	//! Alternatively, the first call to recv() or send() will do
	//! a handshake automatically.

	bool handshake(//! Pending IO state

		       //! If handshake() returns \c false, this gets set to
		       //! \c POLLIN or \c POLLOUT indicating the required IO
		       //! state before the handshake can proceed.
		       int &direction);

	//! Retrieve the certificate type negotiated by the handshake

	//! Available after a succesful handshake.
	//!

	gnutls_certificate_type_t certificate_type_get() const;

	//! Validate peer certificate after handshake

	//! Available after a succesful handshake.
	//! Throws an %exception if the certificate could not be verified.

	void verify_peer()
	{
		verify_peer_internal(0);
	}

	//! Validate peer certificate after handshake

	//! Available after a succesful handshake.
	//! Throws an %exception if the certificate could not be verified.

	void verify_peer(//! Peer's expected hostname
			 const std::string &hostname)
	{
		verify_peer_internal(&hostname);
	}
private:

	//! Internal handshake implementation

	bool handshake(bool force, int &direction);

	//! verify_peer() common implementation

	void verify_peer_internal(//! If not \c NULL, hostname to verify
				  const std::string *hostname);

	//! Extra checking for X.509 peer certs

	void verify_peers_x509(//! If not \c NULL, verify this hostname
			       const std::string *hostname) LIBCXX_INTERNAL;
public:
	//! Retrieve peer's certificates

	//! Available after a succesful handshake.
	//!

	void get_peer_certificates(//! Retrieved peer's certificate list

				   std::list<gnutls::x509::crt>
				   &certList) const;

	//! Tear down the connection

	//! \return \c true if the teardown succeeded, \c false if
	//! further I/O is required
	//! (when the underlying transport is non-blocking).

	bool bye(//! If bye() returns false, this is set to
		 //! \c POLLIN or \c POLLOUT indicating the required IO
		 //! state before the teardown can proceed.

		 int &direction,

		 //! \c GNUTLS_SHUT_WR to tear down without consuming any pending input
		 gnutls_close_request_t how=GNUTLS_SHUT_RDWR);

	//! Return number of bytes currently unread, if any

	size_t read_pending() const;

	//! Retrieve the maximum record size

	//! Available after a succesful handshake.
	//!
	size_t get_max_size() const noexcept;

	//! Send data

	//! \return \c number of bytes written. If 0, check the value of
	//! \c direction. If \c direction is non-0, poll() the underlying
	//! file descriptor for this event, and try again. If \c direction
	//! is 0, check errno. If errno is 0, the peer has initiated a
	//! shutdown of this TLS session.
	//!
	//! An %exception gets thrown if a GNUTLS error occurs,
	//! including the SSL/TLS session terminated by the peer.

	size_t send(//! Output buffer
		    const void *data,

		    //! Size of the output buffer.
		    size_t cnt,

		    //! Pending IO state

		    //! If send() returns \c 0, a 0 value means
		    //! an end of file. Else, this is set to
		    //! \c POLLIN or \c POLLOUT indicating the required IO
		    //! state before the data can be sent.
		    int &direction
		    );

	//! Receive data

	//! \return \c number of bytes read. If 0, check the value of
	//! \c direction. If \c direction is non-0, poll() the underlying
	//! file descriptor for this event, and try again. If \c direction is
	//! 0, check errno.
	//!
	//! An %exception gets thrown if a GNUTLS error occurs.
	//! \ref alertexception "alertexception" gets thrown when a TLS alert
	//! message is received.

	size_t recv(//! Buffer
		    void *data,

		    //! Buffer size
		    size_t cnt,

		    //! Pending IO state

		    //! If recv() returns 0, a 0 value means
		    //! an end of file. Else, this is set to
		    //! \c POLLIN or \c POLLOUT indicating the required IO
		    //! state before the handshake can proceed.
		    int &direction

		    );

	//! Return a stream buffer for the session

	//! \return a reference to a \c std::basic_streambuf subclass that
	//! reads or writes from this session.
	//!
	//! \note
	//! The stream buffer must remain in scope as long as this file
	//! descriptor remains open. If the stream buffer goes out of scope,
	//! unread and/or unflushed data will get lost.
	//!
	//! \note
	//! Stream buffer-based reading and writing is effective only if the
	//! underlying transport uses blocking socket connections. Sessions
	//! based on non-blocking sockets cannot use stream buffers.

	ref<fdstreambufObj> getStreamBuffer();

	//! Return a stream object associated with this session

	//! This is a convenience function that uses getStreamBuffer() to return
	//! an iostream object for this file descriptor.

	ref<basic_streamObj<std::iostream> > getiostream();

	//! Send a rehandshake request

	//! \return \c true if the request succeeded, in which case
	//! handshake() should now be invoked, or \c false if
	//! further I/O is required. An %exception gets thrown when an error
	//! occurs.

	bool rehandshake(//! Pending IO state

			 //! If rehandshake() returns \c false, this gets set to
			 //! \c POLLIN or \c POLLOUT indicating the required IO
			 //! state before the handshake can proceed.
			 int &direction);

	//! Send an alert message

	//! \return \c true if the alert was sent,  or \c false if
	//! further I/O is required
	//! (when the underlying transport is non-blocking).
	//!
	//! A failure to send the alert throws an %exception.

	bool alert_send(//! \c GNUTLS_AL_WARNING or \c GNUTLS_AL_FATAL
			gnutls_alert_level_t level,

			//! One of \c GNUTLS_A_* constants
			gnutls_alert_description_t desc,
			//! If alert_send() returns \c false, this is set to
			//! \c POLLIN or \c POLLOUT indicating the required IO
			//! state before the alert can be sent.
			int &direction);

private:
	//! Implement pubread(), inherited from fdbaseObj

	size_t pubread(//! Buffer

		       char *buffer,

		       //! Element count

		       size_t cnt) override LIBCXX_HIDDEN;

	//! Implement pubread_pending(), inherited from fdbaseObj

	size_t pubread_pending() const override;

	//! Implement pubwrite(), inherited from fdbaseObj

	size_t pubwrite(//! Buffer

			const char *buffer,

			//! Element count

			size_t cnt) override LIBCXX_HIDDEN;

	//! Implement pubseek(), inherited from fdbaseObj

	//! Seek to a new position for the file descriptor.

	//! Errors are reported by throwing an %exception.
	//!
	off64_t pubseek(//! Offset.

			off64_t offset,

			//! Either \c SEEK_SET, \c SEEK_CUR, or \c SEEK_END.

			int whence) override LIBCXX_HIDDEN;

	//! Return the underlying file descriptor

	int getFd() const noexcept override;

	//! Implement pubconnect(), inherited from fdbaseObj

	void pubconnect(//! The server's address.

			const struct ::sockaddr *serv_addr,

			//! serv_addr's length, in bytes

			socklen_t addrlen) override;

	using fdbaseObj::pubaccept;

	//! Implement pubconnect(), inherited from fdbaseObj

	fdptr pubaccept(//! Connected peer's address
			sockaddrptr &peername) override;

	//! Implement pubclose(), inherited from fdbaseObj

	void pubclose() override;
public:
	class factoryObj;
};

//! A session factory

//! Subclass sessionObj::factoryObj to define a custom session factory.
//! This is a reference-counted object, that should be referenced as
//! \c session::factory.
//!
//! The session factory is used by higher level classes, like
//! gnutls::http::fdtlsserver.

class gnutls::sessionObj::factoryObj : virtual public obj {

	//! Make this thread-safe

	std::mutex mutex;

	//! Credentials used by the factory

	credentials::certificateptr cred;
public:
	//! Constructor

	factoryObj();

	//! Constructor with an initial certificate

	factoryObj(const credentials::certificate &credArg);

	//! Destructor

	~factoryObj();

	//! Update credentials

	void credentials_set(const credentials::certificate &credArg)
	{
		std::unique_lock<std::mutex> lock(mutex);

		cred=credArg;
	}

	//! Create a new session object

	virtual session create(//! Connection mode
			       unsigned mode,

			       //! The underlying transport
			       const fdbase &transportArg);
};

#if 0
{
#endif
}
#endif
