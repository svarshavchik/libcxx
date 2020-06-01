/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/signalfd.H"
#include "x/sighandler.H"

static void testsignalfd()
{
	LIBCXX_NAMESPACE::signalfd fd=LIBCXX_NAMESPACE::signalfd::create();

	fd->capture(SIGHUP);

	kill(getpid(), SIGHUP);

	if (fd->getsignal().ssi_signo != SIGHUP)
		throw EXCEPTION("Failed to capture SIGHUP");

	{
		LIBCXX_NAMESPACE::signalfd fd2=LIBCXX_NAMESPACE::signalfd::create();

		fd2->capture(SIGHUP);
	}

	fd->nonblock(true);

	if (fd->getsignal().ssi_signo != 0)
		throw EXCEPTION("Nonblocking signal file descriptor failed");
}

class hObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	bool flag;
	std::mutex mutex;
	std::condition_variable cond;

	hObj() : flag(false) {}

	~hObj()=default;

	void signal(int signum)
	{
		std::lock_guard<std::mutex> lock(mutex);

		flag=true;
		cond.notify_all();
	}

	void wait()
	{
		std::unique_lock<std::mutex> lock(mutex);

		while (!flag)
			cond.wait(lock);
	}
};

void testsigexcept()
{
	auto h1=LIBCXX_NAMESPACE::ref<hObj>::create();
	auto h2=LIBCXX_NAMESPACE::ref<hObj>::create();

	auto m1=LIBCXX_NAMESPACE::install_sighandler(SIGHUP,
						     [h1]
						     (int signum)
						     {
							     h1->signal(signum);
						     });
	auto m2=LIBCXX_NAMESPACE::install_sighandler(SIGHUP,
						     [h2]
						     (int signum)
						     {
							     h2->signal(signum);
						     });

	kill(getpid(), SIGHUP);

	h1->wait();
	h2->wait();
}

int main(int argc, char **argv)
{
	try {
		(LIBCXX_NAMESPACE::sigset() + "SIGHUP").block();

		testsignalfd();
		testsigexcept();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testcsv: "
			  << e << std::endl;
	}

	std::cout << "Done." << std::endl;

	return (0);
}
