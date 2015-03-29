/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/fdimplbase.H"
#include "x/http/serverimpl.H"
#include "x/threads/run.H"
#include "x/ref.H"
#include "x/fdtimeoutobj.H"

#include <algorithm>
#include <iterator>
#include <sstream>

static void testhttpserver(const std::string &str)

{
	std::stringstream ss;

	LIBCXX_NAMESPACE::http::serverimpl<std::string::const_iterator,
					 std::ostreambuf_iterator<char> >
		(100, str.begin(), str.end(),
		 std::ostreambuf_iterator<char>(ss)).run();

	std::string s;

	int cnt404=0;
	while (!std::getline(ss, s).eof())
	{
		if (s == "HTTP/1.1 404 Not Found\r")
			++cnt404;
	}

	if (cnt404 != 2)
		throw EXCEPTION("serverimpl did not report a 404 twice");
}

class threadwriter : virtual public LIBCXX_NAMESPACE::obj {

	std::string str;

	LIBCXX_NAMESPACE::fd fd;

public:
	threadwriter(const std::string &strArg,
		     const LIBCXX_NAMESPACE::fd &fdArg);

	~threadwriter() noexcept;

	void run();
};

threadwriter::threadwriter(const std::string &strArg,
			   const LIBCXX_NAMESPACE::fd &fdArg)
	: str(strArg), fd(fdArg)
{
}

threadwriter::~threadwriter() noexcept
{
}

void threadwriter::run()
{
	LIBCXX_NAMESPACE::ostream o(fd->getostream());

	std::copy(str.begin(), str.end(),
		  std::ostreambuf_iterator<char>(*o));
}

class bodycollecter : protected LIBCXX_NAMESPACE::http::fdimplbase,
		      public LIBCXX_NAMESPACE::http
		      ::serverimpl<LIBCXX_NAMESPACE::http::fdimplbase
				   ::input_iter_t,
				   LIBCXX_NAMESPACE::http::discardoutput>
{
	typedef LIBCXX_NAMESPACE::http
	::serverimpl<LIBCXX_NAMESPACE::http::fdimplbase::input_iter_t,
		     LIBCXX_NAMESPACE::http::discardoutput> superclass_t;

	std::ostringstream o;

public:
	bodycollecter(const LIBCXX_NAMESPACE::fdbase &fdArg);
	~bodycollecter() noexcept;

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag)
;

	std::string str() const noexcept
	{
		return o.str();
	}
};

bodycollecter::bodycollecter(const LIBCXX_NAMESPACE::fdbase &fdArg)
	: superclass_t(100)
{

	LIBCXX_NAMESPACE::http::fdimplbase::install(fdArg,
						  LIBCXX_NAMESPACE::fdptr());

	LIBCXX_NAMESPACE::http::serverimpl<LIBCXX_NAMESPACE::http::
					 fdimplbase::input_iter_t,
					 LIBCXX_NAMESPACE::http::discardoutput>
		::install
		(LIBCXX_NAMESPACE::http::fdimplbase::input_iter_t(filedesc()),
		 LIBCXX_NAMESPACE::http::fdimplbase::input_iter_t(),
		 LIBCXX_NAMESPACE::http::discardoutput(), 0);

	filedesc_timeout->set_read_timeout(5);
	filedesc_timeout->set_write_timeout(5);
}

bodycollecter::~bodycollecter() noexcept
{
}

void bodycollecter::received(const LIBCXX_NAMESPACE::http::requestimpl &req,
			     bool bodyflag)

{
	if (!bodyflag)
	{
		o << "(null)\r\n";
	}
	else
	{
		std::copy(begin(), end(),
			  std::ostreambuf_iterator<char>(o.rdbuf()));
	}

	LIBCXX_NAMESPACE::http::responseimpl resp;

	std::string dummy;

	send(resp, req, dummy.begin(), dummy.end());
}

static LIBCXX_NAMESPACE::fd createwritingthread(LIBCXX_NAMESPACE::runthreadptr<void> &thr,
					      const char *str,
					      LIBCXX_NAMESPACE::fdptr &w,
					      LIBCXX_NAMESPACE::fdptr &r)

{
	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p=LIBCXX_NAMESPACE::fd::base::pipe();

		r=p.first;
		w=p.second;
	}

	thr=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<threadwriter>::create(str,
									  w));
	return r;
}

static LIBCXX_NAMESPACE::fd createwritingthread(LIBCXX_NAMESPACE::runthreadptr<void> &thr,
						const char *str)

{
	LIBCXX_NAMESPACE::fdptr w, r;

	return createwritingthread(thr, str, w, r);
}

static void testhttpclient(const char *name,
			   const char *req,
			   const char *expected)

{
	LIBCXX_NAMESPACE::runthreadptr<void> thr;

	LIBCXX_NAMESPACE::fd r=createwritingthread(thr, req);

	bodycollecter coll(r);

	std::string err;

	try {
		coll.run();
	} catch (LIBCXX_NAMESPACE::http::response_exception &e)
	{
		std::ostringstream o;

		o << e;

		err=o.str();
	}

	thr->wait();
	std::string res=coll.str() + err;

	if (res != expected)
		throw EXCEPTION(name + std::string(" failed: ") + res);
}

class bodycollecter_timeout : public bodycollecter {

	time_t eofTimeout;
	time_t messageTimeout;
	time_t bodyTimeout;

public:
	bodycollecter_timeout(const LIBCXX_NAMESPACE::fd &fdArg,
			      time_t eofTimeoutArg,
			      time_t messageTimeoutArg,
			      time_t bodyTimeoutArg)
;
	~bodycollecter_timeout() noexcept;

	bool eof();

	bool message(LIBCXX_NAMESPACE::http::requestimpl &req)
;

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag)
;
};

bodycollecter_timeout::bodycollecter_timeout(const LIBCXX_NAMESPACE::fd &fdArg,
					     time_t eofTimeoutArg,
					     time_t messageTimeoutArg,
					     time_t bodyTimeoutArg)

	: bodycollecter(fdArg),
	  eofTimeout(eofTimeoutArg),
	  messageTimeout(messageTimeoutArg),
	  bodyTimeout(bodyTimeoutArg)
{
}

bodycollecter_timeout::~bodycollecter_timeout() noexcept
{
}

bool bodycollecter_timeout::eof()
{
	if (eofTimeout > 0)
	{
		filedesc_timeout->set_read_timeout(eofTimeout);
	}

	bool flag;

	try {
		flag=bodycollecter::eof();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		flag=true;
	}

	if (eofTimeout > 0)
		filedesc_timeout->cancel_read_timer();

	return flag;
}

bool bodycollecter_timeout::message(LIBCXX_NAMESPACE::http::requestimpl &req)

{
	bool flag;

	if (messageTimeout > 0)
	{
		filedesc_timeout->set_read_timeout(messageTimeout);
	}
		
	try {
		flag=bodycollecter::message(req);

		if (messageTimeout > 0)
			filedesc_timeout->cancel_read_timer();

	} catch (const LIBCXX_NAMESPACE::http::response_exception &e)
	{
		if (messageTimeout > 0)
			filedesc_timeout->cancel_read_timer();
		throw;
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		if (messageTimeout > 0)
			filedesc_timeout->cancel_read_timer();
		throw;
	}
	return flag;
}

void bodycollecter_timeout::received(const LIBCXX_NAMESPACE::http::requestimpl
				     &req,
				     bool bodyflag)

{
	if (bodyTimeout > 0)
		filedesc_timeout->set_read_timeout(8192, bodyTimeout);

	try {
		bodycollecter::received(req, bodyflag);

		if (bodyTimeout > 0)
			filedesc_timeout->cancel_read_timer();
	} catch (const LIBCXX_NAMESPACE::http::response_exception &e)
	{
		if (bodyTimeout > 0)
			filedesc_timeout->cancel_read_timer();
		throw;
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		if (bodyTimeout > 0)
			filedesc_timeout->cancel_read_timer();
		throw;
	}
}

static void testhttp11eoftimeout(const char *name,
				 const char *req,
				 const char *expected)

{
	LIBCXX_NAMESPACE::runthreadptr<void> thr;
	LIBCXX_NAMESPACE::fdptr r, w;

	createwritingthread(thr, req, w, r);

	bodycollecter_timeout coll(r, 1, 0, 0);

	std::string err;

	try {
		coll.run();
	} catch (const LIBCXX_NAMESPACE::http::response_exception &e)
	{
		std::ostringstream o;

		o << e;

		err=o.str();
	}

	thr->wait();
	std::string res=coll.str() + err;

	if (res != expected)
		throw EXCEPTION(name + std::string(" failed: ") + res);
}

static void testhttp11messagetimeout(const char *name,
				     const char *req,
				     time_t messageTimeout,
				     time_t dataTimeout)

{
	LIBCXX_NAMESPACE::runthreadptr<void> thr;
	LIBCXX_NAMESPACE::fdptr r, w;

	createwritingthread(thr, req, w, r);

	bodycollecter_timeout coll(r, 0, messageTimeout, dataTimeout);

	std::string err;

	try {
		coll.run();
	} catch (const LIBCXX_NAMESPACE::http::response_exception &e)
	{
		err=(std::string)*e;
	}

	thr->wait();

	if (err != "408 Request Timeout")
		throw EXCEPTION(name + std::string(" did not timeout"));
}

int main(int argc, char **argv)
{
	try {
		alarm(30);
		testhttpserver("POST / HTTP/1.1\r\n"
			       "Host: localhost\r\n"
			       "Content-Length: 7\r\n"
			       "\r\n"
			       "dummy\r\n"
			       "GET / HTTP/1.0\r\n"
			       "Host: localhost\r\n"
			       "\r\n");

		testhttpclient("http 1.0 request (EOF)",
			       "POST / HTTP/1.0\r\n"
			       "Content-Length: 7\r\n"
			       "\r\n"
			       "dummy\r\n",
			       "dummy\r\n");

		testhttpclient("http 1.0 request (not EOF)",
			       "POST / HTTP/1.0\r\n"
			       "Content-Length: 7\r\n"
			       "\r\n"
			       "dummy\r\n"
			       "POST / HTTP/1.0\r\n"
			       "Content-Length: 7\r\n"
			       "\r\n"
			       "dummy\r\n",
			       "dummy\r\n");

		testhttpclient("http 1.1 request (EOF)",
			       "POST / HTTP/1.1\r\n"
			       "Host: localhost\r\n"
			       "Content-Length: 7\r\n"
			       "\r\n"
			       "dummy\r\n",
			       "dummy\r\n");

		testhttpclient("http 1.1 request (not EOF)",
			       "POST / HTTP/1.1\r\n"
			       "Host: localhost\r\n"
			       "Content-Length: 7\r\n"
			       "\r\n"
			       "dummy\r\n"
			       "POST / HTTP/1.1\r\n"
			       "Host: localhost\r\n"
			       "Content-Length: 7\r\n"
			       "\r\n"
			       "dummy\r\n",
			       "dummy\r\n"
			       "dummy\r\n");


		testhttpclient("http 1.1 request (Connection: close)",
			       "POST / HTTP/1.1\r\n"
			       "Host: localhost\r\n"
			       "Connection: dummy\r\n"
			       "Content-Length: 7\r\n"
			       "Connection: dummy2 , , close , dummy3\r\n"
			       "\r\n"
			       "dummy\r\n"
			       "POST / HTTP/1.1\r\n"
			       "Host: localhost\r\n"
			       "Content-Length: 7\r\n"
			       "\r\n"
			       "dummy\r\n",
			       "dummy\r\n");

		testhttpclient("http 1.1 request (Connection: close)",
			       "POST / HTTP/1.1\r\n"
			       "Host: localhost\r\n"
			       "Content-Length: 7\r\n"
			       "\r\n"
			       "dummy\r\n"
			       "GET / HTTP/1.1\r\n"
			       "Host: localhost\r\n"
			       "\r\n"
			       "POST / HTTP/1.1\r\n"
			       "Host: localhost\r\n"
			       "Content-Length: 8\r\n"
			       "Expect: 100-continue\r\n"
			       "\r\n"
			       "dummy1\r\n"
			       "POST / HTTP/1.1\r\n"
			       "Host: localhost\r\n"
			       "Content-Length: 8\r\n"
			       "Expect: 100-continue\r\n"
			       "\r\n"
			       "dummy2\r\n",
			       "dummy\r\n"
			       "(null)\r\n"
			       "dummy1\r\n"
			       "dummy2\r\n");

		testhttp11eoftimeout("http 1.1 request (EOF timeout)",
				     "POST / HTTP/1.1\r\n"
				     "Host: localhost\r\n"
				     "Content-Length: 7\r\n"
				     "\r\n"
				     "dummy\r\n",
				     "dummy\r\n");

		testhttp11messagetimeout("http 1.1 request (message timeout)",
					 "POST / HTTP/1.1\r\n"
					 "Host: localhost\r\n", 1, 0);

		testhttp11messagetimeout("http 1.1 request (body timeout)",
					 "POST / HTTP/1.1\r\n"
					 "Host: localhost\r\n"
					 "Content-Length: 20\r\n"
					 "\r\n"
					 "dummy\r\n", 0, 1);

	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

