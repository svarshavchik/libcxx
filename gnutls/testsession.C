/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/exceptions.H"
#include "x/gnutls/session.H"
#include "x/gnutls/rdn.H"
#include "x/fd.H"
#include "x/exception.H"
#include "x/gnutls/x509_crtobj.H"
#include "x/options.H"
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <tuple>
#include <poll.h>

class server_thread_setup : virtual public LIBCXX_NAMESPACE::obj {

public:
	void run(const LIBCXX_NAMESPACE::fd &fd);

	virtual void setup_sess(const LIBCXX_NAMESPACE::gnutls::session &)
	{
	}
	virtual void run_server(const LIBCXX_NAMESPACE::gnutls::session &)=0;
};

class server_thread : public server_thread_setup {

public:
	void run_server(const LIBCXX_NAMESPACE::gnutls::session &) override;
};

class client_thread_setup : virtual public LIBCXX_NAMESPACE::obj {

public:
	void run(const LIBCXX_NAMESPACE::fd &fd);

	virtual void setup_sess(const LIBCXX_NAMESPACE::gnutls::session &)
	{
	}

	virtual void run_client(const LIBCXX_NAMESPACE::gnutls::session &)=0;
};

class client_thread : public client_thread_setup {

public:
	void run_client(const LIBCXX_NAMESPACE::gnutls::session &) override;
};

class cred_callback : public LIBCXX_NAMESPACE::gnutls::credentials::callbackObj {

public:
	bool expecthostname;

	cred_callback() noexcept;
	~cred_callback();

	keycertptr get(const LIBCXX_NAMESPACE::gnutls::sessionObj *sess,
		       const std::list<std::string> &hostname_list,
		       const std::vector<gnutls_pk_algorithm_t> &algos,
		       const gnutls_datum_t *req_ca_dn,
		       size_t n_req_ca_dn) override;
};

cred_callback::cred_callback() noexcept
	: expecthostname(false)
{
}

cred_callback::~cred_callback()
{
}

cred_callback::keycertptr
cred_callback::get(const LIBCXX_NAMESPACE::gnutls::sessionObj *sess,
		  const std::list<std::string> &hostname_list,
		  const std::vector<gnutls_pk_algorithm_t> &algos,
		  const gnutls_datum_t *req_ca_dn,
		  size_t n_req_ca_dn)
{
	if (expecthostname && (hostname_list.size() != 1 ||
			       hostname_list.front() != "nobody.example.com"))
		throw EXCEPTION("Did not receive expected hostname");

	if (expecthostname)
		std::cout << "Client is looking for " << hostname_list.front()
			  << std::endl;

	size_t i;

	for (i=0; i<n_req_ca_dn; i++)
	{
		std::set<std::string> oids;

		LIBCXX_NAMESPACE::gnutls::rdn::raw_get_oid(req_ca_dn[i], oids);

		std::cout << "Client certificate request: ";

		const char *sep="";

		std::set<std::string>::const_iterator ob, oe;

		for (ob=oids.begin(), oe=oids.end(); ob != oe; ++ob)
		{
			std::list<std::string> values;

			LIBCXX_NAMESPACE::gnutls::rdn::raw_get_oid(req_ca_dn[i], values,
						    ob->c_str());

			std::list<std::string>::const_iterator vb, ve;

			for (vb=values.begin(), ve=values.end(); vb != ve;
			     ++vb)
			{
				std::cout << sep << *ob << "=" << *vb;
				sep=", ";
			}
		}
		std::cout << std::endl
			  << "  (" << LIBCXX_NAMESPACE::gnutls::rdn::raw_get(req_ca_dn[i])
			  << ")" << std::endl;
	}

	keycert retval(keycert::create());

	LIBCXX_NAMESPACE::gnutls::x509::crt::base::import_cert_list(retval->cert,
								  "testrsa3i.crt",
								  GNUTLS_X509_FMT_PEM);
	retval->key->import_pkcs1("testrsa3.key", GNUTLS_X509_FMT_PEM);

	return retval;
}

void server_thread_setup::run(const LIBCXX_NAMESPACE::fd &fd)
{
	int direction;

	std::list<LIBCXX_NAMESPACE::gnutls::x509::crt> calist;

	LIBCXX_NAMESPACE::gnutls::x509::crt::base::import_cert_list(calist, "testrsa1.crt",
					       GNUTLS_X509_FMT_PEM);

	auto cred=LIBCXX_NAMESPACE::ref<cred_callback>::create();

	cred->expecthostname=true;

	auto serverCert=
		LIBCXX_NAMESPACE::gnutls::credentials::certificate::create();

	serverCert->set_x509_trust_file("testrsa1.crt", GNUTLS_X509_FMT_PEM);

	auto sess=LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_SERVER,
							    fd);
	sess->credentials_set(serverCert);
	serverCert->set_callback(cred);

	sess->server_set_certificate_request();

	sess->set_default_priority();

	{
		auto dh=LIBCXX_NAMESPACE::gnutls::dhparams::create();
		auto dh_dat=LIBCXX_NAMESPACE::gnutls::datum_t::create();

		dh_dat->load("dhparams.dat");
		dh->import_pk(dh_dat, GNUTLS_X509_FMT_PEM);
		serverCert->set_dh_params(dh);
	}

	setup_sess(sess);

	try {
		int direction;

		sess->handshake(direction);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "server handshake failed: " << e << std::endl;
		throw;
	}

	run_server(sess);

	sess->bye(direction);
}


void server_thread::run_server(const LIBCXX_NAMESPACE::gnutls::session &sess)
{
	std::list<LIBCXX_NAMESPACE::gnutls::x509::crt> clientCerts;

	sess->get_peer_certificates(clientCerts);

	if (clientCerts.size() != 2
	    || clientCerts.front()->get_dn_hostname() != "*.domain.com")
	{
		std::cerr << "Did not receive expected client cert"
			  << std::endl;
		throw EXCEPTION("Did not receive expected client cert");
	}

	auto si=sess->getiostream();

	std::string buf;

	std::getline(*si, buf);

	if (buf != "Hello world")
		throw EXCEPTION("Did not read hello world!\n");

	if (si->get() != EOF)
		throw EXCEPTION("Received unexpected data");
}

void client_thread_setup::run(const LIBCXX_NAMESPACE::fd &fd)

{
	int direction;

	auto cred=LIBCXX_NAMESPACE::ref<cred_callback>::create();

	auto clientCert=LIBCXX_NAMESPACE::gnutls::credentials::certificate::create();

	clientCert->set_x509_trust_file("testrsa1.crt", GNUTLS_X509_FMT_PEM);

	auto sess=LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_CLIENT, fd);

	clientCert->set_callback(cred);
	sess->credentials_set(clientCert);
	sess->set_server_name("nobody.example.com");
	sess->set_default_priority();

	setup_sess(sess);

	try {
		sess->handshake(direction);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "client handshake failed: "
			  << e << std::endl;
		throw;
	}

	std::cout << "Session: " << sess->get_suite() << std::endl;

	try {
		sess->verify_peer("subcert.domain.com");
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "Server certification verification failed: "
			  << e << std::endl;
		throw;
	}

	run_client(sess);

	sess->bye(direction);
}

void client_thread::run_client(const LIBCXX_NAMESPACE::gnutls::session &sess)
{
	{
		std::list<LIBCXX_NAMESPACE::gnutls::x509::crt> certList;

		sess->get_peer_certificates(certList);

		for (auto cert : certList)
			std::cout << cert->print();
	}

	auto ios=sess->getiostream();

	(*ios) << "Hello world" << std::endl << std::flush;

	if (!ios->good())
		throw EXCEPTION("Send failed");
}

static void testsession()
{
	alarm(10);

	auto [a, b]=LIBCXX_NAMESPACE::fd::base::socketpair();

	auto server_threadObj=LIBCXX_NAMESPACE::ref<server_thread>::create();
	auto client_threadObj=LIBCXX_NAMESPACE::ref<client_thread>::create();

	auto ta=LIBCXX_NAMESPACE::run(server_threadObj, a),
		tb=LIBCXX_NAMESPACE::run(client_threadObj, b);

	ta->wait();
	tb->wait();
	alarm(0);
}

static void testsession2()
{
	LIBCXX_NAMESPACE::fd tmpfile=LIBCXX_NAMESPACE::fd::base::tmpfile();

	auto sess=LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_SERVER,
							    tmpfile);

	sess->set_default_priority();
}

class serverRehandshakeThread : virtual public LIBCXX_NAMESPACE::obj {

public:
	void run(const LIBCXX_NAMESPACE::fd &fd);
};

void serverRehandshakeThread::run(const LIBCXX_NAMESPACE::fd &fd)

{
	LIBCXX_NAMESPACE::gnutls::credentials::certificate
		serverCert(LIBCXX_NAMESPACE::gnutls::credentials::certificate::create());

	serverCert->set_x509_keyfile("testrsa1.crt", "testrsa1.key",
				     GNUTLS_X509_FMT_PEM);

	{
		LIBCXX_NAMESPACE::gnutls::dhparams dh(LIBCXX_NAMESPACE::gnutls::dhparams::create());
		LIBCXX_NAMESPACE::gnutls::datum_t dh_dat(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		dh_dat->load("dhparams.dat");
		dh->import_pk(dh_dat, GNUTLS_X509_FMT_PEM);
		serverCert->set_dh_params(dh);
	}

	LIBCXX_NAMESPACE::gnutls::session sess(LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_SERVER,
							   fd));
	sess->credentials_set(serverCert);
	sess->set_default_priority();

	int rc;

	char dummy=0;

	int direction;

	while ((rc=sess->recv(&dummy, 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= fd->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	if (dummy != 'A')
	{
		std::cout << "Rehandshake failed -- part I" << std::endl;
		throw EXCEPTION("Unexpected response from client");
	}

	while (!sess->rehandshake(direction))
	{
		if (direction == 0)
		{
			std::cerr << "Handshake renegotiation failed"
				  << std::endl;
			throw EXCEPTION("Unexpected response from client");
		}
		struct pollfd pfd;

		pfd.fd= fd->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	try {
		while (1)
		{
			while ((rc=sess->recv(&dummy, 1, direction)) == 0)
			{
				if (direction == 0)
				{
					std::cerr << "Handshake renegotiation failed"
						  << std::endl;
					throw EXCEPTION("Unexpected response from client");
				}

				struct pollfd pfd;

				pfd.fd= fd->get_fd();
				pfd.events=direction;
				poll(&pfd, 1, -1);
			}
		}
	} catch (const LIBCXX_NAMESPACE::gnutls::errexception &e)
	{
		if (e.errcode != GNUTLS_E_REHANDSHAKE)
			throw;
	}

	if (dummy != 'B')
	{
		std::cout << "Rehandshake failed -- part II" << std::endl;
		throw EXCEPTION("Unexpected response from client");
	}

	while (!sess->handshake(direction))
	{
		if (direction == 0)
		{
			std::cerr << "Handshake renegotiation failed"
				  << std::endl;
			throw EXCEPTION("Unexpected response from client");
		}
		struct pollfd pfd;

		pfd.fd= fd->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	while ((rc=sess->send("C", 1, direction)) == 0)
	{
		if (direction == 0)
		{
			std::cerr << "Handshake renegotiation failed"
				  << std::endl;
			throw EXCEPTION("Unexpected response from client");
		}

		struct pollfd pfd;

		pfd.fd= fd->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	while (!sess->bye(direction))
	{
		if (direction == 0)
		{
			std::cerr << "Handshake renegotiation failed"
				  << std::endl;
			throw EXCEPTION("Unexpected response from client");
		}

		struct pollfd pfd;

		pfd.fd= fd->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

}

static void testrehandshake()
{
	int direction;

	alarm(10);

	std::cout << "Testing rehandshake" << std::endl;

	LIBCXX_NAMESPACE::fdptr a,b;

	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p(LIBCXX_NAMESPACE::fd::base::socketpair());

		a=p.first;
		b=p.second;
	}

	a->nonblock(true);
	b->nonblock(true);

	LIBCXX_NAMESPACE::ref<serverRehandshakeThread> server_threadObj=
		LIBCXX_NAMESPACE::ref<serverRehandshakeThread>::create();

	auto server_thread=LIBCXX_NAMESPACE::run(server_threadObj, a);

	a=LIBCXX_NAMESPACE::fdptr();

	LIBCXX_NAMESPACE::gnutls::credentials::certificate
		clientCert(LIBCXX_NAMESPACE::gnutls::credentials::certificate::create());
	LIBCXX_NAMESPACE::gnutls::session sess(LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_CLIENT,
							   b));
	sess->credentials_set(clientCert);
	sess->set_default_priority();

	try {
		int rc;
		char dummy=0;

		while ((rc=sess->send("A", 1, direction)) == 0)
		{
			struct pollfd pfd;

			pfd.fd= b->get_fd();
			pfd.events=direction;
			poll(&pfd, 1, -1);
		}

		try {
			while (1)
			{
				while ((rc=sess->recv(&dummy, 1, direction))
				       == 0)
				{
					struct pollfd pfd;

					pfd.fd= b->get_fd();
					pfd.events=direction;
					poll(&pfd, 1, -1);
				}
			}
		} catch (const LIBCXX_NAMESPACE::gnutls::errexception &e)
		{
			if (e.errcode != GNUTLS_E_REHANDSHAKE)
				throw;
		}

		if (dummy)
		{
			std::cerr << "Did not receive rehandshake request from the server"
				  << std::endl;
			throw EXCEPTION("testrehandshake failed");
		}

		while ((rc=sess->send("B", 1, direction)) == 0)
		{
			struct pollfd pfd;

			pfd.fd= b->get_fd();
			pfd.events=direction;
			poll(&pfd, 1, -1);
		}

		while (!sess->handshake(direction))
		{
			if (direction == 0)
			{
				std::cerr << "Handshake renegotiation failed"
					  << std::endl;
				throw EXCEPTION("Unexpected response from server");
			}
			struct pollfd pfd;

			pfd.fd=b->get_fd();
			pfd.events=direction;
			poll(&pfd, 1, -1);
		}

		while ((rc=sess->recv(&dummy, 1, direction)) == 0)
		{
			struct pollfd pfd;

			pfd.fd= b->get_fd();
			pfd.events=direction;
			poll(&pfd, 1, -1);
		}

		if (dummy != 'C')
		{
			std::cerr << "Did not receive response after handshake"
				  << std::endl;
			throw EXCEPTION("testrehandshake failed");
		}

		while (!sess->bye(direction))
		{
			struct pollfd pfd;

			pfd.fd= b->get_fd();
			pfd.events=direction;
			poll(&pfd, 1, -1);
		}
		server_thread->get();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << (std::string)*e << std::endl;
		throw e;
	}
}

class readAbortThread : virtual public LIBCXX_NAMESPACE::obj {

public:
	void run(const LIBCXX_NAMESPACE::fd &fd);
};

void readAbortThread::run(const LIBCXX_NAMESPACE::fd &fd)

{
	int direction;

	LIBCXX_NAMESPACE::gnutls::credentials::certificate
		serverCert(LIBCXX_NAMESPACE::gnutls::credentials::certificate::create());

	serverCert->set_x509_keyfile("testrsa1.crt", "testrsa1.key",
				     GNUTLS_X509_FMT_PEM);

	{
		LIBCXX_NAMESPACE::gnutls::dhparams dh(LIBCXX_NAMESPACE::gnutls::dhparams::create());
		LIBCXX_NAMESPACE::gnutls::datum_t dh_dat(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		dh_dat->load("dhparams.dat");
		dh->import_pk(dh_dat, GNUTLS_X509_FMT_PEM);
		serverCert->set_dh_params(dh);
	}

	LIBCXX_NAMESPACE::gnutls::session sess(LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_SERVER,
							   fd));
	sess->credentials_set(serverCert);
	sess->set_default_priority();

	int rc;

	char dummy=0;

	while ((rc=sess->recv(&dummy, 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= fd->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	if (dummy != 'A')
	{
		std::cout << "readAbortThread failed -- part I" << std::endl;
		throw EXCEPTION("Unexpected response from client");
	}

	while ((rc=sess->send("B", 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= fd->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	while ((rc=sess->recv(&dummy, 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= fd->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	if (dummy != 'C')
	{
		std::cout << "readAbortThread failed -- part II" << std::endl;
		throw EXCEPTION("Unexpected response from client");
	}
}

static void testreadabort()
{
	int direction;

	alarm(10);

	std::cout << "Testing read abort" << std::endl;

	LIBCXX_NAMESPACE::fdptr a,b;

	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p(LIBCXX_NAMESPACE::fd::base::socketpair());

		a=p.first;
		b=p.second;
	}

	a->nonblock(true);
	b->nonblock(true);

	LIBCXX_NAMESPACE::ref<readAbortThread> server_threadObj=
		LIBCXX_NAMESPACE::ref<readAbortThread>::create();

	LIBCXX_NAMESPACE::run(server_threadObj, a);

	LIBCXX_NAMESPACE::gnutls::credentials::certificate
		clientCert(LIBCXX_NAMESPACE::gnutls::credentials::certificate::create());
	LIBCXX_NAMESPACE::gnutls::session sess(LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_CLIENT,
							   b));
	sess->credentials_set(clientCert);
	sess->set_default_priority();

	int rc;
	char dummy=0;

	while ((rc=sess->send("A", 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= b->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	while ((rc=sess->recv(&dummy, 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= b->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	a=LIBCXX_NAMESPACE::fdptr();

	if (dummy != 'B')
		throw EXCEPTION("Unexpected response from server");

	while ((rc=sess->send("C", 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= b->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	try {
		while ((rc=sess->recv(&dummy, 1, direction)) == 0)
		{
			struct pollfd pfd;

			pfd.fd= b->get_fd();
			pfd.events=direction;
			poll(&pfd, 1, -1);
		}
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Read abort correctly handled" << std::endl;
		return;
	}

	throw EXCEPTION("Read abort did not occur");
}

class writeAbortThread : virtual public LIBCXX_NAMESPACE::obj {

public:
	void run(const LIBCXX_NAMESPACE::fd &fd);
};

void writeAbortThread::run(const LIBCXX_NAMESPACE::fd &fd)

{
	int direction;

	LIBCXX_NAMESPACE::gnutls::credentials::certificate
		serverCert(LIBCXX_NAMESPACE::gnutls::credentials::certificate::create());

	serverCert->set_x509_keyfile("testrsa1.crt", "testrsa1.key",
				     GNUTLS_X509_FMT_PEM);

	{
		LIBCXX_NAMESPACE::gnutls::dhparams dh(LIBCXX_NAMESPACE::gnutls::dhparams::create());
		LIBCXX_NAMESPACE::gnutls::datum_t dh_dat(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		dh_dat->load("dhparams.dat");
		dh->import_pk(dh_dat, GNUTLS_X509_FMT_PEM);
		serverCert->set_dh_params(dh);
	}

	LIBCXX_NAMESPACE::gnutls::session sess(LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_SERVER,
							   fd));
	sess->credentials_set(serverCert);
	sess->set_default_priority();

	int rc;

	char dummy=0;

	while ((rc=sess->recv(&dummy, 1, direction)) == 0)
	{
		if (direction == 0)
		{
			dummy=0;
			break;
		}
		struct pollfd pfd;

		pfd.fd= fd->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	if (dummy != 'A')
	{
		std::cout << "writeAbortThread failed -- part I" << std::endl;
		throw EXCEPTION("Unexpected response from client");
	}

	while ((rc=sess->send("B", 1, direction)) == 0)
	{
		if (direction == 0)
			break;

		struct pollfd pfd;

		pfd.fd= fd->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}
}

static void testwriteabort()
{
	int direction;

	alarm(10);

	std::cout << "Testing write abort" << std::endl;

	LIBCXX_NAMESPACE::fdptr a,b;

	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p(LIBCXX_NAMESPACE::fd::base::socketpair());

		a=p.first;
		b=p.second;
	}

	a->nonblock(true);
	b->nonblock(true);

	LIBCXX_NAMESPACE::ref<writeAbortThread> server_threadObj=
		LIBCXX_NAMESPACE::ref<writeAbortThread>::create();

	auto server_thread=LIBCXX_NAMESPACE::run(server_threadObj, a);

	LIBCXX_NAMESPACE::gnutls::credentials::certificate
		clientCert(LIBCXX_NAMESPACE::gnutls::credentials::certificate::create());
	LIBCXX_NAMESPACE::gnutls::session sess(LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_CLIENT,
							   b));
	sess->credentials_set(clientCert);
	sess->set_default_priority();

	int rc;
	char dummy=0;

	while ((rc=sess->send("A", 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= b->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	while ((rc=sess->recv(&dummy, 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= b->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	a=LIBCXX_NAMESPACE::fdptr();

	if (dummy != 'B')
		throw EXCEPTION("Unexpected response from server");

	server_thread->wait();

	while ((rc=sess->send("C", 1, direction)) == 0)
	{
		if (direction == 0)
			break;

		struct pollfd pfd;

		pfd.fd= b->get_fd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	if (errno != EPIPE)
		throw EXCEPTION("Write abort did not happen");
}

class alpn_server : public server_thread_setup {

public:

	std::string protocols;
	std::optional<std::string> selected_protocol;

	alpn_server(const std::string &protocols) : protocols{protocols}
	{
	}

	void setup_sess(const LIBCXX_NAMESPACE::gnutls::session &sess)
		override
	{
		if (!protocols.empty())
			sess->set_alpn(protocols);
	}

	void run_server(const LIBCXX_NAMESPACE::gnutls::session &) override;
};

class alpn_client : public client_thread_setup {

public:

	std::string protocols;
	std::optional<std::string> selected_protocol;

	alpn_client(const std::string &protocols) : protocols{protocols}
	{
	}

	void setup_sess(const LIBCXX_NAMESPACE::gnutls::session &sess)
		override
	{
		if (!protocols.empty())
			sess->set_alpn(protocols);
	}

	void run_client(const LIBCXX_NAMESPACE::gnutls::session &) override;
};

void alpn_server::run_server(const LIBCXX_NAMESPACE::gnutls::session &sess)
{
	selected_protocol=sess->get_alpn();
}

void alpn_client::run_client(const LIBCXX_NAMESPACE::gnutls::session &sess)
{
	selected_protocol=sess->get_alpn();
}

void testalpn()
{
	alarm(30);

	static const struct {
		const char *server_proto;
		const char *client_proto;

		const char *negotiated;
	} tests[]={
		{ "", "", "" },
		{ "server", "", ""},
		{ "", "clent", ""},
		{ "common", "common", "common" },
		{ "server1,server2", "server1", "server1"},
		{ "server1,server2", "server2", "server2"},
		{ "client1", "client1,client2", "client1"},
		{ "client2", "client1,client2", "client2"},
		{ "server1", "client2", NULL},
	};

	for (const auto &t:tests)
	{
		auto [a, b]=LIBCXX_NAMESPACE::fd::base::socketpair();

		auto server=LIBCXX_NAMESPACE::ref<alpn_server>::create(t.server_proto);
		auto client=LIBCXX_NAMESPACE::ref<alpn_client>::create(t.client_proto);

		auto ta=LIBCXX_NAMESPACE::run(server, a),
			tb=LIBCXX_NAMESPACE::run(client, b);
		std::tie(a, b) = LIBCXX_NAMESPACE::fd::base::socketpair();

		ta->wait();
		tb->wait();

		for (const auto &[thr, res]:
			     {std::tuple{ta, server->selected_protocol},
			      std::tuple{tb, client->selected_protocol}} )
		{
			bool thrown=false;

			try {
				thr->get();
			} catch(...) {
				if (t.negotiated)
					throw;

				thrown=true;
			}

			if (t.negotiated)
			{
				if (!*t.negotiated)
				{
					if (res)
						throw EXCEPTION
							("Expected no "
							 "negotiated protocol "
							 "but got "
							 << *res);
				}
				else if (!res || t.negotiated != *res)
				{
					throw EXCEPTION
						("Expected to "
						 "negotiate ["
						 << t.negotiated
						 << "] but got ["
						 << (res ? *res:"nothing")
						 << "]");
				}
			}
			else if (!thrown)
				throw EXCEPTION("Expected ALPN"
						" negotiation "
						"to fail.");
		}
	}
	alarm(0);

}

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser parser(LIBCXX_NAMESPACE::option::parser::create());

	parser->setOptions(options);

	if (parser->parseArgv(argc, argv) ||
	    parser->validate())
		exit(1);

	try {
		testsession();
		testsession2();
		if (0) // TODO: no rehandshake in TLS 1.3
			testrehandshake();
		testreadabort();
		testwriteabort();

		testalpn();
	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
	}
	return 0;
}
