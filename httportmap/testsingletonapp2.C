/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/managedsingletonapp.H"
#include "x/threadmsgdispatcher.H"
#include "x/option_parser.H"

class test1infoObj : virtual public LIBCXX_NAMESPACE::obj {

public:

	std::mutex m;
	std::condition_variable c;

	bool didnotprocess;
	bool bye;
	bool inrun;
	int counter;

	test1infoObj() : didnotprocess(false), bye(false), inrun(false), counter(0)
	{
	}
};

typedef LIBCXX_NAMESPACE::ref<test1infoObj> test1info;

class test1argsObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	std::string what;

	test1argsObj() {}
	~test1argsObj() {}

	test1argsObj(const std::string &whatArg) : what(whatArg)
	{
	}

	template<typename iter_type> void serialize(iter_type &iter)
	{
		iter(what);
	}
};

typedef LIBCXX_NAMESPACE::ref<test1argsObj> test1args;
typedef LIBCXX_NAMESPACE::ptr<test1argsObj> test1argsptr;

class test1app : virtual public LIBCXX_NAMESPACE::obj {

public:
	test1info info;

	std::mutex m;
	std::condition_variable c;
	bool quitit;

	test1app(const test1info &infoArg) : info(infoArg), quitit(false)
	{
		std::unique_lock<std::mutex> l(info->m);
		++info->counter;
	}

	test1args
	run(uid_t dummy, test1argsptr &args)
	{
		LIBCXX_NAMESPACE::destroy_callback cb=LIBCXX_NAMESPACE::destroy_callback::create();

		args->ondestroy([cb]{cb->destroyed();});

		args=test1argsptr();
		cb->wait();

		{
			std::unique_lock<std::mutex> l(info->m);

			info->inrun=true;
			info->c.notify_all();
		}

		std::unique_lock<std::mutex> l(m);

		c.wait(l, [this] { return quitit; });

		return test1args::create("runbye");
	}

	void instance(uid_t uid, test1argsptr &args,
		      test1args &ret,
		      const LIBCXX_NAMESPACE::singletonapp::processed &flag,
		      const LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj> &mcguffin)
	{
		ret->what=args->what;

		{
			std::unique_lock<std::mutex> l(info->m);

			if (!info->didnotprocess)
			{
				info->didnotprocess=true;
				std::cout << "Did not process, should try again"
					  << std::endl;
				return;
			}

			std::cout << "Processed" << std::endl;
			flag->processed();
		}

		std::unique_lock<std::mutex> l(m);

		quitit=true;
		c.notify_all();
	}

	void stop()
	{
	}
};

class test1threadObj : virtual public LIBCXX_NAMESPACE::obj {

public:

	test1threadObj()
	{
	}

	~test1threadObj()
	{
	}

	test1args run(const test1info &info)
	{
		std::cout << "Waiting for the instance to start" << std::endl;

		{
			std::unique_lock<std::mutex> lock(info->m);

			info->c.wait(lock, [&info] { return info->inrun; });
		}

		std::cout << "Making another instance" << std::endl;
		auto ret=LIBCXX_NAMESPACE::singletonapp
			::managed([&info]
				  { return LIBCXX_NAMESPACE::ref<test1app>
						  ::create(info); },
				  test1args::create("instance"));
		std::cout << "Another instance request processed"
			  << std::endl;
		return ret.first;
	}
};

void test1()
{
	auto info=test1info::create();

	auto thread=LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<test1threadObj>
					::create(), info);

	auto ret=LIBCXX_NAMESPACE::singletonapp
		::managed([&info]
			  {
				  return LIBCXX_NAMESPACE::ref<test1app>
					  ::create(info);
			  }, test1args::create("dummy"));

	if (ret.first->what != "runbye")
		throw EXCEPTION("test1 failed (part 1)");

	if (thread->get()->what != "instance")
		throw EXCEPTION("test1 failed (part 2)");

	if (info->counter != 1)
		throw EXCEPTION("test1 failed (part 3)");
}


class test1altArgsObj : public LIBCXX_NAMESPACE::threadmsgdispatcherObj {

public:

	test1altArgsObj() {}
	~test1altArgsObj() {}

	test1args run(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>
		      &threadmsgdispatcher_mcguffin,
		      uid_t dummy, const test1args &args)
	{
		msgqueue_auto msgqueue(this);

		threadmsgdispatcher_mcguffin=nullptr;
		return args;
	}

	void instance(uid_t uid, const test1argsptr &args,
		      const test1args &ret,
		      const LIBCXX_NAMESPACE::singletonapp::processed &flag,
		      const LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj> &mcguffin)
	{
	}

	void stop() override
	{
	}
};


void test1alt()
{
	LIBCXX_NAMESPACE::singletonapp
		::managed([]
			  {
				  return LIBCXX_NAMESPACE::ref<test1altArgsObj>
					  ::create();
			  }, test1args::create("dummy"));
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
		test1();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
	}
	return 0;
}
