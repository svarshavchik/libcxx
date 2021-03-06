#include "libcxx_config.h"
#include "x/http/fdserver.H"
#include "x/http/fdclientimpl.H"
#include "x/threads/run.H"

#ifndef FORCE_PROP
#define FORCE_PROP(n, v)						\
	LIBCXX_NAMESPACE::property::load_property(n, v, true, true);
#endif

template<typename conn_type>
class discardthread : virtual public LIBCXX_NAMESPACE::obj {

	typename conn_type::filedesc fd;

public:
	discardthread(const typename conn_type::filedesc &fdArg)
		: fd(fdArg)
	{
	}

	~discardthread()
	{
	}

	void run()
	{
		try {
			std::pair<LIBCXX_NAMESPACE::http::fdimplbase
				  ::input_iter_t,
				  LIBCXX_NAMESPACE::http::fdimplbase
				  ::input_iter_t>
				i(fd->getistream());

			while (i.first != i.second)
				++i.first;

		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cerr << e << std::endl;
		}
	}
};

template<typename conn_type>
LIBCXX_NAMESPACE::runthread<void>
discardfd(const typename conn_type::filedesc &fdRef)
{
	return LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<discardthread<conn_type>
				   >::create(fdRef));
}

template<typename conn_type>
void testwritetimeout()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	{
		sclient->get_fd()->nonblock(true);

		char dummy=0;

		while (sclient->get_fd()->write(&dummy, 1) > 0)
			;
		sclient->get_fd()->nonblock(false);
	}

	FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::client::timeout", "2");
	FORCE_PROP(LIBCXX_NAMESPACE_STR "::https::client::handshake_timeout","2");
	FORCE_PROP(LIBCXX_NAMESPACE_STR "::https::client::bye_timeout", "2");

	typename conn_type::fdclientimpl cl;

	sclient->install(cl);

	try {
		LIBCXX_NAMESPACE::http::requestimpl req;
		LIBCXX_NAMESPACE::http::responseimpl resp;

		req.set_URI("http://example.com");
		req.set_method(LIBCXX_NAMESPACE::http::GET);

		cl.send(req, resp);

	} catch (LIBCXX_NAMESPACE::exception &exc)
	{
		FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::client::timeout", "30");
		FORCE_PROP(LIBCXX_NAMESPACE_STR
			   "::https::client::handshake_timeout", "30");
		FORCE_PROP(LIBCXX_NAMESPACE_STR
			   "::https::client::bye_timeout", "2");
		return;
	}
	throw EXCEPTION("testwritetimeout: did not throw an exception");
}

template<typename conn_type>
void testrecvtimeout()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::client::response_timeout", "2");

	FORCE_PROP(LIBCXX_NAMESPACE_STR "::https::client::handshake_timeout",
		   "2");

	FORCE_PROP(LIBCXX_NAMESPACE_STR "::https::client::bye_timeout", "2");
	typename conn_type::fdclientimpl cl;

	sclient->install(cl);

	try {
		LIBCXX_NAMESPACE::http::requestimpl req;
		LIBCXX_NAMESPACE::http::responseimpl resp;

		req.set_URI("http://example.com");
		req.set_method(LIBCXX_NAMESPACE::http::GET);

		cl.send(req, resp);

	} catch (LIBCXX_NAMESPACE::exception &exc)
	{
		FORCE_PROP(LIBCXX_NAMESPACE_STR
			   "::http::client::response_timeout", "30");
		FORCE_PROP(LIBCXX_NAMESPACE_STR
			   "::https::client::handshake_timeout", "30");
		FORCE_PROP(LIBCXX_NAMESPACE_STR
			   "::https::client::bye_timeout", "30");
		return;
	}
	throw EXCEPTION("testrecvtimeout: did not throw an exception");
}

template<typename conn_type>
class threadwriter : virtual public LIBCXX_NAMESPACE::obj {

	std::string str;

	typename conn_type::filedesc fd;

public:
	threadwriter(const std::string &strArg,
		     const typename conn_type::filedesc &fdArg)
		: str(strArg), fd(fdArg)
	{
	}

	~threadwriter()
	{
	}

	void run()
	{
		try {
			LIBCXX_NAMESPACE::http::fdimplbase::output_iter_t
				o(fd->getostream());

			o=std::copy(str.begin(), str.end(), o);
			o.flush();
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cerr << e << std::endl;
		}
	}
};

template<typename conn_type>
void testbodytimeout(const char *str)
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	LIBCXX_NAMESPACE::runthread<void>
		thr=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<threadwriter
					<conn_type> >
					::create(str, sserver)),
		thr2=discardfd<conn_type>(sserver);

	FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::client::timeout", "2");
	FORCE_PROP(LIBCXX_NAMESPACE_STR "::https::client::handshake_timeout",
		   "2");
	FORCE_PROP(LIBCXX_NAMESPACE_STR "::https::client::bye_timeout", "2");

	typename conn_type::fdclientimpl cl;

	sclient->install(cl);

	try {
		LIBCXX_NAMESPACE::http::requestimpl req;
		LIBCXX_NAMESPACE::http::responseimpl resp;

		req.set_URI("http://example.com");
		req.set_method(LIBCXX_NAMESPACE::http::GET);

		cl.send(req, resp);

		if (req.response_has_message_body(resp))
			for (typename conn_type::fdclientimpl::iterator
				     b=cl.begin(), e=cl.end(); b != e; ++b)
				;

	} catch (const LIBCXX_NAMESPACE::exception &exc)
	{
		FORCE_PROP(LIBCXX_NAMESPACE_STR "::http::client::timeout", "30");
		FORCE_PROP(LIBCXX_NAMESPACE_STR
			   "::https::client::handshake_timeout", "300");
		FORCE_PROP(LIBCXX_NAMESPACE_STR "::https::client::bye_timeout",
			   "30");
		thr->wait();
		sclient->get_fd()->close();
		thr2->wait();
		return;
	}
	throw EXCEPTION("testbodytimeout: did not throw an exception");
}

template<typename conn_type>
class headerwriter : virtual public LIBCXX_NAMESPACE::obj {

	typename conn_type::filedesc fd;

public:

	headerwriter(const typename conn_type::filedesc &fdArg)
		: fd(fdArg)
	{
	}

	~headerwriter()
	{
	}

	void run()
	{
		LIBCXX_NAMESPACE::http::fdimplbase::output_iter_t
			o(fd->getostream());

		{
			std::string s("HTTP/1.1 200 Ok\r\n");
			o=std::copy(s.begin(), s.end(), o);
		}

		try {
			while (1)
			{
				std::string s("Host: example.com\r\n");

				o=std::copy(s.begin(), s.end(), o);
			}
		} catch (...)
		  {
		  }
	}
};

template<typename conn_type>
void testheaderlimit()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	LIBCXX_NAMESPACE::runthread<void> thr=
		LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<headerwriter<conn_type>
				    >::create(sserver)),
		thr2=discardfd<conn_type>(sserver);

	typename conn_type::fdclientimpl cl;

	sclient->install(cl);

	try {
		LIBCXX_NAMESPACE::http::requestimpl req;
		LIBCXX_NAMESPACE::http::responseimpl resp;

		req.set_URI("http://example.com");
		req.set_method(LIBCXX_NAMESPACE::http::GET);

		cl.send(req, resp);

	} catch (const LIBCXX_NAMESPACE::exception &exc)
	{
		sclient->get_fd()->close();
		thr->wait();
		thr2->wait();
		return;
	}
	throw EXCEPTION("testheaderlimit: did not throw an exception");
}

template<typename conn_type>
void testrequest()
{
	typename conn_type::filedesc sclient, sserver;

	conn_type::createClientServerPair(sclient, sserver);

	LIBCXX_NAMESPACE::runthread<void> thr=
		LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<threadwriter<conn_type>
				    >::create("HTTP/1.1 200 Ok\r\n"
					      "Content-Length: 5\r\n"
					      "\r\n"
					      "yes\r\n"
					      "HTTP/1.1 200 Ok\r\n"
					      "Content-Length: 5\r\n"
					      "\r\n"
					      "yes\r\n", sserver)),
		thr2=discardfd<conn_type>(sserver);

	sserver=typename conn_type::filedesc();

	typename conn_type::fdclientimpl cl;

	sclient->install(cl);

	for (size_t i=0; i<2; i++)
	{
		LIBCXX_NAMESPACE::http::requestimpl req;
		LIBCXX_NAMESPACE::http::responseimpl resp;

		req.set_URI("http://example.com");
		req.set_method(LIBCXX_NAMESPACE::http::GET);

		if (!cl.send(req, resp))
			throw EXCEPTION("Send failed");

		if (req.response_has_message_body(resp))
		{
			std::copy(cl.begin(), cl.end(),
				  std::ostreambuf_iterator<char>(std::cout));
			std::cout << std::flush;
		}
	}

	sclient->get_fd()->close();
	thr->wait();
	thr2->wait();
}

template<typename conn_type>
void testsuite()
{
	try {
		std::cout << "Testing plain request" << std::endl; alarm(10);
		testrequest<conn_type>();
		std::cout << "Testing header limits" << std::endl; alarm(10);
		testheaderlimit<conn_type>();
		std::cout << "Testing write timeout" << std::endl; alarm(10);
		testwritetimeout<conn_type>();
		std::cout << "Testing receive timeout" << std::endl; alarm(10);
		testrecvtimeout<conn_type>();
		std::cout << "Testing no-content-length response timeout"
			  << std::endl; alarm(10);
		testbodytimeout<conn_type>("HTTP/1.0 200 Ok\r\n"
					   "Host: example.com\r\n"
					   "\r\n");
		std::cout << "Testing content-length response timeout"
			  << std::endl; alarm(10);
		testbodytimeout<conn_type>("HTTP/1.0 200 Ok\r\n"
					   "Host: example.com\r\n"
					   "Content-Length: 10\r\n"
					   "\r\n");
		std::cout << "Testing chunked response timeout"
			  << std::endl; alarm(10);
		testbodytimeout<conn_type>("HTTP/1.0 200 Ok\r\n"
					   "Host: example.com\r\n"
					   "Transfer-Encoding: chunked\r\n"
					   "\r\n"
					   "e\r\n"); alarm(10);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
}
