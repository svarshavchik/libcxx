/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/singletonapp.H"
#include "x/options.H"
#include "x/threadmsgdispatcher.H"
#include "x/sigset.H"

#include <signal.h>
#include <mutex>
#include <condition_variable>

class testapp : virtual public LIBCXX_NAMESPACE::obj {

public:
	std::mutex mutex;
	std::condition_variable cond;
	size_t counter;
	bool quit;

	testapp() : counter(0), quit(false)
	{
	}

	void child()
	{
		std::unique_lock<std::mutex> lock(mutex);

		++counter;
		cond.notify_all();

		while (!quit)
			cond.wait(lock);
	}

	class thr : virtual public LIBCXX_NAMESPACE::obj {

	public:
		LIBCXX_NAMESPACE::ptr<testapp> app;

		thr(const LIBCXX_NAMESPACE::ptr<testapp> &appArg)
			: app(appArg)
		{
		}

		~thr()
		{
		}

		void run(const LIBCXX_NAMESPACE::fd &fd)
		{
			LIBCXX_NAMESPACE::singletonapp::validate_peer(fd);

			fd->write("", 1);
			app->child();
		}
	};

	LIBCXX_NAMESPACE::ref<thr> new_thread()
	{
		return LIBCXX_NAMESPACE::ref<thr>
			::create(LIBCXX_NAMESPACE::ptr<testapp>(this));

	}
};

static void testsingleton()
{
	char dummy;

	auto app=LIBCXX_NAMESPACE::ref<testapp>::create();

	LIBCXX_NAMESPACE::singletonapp::instance
		inst1=LIBCXX_NAMESPACE::singletonapp::create(app),
		inst2=LIBCXX_NAMESPACE::singletonapp::create(app);

	LIBCXX_NAMESPACE::singletonapp::validate_peer(inst1->connection);
	LIBCXX_NAMESPACE::singletonapp::validate_peer(inst2->connection);

	inst1->connection->read(&dummy, 1);
	inst2->connection->read(&dummy, 1);

	bool caught_exception=true;

	{
		std::unique_lock<std::mutex> lock(app->mutex);

		while (app->counter < 2)
			app->cond.wait(lock);

		if (getuid())
		{
			try {
				LIBCXX_NAMESPACE::singletonapp::instance
					dummy=LIBCXX_NAMESPACE::singletonapp
					::create(app, 0);
				caught_exception=false;
			} catch (const x::exception &e)
			{
				std::cerr << "Expected exception: "
					  << e << std::endl;
			}
		}


		app->quit=true;
		app->cond.notify_all();
	}

	if (!caught_exception)
		throw EXCEPTION("Different userid connect does not work");
}

class testapp2 : virtual public LIBCXX_NAMESPACE::obj {

public:


	std::mutex mutex;
	std::condition_variable cond;

	bool started;

	testapp2() : started(false)
	{
	}

	~testapp2() {}

	class thr : public LIBCXX_NAMESPACE::threadmsgdispatcherObj {

	public:
		testapp2 *p;

		thr(testapp2 *pArg) : p(pArg)
		{
		}

		~thr()
		{
		}

		void run(x::ptr<x::obj> &threadmsgdispatcher_mcguffin,
			 const LIBCXX_NAMESPACE::fd &dummy)
		{
			msgqueue_auto msgqueue(this);

			threadmsgdispatcher_mcguffin=nullptr;

			{
				std::lock_guard<std::mutex> lock(p->mutex);

				p->started=true;

				p->cond.notify_all();
			}

			while (1)
				msgqueue.event();
		}
	};

	LIBCXX_NAMESPACE::ref<thr> new_thread()
	{
		return LIBCXX_NAMESPACE::ref<thr>::create(this);

	}
};

void testsingleton2()
{
	std::cout << "testsingleton2" << std::endl;

	LIBCXX_NAMESPACE::ref<testapp2> app=
		LIBCXX_NAMESPACE::ref<testapp2>::create();

	LIBCXX_NAMESPACE::singletonapp::instance
		inst=LIBCXX_NAMESPACE::singletonapp::create(app);

	{
		std::unique_lock<std::mutex> lock(app->mutex);

		while (!app->started)
			app->cond.wait(lock);
	}

	std::cout << "killing" << std::endl;

	kill(getpid(), SIGHUP);
}

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::list
		options(LIBCXX_NAMESPACE::option::list::create());

	options->addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser
		optionParser(LIBCXX_NAMESPACE::option::parser::create());

	optionParser->setOptions(options);

	int err=optionParser->parseArgv(argc, argv);

	if (err == 0)
		err=optionParser->validate();

	switch (err) {
	case 0:
		break;
	case LIBCXX_NAMESPACE::option::parser::base::err_builtin:
		exit(1);
	default:
		std::cerr << optionParser->errmessage();
		exit(1);
	}

	try {
		testsingleton();
		testsingleton2();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
	}
	return 0;
}
