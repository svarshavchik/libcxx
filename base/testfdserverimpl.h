/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/fdserver.H"
#include "x/http/fdclientimpl.H"
#include "x/threads/run.H"

#ifndef FORCE_PROP
#define FORCE_PROP(n, v)						\
	LIBCXX_NAMESPACE::property::load_property(n, v, true, true)
#endif

template<typename conn_type>
void testhttpnull()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	typename conn_type::fdserver
		server(conn_type::fdserver::create());

	auto thr=sserver->runserver(server);

	{
		typename conn_type::fdclientimpl tempclient;

		sclient->install(tempclient);
	}

	sserver=typename conn_type::filedesc();
	sclient=typename conn_type::filedesc();

	thr->wait();  // fdtlsserverimpl throws an exception in handshake()
}

template<typename conn_type>
class testhttp10_serverObj : public conn_type::fdserverObj {

public:
	testhttp10_serverObj()
	{
	}

	~testhttp10_serverObj()
	{
	}

	template<typename ...Args>
	void run(Args && ...args)
	{
		try {
			conn_type::fdserverObj::run(std::forward<Args>(args)...);
		} catch (LIBCXX_NAMESPACE::exception &e)
		{
			std::cout << "http10 thread terminated by an exception: "
				  << e << std::endl;
			throw;
		}
	}

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag) override
	{
		if (bodyflag)
			this->discardbody();

		LIBCXX_NAMESPACE::http::responseimpl resp;

		resp.setVersion(LIBCXX_NAMESPACE::http::httpver_t::http11);
		resp.setStatusCode(200);

		std::list<char> dummy;

		dummy.push_back('y');
		dummy.push_back('\n');

		this->send(resp, req, dummy.begin(), dummy.end());
	};
};

template<typename conn_type>
void testhttp10()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	auto server=LIBCXX_NAMESPACE::ref<testhttp10_serverObj<conn_type> >
		::create();

	auto thr=sserver->runserver(server);

	{
		typename conn_type::fdclientimpl cl;

		sclient->install(cl);

		sclient=typename conn_type::filedesc();
		sserver=typename conn_type::filedesc();

		LIBCXX_NAMESPACE::http::requestimpl req;

		req.setVersion(LIBCXX_NAMESPACE::http::httpver_t::http10);
		req.setURI("http://example.com");

		LIBCXX_NAMESPACE::http::responseimpl resp;

		if (!cl.send(req, resp))
			throw EXCEPTION("testhttp10: send refused");

		resp.toString(std::ostreambuf_iterator<char>(std::cout));

		if (req.responseHasMessageBody(resp))
		{
			std::copy(cl.begin(), cl.end(),
				  std::ostreambuf_iterator<char>(std::cout));
		}

		req=LIBCXX_NAMESPACE::http::requestimpl();
		req.setVersion(LIBCXX_NAMESPACE::http::httpver_t::http10);
		req.setURI("http://example.com");

		if (cl.send(req, resp))
			throw EXCEPTION("testhttp10: message sent when it should not have been");
	}

	thr->get();
	std::cout << "http10 thread terminated normally" << std::endl;
}

template<typename conn_type>
void testhttp11()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	typename conn_type::fdserver server(conn_type::fdserver::create());

	auto thr=sserver->runserver(server);

	{
		typename conn_type::fdclientimpl cl;

		sclient->install(cl);

		sclient=typename conn_type::filedesc();
		sserver=typename conn_type::filedesc();

		for (size_t i=0; i<2; i++)
		{
			LIBCXX_NAMESPACE::http::requestimpl req;

			req.setVersion(LIBCXX_NAMESPACE::http::httpver_t::http11);
			req.setURI("http://example.com");

			if (i)
				req.replace("Connection","close");

			LIBCXX_NAMESPACE::http::responseimpl resp;

			if (!cl.send(req, resp))
				throw EXCEPTION("testhttp11: send refused");

			resp.toString(std::ostreambuf_iterator<char>(std::cout));

			if (req.responseHasMessageBody(resp))
			{
				std::copy(cl.begin(), cl.end(),
					  std::ostreambuf_iterator<char>(std::cout));
			}
		}

		LIBCXX_NAMESPACE::http::requestimpl req;
		LIBCXX_NAMESPACE::http::responseimpl resp;

		req.setVersion(LIBCXX_NAMESPACE::http::httpver_t::http11);
		req.setURI("http://example.com");

		if (cl.send(req, resp))
			throw EXCEPTION("testhttp11: message sent when it should not have been");
	}

	thr->get();
}


template<typename conn_type>
void fillpipe(const typename conn_type::filedesc &fd)
{
	fd->get_fd()->nonblock(true);

	char dummy=0;

	while (fd->get_fd()->write(&dummy, 1) > 0)
		;
	fd->get_fd()->nonblock(false);
}

template<typename conn_type>
void testhttpsendmsgerror()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	fillpipe<conn_type>(sclient);

	sserver=typename conn_type::filedesc();

	typename conn_type::fdclientimpl cl;

	sclient->install(cl);

	cl.setPeerHttp11();

	LIBCXX_NAMESPACE::http::requestimpl req;

	req.setVersion(LIBCXX_NAMESPACE::http::httpver_t::http11);
	req.setURI("http://example.com");
	LIBCXX_NAMESPACE::http::responseimpl resp;

	bool caught=false;

	try
	{
		cl.send(req, resp);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("testhttpmessageerror: did not catch an exception");
}

template<typename conn_type>
class endlessinputiter : public std::iterator<std::input_iterator_tag, char> {

public:
	endlessinputiter() {}
	~endlessinputiter() {}

	char operator*() { return 0; }

	endlessinputiter<conn_type> &operator++() { return *this; }

	endlessinputiter<conn_type> &operator++(int) { return *this; }

	bool operator==(const endlessinputiter<conn_type> &o)
	{
		return false;
	}

	bool operator!=(const endlessinputiter<conn_type> &o)
	{
		return false;
	}
};

template<typename conn_type>
class testhttpresponsemsgerrorObj : public conn_type::fdserverObj {

public:
	testhttpresponsemsgerrorObj()
	{
	}

	~testhttpresponsemsgerrorObj()
	{
	}

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag) override
	{
		throw EXCEPTION("Dummy");
	}
};

template<typename conn_type>
void testhttpresponsemsgerror()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	LIBCXX_NAMESPACE::ref<testhttpresponsemsgerrorObj<conn_type> >
		server(LIBCXX_NAMESPACE::ref<testhttpresponsemsgerrorObj<conn_type> >
		       ::create());

	sserver->runserver(server);

	sserver=typename conn_type::filedesc();

	typename conn_type::fdclientimpl cl;

	sclient->install(cl);

	LIBCXX_NAMESPACE::http::requestimpl req;

	req.setVersion(LIBCXX_NAMESPACE::http::httpver_t::http11);
	req.setURI("http://example.com");

	LIBCXX_NAMESPACE::http::responseimpl resp;

	bool caught=false;

	try {
		cl.send(req, resp);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("testhttpmessageerror: did not result in an exception");
}

template<typename conn_type>
class hugeheaderwriter : virtual public LIBCXX_NAMESPACE::obj {

	typename conn_type::filedesc write2fd;
public:

	hugeheaderwriter(const typename conn_type::filedesc &write2fdArg)
		: write2fd(write2fdArg)
	{
	}

	~hugeheaderwriter()
	{
	}

	void run()
	{
		LIBCXX_NAMESPACE::http::fdimplbase::output_iter_t
			o(write2fd->getostream());

		{
			std::string s("GET http://example.com HTTP/1.1\r\n");
			o=std::copy(s.begin(), s.end(), o);
		}

		try {
			while (1)
			{
				std::string s("Foo: bar\r\n");

				o=std::copy(s.begin(), s.end(), o);
			}
		} catch (...)
		  {
		  }
	}
};

template<typename conn_type>
class requestwriter : virtual public LIBCXX_NAMESPACE::obj {

	std::string msg2write;
	typename conn_type::filedesc write2fd;
public:

	requestwriter(const std::string &msg2writeArg,
		      const typename conn_type::filedesc &write2fdArg)
		: msg2write(msg2writeArg),
		write2fd(write2fdArg)
	{
	}

	~requestwriter()
	{
	}

	void run()
	{
		try {
			LIBCXX_NAMESPACE::http::fdimplbase::output_iter_t
				o(write2fd->getostream());

			o=std::copy(msg2write.begin(), msg2write.end(), o);

			o.flush();
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cerr << e << std::endl;
		}
	}
};

template<typename conn_type>
class discardthread : virtual public LIBCXX_NAMESPACE::obj {

	typename conn_type::filedesc readfd;
public:

	discardthread(const typename conn_type::filedesc &readfdArg)
		: readfd(readfdArg)
	{
	}

	~discardthread()
	{
	}

	void run()
	{
		std::pair<LIBCXX_NAMESPACE::http::fdimplbase::input_iter_t,
			LIBCXX_NAMESPACE::http::fdimplbase::input_iter_t>
			i(readfd->getistream());

		while (i.first != i.second)
			++i.first;
	}
};

template<typename conn_type>
void testserverheaderlimit()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	auto wrthread=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE
					  ::ref<hugeheaderwriter<conn_type> >
					  ::create(sclient));

	auto dropthread=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE
					    ::ref<discardthread<conn_type> >
					    ::create(sclient));

	sserver->runserver(conn_type::fdserver::create());

	sclient=typename conn_type::filedesc();
	sserver=typename conn_type::filedesc();

	dropthread->get();
	wrthread->get();
}

template<typename conn_type>
void testpipelinetimeout()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::server::pipeline_timeout", "2");

	auto wrthread=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE
					  ::ref<requestwriter<conn_type> >
					  ::create("GET / HTTP/1.1\r\n"
						   "Host: example.com\r\n"
						   "Content-Length: 6\r\n"
						   "\r\n"
						   "test\r\n",
						   sclient));

	auto dropthread=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE
					    ::ref<discardthread<conn_type> >
					    ::create(sclient));


	sserver->runserver(conn_type::fdserver::create());

	sclient=typename conn_type::filedesc();
	sserver=typename conn_type::filedesc();

	dropthread->get();
	wrthread->get();
	FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::server::pipeline_timeout", "30");
}

template<typename conn_type>
static void testheadertimeout()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::server::headers::timeout", "2");

	auto wrthread=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE
					  ::ref<requestwriter<conn_type> >
					  ::create("GET / HTTP/1.1\r\n"
						   "Host: example.com\r\n"
						   "Content-Length: 6\r\n",
						   sclient));

	auto dropthread=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE
					    ::ref<discardthread<conn_type> >
					    ::create(sclient));

	sserver->runserver(conn_type::fdserver::create());

	sclient=typename conn_type::filedesc();
	sserver=typename conn_type::filedesc();

	dropthread->get();
	wrthread->get();
	FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::server::headers::timeout", "30");
}

template<typename conn_type>
void testwritetimeout()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	fillpipe<conn_type>(sserver);

	FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::server::body_timeout", "2");
	FORCE_PROP(LIBCXX_NAMESPACE_STR "::https::server::handshake_timeout","2");
	FORCE_PROP(LIBCXX_NAMESPACE_STR "::https::server::bye_timeout", "2");

	auto wrthread=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE
					  ::ref<requestwriter<conn_type> >
					  ::create("GET / HTTP/1.1\r\n"
						   "Host: example.com\r\n"
						   "\r\n",
						   sclient));

	try {
		sserver->runserver(conn_type::fdserver::create());
	} catch (...) {
	}

	sclient=typename conn_type::filedesc();
	sserver=typename conn_type::filedesc();

	wrthread->get();

	FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::server::body_timeout", "30");

	FORCE_PROP(LIBCXX_NAMESPACE_STR "::https::server::handshake_timeout",
		   "300");
	FORCE_PROP(LIBCXX_NAMESPACE_STR "::https::server::bye_timeout", "30");
}

template<typename conn_type>
void testbodytimeout(const std::string &bodystr)
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::server::body_timeout", "2");

	auto wrthread=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE
					  ::ref<requestwriter<conn_type> >
					  ::create(bodystr, sclient));

	auto dropthread=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE
					    ::ref<discardthread<conn_type> >
					    ::create(sclient));

	sserver->runserver(conn_type::fdserver::create());

	sclient=typename conn_type::filedesc();
	sserver=typename conn_type::filedesc();

	dropthread->get();
	wrthread->get();
	FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::server::body_timeout", "30");
}

template<typename conn_type>
void testsuite()
{
	try {
		alarm(120);
		testhttpnull<conn_type>();
		testhttp10<conn_type>();
		testhttp11<conn_type>();

		testhttpsendmsgerror<conn_type>();

		testhttpresponsemsgerror<conn_type>();

		std::cout << "test server header limit" << std::endl; alarm(120);
		testserverheaderlimit<conn_type>();
		std::cout << "test pipeline timeout" << std::endl; alarm(120);
		testpipelinetimeout<conn_type>();
		std::cout << "test header timeout" << std::endl; alarm(120);
		testheadertimeout<conn_type>();
		std::cout << "test content-length timeout" << std::endl; alarm(120);
		testbodytimeout<conn_type>("GET / HTTP/1.1\r\n"
					   "Host: example.com\r\n"
					   "Content-Length: 6\r\n"
					   "\r\n");

		std::cout << "test chunked timeout 1" << std::endl; alarm(120);
		testbodytimeout<conn_type>("GET / HTTP/1.1\r\n"
					   "Host: example.com\r\n"
					   "Transfer-Encoding: chunked\r\n"
					   "\r\n");

		std::cout << "test chunked timeout 2" << std::endl; alarm(120);
		testbodytimeout<conn_type>("GET / HTTP/1.1\r\n"
					   "Host: example.com\r\n"
					   "Transfer-Encoding: chunked\r\n"
					   "\r\n"
					   "e\r\n");

		std::cout << "test chunked timeout 3" << std::endl; alarm(120);
		testbodytimeout<conn_type>("GET / HTTP/1.1\r\n"
					   "Host: example.com\r\n"
					   "Transfer-Encoding: chunked\r\n"
					   "\r\n"
					   "3\r\n"
					   "y\r\n");

		std::cout << "test chunked timeout 4" << std::endl; alarm(120);
		testbodytimeout<conn_type>("GET / HTTP/1.1\r\n"
					   "Host: example.com\r\n"
					   "Transfer-Encoding: chunked\r\n"
					   "\r\n"
					   "3\r\n"
					   "y\r\n"
					   "\r\n"
					   "0\r\n");
		std::cout << "test write timeout" << std::endl; alarm(120);
		testwritetimeout<conn_type>();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
}
