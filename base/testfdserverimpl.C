/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ref.H"
#include "testfdserverimpl.h"
#include "testimpl.h"
#include "x/netaddr.H"
#include "x/fdlistener.H"
#include "x/http/fdserver.H"

class myServerObj : public LIBCXX_NAMESPACE::http::fdserverimpl,
		    virtual public LIBCXX_NAMESPACE::obj {

public:
	myServerObj()
	{
	}

	~myServerObj() noexcept
	{
	}

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag);
};

void myServerObj::received(const LIBCXX_NAMESPACE::http::requestimpl &req,
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

class myfdserverObj : virtual public LIBCXX_NAMESPACE::obj {

public:
       myfdserverObj() {}
       ~myfdserverObj() noexcept {}

       LIBCXX_NAMESPACE::ref<myServerObj> create()
       {
               return LIBCXX_NAMESPACE::ref<myServerObj>::create();
       }
};

static void testfdlistener()
{
	std::cout << "Creating a local server" << std::endl;

	LIBCXX_NAMESPACE::ref<myfdserverObj>
		server(LIBCXX_NAMESPACE::ref<myfdserverObj>::create());

	LIBCXX_NAMESPACE::fdlistenerptr listener;

	int portnum;

	{
		std::list<LIBCXX_NAMESPACE::fd> fdlist;

		LIBCXX_NAMESPACE::netaddr::create("localhost", "")
			->bind(fdlist, true);

		portnum=fdlist.front()->getsockname()->port();

		listener=LIBCXX_NAMESPACE::fdlistener::create(fdlist);
	}

	std::cout << "Starting listener" << std::endl;
	listener->start(LIBCXX_NAMESPACE::http::fdserver::create(),
			LIBCXX_NAMESPACE::ref<myfdserverObj>::create());

	std::cout << "Connecting" << std::endl;
	LIBCXX_NAMESPACE::http::fdclientimpl client;

	client.install(LIBCXX_NAMESPACE::netaddr::create("", portnum)->connect(),
		       LIBCXX_NAMESPACE::fdptr());

	LIBCXX_NAMESPACE::http::responseimpl resp;
	LIBCXX_NAMESPACE::http::requestimpl req;

	req.setURI("http://localhost");
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

void testpipelinetimeout()
{
	std::cout << "Creating a local server" << std::endl;

	LIBCXX_NAMESPACE::ref<myfdserverObj>
		server(LIBCXX_NAMESPACE::ref<myfdserverObj>::create());

	LIBCXX_NAMESPACE::fdlistenerptr listener;

	int portnum;

	{
		std::list<LIBCXX_NAMESPACE::fd> fdlist;

		LIBCXX_NAMESPACE::netaddr::create("localhost", "")
			->bind(fdlist, true);

		portnum=fdlist.front()->getsockname()->port();

		listener=LIBCXX_NAMESPACE::fdlistener::create(fdlist);
	}

	std::cout << "Starting listener" << std::endl;
	listener->start(LIBCXX_NAMESPACE::http::fdserver::create(), server);

	std::cout << "Connecting" << std::endl;

	LIBCXX_NAMESPACE::fd
		sockfd(LIBCXX_NAMESPACE::netaddr::create("", portnum)->connect());

	LIBCXX_NAMESPACE::http::fdclientimpl client;

	client.install(sockfd, LIBCXX_NAMESPACE::fdptr());

	LIBCXX_NAMESPACE::http::responseimpl resp;
	LIBCXX_NAMESPACE::http::requestimpl req;

	req.setURI("http://localhost");
	req.setMethod(LIBCXX_NAMESPACE::http::HEAD);

	if (!client.send(req, resp))
		throw EXCEPTION("testfdlistener: send refused");

	if (req.responseHasMessageBody(resp))
		throw EXCEPTION("Response to a HEAD has a body");

	std::cout << "Waiting for the server to time out" << std::endl;

	char buf;

	sockfd->read(&buf, 1);
}

int main(int argc, char **argv)
{
	try {
		testsuite<testimpl>();
		alarm(10);
		testfdlistener();
		LIBCXX_NAMESPACE::property::load_property
			(LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE
						   ::property::propvalue,
						   std::string>
			 ::tostr(LIBCXX_NAMESPACE_STR
				 "::http::server::pipeline_timeout"),
			 LIBCXX_NAMESPACE::stringize<LIBCXX_NAMESPACE
						   ::property::propvalue,
						   std::string>
			 ::tostr("2"), true, true);
		alarm(10);
		testpipelinetimeout();
		alarm(0);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
