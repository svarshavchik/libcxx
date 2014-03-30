/*
** Copyright 2012-2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/obj.H"
#include "x/ptr.H"
#include "x/weaklist.H"
#include "x/weakmap.H"
#include "x/weakmultimap.H"
#include "x/destroycallbackobj.H"
#include "x/ondestroy.H"
#include "x/mutex.H"
#include "x/vector.H"
#include "x/cond.H"
#include "x/threads/run.H"
#include "x/property_properties.H"
#include "x/rwlock.H"
#include "x/fd.H"
#include "x/fditer.H"
#include "x/fdtimeout.H"
#include "x/fdreadlimit.H"
#include "x/eventfd.H"
#include "x/attr.H"
#include "x/epoll.H"
#include "x/fileattr.H"
#include "x/sysexception.H"
#include "x/netaddr.H"
#include "x/dir.H"
#include "x/fdstreambufobj.H"
#include "x/timerfd.H"
#include "x/timespec.H"
#include "x/eventqueue.H"
#include "x/eventdestroynotify.H"
#include "x/eventqueuedestroynotify.H"
#include "x/dirwalk.H"
#include "x/destroycallbackflagobj.H"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

class cl1 : virtual public LIBCXX_NAMESPACE::obj {

	cl1(); // UNDEFINED
};

class cl2 : public cl1 {

	cl2(); // UNDEFINED
};

typedef LIBCXX_NAMESPACE::ptr<cl1> refcl1;
typedef LIBCXX_NAMESPACE::ptr<cl2> refcl2;

void testref1(refcl1 r1)
{
}

void testref2(refcl2 r2)
{
	testref1(r2);

	refcl1 r1(r2);

	refcl2 r2b(r1);
}

class cl3 : virtual public LIBCXX_NAMESPACE::obj {

public:
	cl3() noexcept {}
	~cl3() noexcept {}
};

class cl4 : virtual public cl3 {
public:
	cl4() noexcept {}
	~cl4() noexcept {}
};

void testref3()
{
	LIBCXX_NAMESPACE::ptr<cl3> r1, r2=LIBCXX_NAMESPACE::ptr<cl3>::create();

	LIBCXX_NAMESPACE::const_ptr<cl3> cr1,
		cr2=LIBCXX_NAMESPACE::ptr<cl3>::create();

	LIBCXX_NAMESPACE::ptr<cl3> r3(r1);

	cr1=r1;

	LIBCXX_NAMESPACE::ptr<cl4> r4(r1);
	LIBCXX_NAMESPACE::const_ptr<cl4> cr3(r3), cr4(cr1);
}

#include <typeinfo>

class base : virtual public LIBCXX_NAMESPACE::obj {

public:

	int value;

	base(int v) : value(v)
	{
		std::cerr << "Created " << v << std::endl;
	}

	~base() noexcept
	{
		std::cerr << "Destroyed " << value << std::endl;
	}
};

class derived : public base {

public:

	derived(int v) : base(v) {}
	~derived() noexcept {}
};

class notderived : virtual public LIBCXX_NAMESPACE::obj {
public:
	notderived() {}
	~notderived() noexcept {}
};

class threadtestinfo : virtual public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::condptr condvar;
	LIBCXX_NAMESPACE::mutexptr mutex;
	volatile int *lockflag;
	int processnum;

	~threadtestinfo() noexcept {}
};

class threadtestproc : virtual public LIBCXX_NAMESPACE::obj {

public:
	int run(const LIBCXX_NAMESPACE::ref<threadtestinfo> &arg);
};

int threadtestproc::run(const LIBCXX_NAMESPACE::ref<threadtestinfo> &arg)
{
	int i;

	threadtestinfo &testinfo=*arg;

	for (i=0; i<5; i++)
	{
		{
			LIBCXX_NAMESPACE::mutex::base::mlock
				lock1=testinfo.mutex->lock();

			while (*testinfo.lockflag != testinfo.processnum)
				testinfo.condvar->wait(lock1);

			std::cerr << "Thread " << testinfo.processnum
				  << " got signaled." << std::endl
				  << std::flush;

			*testinfo.lockflag= 1 - testinfo.processnum;

			testinfo.condvar->notify_one();
		}
	}

	return testinfo.processnum;
}

static void objtest()
{

	LIBCXX_NAMESPACE::ptr<base>
		p4=LIBCXX_NAMESPACE::ptr<base>::create(4),
		p5=LIBCXX_NAMESPACE::ptr<base>::create(5), p6;

	if (p4->isa<LIBCXX_NAMESPACE::ref<derived>>())
		throw EXCEPTION("False derived");

	p6=p5;
	p5=p4;
	std::cerr << "Swap completed" << std::endl;
	p4=LIBCXX_NAMESPACE::ptr<base>();
	std::cerr << "p4 erased" << std::endl;
	p5=LIBCXX_NAMESPACE::ptr<base>();
	std::cerr << "p5 erased" << std::endl;

	try {
		{
			std::mutex test_mutex;
		}
		throw EXCEPTION("Test exception");
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "Exception thrown" << std::endl;
	}

	LIBCXX_NAMESPACE::mpcobj<bool> flag(false);

	if (LIBCXX_NAMESPACE::mpcobj<bool>::lock(flag)
	    .wait_until(LIBCXX_NAMESPACE::timer::base::clock_t::now()
			+ std::chrono::milliseconds(250)))
	{
		throw EXCEPTION("How did mpcobj get signaled?");
	}

	auto d=LIBCXX_NAMESPACE::ref<derived>::create(2);

	if (!d->isa<LIBCXX_NAMESPACE::ref<derived>>() ||
	    !d->isa<LIBCXX_NAMESPACE::ptr<base>>() ||
	    d->isa<LIBCXX_NAMESPACE::ref<notderived>>())
		throw EXCEPTION("isa() test failed");
}

static void threadtest()
{
	std::cerr << std::flush;

	LIBCXX_NAMESPACE::condptr condvar(LIBCXX_NAMESPACE::condptr::create());
	LIBCXX_NAMESPACE::mutex mutex(LIBCXX_NAMESPACE::mutex::create());
	volatile int lockflag=0;

	LIBCXX_NAMESPACE::ref<threadtestinfo>
		thread1_arg(LIBCXX_NAMESPACE::ref<threadtestinfo>::create()),
		thread2_arg(LIBCXX_NAMESPACE::ref<threadtestinfo>::create());

	thread1_arg->condvar=condvar;
	thread1_arg->mutex=mutex;
	thread1_arg->lockflag= &lockflag;
	thread1_arg->processnum=0;

	thread2_arg->condvar=condvar;
	thread2_arg->mutex=mutex;
	thread2_arg->lockflag= &lockflag;
	thread2_arg->processnum=1;

	LIBCXX_NAMESPACE::ref<threadtestproc>
		thread1Obj=LIBCXX_NAMESPACE::ref<threadtestproc>::create();
	LIBCXX_NAMESPACE::ref<threadtestproc>
		thread2Obj=LIBCXX_NAMESPACE::ref<threadtestproc>::create();

	auto thread1_ret=LIBCXX_NAMESPACE::run(thread1Obj, thread1_arg);
	auto thread2_ret=LIBCXX_NAMESPACE::run(thread2Obj, thread2_arg);

	int exit1=thread1_ret->get();
	int exit2=thread2_ret->get();

	std::cerr << "Return codes: " << exit1 << ", " << exit2
		  << std::endl;
}

static void exceptiontest()
{
	try {
		LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> dummy;

		*dummy;
	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "Null ptr dereference" << std::endl;
	}
}

class custom_exception1 {

public:
	int errcode;

	custom_exception1(int n) : errcode(n)
	{
	}

	int getErrorCode()
	{
		return errcode;
	}
};

class custom_exception2 {

public:

	typedef int base;

	custom_exception2()
	{
	}

	void describe(LIBCXX_NAMESPACE::exception &e)
	{
		e << "foo";
	}
};

char custom_exception_base_class_works
[sizeof(LIBCXX_NAMESPACE::custom_exception<custom_exception2>
	::base) == sizeof(int) ? 1:-1];
					       
static void exceptiontest2()
{
	try {
		throw CUSTOM_EXCEPTION(custom_exception1, 100);
	} catch (const LIBCXX_NAMESPACE::custom_exception<custom_exception1> &c)
	{
		c->getErrorCode();
	}

	try {
		throw CUSTOM_EXCEPTION(custom_exception1, 100);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}

	std::ostringstream o;

	try {
		throw CUSTOM_EXCEPTION(custom_exception2);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		o << e;
	}

	if (o.str() != "foo")
	{
		std::cerr << "Custom exception describe() did not work"
			  << std::endl;
		exit(1);
	}
}

static void testattr(LIBCXX_NAMESPACE::attr tmp)
{
	tmp->setattr("user.foo", "foo");
	tmp->setattr("user.bar", "bar");

	std::set<std::string> attrs=tmp->listattr();

	if (attrs.find("user.foo") == attrs.end() ||
	    attrs.find("user.bar") == attrs.end() ||
	    tmp->getattr("user.foo") != "foo" ||
	    tmp->getattr("user.bar") != "bar")
		throw EXCEPTION("testattr failed");

	tmp->setattr("user.foo", "foobar");
	tmp->removeattr("user.foo");
}

static void fdtest()
{
	try {
		{
			LIBCXX_NAMESPACE::fdptr r, w;

			{
				std::pair<LIBCXX_NAMESPACE::fd,
					  LIBCXX_NAMESPACE::fd>
					p(LIBCXX_NAMESPACE::fd::base::pipe());

				r=p.first;
				w=p.second;
			}

			std::string buf;

			LIBCXX_NAMESPACE::istream rseq(r->getistream());
			LIBCXX_NAMESPACE::ostream wseq(w->getostream());

			(*wseq) << "foo" << std::endl << std::flush;

			std::getline(*rseq, buf);

			if (buf != "foo")
				throw EXCEPTION("Did not read foo!\n");
		}

		{
			LIBCXX_NAMESPACE::fd tmp=LIBCXX_NAMESPACE::fd::base::tmpfile();

			tmp->futimens();

			LIBCXX_NAMESPACE::iostream seq(tmp->getiostream());

			char buf[5];

			(*seq) << "foo\nbar\n";

			seq->seekp(-8, std::ios::cur);

			seq->read(buf, sizeof(buf)-1);
			buf[sizeof(buf)-1]=0;

			if (seq->fail() || strcmp(buf, "foo\n"))
				throw EXCEPTION("Did not read foo!\n");

			(*seq) << "baz\n";

			seq->seekp(-4, std::ios::cur);

			seq->read(buf, sizeof(buf)-1);
			buf[sizeof(buf)-1]=0;

			if (seq->fail() || strcmp(buf, "baz\n"))
				throw EXCEPTION("Did not read foo!\n");

			bool has_xattr=false;

			try {
				tmp->setattr("user.foo", "foo");
				has_xattr=true;
			} catch (...)
			{
			}
			if (has_xattr)
			{
				testattr(tmp);

				LIBCXX_NAMESPACE::fd::base::open("testattr.tmp", O_CREAT|O_RDWR,
					    0600);

				testattr(LIBCXX_NAMESPACE::fileattr(LIBCXX_NAMESPACE::fileattr::create("testattr.tmp")));
				unlink("testattr.tmp");
			}
		}
	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
	}
}

static void fdtest2()
{
	try {
		LIBCXX_NAMESPACE::fdptr r, w, r2, w2;

		{
			std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
				p(LIBCXX_NAMESPACE::fd::base::pipe());

			r=p.first;
			w=p.second;
		}

		{
			std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
				p(LIBCXX_NAMESPACE::fd::base::pipe());

			r2=p.first;
			w2=p.second;
		}

		r2->nonblock(true);
		w2->nonblock(true);

		char dummy;
		r2->read(&dummy, 1);

		LIBCXX_NAMESPACE::fdtimeout tfd(LIBCXX_NAMESPACE::fdtimeout
					      ::create(r2));

		tfd->set_terminate_fd(r);
		w->close();

		bool caught=false;

		try {

			tfd->pubread(&dummy, 1);
		} catch(const LIBCXX_NAMESPACE::exception &e)
		{
			caught=true;
		}

		if (!caught)
			throw EXCEPTION("fdtest2: Expected exception was not thrown");
	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
	}
}

static void dirtest() noexcept
{
	try {
		LIBCXX_NAMESPACE::dir::base::rmrf("testdir");
		LIBCXX_NAMESPACE::dir::base::mkdir_parent("testdir/a", 0777);

		LIBCXX_NAMESPACE::fd::base::open("testdir/a", O_CREAT|O_RDWR, 0666);
		mkdir("testdir/b", 0777);
		LIBCXX_NAMESPACE::fd::base::open("testdir/c", O_CREAT|O_RDWR, 0666);


		std::vector<std::string> dirContents;

		LIBCXX_NAMESPACE::dir d=LIBCXX_NAMESPACE::dir::create("testdir");

		dirContents.insert(dirContents.end(), d->begin(), d->end());

		std::sort(dirContents.begin(), dirContents.end());

		std::vector<std::string>::iterator b=dirContents.begin(),
			e=dirContents.end();

		while (b != e)
		{
			std::cout << "entry: " << *b++ << std::endl;
		}

		LIBCXX_NAMESPACE::dir::base::rmrf("testdir");
		std::cout << "rm -rf completed" << std::endl;
		std::cout << std::flush;

		LIBCXX_NAMESPACE::dir::base::mkdir("testdir/a/b");

		std::cout << "Testing dirwalk" << std::endl;

		LIBCXX_NAMESPACE::dir::base::rmrf("testdir");

		LIBCXX_NAMESPACE::dir::base::mkdir("testdir/a/b/c");

		{
			LIBCXX_NAMESPACE::dirwalk
				dirwalk=LIBCXX_NAMESPACE::dirwalk
				::create("testdir");

			std::list<LIBCXX_NAMESPACE::dir::base::entry> strlist;

			strlist.insert(strlist.end(),
				       dirwalk->begin(),
				       dirwalk->end());

			while (!strlist.empty())
			{
				std::cout << "preorder: "
					  << strlist.front().fullpath()
					  << std::endl;
				strlist.pop_front();
			}
		}

		{
			LIBCXX_NAMESPACE::dirwalk
				dirwalk=LIBCXX_NAMESPACE::dirwalk
				::create("testdir", true);

			std::list<LIBCXX_NAMESPACE::dir::base::entry> strlist;

			strlist.insert(strlist.end(),
				       dirwalk->begin(),
				       dirwalk->end());

			while (!strlist.empty())
			{
				std::cout << "postorder: "
					  << strlist.front().fullpath()
					  << std::endl;
				strlist.pop_front();
			}
		}

		LIBCXX_NAMESPACE::fd::base::open("testdir/a/b/c/d",
					       O_CREAT|O_RDWR, 0666);

		{
			LIBCXX_NAMESPACE::dirwalk 
				dirwalk=LIBCXX_NAMESPACE::dirwalk
				::create("testdir");

			std::list<LIBCXX_NAMESPACE::dir::base::entry> strlist;

			strlist.insert(strlist.end(),
				       dirwalk->begin(),
				       dirwalk->end());

			while (!strlist.empty())
			{
				std::cout << "preorder: "
					  << strlist.front().fullpath()
					  << std::endl;
				strlist.pop_front();
			}
		}

		{
			LIBCXX_NAMESPACE::dirwalk
				dirwalk=LIBCXX_NAMESPACE::dirwalk
				::create("testdir", true);

			std::list<LIBCXX_NAMESPACE::dir::base::entry> strlist;

			strlist.insert(strlist.end(),
				       dirwalk->begin(),
				       dirwalk->end());

			while (!strlist.empty())
			{
				std::cout << "postorder: "
					  << strlist.front().fullpath()
					  << std::endl;
				strlist.pop_front();
			}
		}
		LIBCXX_NAMESPACE::dir::base::rmrf("testdir");
	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "dirtest: " << e << std::endl;
	}
}

class testmutexObj : public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::mutex m;
	LIBCXX_NAMESPACE::cond c;

	size_t lockflag;
	bool quit;

	testmutexObj() : m(LIBCXX_NAMESPACE::mutex::create()),
			 c(LIBCXX_NAMESPACE::cond::create()), lockflag(0),
			 quit(false)
	{
	}

	~testmutexObj() noexcept {
	}

	void run()
	{
		LIBCXX_NAMESPACE::mutex::base::mlock l=m->lock();

		++lockflag;
		c->notify_all();

		while (!quit)
			c->wait(l);
		--lockflag;
		c->notify_one();
	}
};

void testmutex()
{
	{

		LIBCXX_NAMESPACE::mutex m=LIBCXX_NAMESPACE::mutex::create();

		LIBCXX_NAMESPACE::mutex::base::mlock l=m->lock();

		LIBCXX_NAMESPACE::mutex::base::mlockptr p=
			m->wait_until(LIBCXX_NAMESPACE::timer::base::clock_t::now()
				      + std::chrono::milliseconds(500));
		if (!p.null())
			throw EXCEPTION("How did I get this lock?");

		LIBCXX_NAMESPACE::cond c=LIBCXX_NAMESPACE::cond::create();

		if (c->wait_until(l, LIBCXX_NAMESPACE::timer::base::clock_t::now()
				  + std::chrono::milliseconds(250)))
			throw EXCEPTION("How did I get signaled?");
	}

	LIBCXX_NAMESPACE::ref<testmutexObj> tm=
		LIBCXX_NAMESPACE::ref<testmutexObj>::create();

	std::vector<LIBCXX_NAMESPACE::runthreadbaseptr> threads;

	threads.resize(5);

	std::cout << "Starting " << threads.size() << " threads" << std::endl;

	{
		LIBCXX_NAMESPACE::mutex::base::mlock l=tm->m->lock();

		if (!tm->m->trylock().null())
			throw EXCEPTION("testmutex: locked twice?");

		for (LIBCXX_NAMESPACE::runthreadbaseptr &thr : threads)
		{
			thr=LIBCXX_NAMESPACE::run(tm);
		}

		std::cout << "Waiting until they get started" << std::endl;

		while (tm->lockflag < threads.size())
			tm->c->wait(l);

		tm->quit=true;
		tm->c->notify_all();

		std::cout << "Waiting until they stop" << std::endl;

		while (tm->lockflag > 0)
			tm->c->wait(l);

		std::cout << "Waiting until they really stop" << std::endl;

		for (auto thr : threads)
		{
			thr->wait();
		}
	}
}

class rwlocktestinfo : virtual public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::rwlock rwl;

	LIBCXX_NAMESPACE::mutex globmutex;
	LIBCXX_NAMESPACE::cond globcond;

	volatile int counter;

	rwlocktestinfo() noexcept;
	~rwlocktestinfo() noexcept;
};

class rthread : virtual public LIBCXX_NAMESPACE::obj {

public:
	void run(const LIBCXX_NAMESPACE::ref<rwlocktestinfo> &rwti);
};

class wthread : virtual public LIBCXX_NAMESPACE::obj {

public:
	void run(const LIBCXX_NAMESPACE::ref<rwlocktestinfo> &rwti);
};

rwlocktestinfo::rwlocktestinfo() noexcept
: rwl(LIBCXX_NAMESPACE::rwlock::create()),
			     globmutex(LIBCXX_NAMESPACE::mutex::create()),
			     globcond(LIBCXX_NAMESPACE::cond::create()),
			     counter(0)
{
}

rwlocktestinfo::~rwlocktestinfo() noexcept
{
}

void rthread::run(const LIBCXX_NAMESPACE::ref<rwlocktestinfo> &rwti)
{
	std::cout << "Starting read lock thread" << std::endl
		  << std::flush;
	

	LIBCXX_NAMESPACE::rwlock::base::rlock rlock=rwti->rwl->readlock();

	LIBCXX_NAMESPACE::mutex::base::mlock lock=rwti->globmutex->lock();

	++rwti->counter;

	rwti->globcond->notify_all();

	std::cout << "Started read lock thread" << std::endl
		  << std::flush;

	while (rwti->counter)
		rwti->globcond->wait(lock);
}

void wthread::run(const LIBCXX_NAMESPACE::ref<rwlocktestinfo> &rwti)
{
		LIBCXX_NAMESPACE::rwlock::base::wlock wlock=rwti->rwl->writelock();
}

static void rwlocktest() noexcept
{
	{
		auto rw=LIBCXX_NAMESPACE::rwlock::create();

		rw->readlock();
		rw->writelock();
		rw->readlock();

		auto w=rw->writelock();

		if (!rw->try_writelock().null())
		{
			throw EXCEPTION("How did rwlock get a write lock?");
		}

		if (!rw->try_writelock_until(LIBCXX_NAMESPACE::timer::base::clock_t::now()
					     + std::chrono::milliseconds(250))
		    .null())
		{
			throw EXCEPTION("How did rwlock get a write lock?");
		}

		if (!rw->try_writelock_for(std::chrono::milliseconds(250))
		    .null())
		{
			throw EXCEPTION("How did rwlock get a write lock?");
		}

		if (!rw->try_readlock().null())
		{
			throw EXCEPTION("How did rwlock get a read lock?");
		}

		if (!rw->try_readlock_until(LIBCXX_NAMESPACE::timer::base::clock_t::now()
					    + std::chrono::milliseconds(250)).null())
		{
			throw EXCEPTION("How did rwlock get a read lock?");
		}

		if (!rw->try_readlock_for(std::chrono::milliseconds(250)).null())
		{
			throw EXCEPTION("How did rwlock get a read lock?");
		}

	}

	std::cout << "Starting two threads" << std::endl;

	auto rwti=LIBCXX_NAMESPACE::ref<rwlocktestinfo>::create();

	auto r1=LIBCXX_NAMESPACE::ref<rthread>::create(),
		r2=LIBCXX_NAMESPACE::ref<rthread>::create();

	auto r1_ret=LIBCXX_NAMESPACE::run(r1, rwti);

	{
		LIBCXX_NAMESPACE::mutex::base::mlock
			lock=rwti->globmutex->lock();

		while (rwti->counter < 1)
			rwti->globcond->wait(lock);
	}

	auto w1=LIBCXX_NAMESPACE::ref<wthread>::create();

	auto w1_ret=LIBCXX_NAMESPACE::run(w1, rwti);

	sleep(3);

	auto r2_ret=LIBCXX_NAMESPACE::run(r2, rwti);

	{
		int acc=0;

		LIBCXX_NAMESPACE::mutex::base::mlock
			lock=rwti->globmutex->lock();

		while (acc < 2)
		{
			while (rwti->counter == 0)
				rwti->globcond->wait(lock);

			acc += rwti->counter;
			rwti->counter=0;
			rwti->globcond->notify_all();
		}

		std::cout << "Started two threads" << std::endl;
	}

	r1_ret->wait();
	r2_ret->wait();
	w1_ret->wait();
	std::cout << "Threads terminated" << std::endl;
}

class testepollObj : public LIBCXX_NAMESPACE::epoll::base::callbackObj {

public:
	bool flag;

	testepollObj() : flag(false)
	{
	}

	~testepollObj() noexcept
	{
	}

	void event(const LIBCXX_NAMESPACE::fd &fileDesc,
		   event_t events)
	{
		flag=true;
	}
};

static void testepoll() noexcept
{
	try {
		LIBCXX_NAMESPACE::fdptr r, w;

		{
			std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
				p(LIBCXX_NAMESPACE::fd::base::pipe());

			r=p.first;
			w=p.second;
		}

		LIBCXX_NAMESPACE::epoll epoll(LIBCXX_NAMESPACE::epoll::create());

		LIBCXX_NAMESPACE::ptr<testepollObj>
			testepoll(LIBCXX_NAMESPACE::ptr<testepollObj>::create());

		r->epoll_add(EPOLLIN, epoll, testepoll);

		w->write("foo", 4);

		epoll->epoll_wait();

		if (!testepoll->flag)
			throw EXCEPTION("Unexpected result from epoll_wait()");

	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
}

class testnetObj : public LIBCXX_NAMESPACE::epoll::base::callbackObj {

public:
	bool flag;

	testnetObj() : flag(false)
	{
	}

	~testnetObj() noexcept
	{
	}

	void event(const LIBCXX_NAMESPACE::fd &fileDesc,
		   event_t events)
	{
		LIBCXX_NAMESPACE::sockaddrptr addr;
		const LIBCXX_NAMESPACE::fdptr newfd= fileDesc->accept(addr);

		if (!newfd.null())
		{
			std::cout << "Accepted connection" << std::endl;
			flag=true;
		}
	}
};

static void testnet() noexcept
{
	try {
		unlink("testsock.tmp");

		std::list<LIBCXX_NAMESPACE::fd> listen_fd;

		LIBCXX_NAMESPACE::netaddr::base::result res(*LIBCXX_NAMESPACE::netaddr::create(SOCK_STREAM,
							   "file::testsock.tmp")
				       );

		{
			LIBCXX_NAMESPACE::netaddr::base::result
				res2(*res + *res);
		}

		for (auto as: *res)
		{
			;
		}

		LIBCXX_NAMESPACE::netaddr::create(SOCK_STREAM,
				   "file:testsock.tmp")
			->bind(listen_fd, true);

		LIBCXX_NAMESPACE::fd::base::listen(listen_fd);

		std::list<LIBCXX_NAMESPACE::fd>::iterator b, e;

		for (b=listen_fd.begin(), e=listen_fd.end(); b != e; ++b)
		{
			(*b)->nonblock(true);
		}

		LIBCXX_NAMESPACE::epoll epollSet(LIBCXX_NAMESPACE::epoll::create());

		LIBCXX_NAMESPACE::ptr<testnetObj>
			callback(LIBCXX_NAMESPACE::ptr<testnetObj>::create());

		epollSet->epoll_add(listen_fd, EPOLLIN, callback);

		auto ret=LIBCXX_NAMESPACE::run_lambda
			([]
			 (const LIBCXX_NAMESPACE::netaddr &addr)
			 {
				 LIBCXX_NAMESPACE::fd fd(addr->connect());

				 char dummy;

				 fd->read(&dummy, 1);
			 },

			 LIBCXX_NAMESPACE::netaddr::create
			 (SOCK_STREAM, "file:testsock.tmp"));

		epollSet->epoll_wait();

		if (!callback->flag)
			throw EXCEPTION("Filesystem socket connect failed");
		ret->wait();
		unlink("testsock.tmp");
	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}

	try {
		LIBCXX_NAMESPACE::sockaddrptr addr1;
		LIBCXX_NAMESPACE::sockaddrptr addr2;

		addr1=LIBCXX_NAMESPACE::sockaddr::create(AF_INET, "127.0.0.1", 80);
		addr2=LIBCXX_NAMESPACE::sockaddr::create(AF_INET6, "::ffff:127.0.0.1", 80);

		if (!addr1->is46(*addr2) ||
		    !addr2->is46(*addr1) ||
		    addr1->is46(*addr1))
			throw EXCEPTION("sockaddr::is46 does not work");

		if (*addr1 == *addr2)
			throw EXCEPTION("Two addresses shouldn't be same");

		if (*addr1 != *addr1 || *addr2 != *addr2)
			throw EXCEPTION("An address is not the same as itself?");

		if (*addr1 < *addr2 && *addr2 < *addr1)
			throw EXCEPTION("Something's wrong with operator< for LIBCXX_NAMESPACE::sockaddr");

		int n;

		std::list<LIBCXX_NAMESPACE::fd> bind_list;

		for (n=4000; ; n++)
		{
			try {
				LIBCXX_NAMESPACE::netaddr::create("", n)
					->bind(bind_list, true);
			} catch(const LIBCXX_NAMESPACE::exception &e)
			{
				if (n >= 5000)
					throw;

				continue;
			}
			break;
		}

		LIBCXX_NAMESPACE::fd::base::listen(bind_list);

	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}

}

void foo(LIBCXX_NAMESPACE::fd *x) {}

void test_run_lambda_return_value(LIBCXX_NAMESPACE::netaddr &addr)
{
	auto ret=LIBCXX_NAMESPACE::run_lambda
		([]
		 (const LIBCXX_NAMESPACE::netaddr &addr)
		 {
			 LIBCXX_NAMESPACE::fd sock=addr->connect();

			 return sock;
		 }, addr);

	ret->wait();

	auto val=ret->get();

	LIBCXX_NAMESPACE::fd *fdptr=&val;
	foo(fdptr);
}

class eventthread : virtual public LIBCXX_NAMESPACE::obj {
public:
	eventthread() noexcept;
	~eventthread() noexcept;
	void run(const LIBCXX_NAMESPACE::eventfd &eventfd);
};

eventthread::eventthread() noexcept
{
}

eventthread::~eventthread() noexcept
{
}

void eventthread::run(const LIBCXX_NAMESPACE::eventfd &eventfd)
{
	eventfd->event(1);
}

static void testeventfd() noexcept
{
	try {
		LIBCXX_NAMESPACE::eventfd eventfd=LIBCXX_NAMESPACE::eventfd::create();

		LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<eventthread>::create(),
				    eventfd);

		std::cout << "Received " << eventfd->event()
			  << " event." << std::endl;

		eventfd->event(1);
		eventfd->event(2);

		if (eventfd->event() != 3)
			throw EXCEPTION("eventfd failed (test 2)");

		eventfd->nonblock(true);

		struct pollfd pfd;

		pfd.fd=eventfd->getFd();
		pfd.events=POLLIN;

		if (poll(&pfd, 1, 0) != 0)
			throw EXCEPTION("eventfd failed (test 3)");

		eventfd->event(1);
		if (poll(&pfd, 1, 0) != 1 || !(pfd.revents & POLLIN))
			throw EXCEPTION("eventfd failed (test 4)");
		if (eventfd->event() != 1)
			throw EXCEPTION("eventfd failed (test 5)");

		if (poll(&pfd, 1, 0) != 0)
			throw EXCEPTION("eventfd failed (test 6)");
		
		bool caught=false;

		try {
			eventfd->event();
		} catch (const LIBCXX_NAMESPACE::sysexception &e)
		{
			int n=e.getErrorCode();

			if (n == EAGAIN || n == EWOULDBLOCK)
			{
				caught=true;
			}
		}

		if (!caught)
			throw EXCEPTION("eventfd failed (test 7)");

	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
}

static void teststreambufferobj() noexcept
{
	try {
		unlink("testfile.dat");
		unlink("testfile.dat.tmp");

		{
			LIBCXX_NAMESPACE::fd testfile(LIBCXX_NAMESPACE::fd::create("testfile.dat"));

			testfile->cancel();
		}

		if (access("testfile.dat", 0) == 0 ||
		    access("testfile.dat.tmp", 0) == 0)
			throw EXCEPTION("create(), test 0, failed");

		LIBCXX_NAMESPACE::fd testfile(LIBCXX_NAMESPACE::fd::create("testfile.dat"));

		if (access("testfile.dat", 0) == 0 ||
		    access("testfile.dat.tmp", 0) < 0)
			throw EXCEPTION("create(), test 1, failed");

		testfile->write("X", 1);
		testfile->close();

		if (access("testfile.dat", 0) < 0 ||
		    access("testfile.dat.tmp", 0) == 0)
			throw EXCEPTION("create(), test 1, failed");

		testfile=LIBCXX_NAMESPACE::fd::base
			::open("testfile.dat", O_CREAT|O_RDWR|O_TRUNC);

		{
			LIBCXX_NAMESPACE::streambuf strbuf(testfile->getiostream()->rdbuf());

			std::iostream ios(&*strbuf);

			ios << "Hello world!" << std::endl;

			ios.seekg(-13, ios.cur);

			std::string s;

			if (std::getline(ios, s).eof() ||
			    s != "Hello world!")
				throw EXCEPTION("fdstreambufObj test 1 failed");
			ios.seekg(0);
			ios.get();
			ios.get();
			ios.seekg(-2, ios.cur);
			if (std::getline(ios, s).eof() ||
			    s != "Hello world!")
				throw EXCEPTION("fdstreambufObj test 2 failed");
			ios.seekp(0, ios.beg);
			ios << "HELLO ";
			if (std::getline(ios, s).eof() ||
			    s != "world!")
				throw EXCEPTION("fdstreambufObj test 3 failed");
			ios.seekg(0, ios.beg);
			ios.get();
			ios.get();
			ios.get();
			ios.get();
			ios.get();
			ios.get();
			ios << "WORLD";
			ios.seekg(0, ios.beg);
			if (std::getline(ios, s).eof() ||
			    s != "HELLO WORLD!")
				throw EXCEPTION("fdstreambufObj test 4 failed");
			char buf[8];

			ios.rdbuf()->pubsetbuf(buf, 8);
			ios.seekg(0, ios.beg);
			if (std::getline(ios, s).eof() ||
			    s != "HELLO WORLD!")
				throw EXCEPTION("fdstreambufObj test 5 failed");

		}
		testfile->close();

		testfile=LIBCXX_NAMESPACE::fd::base::open("testfile.dat", O_WRONLY);

		{
			LIBCXX_NAMESPACE::streambuf strbuf(testfile->getiostream()->rdbuf());

			std::iostream ios(&*strbuf);

			std::string l;

			if (!std::getline(ios, l).fail())
				throw EXCEPTION("fdstreambufObj test 6 failed");
		}

		testfile->close();

		testfile=LIBCXX_NAMESPACE::fd::base::open("testfile.dat", O_RDONLY);

		{
			LIBCXX_NAMESPACE::streambuf strbuf(testfile->getiostream()->rdbuf());

			std::iostream ios(&*strbuf);

			ios << "Foo" << std::flush;

			if (!ios.fail())
				throw EXCEPTION("fdstreambufObj test 7 failed");
		}
		testfile->close();

		auto v=LIBCXX_NAMESPACE::vector<char>::base::load
			(LIBCXX_NAMESPACE::fd::base::open("testfile.dat",
							  O_RDONLY));

		if (v->size() == 0)
			throw EXCEPTION("fdstreambufObj test 8 failed");

		unlink("testfile.dat");
	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
}

static void testlockunlink() noexcept
{
	unlink("testlock");

	{
		const LIBCXX_NAMESPACE::fdptr
			fd=LIBCXX_NAMESPACE::fd::base::lockf("testlock");

		if (access("testlock", O_RDONLY) < 0 || fd.null())
		{
			std::cerr << "testunlink failed" << std::endl;
			exit(1);
		}
	}

	if (access("testlock", O_RDONLY) == 0)
	{
		std::cerr << "testunlink failed" << std::endl;
		exit(1);
	}

	{
		const LIBCXX_NAMESPACE::fdptr
			fd=LIBCXX_NAMESPACE::fd::base::lockf("testlock");

		if (access("testlock", O_RDONLY) < 0 || fd.null())
		{
			std::cerr << "testunlink failed" << std::endl;
			exit(1);
		}

		pid_t p=fork();

		if (p == 0)
		{
			LIBCXX_NAMESPACE::fdptr fd2=
				LIBCXX_NAMESPACE::fd::base::lockf("testlock", F_TLOCK);

			_exit(access("testlock", O_RDONLY) < 0
			      || !fd2.null());
		}
		int waitstat;

		wait(&waitstat);

		if (waitstat)
		{
			std::cerr << "testunlink failed (child proc): "
				  << waitstat
				  << std::endl;
			exit(1);
		}

		if (access("testlock", O_RDONLY) < 0)
		{
			std::cerr << "testunlink failed" << std::endl;
			exit(1);
		}
	}
	if (access("testlock", O_RDONLY) == 0)
	{
		std::cerr << "testunlink failed" << std::endl;
		exit(1);
	}
}

static void testtimerfd() noexcept
{
	try {
		LIBCXX_NAMESPACE::timerfd tfd(LIBCXX_NAMESPACE::timerfd::create(CLOCK_MONOTONIC));

		LIBCXX_NAMESPACE::timerfd tfdabs(LIBCXX_NAMESPACE::timerfd::create(CLOCK_REALTIME));

		std::cout << "Testing timer: " << std::flush;

		tfd->nonblock(true);

		if (tfd->wait() != 0)
			throw EXCEPTION("Something wrong with NONBLOCK timerfd");

		tfd->nonblock(false);

		tfd->set(TFD_TIMER_ABSTIME,
			 LIBCXX_NAMESPACE::timespec::getclock(CLOCK_MONOTONIC)
			 + LIBCXX_NAMESPACE::timespec(1));

		tfdabs->set(TFD_TIMER_ABSTIME,
			    LIBCXX_NAMESPACE::timespec::getclock(CLOCK_REALTIME)
			    + LIBCXX_NAMESPACE::timespec(1));

		if (tfdabs->wait() != 1)
			throw EXCEPTION("abstime failed");

		tfd->set(0, 1);

		std::cout << tfd->wait() << std::endl;

		std::cout << "Testing periodic timer: " << std::flush;

		tfd->set_periodically(LIBCXX_NAMESPACE::timespec(0, 1, 2));

		std::cout << (tfd->wait() ? 1:0 ) << std::flush << " ";
		std::cout << (tfd->wait() ? 1:0 ) << std::flush << " ";
		std::cout << (tfd->wait() ? 1:0 ) << std::flush << " ";
		std::cout << (tfd->wait() ? 1:0 ) << std::flush << " ";
		std::cout << (tfd->wait() ? 1:0 ) << std::endl;
		tfd->cancel();

		{
			LIBCXX_NAMESPACE::timespec a(100, 400000000, 1000000000);
			LIBCXX_NAMESPACE::timespec b(100, 600000000, 1000000000);

			a += b;

			std::cout << a.tv_sec << "." << a.tv_nsec << std::endl;
		}

		{
			LIBCXX_NAMESPACE::timespec a(100, 400000000, 1000000000);
			LIBCXX_NAMESPACE::timespec b(50, 600000000, 1000000000);

			a -= b;

			std::cout << a.tv_sec << "." << a.tv_nsec << std::endl;
		}

	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
}

class testdestroy : public LIBCXX_NAMESPACE::destroyCallbackObj {

public:
	testdestroy() noexcept;
	~testdestroy() noexcept;

	void destroyed() noexcept;
};

testdestroy::testdestroy() noexcept
{
}

testdestroy::~testdestroy() noexcept
{
}

void testdestroy::destroyed() noexcept
{
	sleep(2);
	std::cout << "destroyed() called" << std::endl;
}

void testdestroyfunc1()
{
	try {
		LIBCXX_NAMESPACE::ptr<testdestroy> td(LIBCXX_NAMESPACE::ptr<testdestroy>::create());

		std::cout << "testdestroy object created" << std::endl;

		LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> obj(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>::create());

		std::cout << "dummy object created" << std::endl;

		obj->addOnDestroy(td);

		std::cout << "ondestroy callback installed" << std::endl;

		td=LIBCXX_NAMESPACE::ptr<testdestroy>();

		std::cout << "ondestroy callback pointer cleared" << std::endl;

		obj=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

		std::cout << "dummy object cleared" << std::endl;
	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Exception: testdestroy: " << e << std::endl;
	}
}


void testdestroyfunc2()
{
	try {
		LIBCXX_NAMESPACE::ptr<testdestroy> td(LIBCXX_NAMESPACE::ptr<testdestroy>::create());

		std::cout << "testdestroy object created" << std::endl;

		LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> obj(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>::create());

		std::cout << "dummy object created" << std::endl;

		LIBCXX_NAMESPACE::ondestroyptr od(LIBCXX_NAMESPACE::ondestroy::create(td, obj, false));

		std::cout << "ondestroy created" << std::endl;

		td=LIBCXX_NAMESPACE::ptr<testdestroy>();

		std::cout << "testdestroy reference dropped" << std::endl;

		od->cancel();

		std::cout << "ondestroy cancelled" << std::endl;

		od->cancel();

		std::cout << "ondestroy cancelled again" << std::endl;

		od=LIBCXX_NAMESPACE::ondestroyptr();

		std::cout << "ondestroy reference released" << std::endl;

		obj=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

		std::cout << "Dummy object released" << std::endl;
	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Exception: testdestroy: " << e << std::endl;
	}
}

void testdestroyfunc3()
{
	try {
		LIBCXX_NAMESPACE::ptr<testdestroy> td(LIBCXX_NAMESPACE::ptr<testdestroy>::create());

		std::cout << "testdestroy object created" << std::endl;

		LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> obj(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>::create());

		std::cout << "dummy object created" << std::endl;

		LIBCXX_NAMESPACE::ondestroyptr od(LIBCXX_NAMESPACE::ondestroy::create(td, obj, false));

		std::cout << "ondestroy created" << std::endl;

		td=LIBCXX_NAMESPACE::ptr<testdestroy>();

		std::cout << "testdestroy reference dropped" << std::endl;

		obj=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

		std::cout << "Dummy object released" << std::endl;

		od->cancel();

		std::cout << "ondestroy cancelled" << std::endl;

		od->cancel();

		std::cout << "ondestroy cancelled again" << std::endl;

		od=LIBCXX_NAMESPACE::ondestroyptr();

		std::cout << "ondestroy reference released" << std::endl;

	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Exception: testdestroy: " << e << std::endl;
	}
}

void testdestroyfunc4()
{
	try {
		LIBCXX_NAMESPACE::ptr<testdestroy> td(LIBCXX_NAMESPACE::ptr<testdestroy>::create());

		std::cout << "testdestroy object created" << std::endl;

		LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> obj(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>::create());

		std::cout << "dummy object created" << std::endl;

		LIBCXX_NAMESPACE::ondestroyptr od(LIBCXX_NAMESPACE::ondestroy::create(td, obj, false));

		std::cout << "ondestroy created" << std::endl;

		od=LIBCXX_NAMESPACE::ondestroyptr();

		std::cout << "ondestroy reference released" << std::endl;

		td=LIBCXX_NAMESPACE::ptr<testdestroy>();

		std::cout << "testdestroy reference dropped" << std::endl;

		obj=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

		std::cout << "Dummy object released" << std::endl;

	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Exception: testdestroy: " << e << std::endl;
	}
}

void testdestroyfunc5()
{
	try {
		LIBCXX_NAMESPACE::ptr<testdestroy> td(LIBCXX_NAMESPACE::ptr<testdestroy>::create());

		std::cout << "testdestroy object created" << std::endl;

		LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> obj(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>::create());

		std::cout << "dummy object created" << std::endl;

		LIBCXX_NAMESPACE::ondestroyptr od(LIBCXX_NAMESPACE::ondestroy::create(td, obj, true));

		std::cout << "ondestroy created" << std::endl;

		od=LIBCXX_NAMESPACE::ondestroyptr();

		std::cout << "ondestroy reference released" << std::endl;

		td=LIBCXX_NAMESPACE::ptr<testdestroy>();

		std::cout << "testdestroy reference dropped" << std::endl;

		obj=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

		std::cout << "Dummy object released" << std::endl;

	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Exception: testdestroy: " << e << std::endl;
	}
}

class df6logObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	df6logObj() {}
	~df6logObj() noexcept {}

	std::string log;
};

typedef LIBCXX_NAMESPACE::ref<df6logObj> df6log;

class df6obj1Obj : virtual public LIBCXX_NAMESPACE::destroyCallbackObj {

public:
	df6log l;
	std::string id;

	df6obj1Obj(const df6log &lArg, const std::string &idArg) : l(lArg),
								   id(idArg)
	{
	}

	~df6obj1Obj() noexcept
	{
		l->log += id;
	}

	void destroyed() noexcept
	{
		l->log += "[" + id + "]";
	}
};

typedef LIBCXX_NAMESPACE::ptr<df6obj1Obj> df6obj1;

class df6iter {
public:
	int n;

	mutable df6obj1 *p;

	df6iter(int nArg, df6obj1 *pArg) : n(nArg), p(pArg) {}


	df6obj1 &operator*() { return *p; }

	df6obj1 *operator->() { return p; }

	df6iter &operator++() { ++n; return *this; }

	df6iter operator++(int) { df6iter cp=*this; operator++(); return cp; }

	bool operator==(const df6iter &o) const
	{
		if (n == 1 && o.n == 1)
		{
			if (p) *p=df6obj1();
			if (o.p) *o.p=df6obj1();
		}
		return n == o.n;
	}

	bool operator!=(const df6iter &o) const
	{
		return !operator==(o);
	}
};

void testdestroyfunc6()
{
	df6log l=df6log::create();

	df6obj1 a=df6obj1::create(l, "a"),
		b=df6obj1::create(l, "b"),
		c=df6obj1::create(l, "c");

	df6obj1 v[2]={a, c};

	b->onAnyDestroyed(v, v+2);

	v[0]=df6obj1();
	v[1]=df6obj1();
	b=df6obj1();
	a=df6obj1();
	c=df6obj1();

	if (l->log != "a[b]bc")
		throw EXCEPTION("testdestroyfunc6, test1, failed");

	l=df6log::create();

	a=df6obj1::create(l, "a");
	b=df6obj1::create(l, "b");

	{
		df6iter bp(0, &a), ep(1, 0);

		b->onAnyDestroyed(bp, ep);
	}

	if (l->log != "a[b]")
		throw EXCEPTION("testdestroyfunc6, test2, failed");
}

class threadtestsendfd : virtual public LIBCXX_NAMESPACE::obj {

	size_t nfds;

public:
	threadtestsendfd(size_t nfdsArg) : nfds(nfdsArg)
	{
	}

	void run(const LIBCXX_NAMESPACE::fd &sockFd);
};

void threadtestsendfd::run(const LIBCXX_NAMESPACE::fd &sockFd)
{
	LIBCXX_NAMESPACE::fdptr devnull[nfds];

	size_t i;

	for (i=0; i<nfds; i++)
	{
		devnull[i]=LIBCXX_NAMESPACE::fd::base::open("/dev/null", O_WRONLY);
	}

	sockFd->send_fd(devnull, i);
}


int testsendfd(size_t cnt)
{
	try {
#ifdef SOCK_CLOEXEC
		{
			int pipefd[2];

			if (::socketpair(AF_UNIX,
					 SOCK_CLOEXEC |
					 SOCK_STREAM,
					 0, pipefd) < 0)
				throw SYSEXCEPTION("socketpair");

			int flags=fcntl(pipefd[0], F_GETFD, 0);

			if (flags < 0)
				throw SYSEXCEPTION("F_GETFD");

			if (!(flags & FD_CLOEXEC))
				throw EXCEPTION("FD_CLOEXEC not set (socketpair)");

			close(pipefd[0]);
			close(pipefd[1]);
		}
#endif

#if HAVE_PIPE2
		{
			int pipefd[2];

			pipefd[0]=open("/dev/null", O_RDONLY | O_CLOEXEC);

			if (pipefd[0] < 0)
				throw SYSEXCEPTION("/dev/null");

			int flags=fcntl(pipefd[0], F_GETFD, 0);

			if (flags < 0)
				throw SYSEXCEPTION("F_GETFD");

			if (!(flags & FD_CLOEXEC))
				throw EXCEPTION("FD_CLOEXEC not set (open)");
			close(pipefd[0]);

			if (::pipe2(pipefd, O_CLOEXEC) < 0)
				throw SYSEXCEPTION("pipe2");

			close(pipefd[0]);
			close(pipefd[1]);
		}
#endif

		LIBCXX_NAMESPACE::fdptr s1, s2;

		{
			std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
				p(LIBCXX_NAMESPACE::fd::base::socketpair());

			s1=p.first;
			s2=p.second;
		}

		if (!s1->closeonexec() || !s2->closeonexec())
			throw EXCEPTION("socketpair() did not produce close-on-exec file descriptors");

		auto threadObj=
			LIBCXX_NAMESPACE::ref<threadtestsendfd>::create(cnt);

		auto ret=LIBCXX_NAMESPACE::run(threadObj, s1);

		s1=LIBCXX_NAMESPACE::fdptr();

		ret->wait();

		std::vector<LIBCXX_NAMESPACE::fdptr> fdvec;

		fdvec.resize(cnt);

		s2->recv_fd(fdvec);

		std::cout << "Received " << fdvec.size()
			  << " file descriptors" << std::endl;

		for (size_t i=0; i<fdvec.size(); ++i)
		{
			fdvec[i]->write("foo", 3);
		}

		ret->get();

		std::cout << "File descriptor thread returned 0"
			  << std::endl;

	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Exception: testsendfd: " << e << std::endl;
		return -1;
	}
	return 0;
}

void testeventqueue()
{
	try {
		typedef LIBCXX_NAMESPACE::eventqueueptr<int> queue_t;

		queue_t q=queue_t::create(LIBCXX_NAMESPACE::eventfd::create());

		q->event(1);
		(void)q->empty();
		q->pop();

	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Exception: testeventqueue: " << e << std::endl;
	}
}

class testweakcontainerObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	testweakcontainerObj() noexcept {}
	~testweakcontainerObj() noexcept {}

};

typedef LIBCXX_NAMESPACE::weaklist< testweakcontainerObj > testweaklist;
typedef LIBCXX_NAMESPACE::weaklistptr< testweakcontainerObj > testweaklistptr;

void dotestweaklist1()
{
	testweaklist twl(testweaklist::create());

	std::cout << "list: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	LIBCXX_NAMESPACE::ptr<testweakcontainerObj>
		testobj1(LIBCXX_NAMESPACE::ptr<testweakcontainerObj>::create());

	twl->push_back(testobj1);

	std::cout << "list: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	testweaklist::base::iterator b=twl->begin(), e=twl->end();

	while (b != e)
	{
		std::cout << "list: null " << b->getptr().null() << std::endl;
		++b;
	}

	testobj1=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>();

	std::cout << "list: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	testobj1=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>::create();

	twl->push_front(testobj1);

	std::cout << "list: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	b=twl->begin(), e=twl->end();

	while (b != e)
	{
		std::cout << "list: null " << b->getptr().null() << std::endl;
		++b;
	}

	testobj1=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>();

	std::cout << "list: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;
}

void dotestweaklist2()
{
	LIBCXX_NAMESPACE::ptr<testweakcontainerObj> testobj1[2];

	testobj1[0]=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>::create();
	testobj1[1]=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>::create();

	testweaklistptr twl;

	testweaklistptr twl2=testweaklistptr::create();

	twl2->push_back(testobj1[0]);
	twl2->push_back(testobj1[1]);

	twl=twl2;

	testweaklistptr twltmp;

	twl2=twltmp;

	std::cout << "list: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	testweaklist::base::iterator b=twl->begin(), e=twl->end();

	while (b != e)
	{
		std::cout << "list: null " << b->getptr().null() << std::endl;
		++b;
	}

	testobj1[0]=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>();

	std::cout << "list: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	b=twl->begin(), e=twl->end();

	while (b != e)
	{
		std::cout << "list: null " << b->getptr().null() << std::endl;
		++b;
	}

	testobj1[1]=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>();

	std::cout << "list: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	testobj1[0]=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>::create();
	testobj1[1]=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>::create();

	twl=testweaklistptr::create();
	twl->push_back(testobj1[0]);
	twl->push_back(testobj1[1]);

	std::cout << "list: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	b=twl->begin(), e=twl->end();

	while (b != e)
	{
		std::cout << "list: null " << b->getptr().null() << std::endl;
		++b;
	}

	testobj1[0]=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>();

	std::cout << "list: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	b=twl->begin(), e=twl->end();

	while (b != e)
	{
		std::cout << "list: null " << b->getptr().null() << std::endl;
		++b;
	}

	testobj1[1]=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>();

	std::cout << "list: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;
}

void dotestweaklist3()
{
	LIBCXX_NAMESPACE::ptr<testweakcontainerObj>
		testobj1(LIBCXX_NAMESPACE::ptr<testweakcontainerObj>::create());

	testweaklistptr twl(testweaklist::create());

	twl->push_back(testobj1);

	testweaklistptr twltmp;

	twl=twltmp;

	testobj1=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>();
}

typedef LIBCXX_NAMESPACE::weakmap< std::string, testweakcontainerObj> testweakmap;

void dotestweakmap1()
{
	testweakmap twl(testweakmap::create());

	std::cout << "map: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	LIBCXX_NAMESPACE::ptr<testweakcontainerObj> testobj1(LIBCXX_NAMESPACE::ptr<testweakcontainerObj>
					      ::create());
	(*twl)["1"]=testobj1;

	testweakmap::base::iterator it1(twl->find("1"));

	if (it1 == twl->end() || twl->count("1") != 1 ||
	    it1 != twl->lower_bound("1") || ++it1 != twl->upper_bound("1"))
		abort();

	LIBCXX_NAMESPACE::ptr<testweakcontainerObj> testobj2(LIBCXX_NAMESPACE::ptr<testweakcontainerObj>
					      ::create());

	(*twl)["1"]=testobj2;

	testobj2=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>();

	std::cout << "map: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	testweakmap::base::iterator b=twl->begin(), e=twl->end();

	while (b != e)
	{
		std::cout << "map: null " << b->second.getptr().null()
			  << std::endl;
		++b;
	}

	testobj1=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>();
	std::cout << "map: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;
}

void dotestweakmap2()
{
	LIBCXX_NAMESPACE::ptr<testweakcontainerObj> testobj1(LIBCXX_NAMESPACE::ptr<testweakcontainerObj>
					      ::create());
	LIBCXX_NAMESPACE::ptr<testweakcontainerObj> testobj2(LIBCXX_NAMESPACE::ptr<testweakcontainerObj>
					      ::create());

	testweakmap twl(testweakmap::create());

	{
		std::pair<std::string, LIBCXX_NAMESPACE::ptr<testweakcontainerObj> >
			init_array[]
			={ std::make_pair(std::string("1"), testobj1),
			   std::make_pair(std::string("2"), testobj2)
		};

		testweakmap twl2=testweakmap::create();

		twl2->insert(init_array[0]);
		twl2->insert(init_array[1]);
		twl=twl2;
	};

	std::cout << "map: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	testweakmap::base::iterator b=twl->begin(), e=twl->end();

	while (b != e)
	{
		std::cout << "list: null " << b->second.getptr().null()
			  << std::endl;
		++b;
	}

	testobj1=testobj2=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>();

	std::cout << "map: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	b=twl->begin();
	e=twl->end();

	while (b != e)
	{
		std::cout << "list: null " << b->second.getptr().null()
			  << std::endl;
		++b;
	}
}

void dotestweakmap3()
{
	testweakmap twl(testweakmap::create());

	(*twl)["1"]=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>::create();

	std::cout << "map: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;
}

typedef LIBCXX_NAMESPACE::weakmultimap< std::string, 
				      testweakcontainerObj > testweakmultimap;

void dotestweakmap4()
{
	LIBCXX_NAMESPACE::ptr<testweakcontainerObj> testobj1(LIBCXX_NAMESPACE::ptr<testweakcontainerObj>
					      ::create());
	LIBCXX_NAMESPACE::ptr<testweakcontainerObj> testobj2(LIBCXX_NAMESPACE::ptr<testweakcontainerObj>
					      ::create());

	testweakmultimap twl(testweakmultimap::create());

	{
		std::pair<std::string, LIBCXX_NAMESPACE::ptr<testweakcontainerObj> >
			init_array[]
			={ std::make_pair(std::string("1"), testobj1),
			   std::make_pair(std::string("1"), testobj2)
		};

		testweakmultimap twl2=testweakmultimap::create();

		twl2->insert(init_array[0]);
		twl2->insert(init_array[1]);
		twl=twl2;
	};

	std::cout << "map: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	std::pair<testweakmultimap::base::iterator,
		  testweakmultimap::base::iterator> itp;

	if (twl->find("2") != twl->end() ||
	    twl->find("1") == twl->end())
		abort();

	itp=twl->equal_range("1");

	if (twl->lower_bound("1") != itp.first ||
	    twl->upper_bound("1") != itp.second ||
	    ++++itp.first != itp.second)
		abort();

	testweakmultimap::base::iterator b=twl->begin(), e=twl->end();

	while (b != e)
	{
		std::cout << "list: null " << b->second.getptr().null()
			  << std::endl;
		++b;
	}

	testobj1=testobj2=LIBCXX_NAMESPACE::ptr<testweakcontainerObj>();

	std::cout << "map: empty: " << twl->empty() << ", size: " << twl->size()
		  << std::endl;

	b=twl->begin();
	e=twl->end();

	while (b != e)
	{
		std::cout << "list: null " << b->second.getptr().null()
			  << std::endl;
		++b;
	}
}

void testvector()
{
	{
		LIBCXX_NAMESPACE::vector<int>
			v(LIBCXX_NAMESPACE::vector<int>::create());
	}

	{
		LIBCXX_NAMESPACE::vector<int>
			v(LIBCXX_NAMESPACE::vector<int>::create(4, 2));
	}

	{
		int buf[2];
		LIBCXX_NAMESPACE::vector<int>
			v(LIBCXX_NAMESPACE::vector<int>::create(&buf[0],
							      &buf[2]));
	}
}

void testeventdestroynotify()
{
	try {
		LIBCXX_NAMESPACE::eventfdptr efd(LIBCXX_NAMESPACE::eventfd::create());

		LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> r(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>::create());

		LIBCXX_NAMESPACE::eventdestroynotify
			edn(LIBCXX_NAMESPACE::eventdestroynotify::create(efd, r));

		if (edn->wasdestroyed())
			throw EXCEPTION("False positive");

		r=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

		if (!edn->wasdestroyed())
			throw EXCEPTION("Didn't work");
		efd->event();

		r=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>::create();
		edn=LIBCXX_NAMESPACE::eventdestroynotify::create(efd, r);

		efd=LIBCXX_NAMESPACE::eventfdptr();
		r=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

		if (!edn->wasdestroyed())
			throw EXCEPTION("Didn't work (again)");

	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testeventdestroynotify: "
			  << e << std::endl;
	}
}

void testeventqueuedestroynotify()
{
	try {
		LIBCXX_NAMESPACE::eventfd efd(LIBCXX_NAMESPACE::eventfd::create());

		typedef LIBCXX_NAMESPACE::eventqueueptr<int> q_type;

		q_type q(q_type::create(efd));

		LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> r(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>::create());

		LIBCXX_NAMESPACE::eventqueuedestroynotify<int>
			edn(LIBCXX_NAMESPACE::eventqueuedestroynotify<int>
			    ::create(q, 1, r));

		if (edn->wasdestroyed())
			throw EXCEPTION("False positive");

		r=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

		if (!edn->wasdestroyed())
			throw EXCEPTION("Didn't work");

		while (q->empty())
		{
			efd->event();
		}

		if (q->pop() != 1)
			throw EXCEPTION("pop() didn't work");

		r=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>::create();
		edn=LIBCXX_NAMESPACE::eventqueuedestroynotify<int>
			::create(q, 1, r);

		q=q_type();
		r=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

		if (!edn->wasdestroyed())
			throw EXCEPTION("Didn't work (again)");

	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testeventqueuedestroynotify: "
			  << e << std::endl;
	}
}

static void testtimer()
{
	try {
		{
			LIBCXX_NAMESPACE::fd
				null(LIBCXX_NAMESPACE::fd::base::open("/dev/null",
								    O_RDONLY));

			LIBCXX_NAMESPACE::istream
				i(null->getistream());

			std::string line;

			std::getline(*i, line);

			if (i->fail() && i->eof())
				;
			else
				throw EXCEPTION("istream status not eof");

		}

		{
			LIBCXX_NAMESPACE::fdptr r, w;

			{
				std::pair<LIBCXX_NAMESPACE::fd,
					  LIBCXX_NAMESPACE::fd> p=
					LIBCXX_NAMESPACE::fd::base::pipe();
				r=p.first;
				w=p.second;
			}

			LIBCXX_NAMESPACE::fdtimeout
				to(LIBCXX_NAMESPACE::fdtimeout::create(r));

			to->set_read_timeout(LIBCXX_NAMESPACE::timespec(0, 1, 2));

			LIBCXX_NAMESPACE::istream
				i(to->getistream());

			bool flag=false;

			try {
				std::string line;

				std::getline(*i, line);
			} catch (const LIBCXX_NAMESPACE::sysexception &e)
			{
				if (e.getErrorCode() == ETIMEDOUT)
					flag=true;
			}

			if (!flag)
				throw EXCEPTION("istream status not timeout");

		}

		{
			LIBCXX_NAMESPACE::fd tmp=LIBCXX_NAMESPACE::fd::base::tmpfile();

			LIBCXX_NAMESPACE::fdreadlimit
				rl(LIBCXX_NAMESPACE::fdreadlimit::create(tmp));

			LIBCXX_NAMESPACE::iostream i(rl->getiostream());

			(*i) << "Abracadabra" << std::endl;

			i->seekg(0);

			rl->set_read_limit(5, "Test timeout");

			bool flag=false;

			try {
				std::string line;

				std::getline(*i, line);
			} catch (const LIBCXX_NAMESPACE::sysexception &e)
			{
				if (e.getErrorCode() == EOVERFLOW)
					flag=true;
			}

			if (!flag)
				throw EXCEPTION("read_limit did not work");
		}
	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testtimer: "
			  << e << std::endl;
	}
}

static void bindtest() noexcept
{
	try {
		{
			std::list<LIBCXX_NAMESPACE::fd> socket_list;

			LIBCXX_NAMESPACE::netaddr::create("", 0)
				->bind(socket_list, true);

			LIBCXX_NAMESPACE::fd::base::listen(socket_list);
		}

		{
			std::list<LIBCXX_NAMESPACE::fd> socket_list;

			LIBCXX_NAMESPACE::netaddr::create("localhost", 0)
				->bind(socket_list, true);

			LIBCXX_NAMESPACE::fd::base::listen(socket_list);
		}

	} catch(const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "bindtest: " << e << std::endl;
	}
}

static void testfditer()
{
	LIBCXX_NAMESPACE::fd tmp(LIBCXX_NAMESPACE::fd::base::tmpfile());

	std::string helloworld("Hello world!");

	{
		LIBCXX_NAMESPACE::fd::base::outputiter iter(tmp, 512);

		iter.resize(1024);

		iter=std::copy(helloworld.begin(), helloworld.end(), iter);

		iter.flush();
	}

	std::string helloworld2;

	tmp->seek(0, SEEK_SET);

	std::copy(LIBCXX_NAMESPACE::fd::base::inputiter(tmp, 512),
		  LIBCXX_NAMESPACE::fd::base::inputiter(),
		  std::back_insert_iterator<std::string>(helloworld2));
	if (helloworld != helloworld2)
	{
		std::cerr << "fd::base::inputiter and fd::base::outputiter do not work"
			  << std::endl;
		exit(1);
	}
}

int main()
{
	alarm(60);
	objtest();
	exceptiontest();
	exceptiontest2();
	threadtest();
	fdtest();
	fdtest2();
	dirtest();
	testmutex();
	rwlocktest();
	testepoll();
	alarm(0);
	testnet();
	bindtest();
	alarm(30);
	testeventfd();
	alarm(0);
	teststreambufferobj();
	testlockunlink();
	testtimerfd();
	testdestroyfunc1();
	testdestroyfunc2();
	testdestroyfunc3();
	testdestroyfunc4();
	testdestroyfunc5();
	testdestroyfunc6();
	if (testsendfd(128) < 0) // Max appears to be 253 as of 2.6.38
		exit(1);
	alarm(30);
	testeventqueue();
	testeventdestroynotify();
	testeventqueuedestroynotify();
	alarm(0);
	dotestweaklist1();
	dotestweaklist2();
	dotestweaklist3();
	dotestweakmap1();
	dotestweakmap2();
	dotestweakmap3();
	dotestweakmap4();
	alarm(10);
	testtimer();
	alarm(10);

	try {
		unlink("foo");

		std::list<LIBCXX_NAMESPACE::fd> sock;

		LIBCXX_NAMESPACE::netaddr::create(SOCK_STREAM, "file:foo")
			->bind(sock, false);
		sock.clear();

		LIBCXX_NAMESPACE::netaddr::create(SOCK_STREAM, "file:foo")
			->bind(sock, false);
	} catch (const x::sysexception &e)
	{
		if (e.getErrorCode() != EADDRINUSE)
		{
			std::cerr << "Existing filesystem socket error is not EADDRINUSE"
				  << std::endl;
			exit(1);
		}
	}
	unlink("foo");

	alarm(0);
	testfditer();
	return 0;
}
