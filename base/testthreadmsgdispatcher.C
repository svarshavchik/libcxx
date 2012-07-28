/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "eventqueuemsgdispatcher.H"
#include "threads/run.H"

#include <vector>

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

		typedef LIBCXX_NAMESPACE::eventqueue<intarraymsg> ack_queue_t;

		mythr->stop();
		thr->wait();

		if (mythr->sum != 7)
			throw EXCEPTION("Unexpected result from test message");

	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

