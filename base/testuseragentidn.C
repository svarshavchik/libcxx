/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/useragent.H"
#include "x/http/fdserver.H"
#include "x/http/form.H"
#include "x/http/cookie.H"
#include "x/http/cookiejar.H"
#include "x/http/upload.H"
#include "x/fdlistener.H"
#include "x/eventdestroynotify.H"
#include "x/netaddr.H"
#include "x/options.H"
#include "x/property_properties.H"
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <sys/syscall.h>

#define LIBCXX_DEBUG_GETADDRINFO(c) \
	(std::string(c).substr(0, 8) == "www.xn--" ? "localhost":(c))

#include "netaddrobj.C"

class myfdserverObj : public LIBCXX_NAMESPACE::http::fdserverimpl,
		      virtual public LIBCXX_NAMESPACE::obj {

public:
	myfdserverObj();
	~myfdserverObj() noexcept;

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag);

	x::ref<myfdserverObj> create() { return x::ref<myfdserverObj>(this); }
};


myfdserverObj::myfdserverObj()
{
}

myfdserverObj::~myfdserverObj() noexcept
{
}

void myfdserverObj::received(const LIBCXX_NAMESPACE::http::requestimpl &req,
			     bool bodyflag)
{
	std::cout << "In received" << std::endl;

	if (req.hasMessageBody())
		for (iterator b(begin()), e(end()); b != e; ++b)
			;

	std::cout << "Getting URI" << std::endl;

	LIBCXX_NAMESPACE::uriimpl uri=req.getURI();

	if (uri.getPath() == "/")
		uri.setPath("");

	std::string uri_str=LIBCXX_NAMESPACE::tostring(uri);

	std::cout << "Sending" << std::endl;
	send(req, "text/plain; charset=UTF-8", uri_str.begin(), uri_str.end());
	std::cout << "Sent" << std::endl;
}

#define I18NAME "http://www.r\xc3\xa4" \
	"ksm\xc3\xb6"		       \
	"rg\xc3\xa5"		       \
	"sa.example:"

template<typename server_type>
static std::string createlistener(LIBCXX_NAMESPACE::fdlistenerptr &listener,
				  const server_type &precreated_server)
{
	std::list<LIBCXX_NAMESPACE::fd> fdlist;

	LIBCXX_NAMESPACE::netaddr::create("localhost", "")->bind(fdlist, true);

	int portnum=fdlist.front()->getsockname()->port();

	listener=LIBCXX_NAMESPACE::fdlistener::create(fdlist, 100);
	listener->start(LIBCXX_NAMESPACE::http::fdserver::create(),
			precreated_server);

	std::ostringstream o;

	o << I18NAME << portnum;

	return o.str();
}

static void test1req()
{
	LIBCXX_NAMESPACE::fdlistenerptr listener;
	auto server=LIBCXX_NAMESPACE::ref<myfdserverObj>::create();

	std::string serveraddr=createlistener(listener, server);

	std::cout << "Started a listener" << std::endl;

	LIBCXX_NAMESPACE::http::useragent ua(LIBCXX_NAMESPACE::http::useragent
					   ::create());

	std::cout << "Sending a test request" << std::endl;

	LIBCXX_NAMESPACE::http::useragent::base::response
		resp=ua->request(LIBCXX_NAMESPACE::http::GET,
				 LIBCXX_NAMESPACE
				 ::uriimpl(serveraddr));

	std::string body=std::string(resp->begin(), resp->end());

	auto uri=LIBCXX_NAMESPACE::tostring(resp->uri);
	std::cout << "Stopping listener" << std::endl;
	std::cout << uri << std::endl;
	std::cout << body << std::endl;
	listener->stop();
	listener->wait();
	if (resp->message.getStatusCode() != 200)
		throw EXCEPTION("Test request failed");
	if (uri != body)
		throw EXCEPTION("Uris don't match");

	std::string expected="http://www.xn--rksmrgsa-0zap8p.example:";

	if (uri.substr(0, expected.size()) != expected)
		throw EXCEPTION("Uri other than expected");

	std::string uri_utf8 =
		resp->uri.toStringi18n(LIBCXX_NAMESPACE::locale::base::utf8());

	if (uri_utf8.substr(0, sizeof(I18NAME)-1) != I18NAME)
		throw EXCEPTION("Unexpected result of URI conversion back to UTF8");
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::option::list
			options(LIBCXX_NAMESPACE::option::list::create());

		options->addDefaultOptions();

		LIBCXX_NAMESPACE::option::parser
			opt_parser(LIBCXX_NAMESPACE::option::parser::create());

		opt_parser->setOptions(options);
		int err=opt_parser->parseArgv(argc, argv);

		if (err == 0) err=opt_parser->validate();

		if (err)
		{
			if (err == LIBCXX_NAMESPACE::option::parser::base
			    ::err_builtin)
				exit(0);

			std::cerr << opt_parser->errmessage();
			std::cerr.flush();
			exit(1);
		}

		test1req();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
