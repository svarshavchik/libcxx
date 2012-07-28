/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "gnutls/exceptions.H"
#include "gnutls/session.H"
#include "gnutls/rdn.H"
#include "fd.H"
#include "exception.H"
#include "gnutls/x509_crtobj.H"
#include "x/options.H"
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <poll.h>

class serverThread : virtual public LIBCXX_NAMESPACE::obj {

public:
	void run(const LIBCXX_NAMESPACE::fd &fd);
};

class clientThread : virtual public LIBCXX_NAMESPACE::obj {

public:
	void run(const LIBCXX_NAMESPACE::fd &fd);
};

class credCallback : public LIBCXX_NAMESPACE::gnutls::credentials::callbackObj {

public:
	bool expecthostname;

	credCallback() noexcept;
	~credCallback() noexcept;

	keycertptr get(const LIBCXX_NAMESPACE::gnutls::sessionObj *sess,
		       const std::list<std::string> &hostname_list,
		       const std::vector<gnutls_pk_algorithm_t> &algos,
		       const gnutls_datum_t *req_ca_dn,
		       size_t n_req_ca_dn);
};

credCallback::credCallback() noexcept
	: expecthostname(false)
{
}

credCallback::~credCallback() noexcept
{
}

credCallback::keycertptr
credCallback::get(const LIBCXX_NAMESPACE::gnutls::sessionObj *sess,
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

void serverThread::run(const LIBCXX_NAMESPACE::fd &fd)

{
	int direction;

	std::list<LIBCXX_NAMESPACE::gnutls::x509::crt> calist;

	LIBCXX_NAMESPACE::gnutls::x509::crt::base::import_cert_list(calist, "testrsa1.crt",
					       GNUTLS_X509_FMT_PEM);

	LIBCXX_NAMESPACE::ptr<credCallback> cred(LIBCXX_NAMESPACE::ptr<credCallback>::create());

	cred->expecthostname=true;

	LIBCXX_NAMESPACE::gnutls::credentials::certificate
		serverCert(LIBCXX_NAMESPACE::gnutls::credentials::certificate::create());

	serverCert->set_x509_trust_file("testrsa1.crt", GNUTLS_X509_FMT_PEM);

	LIBCXX_NAMESPACE::gnutls::session sess(LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_SERVER,
							   fd));
	sess->credentials_set(serverCert);
	serverCert->set_callback(cred);

	sess->server_set_certificate_request();

	sess->set_default_priority();

	{
		LIBCXX_NAMESPACE::gnutls::rsaparams rsa(LIBCXX_NAMESPACE::gnutls::rsaparams::create());

		LIBCXX_NAMESPACE::gnutls::datum_t rsa_dat(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		rsa_dat->load("rsaparams.dat");
		rsa->import_pk(rsa_dat, GNUTLS_X509_FMT_PEM);

		serverCert->set_rsa_params(rsa);

		LIBCXX_NAMESPACE::gnutls::dhparams dh(LIBCXX_NAMESPACE::gnutls::dhparams::create());
		LIBCXX_NAMESPACE::gnutls::datum_t dh_dat(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		dh_dat->load("dhparams.dat");
		dh->import_pk(dh_dat, GNUTLS_X509_FMT_PEM);
		serverCert->set_dh_params(dh);
	}

	try {
		int direction;

		sess->handshake(direction);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "server handshake failed: " << e << std::endl;
		throw;
	}

	std::list<LIBCXX_NAMESPACE::gnutls::x509::crt> clientCerts;

	sess->get_peer_certificates(clientCerts);

	if (clientCerts.size() != 2
	    || clientCerts.front()->get_dn_hostname() != "*.domain.com")
	{
		std::cerr << "Did not receive expected client cert"
			  << std::endl;
		throw EXCEPTION("Did not receive expected client cert");
	}

	LIBCXX_NAMESPACE::iostream si(sess->getiostream());

	std::string buf;

	std::getline(*si, buf);

	if (buf != "Hello world")
		throw EXCEPTION("Did not read hello world!\n");

	if (si->get() != EOF)
		throw EXCEPTION("Received unexpected data");
	sess->bye(direction);
}

void clientThread::run(const LIBCXX_NAMESPACE::fd &fd)

{
	int direction;

	LIBCXX_NAMESPACE::ptr<credCallback> cred(LIBCXX_NAMESPACE::ptr<credCallback>::create());

	LIBCXX_NAMESPACE::gnutls::credentials::certificate
		clientCert(LIBCXX_NAMESPACE::gnutls::credentials::certificate::create());

	clientCert->set_x509_trust_file("testrsa1.crt", GNUTLS_X509_FMT_PEM);

	LIBCXX_NAMESPACE::gnutls::session sess(LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_CLIENT, fd));

	clientCert->set_callback(cred);
	sess->credentials_set(clientCert);
	sess->set_server_name("nobody.example.com");
	sess->set_default_priority();

	try {
		sess->handshake(direction);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "client handshake failed: "
			  << e << std::endl;
		throw;
	}

	std::cout << "Session: " << sess->getSuite() << std::endl;

	try {
		sess->verify_peer("subcert.domain.com");
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "Server certification verification failed: "
			  << e << std::endl;
		throw;
	}

	{
		std::list<LIBCXX_NAMESPACE::gnutls::x509::crt> certList;

		sess->get_peer_certificates(certList);

		std::list<LIBCXX_NAMESPACE::gnutls::x509::crt>::iterator
			b=certList.begin(),
			e=certList.end();

		while (b != e)
			std::cout << (*b++)->print();
	}

	LIBCXX_NAMESPACE::iostream ios(sess->getiostream());

	(*ios) << "Hello world" << std::endl << std::flush;

	if (!ios->good())
		throw EXCEPTION("Send failed");
	sess->bye(direction);
}

static void testsession()
{
	alarm(10);

	LIBCXX_NAMESPACE::fdptr a,b;

	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p(LIBCXX_NAMESPACE::fd::base::socketpair());

		a=p.first;
		b=p.second;
	}


	LIBCXX_NAMESPACE::ref<serverThread> server_threadObj=LIBCXX_NAMESPACE::ref<serverThread>::create();
	LIBCXX_NAMESPACE::ref<clientThread> client_threadObj=LIBCXX_NAMESPACE::ref<clientThread>::create();

	auto ta=LIBCXX_NAMESPACE::run(server_threadObj, a),
		tb=LIBCXX_NAMESPACE::run(client_threadObj, b);

	ta->wait();
	tb->wait();
	alarm(0);
}

static void testsession2()
{
	LIBCXX_NAMESPACE::fd tmpfile=LIBCXX_NAMESPACE::fd::base::tmpfile();

	LIBCXX_NAMESPACE::gnutls::session sess(LIBCXX_NAMESPACE::gnutls::session::create
				(GNUTLS_SERVER, tmpfile));

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
		LIBCXX_NAMESPACE::gnutls::rsaparams rsa(LIBCXX_NAMESPACE::gnutls::rsaparams::create());

		LIBCXX_NAMESPACE::gnutls::datum_t rsa_dat(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		rsa_dat->load("rsaparams.dat");
		rsa->import_pk(rsa_dat, GNUTLS_X509_FMT_PEM);

		serverCert->set_rsa_params(rsa);

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

		pfd.fd= fd->getFd();
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

		pfd.fd= fd->getFd();
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

				pfd.fd= fd->getFd();
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

		pfd.fd= fd->getFd();
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

		pfd.fd= fd->getFd();
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

		pfd.fd= fd->getFd();
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

			pfd.fd= b->getFd();
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

					pfd.fd= b->getFd();
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

			pfd.fd= b->getFd();
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

			pfd.fd=b->getFd();
			pfd.events=direction;
			poll(&pfd, 1, -1);
		}

		while ((rc=sess->recv(&dummy, 1, direction)) == 0)
		{
			struct pollfd pfd;

			pfd.fd= b->getFd();
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

			pfd.fd= b->getFd();
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
		LIBCXX_NAMESPACE::gnutls::rsaparams rsa(LIBCXX_NAMESPACE::gnutls::rsaparams::create());

		LIBCXX_NAMESPACE::gnutls::datum_t rsa_dat(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		rsa_dat->load("rsaparams.dat");
		rsa->import_pk(rsa_dat, GNUTLS_X509_FMT_PEM);

		serverCert->set_rsa_params(rsa);

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

		pfd.fd= fd->getFd();
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

		pfd.fd= fd->getFd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	while ((rc=sess->recv(&dummy, 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= fd->getFd();
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

		pfd.fd= b->getFd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	while ((rc=sess->recv(&dummy, 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= b->getFd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	a=LIBCXX_NAMESPACE::fdptr();

	if (dummy != 'B')
		throw EXCEPTION("Unexpected response from server");

	while ((rc=sess->send("C", 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= b->getFd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	try {
		while ((rc=sess->recv(&dummy, 1, direction)) == 0)
		{
			struct pollfd pfd;
			
			pfd.fd= b->getFd();
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
		LIBCXX_NAMESPACE::gnutls::rsaparams rsa(LIBCXX_NAMESPACE::gnutls::rsaparams::create());

		LIBCXX_NAMESPACE::gnutls::datum_t rsa_dat(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		rsa_dat->load("rsaparams.dat");
		rsa->import_pk(rsa_dat, GNUTLS_X509_FMT_PEM);

		serverCert->set_rsa_params(rsa);

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

		pfd.fd= fd->getFd();
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
		struct pollfd pfd;

		pfd.fd= fd->getFd();
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

		pfd.fd= b->getFd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	while ((rc=sess->recv(&dummy, 1, direction)) == 0)
	{
		struct pollfd pfd;

		pfd.fd= b->getFd();
		pfd.events=direction;
		poll(&pfd, 1, -1);
	}

	a=LIBCXX_NAMESPACE::fdptr();

	if (dummy != 'B')
		throw EXCEPTION("Unexpected response from server");

	server_thread->wait();

	try {
		while ((rc=sess->send("C", 1, direction)) == 0)
		{
			struct pollfd pfd;

			pfd.fd= b->getFd();
			pfd.events=direction;
			poll(&pfd, 1, -1);
		}
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "write abort correctly handled" << std::endl;
		return;
	}

	throw EXCEPTION("Write abort did not happen");
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
		testrehandshake();
		testreadabort();
		testwriteabort();
	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
	}
	return 0;
}

