/*
** Copyright 2012-2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/eventqueuemsgdispatcher.H"
#include "x/threadeventqueuemsgdispatcher.H"
#include "x/threads/run.H"

#include <vector>
#include <stdlib.h>

class intarray : virtual public LIBCXX_NAMESPACE::obj {

public:
	std::vector<int> ints;

	intarray()
	{
	}

	~intarray() noexcept
	{
	}
};

typedef LIBCXX_NAMESPACE::ptr<intarray> intarraymsg;

class mythreadObj : public LIBCXX_NAMESPACE::eventqueuemsgdispatcherObj,
		    virtual public LIBCXX_NAMESPACE::obj {


public:
	template<typename obj_type, typename msg_type>
	friend class LIBCXX_NAMESPACE::dispatchablemsgObj;

	int sum;

	mythreadObj(const LIBCXX_NAMESPACE::eventfd
		    &efd=LIBCXX_NAMESPACE::eventfd::create())

		: LIBCXX_NAMESPACE::eventqueuemsgdispatcherObj(efd)
	{
	}

	~mythreadObj() noexcept
	{
	}

	void run()
	{
		sum=0;
		while (1)
			msgqueue->pop()->dispatch();
	}

private:
	void dispatch(intarraymsg &msg)
	{
		for (std::vector<int>::iterator b(msg->ints.begin()),
			     e(msg->ints.end()); b != e; ++b)
			sum += *b;
	}

public:
	void submit(intarraymsg &msg)
	{
		sendevent<intarraymsg>(this, msg);
	}
};

typedef LIBCXX_NAMESPACE::ptr<mythreadObj> mythread;

class mythread2Obj : public LIBCXX_NAMESPACE::threadeventqueuemsgdispatcherObj<mythread2Obj> {

public:

	mythread2Obj() {}
	~mythread2Obj() noexcept {}

	void run(const LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj> &mcguffin,
		 const msgqueue_t &q,
		 int foo)
	{
		while (1)
			q->pop()->dispatch();
	}
};

int main(int argc, char **argv)
{
	try {
		mythread mythr(mythread::create());

		auto thr=LIBCXX_NAMESPACE::run(mythr);

		{
			intarraymsg msg(intarraymsg::create());
			msg->ints.resize(3);
			msg->ints[0]=1;
			msg->ints[1]=2;
			msg->ints[2]=4;
			mythr->submit(msg);
		}

		mythr->stop();
		thr->wait();

		if (mythr->sum != 7)
			throw EXCEPTION("Unexpected result from test message");

		auto t2=LIBCXX_NAMESPACE::ref<mythread2Obj>::create();

		auto t2_thread=t2->run_thread(2);
		t2->stop();
		t2_thread->wait();
		t2->stop();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
