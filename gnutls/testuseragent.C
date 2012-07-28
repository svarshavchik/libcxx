/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "fdlistener.H"
#include "http/useragent.H"
#include "http/fdtlsserver.H"

class myserverimpl : public LIBCXX_NAMESPACE::gnutls::http::fdtlsserverimpl {

public:
	myserverimpl();
	~myserverimpl() noexcept;

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag);
};

myserverimpl::myserverimpl()
{
}

myserverimpl::~myserverimpl() noexcept
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
	~myserverimplObj() noexcept {}
};

class myserverObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	myserverObj() {}
	~myserverObj() noexcept {}

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

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

		options->addDefaultOptions();

		LIBCXX_NAMESPACE::option::parser opt_parser(LIBCXX_NAMESPACE::option::parser::create());

		opt_parser->setOptions(options);

		if (opt_parser->parseArgv(argc, argv) ||
		    opt_parser->validate())
			exit(0);

		LIBCXX_NAMESPACE::http::useragent::base::https_enable();
		testuseragent();
	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}

