/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http/useragent.H"
#include "http/fdserver.H"
#include "http/form.H"
#include "fdlistener.H"
#include "eventdestroynotify.H"
#include "netaddr.H"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <sys/syscall.h>

static bool fake_connect=false;
static int fake_connect_fd= -1;

#define LIBCXX_DEBUG_CONNECT() if (fake_connect)		\
	{						\
		int pipefd[2];				\
							\
		::pipe(pipefd);				\
		::close(filedesc);			\
		filedesc=pipefd[1];			\
		nonblock(true);				\
		while (::write(filedesc, "", 1) > 0)	\
			;				\
		fake_connect_fd=pipefd[0];		\
		errno=EINPROGRESS;			\
		throw SYSEXCEPTION("Not really");	\
	}

#include "fdobj.C"


class testserverObj;

typedef LIBCXX_NAMESPACE::ref<testserverObj> testserver;

class myfdserverObj : public LIBCXX_NAMESPACE::http::fdserverimpl,
		      virtual public LIBCXX_NAMESPACE::obj {

public:
	testserver ptr;

	myfdserverObj(const testserver &ptrArg);
	~myfdserverObj() noexcept;

	const LIBCXX_NAMESPACE::fd *socketptr;

	void run(const LIBCXX_NAMESPACE::fd &socket,
		 const LIBCXX_NAMESPACE::fd &terminator);

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag);
};

class testserverObj : virtual public LIBCXX_NAMESPACE::obj {

public:

	bool nodie;

	size_t nrunning;
	size_t nstarted;

	std::mutex mutex;
	std::condition_variable start_stop_cond;

	LIBCXX_NAMESPACE::eventfd destroynotifyfd;

	std::mutex semaphore_mutex;
	std::condition_variable semaphore_cond;
	bool semaphore;

	std::mutex waiting_mutex;
	std::condition_variable waiting_cond;
	bool waiting;

	testserverObj();
	~testserverObj() noexcept;

	size_t getRunning()
	{
		std::lock_guard<std::mutex> lock(mutex);

		return nrunning;
	}

	size_t getStarted()
	{
		std::lock_guard<std::mutex> lock(mutex);

		return nstarted;
	}

	LIBCXX_NAMESPACE::ref<myfdserverObj> create()
	{
		return LIBCXX_NAMESPACE::ref<myfdserverObj>
			::create(testserver(this));
	}
};

testserverObj::testserverObj()
	: nodie(false), nrunning(0), nstarted(0),
	  destroynotifyfd(LIBCXX_NAMESPACE::eventfd::create()),
	  semaphore(false)
{
}

testserverObj::~testserverObj() noexcept
{
}

myfdserverObj::myfdserverObj(const testserver &ptrArg) : ptr(ptrArg)
{
}

myfdserverObj::~myfdserverObj() noexcept
{
}

void myfdserverObj::run(const LIBCXX_NAMESPACE::fd &socket,
			const LIBCXX_NAMESPACE::fd &terminator)
{
	socketptr= &socket;

	{
		std::lock_guard<std::mutex> lock(ptr->mutex);
		++ptr->nstarted;
		++ptr->nrunning;

		ptr->start_stop_cond.notify_all();

	}
	std::cout << "Entering run loop" << std::endl;

	try {
		LIBCXX_NAMESPACE::http::fdserverimpl::run(socket, terminator);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Server thread caught an exception: "
			  << e << std::endl;
	}

	{
		std::lock_guard<std::mutex> lock(ptr->mutex);
		--ptr->nrunning;
		ptr->start_stop_cond.notify_all();
	}

}

void myfdserverObj::received(const LIBCXX_NAMESPACE::http::requestimpl &req,
			     bool bodyflag)
{
	std::cout << "In received" << std::endl;

	if (req.hasMessageBody())
		for (iterator b(begin()), e(end()); b != e; ++b)
			;

	std::cout << "Waiting" << std::endl;

	{
		std::unique_lock<std::mutex> lock(ptr->semaphore_mutex);

		{
			std::unique_lock<std::mutex>
				lock2(ptr->waiting_mutex);

			ptr->waiting=true;
			ptr->waiting_cond.notify_all();
		}

		
		ptr->semaphore_cond.wait(lock, [this] {
				return !ptr->semaphore;
			});
	}

	send(req, 200, "Ok");

	std::lock_guard<std::mutex> lock(ptr->mutex);

	if (ptr->nodie)
		return;

	LIBCXX_NAMESPACE::eventdestroynotify
		::create(ptr->destroynotifyfd, *socketptr);
	throw EXCEPTION("Destroyed myself");
}

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

	o << "http://localhost:" << portnum;

	return o.str();
}

class savebodyObj : virtual public LIBCXX_NAMESPACE::obj {

	std::ostringstream o;

public:
	savebodyObj()
	{
	}

	~savebodyObj() noexcept
	{
	}

	typedef LIBCXX_NAMESPACE::http::useragent::base::body_iterator iterator;

	void body(iterator beg_iter, iterator end_iter)

	{
		std::copy(beg_iter, end_iter,
			  std::ostreambuf_iterator<char>(o));
		o << std::endl;
	}

	std::string getString()
	{
		return o.str();
	}
};

typedef LIBCXX_NAMESPACE::ptr<savebodyObj> savebody;

static void test1req(const savebody &body)
{
	LIBCXX_NAMESPACE::fdlistenerptr listener;
	testserver server(testserver::create());

	std::string serveraddr=createlistener(listener, server);

	std::cout << "Started a listener" << std::endl;

	LIBCXX_NAMESPACE::http::useragent ua(LIBCXX_NAMESPACE::http::useragent
					   ::create());

	std::cout << "Sending a test request" << std::endl;

	LIBCXX_NAMESPACE::http::useragent::base::response
		resp=ua->request(LIBCXX_NAMESPACE::http::GET, serveraddr);

	if (!body.null())
		body->body(resp->begin(), resp->end());
	std::cout << "Stopping listener" << std::endl;

	listener->stop();
	listener->wait();
	if (resp->message.getStatusCode() != 200)
		throw EXCEPTION("Test request failed");
}

static void test2req(const savebody &body)
{
	LIBCXX_NAMESPACE::fdlistenerptr listener;
	testserver server(testserver::create());

	server->nodie=true;

	std::string serveraddr=createlistener(listener, server);

	std::cout << "Started a listener" << std::endl;

	LIBCXX_NAMESPACE::http::useragent ua(LIBCXX_NAMESPACE::http::useragent
					   ::create());

	LIBCXX_NAMESPACE::http::requestimpl
		req(LIBCXX_NAMESPACE::http::GET, serveraddr);

	std::cout << "Sending a test request" << std::endl;

	{
		LIBCXX_NAMESPACE::http::useragent::base::response
			resp=ua->request(req);

		if (!body.null())
			body->body(resp->begin(), resp->end());
	}
	std::cout << "Sending another test request" << std::endl;

	req=LIBCXX_NAMESPACE::http::requestimpl(LIBCXX_NAMESPACE::http::GET, serveraddr);

	LIBCXX_NAMESPACE::http::useragent::base::response resp=ua->request(req);

	if (!body.null())
		body->body(resp->begin(), resp->end());

	if (server->getRunning() != 1)
	{
		std::cout << "test2req: expected to end up with one running connection"
			  << std::endl;
		throw EXCEPTION("Unexpected results");
	}

	std::cout << "Stopping listener" << std::endl;

	listener->stop();
	listener->wait();
	if (resp->message.getStatusCode() != 200)
		throw EXCEPTION("Test request failed");
}

static void testhttp10req(const savebody &body)
{
	LIBCXX_NAMESPACE::fdlistenerptr listener;
	testserver server(testserver::create());

	std::string serveraddr=createlistener(listener, server);

	std::cout << "Started a listener" << std::endl;

	LIBCXX_NAMESPACE::http::useragent ua(LIBCXX_NAMESPACE::http::useragent
					   ::create());

	LIBCXX_NAMESPACE::http::requestimpl req(LIBCXX_NAMESPACE::http::GET,
					      serveraddr);

	req.setVersion(LIBCXX_NAMESPACE::http::http10);

	std::cout << "Sending a test request" << std::endl;

	{
		LIBCXX_NAMESPACE::http::useragent::base::response
			resp=ua->request(req);

		if (!body.null())
			body->body(resp->begin(), resp->end());
	}

	std::cout << "Sending another test request" << std::endl;

	LIBCXX_NAMESPACE::http::useragent::base::response resp=ua->request(req);

	if (!body.null())
		body->body(resp->begin(), resp->end());

	if (server->getStarted() != 2)
	{
		std::cout << "testhttp10req: expected to create two connections"
			  << std::endl;
		throw EXCEPTION("Unexpected results");
	}

	std::cout << "Stopping listener" << std::endl;

	listener->stop();
	listener->wait();
	if (resp->message.getStatusCode() != 200)
		throw EXCEPTION("Test request failed");
}

static void testdestroyget(const savebody &body)
{
	LIBCXX_NAMESPACE::fdlistenerptr listener;
	testserver server(testserver::create());
	LIBCXX_NAMESPACE::eventfd destroyeventfd=server->destroynotifyfd;

	std::string serveraddr=createlistener(listener, server);

	std::cout << "Started a listener" << std::endl;

	LIBCXX_NAMESPACE::http::useragent ua(LIBCXX_NAMESPACE::http::useragent
					   ::create());

	LIBCXX_NAMESPACE::http::requestimpl
		req(LIBCXX_NAMESPACE::http::GET, serveraddr);

	std::cout << "Sending a test request" << std::endl;

	{
		LIBCXX_NAMESPACE::http::useragent::base::response
			resp=ua->request(req);

		if (!body.null())
			body->body(resp->begin(), resp->end());
	}

	std::cout << "Waiting for last thread to terminate" << std::endl;

	destroyeventfd->event();

	std::cout << "Sending another test request" << std::endl;

	LIBCXX_NAMESPACE::http::useragent::base::response resp=ua->request(req);

	if (!body.null())
		body->body(resp->begin(), resp->end());

	if (server->getStarted() != 2)
	{
		std::cout << "testdestroyget: expected to create two connections"
			  << std::endl;
		throw EXCEPTION("Unexpected results");
	}

	std::cout << "Stopping listener" << std::endl;

	listener->stop();
	listener->wait();
	if (resp->message.getStatusCode() != 200)
		throw EXCEPTION("Test request failed");
}

static void testdestroypost(const savebody &body)
{
	std::string content("Content");

	LIBCXX_NAMESPACE::fdlistenerptr listener;
	testserver server(testserver::create());
	LIBCXX_NAMESPACE::eventfd destroyeventfd=server->destroynotifyfd;

	std::string serveraddr=createlistener(listener, server);

	std::cout << "Started a listener" << std::endl;

	LIBCXX_NAMESPACE::http::useragent ua(LIBCXX_NAMESPACE::http::useragent
					   ::create());

	LIBCXX_NAMESPACE::http::requestimpl
		req(LIBCXX_NAMESPACE::http::POST, serveraddr);

	std::cout << "Sending a test request" << std::endl;

	{
		LIBCXX_NAMESPACE::http::useragent::base::response
			resp=ua->request(req,
					 LIBCXX_NAMESPACE::http::useragent
					 ::base::body(content));

		if (!body.null())
			body->body(resp->begin(), resp->end());
	}

	std::cout << "Waiting for last thread to terminate" << std::endl;

	destroyeventfd->event();

	std::cout << "Sending another test request" << std::endl;

	LIBCXX_NAMESPACE::http::useragent::base::response
		resp=ua->request(req,
				 LIBCXX_NAMESPACE::http::useragent::base::body(content));

	if (!body.null())
		body->body(resp->begin(), resp->end());

	if (server->getStarted() != 2)
	{
		std::cout << "testdestroypost: expected to create two connections"
			  << std::endl;
		throw EXCEPTION("Unexpected results");
	}

	std::cout << "Stopping listener" << std::endl;

	listener->stop();
	listener->wait();
	if (resp->message.getStatusCode() != 200)
		throw EXCEPTION("Test request failed");
}

class testpoolthreadObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::http::useragent ua;
	LIBCXX_NAMESPACE::uriimpl uri;

	testpoolthreadObj(const LIBCXX_NAMESPACE::http::useragent &uaRef,
			  const LIBCXX_NAMESPACE::uriimpl &uriRef);
	~testpoolthreadObj() noexcept;

	void run();
};

testpoolthreadObj::testpoolthreadObj(const LIBCXX_NAMESPACE::http::useragent
				     &uaRef,
				     const LIBCXX_NAMESPACE::uriimpl &uriRef)
	: ua(uaRef),
	  uri(uriRef)
{
}

testpoolthreadObj::~testpoolthreadObj() noexcept
{
}

void testpoolthreadObj::run()
{
	try {
		LIBCXX_NAMESPACE::http::requestimpl req;

		req.setURI(uri);

		std::cout << "Sending a test request" << std::endl;

		ua->request(req);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "Unexpected exception: " << e << std::endl
			  << " in thread " << LIBCXX_NAMESPACE::gettid()
			  << std::endl;
	}
}

typedef LIBCXX_NAMESPACE::ptr<testpoolthreadObj> testpoolthread;

static void testpool()
{
	LIBCXX_NAMESPACE::fdlistenerptr listener;
	testserver server(testserver::create());

	std::string serveraddr=createlistener(listener, server);

	std::cout << "Started a listener" << std::endl;

	LIBCXX_NAMESPACE::http::useragent
		ua(LIBCXX_NAMESPACE::http::useragent::create(100, 4));

	server->semaphore=true;

	std::list<LIBCXX_NAMESPACE::runthread<void> > reqthreads;

	LIBCXX_NAMESPACE::uriimpl uri(serveraddr);

	std::cout << "URI: ";

	uri.toString(std::ostreambuf_iterator<char>(std::cout));
	std::cout << std::endl;

	testpoolthread testthread=testpoolthread::create(ua, uri);

	for (size_t i=0; i<10; i++)
	{
		{
			std::lock_guard<std::mutex>
				lock1(server->semaphore_mutex);
			std::lock_guard<std::mutex>
				lock2(server->waiting_mutex);

			server->waiting=false;
		}

		std::cout << "Waiting until server thread " << (i+1)
			  << " received the request" << std::endl;

		auto thr=LIBCXX_NAMESPACE::run(testthread);

		{
			std::unique_lock<std::mutex>
				lock(server->waiting_mutex);

			while (!server->waiting)
				server->waiting_cond.wait(lock);
		}
		reqthreads.push_back(thr);
	}

	std::cout << "Signalling all server threads to proceed" << std::endl;

	{
		std::lock_guard<std::mutex> lock(server->semaphore_mutex);

		server->semaphore=false;
		server->semaphore_cond.notify_all();
	}

	std::cout << "Waiting for all the requests to be processed"
		  << std::endl;

	while (!reqthreads.empty())
	{
		reqthreads.front()->get();
		reqthreads.pop_front();
	}

	std::cout << "Stopping listener" << std::endl;

	listener->stop();
	listener->wait();
}

static void testconnecttimeout()
{
	fake_connect=true;

	std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
		p=LIBCXX_NAMESPACE::fd::base::socketpair();

	p.first->close();

	LIBCXX_NAMESPACE::http::useragent ua(LIBCXX_NAMESPACE::http::useragent
					   ::create());

	savebody body(savebody::create());

	bool timedout=false;

	try
	{
		LIBCXX_NAMESPACE::http::requestimpl req(LIBCXX_NAMESPACE::http::GET,
						      "http://localhost:999");

		LIBCXX_NAMESPACE::http::useragent::base::response
			resp=ua->request(p.second, req);

		body->body(resp->begin(), resp->end());

	} catch (const LIBCXX_NAMESPACE::sysexception &e)
	{
		fake_connect=false;
		if (e.getErrorCode() == ETIMEDOUT)
			timedout=true;
		else
			throw;
	}
	fake_connect=false;

	if (!timedout)
		throw EXCEPTION("Did not get the expected timeout");
}

class formpost_serverimpl : public LIBCXX_NAMESPACE::http::fdserverimpl,
			    virtual public LIBCXX_NAMESPACE::obj {

public:

	formpost_serverimpl() {}
	~formpost_serverimpl() noexcept {}

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag)
	{
		auto val=getform(req, bodyflag);

		std::stringstream o;

		{
			std::stringstream reqstr;

			req.toString(std::ostreambuf_iterator<char>
				     (reqstr.rdbuf()));

			std::string request;

			std::getline(reqstr, request);

			std::copy_if(request.begin(),
				     request.end(),
				     std::ostreambuf_iterator<char>(o.rdbuf()),
				     std::bind(std::not_equal_to<char>(),
					       std::placeholders::_1, '\r'));
			o << std::endl;
		}

		for (auto hdr:req.list())
		{
			if (LIBCXX_NAMESPACE::ctype(LIBCXX_NAMESPACE::locale
						  ::create("C"))
			    .tolower(hdr.substr(0, 2)) == "x-")
			{
				o << std::string(hdr) << std::endl;
			}
		}

		for (auto parameter: *val.first)
		{
			o << parameter.first << ": " << parameter.second
			  << std::endl;
		}

		send(req, "text/plain",
		     std::istreambuf_iterator<char>(o.rdbuf()),
		     std::istreambuf_iterator<char>());
	}
};

class formpost_serverimplObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	formpost_serverimplObj() {}
	~formpost_serverimplObj() noexcept {}

	LIBCXX_NAMESPACE::ref<formpost_serverimpl> create()
	{
		return LIBCXX_NAMESPACE::ref<formpost_serverimpl>::create();
	}
};

void formpost()
{
	LIBCXX_NAMESPACE::fdlistenerptr listener;
	LIBCXX_NAMESPACE::ref<formpost_serverimplObj>
		server(LIBCXX_NAMESPACE::ref<formpost_serverimplObj>::create());

	std::string serveraddr=createlistener(listener, server);

	std::cout << "Started a listener on " << serveraddr << std::endl;

	LIBCXX_NAMESPACE::http::useragent ua(LIBCXX_NAMESPACE::http::useragent
					   ::create());

	LIBCXX_NAMESPACE::http::useragent::base::response resp=
		ua->request(LIBCXX_NAMESPACE::http::GET,
			    serveraddr,
			    "X-Request",
			    "header1",
			    LIBCXX_NAMESPACE::http::form::parameters
			    ::create("param1", "value1"));

	std::string got(resp->begin(), resp->end());
	std::string expected=
		"GET /?param1=value1 HTTP/1.1\n"
		"X-Request: header1\n"
		"param1: value1\n";

	if (got != expected)
		throw EXCEPTION("Got: [" + got + "], but expected ["
				+ expected + "]");

	resp=ua->request(LIBCXX_NAMESPACE::http::POST,
			 serveraddr,
			 "X-Request",
			 "header1",
			 LIBCXX_NAMESPACE::http::form::parameters
			 ::create("param1", "value1"));

	got=std::string(resp->begin(), resp->end());
	expected="POST / HTTP/1.1\n"
		"X-Request: header1\n"
		"param1: value1\n";

	if (got != expected)
		throw EXCEPTION("Got: [" + got + "], but expected ["
				+ expected + "]");

	std::cout << "Stopping listener" << std::endl;

	listener->stop();
	listener->wait();
}

int main(int argc, char **argv)
{
	try {
		std::cout << "test1req" << std::endl;

		{
			test1req(savebody());
		}

		{
			savebody body(savebody::create());
			test1req(body);
			std::cout << body->getString();
		}

		std::cout << "test2req" << std::endl;
		{
			test2req(savebody());
		}

		{
			savebody body(savebody::create());
			test2req(body);
			std::cout << body->getString();
		}

		std::cout << "testhttp10req" << std::endl;
		{
			testhttp10req(savebody());
		}

		{
			savebody body(savebody::create());
			testhttp10req(body);
			std::cout << body->getString();
		}

		std::cout << "testdestroyget" << std::endl;
		{
			testdestroyget(savebody());
		}

		{
			savebody body(savebody::create());
			testdestroyget(body);
			std::cout << body->getString();
		}

		std::cout << "testdestroypost" << std::endl;
		{
			testdestroypost(savebody());
		}

		{
			savebody body(savebody::create());
			testdestroypost(body);
			std::cout << body->getString();
		}
		std::cout << "testpool" << std::endl;
		testpool();

		std::cout << "testconnecttimeout" << std::endl;
		testconnecttimeout();

		std::cout << "formpost" << std::endl;
		formpost();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
