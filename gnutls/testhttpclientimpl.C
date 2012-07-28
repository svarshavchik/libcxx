/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "../base/testfdclientimpl.h"
#include "testtlsimpl.h"
#include "threads/run.H"
#include "http/fdtlsclient.H"
#include <iostream>

class myFakeErrorProxy : virtual public LIBCXX_NAMESPACE::obj {

public:

	void run(const LIBCXX_NAMESPACE::fd &filedesc);
};

void myFakeErrorProxy::run(const LIBCXX_NAMESPACE::fd &filedesc)
{
	{
		LIBCXX_NAMESPACE::iostream i(filedesc->getiostream());

		std::string line;

		if (std::getline(*i, line).eof())
			return;

		std::cout << line << std::endl << std::flush;

		do
		{
			if (std::getline(*i, line).eof())
				return;
			std::cout << line << std::endl << std::flush;
		} while (line != "\r");

		std::cout << "Sending 404" << std::endl << std::flush;

		(*i) <<
			"HTTP/1.1 404 Not found\r\n"
			"Content-Type: text/plain\r\n"
			"\r\n"
			"Host not found\r\n"
		     << std::flush;
	}
}

static void testhttpsproxyerror()
{
	LIBCXX_NAMESPACE::fdptr a, b;

	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p(LIBCXX_NAMESPACE::fd::base::socketpair());

		a=p.first;
		b=p.second;
	}

	auto thr=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<myFakeErrorProxy>
				     ::create(), a);

	a=LIBCXX_NAMESPACE::fdptr();

	LIBCXX_NAMESPACE::gnutls::http::fdtlsclient
		client(LIBCXX_NAMESPACE::gnutls::http::fdtlsclient::create());

	client->install(LIBCXX_NAMESPACE::ref<testsessionfactory>::create()
			->create(GNUTLS_CLIENT, b), b,
			LIBCXX_NAMESPACE::fdptr(), LIBCXX_NAMESPACE::http::isproxy);

	LIBCXX_NAMESPACE::http::requestimpl req;
	req.setURI("https://example.com");

	LIBCXX_NAMESPACE::http::responseimpl resp;

	if (!client->send(req, resp))
		throw EXCEPTION("Was not able to send a request");

	resp.toString(std::ostreambuf_iterator<char>(std::cout));
	try {
		if (req.responseHasMessageBody(resp))
			std::copy(client->begin(), client->end(),
				  std::ostreambuf_iterator<char>(std::cout));
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
	}

	if (resp.getStatusCode() != 404)
		throw EXCEPTION("I expected a 404");

	b=LIBCXX_NAMESPACE::fdptr();
	thr->wait();
}

class myFakeContentProxy : virtual public LIBCXX_NAMESPACE::obj {

public:

	void run(const LIBCXX_NAMESPACE::fd &filedesc);
};

void myFakeContentProxy::run(const LIBCXX_NAMESPACE::fd &filedesc)
{
	{
		LIBCXX_NAMESPACE::iostream i(filedesc->getiostream());

		std::string line;

		if (std::getline(*i, line).eof())
			return;

		std::cout << line << std::endl << std::flush;

		do
		{
			if (std::getline(*i, line).eof())
				return;
			std::cout << line << std::endl << std::flush;
		} while (line != "\r");

		std::cout << "Sending proxy 200" << std::endl << std::flush;

		(*i) <<
			"HTTP/1.1 200 Ok\r\n"
			"\r\n"
		     << std::flush;
	}

	LIBCXX_NAMESPACE::gnutls::session sess=
		LIBCXX_NAMESPACE::ptr<testsessionfactory>(LIBCXX_NAMESPACE::ptr<testsessionfactory>
					   ::create())->
		create(GNUTLS_SERVER, filedesc);

	int dummy;

	if (!sess->handshake(dummy))
	{
		std::cerr << "TLS negotiation failed on the server side"
			  << std::endl;
		return;
	}

	LIBCXX_NAMESPACE::iostream i(sess->getiostream());

	std::string line;

	if (std::getline(*i, line).eof())
		return;

	std::cout << line << std::endl << std::flush;

	do
	{
		if (std::getline(*i, line).eof())
			return;
		std::cout << line << std::endl << std::flush;
	} while (line != "\r");

	std::cout << "Sending content 200" << std::endl << std::flush;

	(*i) << "HTTP/1.1 200 Ok\r\n"
		"Connection: close\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n"
		"Hello world\n"
	     << std::flush;

	try {
		sess->bye(dummy);
	} catch (...) {
	}
}

static void testhttpsproxyconnect()
{
	LIBCXX_NAMESPACE::fdptr a, b;

	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p(LIBCXX_NAMESPACE::fd::base::socketpair());

		a=p.first;
		b=p.second;
	}

	auto thr=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE
				     ::ref<myFakeContentProxy>::create(),
				     a);

	a=LIBCXX_NAMESPACE::fdptr();

	{
		LIBCXX_NAMESPACE::gnutls::http::fdtlsclientimpl client;

		client.install(LIBCXX_NAMESPACE::ref<testsessionfactory>::create()
			       ->create(GNUTLS_CLIENT, b), b,
			       LIBCXX_NAMESPACE::fdptr(),
			       LIBCXX_NAMESPACE::http::isproxy);

		LIBCXX_NAMESPACE::http::requestimpl req;
		req.setURI("https://example.com");

		LIBCXX_NAMESPACE::http::responseimpl resp;

		if (!client.send(req, resp))
			throw EXCEPTION("Was not able to send a request");

		resp.toString(std::ostreambuf_iterator<char>(std::cout));

		try {
			if (req.responseHasMessageBody(resp))
				std::copy(client.begin(), client.end(),
					  std::ostreambuf_iterator<char>(std::cout));
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cerr << e << std::endl;
		}
		if (resp.getStatusCode() != 200)
			throw EXCEPTION("I expected a 200");

		if (client.send(req, resp))
			throw EXCEPTION("Did not expect to be able to send a second request");

		b=LIBCXX_NAMESPACE::fdptr();
	}
	thr->wait();
}

static void dorequest(const std::string &url,
		      int opts,
		      const std::string &proxy)
{
	LIBCXX_NAMESPACE::gnutls::http::fdtlsclientimpl client;

	LIBCXX_NAMESPACE::gnutls::credentials::certificate
                cert(LIBCXX_NAMESPACE::gnutls::credentials::certificate
		     ::create());

        cert->set_x509_trust_default();

	LIBCXX_NAMESPACE::http::requestimpl req;
	req.setURI(url);

	if (proxy.size() > 0)
	{
		size_t p=proxy.find(':');

		if (p == proxy.npos)
			throw EXCEPTION("Bad host:proxy");

		opts |= LIBCXX_NAMESPACE::http::isproxy;

		auto sock=LIBCXX_NAMESPACE::netaddr
			::create(proxy.substr(0, p),
				 proxy.substr(p+1))->connect();

		client.install(LIBCXX_NAMESPACE::ref<testsessionfactory>
			       ::create()
			       ->create(GNUTLS_CLIENT, sock),
			       LIBCXX_NAMESPACE::fdptr(), sock, opts);
	}
	else
	{
		auto hostport=req.getURI().getHostPort();

		auto sock=LIBCXX_NAMESPACE::netaddr
			::create(hostport.first, hostport.second)
			->connect();

		client.install(LIBCXX_NAMESPACE::ref<testsessionfactory>
			       ::create()
			       ->create(GNUTLS_CLIENT, sock),
			       LIBCXX_NAMESPACE::fdptr(), sock, opts);

	}

	LIBCXX_NAMESPACE::http::responseimpl resp;

	if (!client.send(req, resp))
		throw EXCEPTION("Was not able to send a request");

	resp.toString(std::ostreambuf_iterator<char>(std::cout));
	if (req.responseHasMessageBody(resp))
		std::copy(client.begin(), client.end(),
			  std::ostreambuf_iterator<char>(std::cout));
}

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::string_value
		proxy_value(LIBCXX_NAMESPACE::option::string_value::create());

	LIBCXX_NAMESPACE::option::int_value
		opts_value(LIBCXX_NAMESPACE::option::int_value::create(0));

	LIBCXX_NAMESPACE::option::or_op<int>::value
		noverifypeer_value(LIBCXX_NAMESPACE::option::or_op<int>::value::create(opts_value, LIBCXX_NAMESPACE::http::noverifypeer)),
		noverifycert_value(LIBCXX_NAMESPACE::option::or_op<int>::value::create(opts_value, LIBCXX_NAMESPACE::http::noverifycert));

	LIBCXX_NAMESPACE::option::list
		options(LIBCXX_NAMESPACE::option::list::create());

	options->add(noverifypeer_value, 0,
		     L"noverifypeer",
		     0,
		     L"Do not verify TLS peer's certificate's name")
		.add(noverifycert_value, 0,
		     L"noverifycert",
		     0,
		     L"Do not verify TLS peer's certificate at all")
		.add(proxy_value, 0,
		     L"proxy",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Set the proxy",
		     L"host:port")
		.addArgument(L"url", 0)
		.addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser
		parser(LIBCXX_NAMESPACE::option::parser::create());

	parser->setOptions(options);

	if (parser->parseArgv(argc, argv) ||
	    parser->validate())
		exit(1);

	if (parser->args.size() > 0)
	{
		dorequest(parser->args.front(),
			  opts_value->value,
			  proxy_value->value);
		return(0);
	}

	testsuite<testtlsimpl>();
	alarm(15);

	testhttpsproxyerror();
	testhttpsproxyconnect();
	alarm(0);
	return 0;
}

