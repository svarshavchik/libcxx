/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "../base/testfdserverimpl.h"
#include "testtlsimpl.h"

#include "x/fdlistener.H"

class myServer : public LIBCXX_NAMESPACE::gnutls::http::fdtlsserverimpl {

public:
	myServer()
	{
	}

	~myServer()
	{
	}

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag) override;
};

void myServer::received(const LIBCXX_NAMESPACE::http::requestimpl &req,
			bool bodyflag)
{
	if (bodyflag) //! Eat any body
	{
		for (iterator b(begin()), e(end()); b != e; ++b)
			;
	}


	LIBCXX_NAMESPACE::http::responseimpl resp(200, "Ok");

	resp.append("Content-Type", "text/plain; charset=\"iso-8859-1\"");

	std::string helloworld("Hello world!\n");

	send(resp, req, helloworld.begin(), helloworld.end());
}

class myServerObj : public myServer, virtual public LIBCXX_NAMESPACE::obj {

public:
	myServerObj() {}
	~myServerObj() {}
};

class myTlsServerObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	myTlsServerObj() {}
	~myTlsServerObj() {}

	LIBCXX_NAMESPACE::ref<myServerObj> create()
	{
		return LIBCXX_NAMESPACE::ref<myServerObj>::create();
	}
};

static void testfdlistener()
{
	std::cout << "Creating a local server" << std::endl;

	LIBCXX_NAMESPACE::fdlistenerptr listener;

	int portnum=4000;

	while (1)
	{
		try {
			listener=LIBCXX_NAMESPACE::fdlistener::create(portnum);
			break;
		} catch (...)
		{
		}

		if (++portnum >= 5000)
			throw EXCEPTION("Cannot create a server");
	}

	std::cout << "Starting listener" << std::endl;
	listener->start(LIBCXX_NAMESPACE::gnutls::http::fdtlsserver::create(),
			LIBCXX_NAMESPACE::ref<testsessionfactory>::create(),
			LIBCXX_NAMESPACE::ref<myTlsServerObj>::create());

	std::cout << "Connecting" << std::endl;

	LIBCXX_NAMESPACE::fd
		sock(LIBCXX_NAMESPACE::netaddr::create("", portnum)->connect());

	LIBCXX_NAMESPACE::gnutls::http::fdtlsclientimpl client;

	client.install(LIBCXX_NAMESPACE::ref<testsessionfactory>::create()
		       ->create(GNUTLS_CLIENT, sock), sock,
		       LIBCXX_NAMESPACE::fdptr(), none);

	LIBCXX_NAMESPACE::http::responseimpl resp;
	LIBCXX_NAMESPACE::http::requestimpl req;

	req.setURI("http://example.com");
	req.setMethod(LIBCXX_NAMESPACE::http::HEAD);

	if (!client.send(req, resp))
		throw EXCEPTION("testfdlistener: send refused");

	std::cout << "HEAD:" << std::endl;
	resp.toString(std::ostreambuf_iterator<char>(std::cout));

	if (req.responseHasMessageBody(resp))
		throw EXCEPTION("Response to a HEAD has a body");

	req.setMethod(LIBCXX_NAMESPACE::http::GET);

	if (!client.send(req, resp))
		throw EXCEPTION("testfdlistener: send refused");

	if (!req.responseHasMessageBody(resp))
		throw EXCEPTION("Response to a GET did not have a body");

	std::cout << "GET:" << std::endl;
	resp.toString(std::ostreambuf_iterator<char>(std::cout));

	std::copy(client.begin(), client.end(),
		  std::ostreambuf_iterator<char>(std::cout));

	std::cout << "Terminating" << std::endl;
}

int main(int argc, char **argv)
{
	try {
		// We have one side of a tls connection bail out and close
		// the socket. It's not an error for the second side of the
		// the tls connection to get a GNUTLS_E_PREMATURE_TERMINATION.

		LIBCXX_NAMESPACE::property::load_property
			(LIBCXX_NAMESPACE_STR
			 "::gnutls::ignore_premature_termination_error",
			 "true", true, true);

		LIBCXX_NAMESPACE::option::list
			options(LIBCXX_NAMESPACE::option::list::create());

		options->addDefaultOptions();

		LIBCXX_NAMESPACE::option::parser
			parser(LIBCXX_NAMESPACE::option::parser::create());

		parser->setOptions(options);

		if (parser->parseArgv(argc, argv) ||
		    parser->validate())
			exit(1);

		testsuite<testtlsimpl>();
		alarm(10);
		testfdlistener();
		alarm(0);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
