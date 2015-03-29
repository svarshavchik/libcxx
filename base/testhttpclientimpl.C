/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/clientimpl.H"
#include "x/http/fdclient.H"
#include "x/http/fdserver.H"
#include "x/threads/run.H"
#include <algorithm>

typedef LIBCXX_NAMESPACE::http::clientimpl
<LIBCXX_NAMESPACE::http::discardoutput, std::string::const_iterator> testclient;

static void dotestclient(const char *testname,
			 std::string buf,
			 std::string reqs,
			 const char *expected,
			 bool ispost)

{
	std::string response;

	testclient cl(100, false,
		      LIBCXX_NAMESPACE::http::discardoutput(),
		      buf.begin(), buf.end());

	size_t cnt=0;

	for (std::string::iterator b(reqs.begin()), e(reqs.end()); b != e; )
	{
		std::string::iterator p=b;

		b=std::find(b, e, '~');

		LIBCXX_NAMESPACE::http::requestimpl r;

		r.parse(p, b, 0);

		if (b != e)
			++b;

		LIBCXX_NAMESPACE::http::responseimpl resp;

		std::ostringstream o;

		o << "req" << ++cnt << "\r\n";

		std::string s=o.str();

		if (ispost ? !cl.send(r, s.begin(), s.end(), resp)
		    : !cl.send(r, resp))
		{
			response += "(truncated)\r\n";
			break;
		}

		if (r.responseHasMessageBody(resp))
		{
			std::copy(cl.begin(), cl.end(),
				  std::back_insert_iterator<std::string>
				  (response));
		}
		else
		{
			response += "(null)\r\n";
		}
	}

	if (response != expected)
		throw EXCEPTION(std::string(testname) + " failed:\n" +
				response);
}

static void dotestpipelineeof()

{
	std::string serverresp=
		"HTTP/1.1 200 Ok\r\n"
		"Content-Length: 0\r\n"
		"\r\n"
		"HTTP/1.1 100 Ok\r\n"
		"\r\n"
		"HTTP/1.1 200 Ok\r\n"
		"Content-Length: 0\r\n"
		"\r\n";

	testclient cl(100, false,
		      LIBCXX_NAMESPACE::http::discardoutput(), serverresp.begin(),
		      serverresp.end());

	LIBCXX_NAMESPACE::http::requestimpl
		r(LIBCXX_NAMESPACE::http::POST,
		  "http://localhost");

	LIBCXX_NAMESPACE::http::responseimpl resp;

	if (!cl.send(r, serverresp.begin(), serverresp.end(), resp))
		throw EXCEPTION("dotestpipelineeof: first request failed");

	if (r.responseHasMessageBody(resp))
		cl.discardbody();

	if (!cl.send(r, serverresp.begin(), serverresp.end(), resp))
		throw EXCEPTION("dotestpipelineeof: first request failed");

	if (r.responseHasMessageBody(resp))
		cl.discardbody();

	if (cl.send(r, serverresp.begin(), serverresp.end(), resp))
		throw EXCEPTION("dotestpipelineeof: did not get EOF indication");
}

static void testnocontinue()

{
	std::string serverresp=
		"HTTP/1.1 404 Not found\r\n"
		"Content-Length: 7\r\n"
		"\r\n"
		"HTTP/1.1 404 Not Found\r\n"
		"Content-Length: 7\r\n"
		"\r\n"
		"error\r\n";

	testclient cl(100, false,
		      LIBCXX_NAMESPACE::http::discardoutput(), serverresp.begin(),
		      serverresp.end());

	{
		LIBCXX_NAMESPACE::http::requestimpl
			req(LIBCXX_NAMESPACE::http::HEAD,
			    "http://localhost",
			    "Accept", "*/*");

		LIBCXX_NAMESPACE::http::responseimpl resp;

		if (!cl.send(req, resp))
			throw EXCEPTION("testnocontinue: message 1 did not send");

		if (req.responseHasMessageBody(resp))
			throw EXCEPTION("testnocontinue: did not expect a body in response to a head");
	}

	{
		LIBCXX_NAMESPACE::http::requestimpl req;
		LIBCXX_NAMESPACE::http::responseimpl resp;

		req.setMethod(LIBCXX_NAMESPACE::http::POST);
		req.setURI("http://localhost");

		std::string dummy;

		if (!cl.send(req, dummy.begin(), dummy.end(), resp))
			throw EXCEPTION("testnocontinue: message 2 did not send");

		if (resp.getStatusCode() != 404)
			throw EXCEPTION("Response was not 404");

		if (!req.responseHasMessageBody(resp))
			throw EXCEPTION("testnocontinue: did not get 404 body");

		cl.discardbody();
	}
}

class testthread : virtual public LIBCXX_NAMESPACE::obj,
		   public LIBCXX_NAMESPACE::http::fdserverimpl {

public:
	testthread() {}
	~testthread() noexcept {}

	void run(const LIBCXX_NAMESPACE::fd &fd)
	{
		auto dummy=LIBCXX_NAMESPACE::fd::base::socketpair();

		LIBCXX_NAMESPACE::http::fdserverimpl::run(fd, dummy.first);
	}
};


static void mkforceepipe(LIBCXX_NAMESPACE::http::fdclientimpl &cl1)

{
	LIBCXX_NAMESPACE::fdptr sock1, sock2;

	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p(LIBCXX_NAMESPACE::fd::base::socketpair());

		sock1=p.first;
		sock2=p.second;
	}

	auto thr=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<testthread>::create(),
				     sock2);

	LIBCXX_NAMESPACE::http::fdclient
		cl2(LIBCXX_NAMESPACE::http::fdclient::create());

	cl2->install(sock1, LIBCXX_NAMESPACE::fdptr());
	cl1.install(sock1, LIBCXX_NAMESPACE::fdptr());

	{
		LIBCXX_NAMESPACE::http::requestimpl req;
		LIBCXX_NAMESPACE::http::responseimpl resp;

		req.setMethod(LIBCXX_NAMESPACE::http::GET);
		req.setURI("http://localhost");

		if (!cl1.send(req, resp))
			throw EXCEPTION("forceepipe: message 1 did not send");

		if (req.responseHasMessageBody(resp))
			cl1.discardbody();
	}

	{
		LIBCXX_NAMESPACE::http::requestimpl req;
		LIBCXX_NAMESPACE::http::responseimpl resp;

		req.setMethod(LIBCXX_NAMESPACE::http::GET);
		req.setURI("http://localhost");

		if (!cl2->send(req, resp))
			throw EXCEPTION("forceepipe: message 2 did not send");

		if (req.responseHasMessageBody(resp))
			cl2->discardbody();

		req.append("Connection", "close");

		if (!cl2->send(req, resp))
			throw EXCEPTION("forceepipe: message 3 did not send");
	}

	thr->get();
}

static void forceepipe()
{

	{
		LIBCXX_NAMESPACE::http::fdclientimpl cl;

		mkforceepipe(cl);

		LIBCXX_NAMESPACE::http::requestimpl req;
		LIBCXX_NAMESPACE::http::responseimpl resp;

		req.setMethod(LIBCXX_NAMESPACE::http::GET);
		req.setURI("http://localhost");

		if (cl.send(req, resp))
			throw EXCEPTION("forceepipe: test message 1 did not err out");
	}

	{
		LIBCXX_NAMESPACE::http::fdclientimpl cl;

		mkforceepipe(cl);

		LIBCXX_NAMESPACE::http::requestimpl req;
		LIBCXX_NAMESPACE::http::responseimpl resp;

		req.setMethod(LIBCXX_NAMESPACE::http::POST);
		req.setURI("http://localhost");

		std::string dummy;

		if (cl.send(req, dummy.begin(), dummy.end(), resp))
			throw EXCEPTION("forceepipe: test message 2 did not err out");
	}

}

int main(int argc, char **argv)
{
	try {
		dotestclient("http10req",
			     "HTTP/1.1 200 Ok\r\n"
			     "Content-Length: 8\r\n"
			     "\r\n"
			     "dummy1\r\n"
			     "HTTP/1.1 200 Ok\r\n"
			     "Content-Length: 8\r\n"
			     "\r\n"
			     "dummy2\r\n",
			     "GET http://localhost HTTP/1.0\r\n"
			     "\r\n"
			     "~"
			     "GET http://localhost HTTP/1.0\r\n"
			     "\r\n",
			     "dummy1\r\n"
			     "(truncated)\r\n", false);
		dotestclient("http11reqs",
			     "HTTP/1.1 200 Ok\r\n"
			     "Content-Length: 8\r\n"
			     "\r\n"
			     "dummy1\r\n"
			     "HTTP/1.1 200 Ok\r\n"
			     "Content-Length: 8\r\n"
			     "\r\n"
			     "dummy2\r\n",
			     "GET http://localhost HTTP/1.1\r\n"
			     "\r\n"
			     "~"
			     "GET http://localhost HTTP/1.1\r\n"
			     "\r\n",
			     "dummy1\r\n"
			     "dummy2\r\n", false);
		dotestclient("http10resp",
			     "HTTP/1.0 200 Ok\r\n"
			     "Content-Length: 8\r\n"
			     "\r\n"
			     "dummy1\r\n"
			     "HTTP/1.1 200 Ok\r\n"
			     "Content-Length: 8\r\n"
			     "\r\n"
			     "dummy2\r\n",
			     "GET http://localhost HTTP/1.1\r\n"
			     "\r\n"
			     "~"
			     "GET http://localhost HTTP/1.1\r\n"
			     "\r\n",
			     "dummy1\r\n"
			     "(truncated)\r\n", false);
		dotestclient("reqconnclose",
			     "HTTP/1.1 200 Ok\r\n"
			     "Content-Length: 8\r\n"
			     "\r\n"
			     "dummy1\r\n"
			     "HTTP/1.1 200 Ok\r\n"
			     "Content-Length: 8\r\n"
			     "\r\n"
			     "dummy2\r\n",
			     "GET http://localhost HTTP/1.1\r\n"
			     "Connection: close\r\n"
			     "\r\n"
			     "~"
			     "GET http://localhost HTTP/1.1\r\n"
			     "\r\n",
			     "dummy1\r\n"
			     "(truncated)\r\n", false);
		dotestclient("respconnclose",
			     "HTTP/1.1 200 Ok\r\n"
			     "Content-Length: 8\r\n"
			     "Connection: close\r\n"
			     "\r\n"
			     "dummy1\r\n"
			     "HTTP/1.1 200 Ok\r\n"
			     "Content-Length: 8\r\n"
			     "\r\n"
			     "dummy2\r\n",
			     "GET http://localhost HTTP/1.1\r\n"
			     "\r\n"
			     "~"
			     "GET http://localhost HTTP/1.1\r\n"
			     "\r\n",
			     "dummy1\r\n"
			     "(truncated)\r\n", false);

		dotestclient("http11postexpectok",
			     "HTTP/1.1 200 Ok\r\n"
			     "Content-Length: 7\r\n"
			     "\r\n"
			     "resp1\r\n"
			     "HTTP/1.1 100 Ok\r\n"
			     "\r\n"
			     "HTTP/1.1 200 Ok\r\n"
			     "Content-Length: 7\r\n"
			     "\r\n"
			     "resp2\r\n",
			     "POST http://localhost HTTP/1.1\r\n"
			     "\r\n"
			     "~"
			     "POST http://localhost HTTP/1.1\r\n"
			     "\r\n",
			     "resp1\r\n"
			     "resp2\r\n", true);
		testnocontinue();
		dotestpipelineeof();
		forceepipe();

	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

