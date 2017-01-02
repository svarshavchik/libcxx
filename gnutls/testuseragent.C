/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fdlistener.H"
#include "x/http/useragent.H"
#include "x/http/fdtlsserver.H"
#include "x/gnutls/sessioncache.H"

class myserverimpl : public LIBCXX_NAMESPACE::gnutls::http::fdtlsserverimpl {

public:
	myserverimpl();
	~myserverimpl();

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag);
};

myserverimpl::myserverimpl()
{
}

myserverimpl::~myserverimpl()
{
}

void myserverimpl::received(const LIBCXX_NAMESPACE::http::requestimpl &req,
			   bool bodyflag)
{
	if (req.hasMessageBody())
	{
		for (iterator b(begin()), e(end()); b != e; ++b)
			;
	}

	LIBCXX_NAMESPACE::http::responseimpl resp;

	std::string hello("Hello world!");

	send(resp, req, hello.begin(), hello.end());
}

class myserverimplObj : public myserverimpl, virtual public LIBCXX_NAMESPACE::obj{

public:
	myserverimplObj() {}
	~myserverimplObj() {}
};

class myserverObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	myserverObj() {}
	~myserverObj() {}

	LIBCXX_NAMESPACE::ref<myserverimplObj> create()
	{
		return LIBCXX_NAMESPACE::ref<myserverimplObj>::create();
	}
};


static void testuseragent()
{
	std::list<LIBCXX_NAMESPACE::fd> fdlist;

	LIBCXX_NAMESPACE::netaddr::create("localhost", "")->bind(fdlist, true);

	int portnum=fdlist.front()->getsockname()->port();

	typedef LIBCXX_NAMESPACE::ref<myserverObj> myserver;

	LIBCXX_NAMESPACE::fdlistener
		listener(LIBCXX_NAMESPACE::fdlistener::create(fdlist));

	listener->start(LIBCXX_NAMESPACE::gnutls::http::fdtlsserver::create(),
			LIBCXX_NAMESPACE::gnutls::session::base
			::factory::create
			(LIBCXX_NAMESPACE::gnutls::credentials
			 ::certificate
			 ::create("testrsa3i.crt", "testrsa3.key",
				  GNUTLS_X509_FMT_PEM)),
			myserver::create());


	std::string response;

	for (size_t i=0; i<2; i++)
	{
		LIBCXX_NAMESPACE::http::requestimpl req;

		{
			std::ostringstream o;

			o << "https://localhost:" << portnum;

			req.setURI(o.str());
		}

		LIBCXX_NAMESPACE::http::useragent
			ua(LIBCXX_NAMESPACE::http::useragent::create
			   ( LIBCXX_NAMESPACE::http::noverifycert));

		LIBCXX_NAMESPACE::http::useragent::base::response resp=
			ua->request(req);

		response.append(resp->begin(), resp->end());
	}
	listener->stop();
	listener->wait();

	if (response != "Hello world!" "Hello world!")
		throw EXCEPTION("Hello world test failed");
}

static void showuri(const LIBCXX_NAMESPACE::uriimpl &uri)
{
	auto ua=LIBCXX_NAMESPACE::http::useragent::create();

	bool has_challenge;

	do
	{
		auto resp=ua->request(LIBCXX_NAMESPACE::http::GET, uri);

		std::copy(resp->begin(), resp->end(),
			  std::ostreambuf_iterator<char>(std::cout));

		std::cout << std::flush;

		has_challenge=false;

		for (const auto &challenge:resp->challenges)
		{
			has_challenge=true;

			std::cout << "Authentication required for "
				  << challenge.first << ":" << std::endl
				  << "Userid: " << std::flush;

			std::string userid, password;

			if (std::getline(std::cin, userid).eof())
				return;

			std::cout << "Password: " << std::flush;

			if (std::getline(std::cin, password).eof())
				return;

			ua->set_authorization(resp, challenge.second,
					      userid,
					      password);
		}

		if (!has_challenge)
		{
			std::cout << "Again? " << std::flush;

			std::string yn;

			if (std::getline(std::cin, yn).eof())
				return;

			switch (*yn.c_str()) {
			case 'y':
			case 'Y':
				has_challenge=true;
				break;
			}
		}
	} while (has_challenge);
}

class sessioncacheserverObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::gnutls::sessioncache cache=
		LIBCXX_NAMESPACE::gnutls::sessioncache::create();

	sessioncacheserverObj() {}
	~sessioncacheserverObj() {}

	void run(const LIBCXX_NAMESPACE::fd &socket,
		 const LIBCXX_NAMESPACE::fd &termfd)
	{
		auto combined_socket=
			LIBCXX_NAMESPACE::fdtimeout::create(socket);
		combined_socket->set_terminate_fd(termfd);

		auto sess=LIBCXX_NAMESPACE::gnutls::session
			::create(GNUTLS_SERVER, combined_socket);
		sess->session_cache(cache);

		LIBCXX_NAMESPACE::gnutls::credentials::certificate
			serverCert(LIBCXX_NAMESPACE::gnutls::credentials::certificate::create());

		serverCert->set_x509_keyfile("testrsa1.crt", "testrsa1.key",
					     GNUTLS_X509_FMT_PEM);
		sess->credentials_set(serverCert);
		sess->set_default_priority();

		LIBCXX_NAMESPACE::gnutls::dhparams dh(LIBCXX_NAMESPACE::gnutls::dhparams::create());
		LIBCXX_NAMESPACE::gnutls::datum_t dh_dat(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		dh_dat->load("dhparams.dat");
		dh->import_pk(dh_dat, GNUTLS_X509_FMT_PEM);
		serverCert->set_dh_params(dh);

		int direction;

		sess->handshake(direction);

		std::string line;

		std::getline(*sess->getistream(), line);
		sess->bye(direction);
	}
};

void testsessioncache()
{
	std::list<LIBCXX_NAMESPACE::fd> fdlist;

	LIBCXX_NAMESPACE::netaddr::create("localhost", "")->bind(fdlist, true);

	int portnum=fdlist.front()->getsockname()->port();

	typedef LIBCXX_NAMESPACE::ref<sessioncacheserverObj> myserver;

	auto listener=LIBCXX_NAMESPACE::fdlistener::create(fdlist);

	listener->start(myserver::create());

	auto sock=LIBCXX_NAMESPACE::netaddr::create("localhost", portnum)->connect();

	auto sess=LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_CLIENT, sock);
	sess->credentials_set(LIBCXX_NAMESPACE::gnutls::credentials::certificate::create());
	sess->set_default_priority();

	int direction;
	sess->handshake(direction);
	if (sess->session_resumed())
		throw EXCEPTION("Resumed from what?");
	auto data=sess->get_session_data();
	sess->bye(direction);

	sock=LIBCXX_NAMESPACE::netaddr::create("localhost", portnum)->connect();

	sess=LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_CLIENT, sock);
	sess->credentials_set(LIBCXX_NAMESPACE::gnutls::credentials::certificate::create());
	sess->set_default_priority();
	sess->set_session_data(data);
	sess->handshake(direction);
	if (!sess->session_resumed())
		throw EXCEPTION("I didn't resume!");
	sess->bye(direction);

	listener->stop();
	listener->wait();
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::option::string_value
			uri(LIBCXX_NAMESPACE::option::string_value::create());

		LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

		options->add(uri, 'u', "uri",
			     LIBCXX_NAMESPACE::option::list::base::hasvalue,
			     "uri",
			     "uri");

		options->addDefaultOptions();

		LIBCXX_NAMESPACE::option::parser opt_parser(LIBCXX_NAMESPACE::option::parser::create());

		opt_parser->setOptions(options);

		if (opt_parser->parseArgv(argc, argv) ||
		    opt_parser->validate())
			exit(0);

		LIBCXX_NAMESPACE::http::useragent::base::https_enable();

		if (uri->isSet())
		{
			showuri(uri->value);
			return 0;
		}

		testuseragent();
		testsessioncache();
	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}
