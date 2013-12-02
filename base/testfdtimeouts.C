/*
** Copyright 2012-2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fd.H"
#include "x/ref.H"
#include "x/fdtimeout.H"
#include "x/sysexception.H"
#include <unistd.h>
#include <cstdlib>

static void testtimers()
{
	LIBCXX_NAMESPACE::fdptr r, w;

	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p=LIBCXX_NAMESPACE::fd::base::pipe();

		r=p.first;
		w=p.second;
	}

	w->nonblock(true);
	r->nonblock(true);

	{
		LIBCXX_NAMESPACE::fdtimeout
			to(LIBCXX_NAMESPACE::fdtimeout::create(w));

		LIBCXX_NAMESPACE::ostream o(to->getostream());

		to->set_write_timeout(10, 2);

		try {
			while(1)
				*o << '\0';
		} catch (const LIBCXX_NAMESPACE::sysexception &e)
		{
			if (e.getErrorCode() != ETIMEDOUT)
				throw;
		}
	}

	{
		LIBCXX_NAMESPACE::fdtimeout
			to(LIBCXX_NAMESPACE::fdtimeout::create(r));

		LIBCXX_NAMESPACE::istream i(to->getistream());
		to->set_read_timeout(10, 2);

		try {
			while(1)
				i->get();
		} catch (const LIBCXX_NAMESPACE::sysexception &e)
		{
			if (e.getErrorCode() != ETIMEDOUT)
				throw;
		}
	}
}

static void testwrite1()
{
	LIBCXX_NAMESPACE::fd a(LIBCXX_NAMESPACE::fd::base::tmpfile()),
		b(LIBCXX_NAMESPACE::fd::base::tmpfile());

	(*a->getostream()) << "Hello" << std::endl << std::flush;

	b->write(a);

	std::string line;

	b->seek(0, SEEK_SET);

	std::getline(*b->getistream(), line);

	if (line != "Hello")
		throw EXCEPTION("file descriptor write failed");
}

static void testwrite2()
{
	LIBCXX_NAMESPACE::fd b(LIBCXX_NAMESPACE::fd::base::tmpfile());

	std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
		pipe(LIBCXX_NAMESPACE::fd::base::pipe());

	(*pipe.second->getostream()) << "Hello" << std::endl << std::flush;
	pipe.second->close();

	b->write(pipe.first);

	std::string line;

	b->seek(0, SEEK_SET);

	std::getline(*b->getistream(), line);

	if (line != "Hello")
		throw EXCEPTION("file descriptor write failed");
}

int main()
{
	alarm(60);
	try {
		testwrite1();
		testwrite2();
		testtimers();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
