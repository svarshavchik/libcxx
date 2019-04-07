/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fdlistener.H"
#include "x/fdtimeout.H"
#include "x/fdtimeoutconfig.H"
#include "x/netaddr.H"

static bool fake_connect=false;
static int fake_connect_fd= -1;

#define LIBCXX_DEBUG_CONNECT() if (fake_connect)		\
	{						\
		int pipefd[2];				\
							\
		if (::pipe(pipefd) < 0) abort();	\
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

#include <iostream>
#include <cstdio>
#include <cstdlib>

class test1Obj : virtual public LIBCXX_NAMESPACE::obj {

public:

	std::mutex m;
	std::condition_variable c;

	bool stopflag;

	void stop()
	{
		std::lock_guard<std::mutex> lock(m);

		stopflag=true;
		c.notify_all();
	}

	test1Obj();
	~test1Obj();

	void run(const LIBCXX_NAMESPACE::fd &socket,
		 const LIBCXX_NAMESPACE::fd &termpipe,
		 const std::string &dummy);
;
};

test1Obj::test1Obj()
{
}

test1Obj::~test1Obj()
{
}

void test1Obj::run(const LIBCXX_NAMESPACE::fd &socket,
		   const LIBCXX_NAMESPACE::fd &termpipe,
		   const std::string &dummy)

{
	std::unique_lock<std::mutex> lock(m);

	c.wait(lock, [this] { return stopflag; });
}

static LIBCXX_NAMESPACE::fdlistener createlistener(int &portnum)
{
	LIBCXX_NAMESPACE::fdlistenerptr listener;
	portnum=4000;

	while (1)
	{
		std::cout << "Trying port " << portnum << std::endl;

		try {
			listener=LIBCXX_NAMESPACE::fdlistener::create(portnum);
			break;
		} catch (LIBCXX_NAMESPACE::exception &e)
		{
		}
		if (++portnum >= 5000)
			throw EXCEPTION("Cannot bind to a port");
	}
	return listener;
}

class two_second_timeout : public LIBCXX_NAMESPACE::fdtimeoutconfig {

public:

	LIBCXX_NAMESPACE::fdbase operator()(const LIBCXX_NAMESPACE::fd &fd)
		const override
	{
		LIBCXX_NAMESPACE::fdtimeout
			to(LIBCXX_NAMESPACE::fdtimeout::create(fd));

		to->set_write_timeout(2);
		return to;
	}
};

static void test1()
{
	LIBCXX_NAMESPACE::ref<test1Obj>
		testthread(LIBCXX_NAMESPACE::ref<test1Obj>::create());

	int portnum;

	LIBCXX_NAMESPACE::fdlistener
		listener(createlistener(portnum));

	std::cout << "Starting listener" << std::endl;
	testthread->stopflag=false;
	listener->start(testthread, std::string("foo"));
	std::cout << "Stopping listener" << std::endl;
	listener=createlistener(portnum);

	std::cout << "Restarting listener for a single connection" << std::endl;
	listener->start(testthread, std::string("bar"));

	{
		std::list<LIBCXX_NAMESPACE::fd> fdlist;

		std::cout << "Making a test connection" << std::endl;
		fdlist.push_back(LIBCXX_NAMESPACE::netaddr::create("", portnum)
				 ->type(SOCK_STREAM)
				 ->connect(two_second_timeout()));
		std::cout << "Stopping listener" << std::endl;
		testthread->stop();
	}

	std::cout << "Waiting for listener to stop" << std::endl;
	listener->stop();
	listener->wait();

	fake_connect=true;
	fake_connect_fd=-1;

	bool caught=false;
	try {
		LIBCXX_NAMESPACE::netaddr
			::create("", portnum)
			->type(SOCK_STREAM)
			->connect(two_second_timeout());

	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
		caught=true;
	}
	fake_connect=false;

	if (!caught || fake_connect_fd < 0)
		throw EXCEPTION("Connect timeout did not work");
	::close(fake_connect_fd);
}

class test2Obj : virtual public LIBCXX_NAMESPACE::obj {

public:

	test2Obj();
	~test2Obj();

	void run(const LIBCXX_NAMESPACE::fd &socket,
		 const LIBCXX_NAMESPACE::fd &termpipe)
;
};


test2Obj::test2Obj()
{
}

test2Obj::~test2Obj()
{
}

void test2Obj::run(const LIBCXX_NAMESPACE::fd &socket,
		   const LIBCXX_NAMESPACE::fd &termpipe)

{
	LIBCXX_NAMESPACE::fdtimeout to(LIBCXX_NAMESPACE::fdtimeout::create(socket));

	to->set_terminate_fd(termpipe);

	LIBCXX_NAMESPACE::istream i(to->getistream());

	std::istream &ii=*i;

	while (ii.get() != EOF)
		;
	std::cout << "server thread terminated" << std::endl;
}

void test2()
{
	LIBCXX_NAMESPACE::ref<test2Obj>
		testthread(LIBCXX_NAMESPACE::ref<test2Obj>::create());

	LIBCXX_NAMESPACE::fdlistenerptr listener;
	int portnum=4000;

	while (1)
	{
		std::cout << "Trying port " << portnum << std::endl;

		try {
			listener=
				LIBCXX_NAMESPACE::fdlistener::create(portnum);
			break;
		} catch (LIBCXX_NAMESPACE::exception &e)
		{
		}
		if (++portnum >= 5000)
			throw EXCEPTION("Cannot bind to a port");
	}

	std::cout << "starting listener" << std::endl;
	listener->start(testthread);

	std::cout << "connecting to the listener" << std::endl;

	LIBCXX_NAMESPACE::fd connfd(LIBCXX_NAMESPACE::netaddr::create("", portnum)
				  ->type(SOCK_STREAM)
				  ->connect());

	std::cout << "Terminating server" << std::endl;

	listener=LIBCXX_NAMESPACE::fdlistenerptr();
}

int main(int argc, char **argv)
{
	try {
		alarm(30);
		std::cout << "test1:" << std::endl;
		test1();
	std::cout << "Done" << std::endl;
		std::cout << "test2:" << std::endl;
		test2();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testfdlistener: "
			  << e << std::endl;
		exit(1);
	}
	return (0);
}
