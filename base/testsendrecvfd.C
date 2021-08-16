/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fd.H"
#include <vector>
#include <unistd.h>

static void testsendrecvfd1()
{
	LIBCXX_NAMESPACE::fdptr a,b;

	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p=LIBCXX_NAMESPACE::fd::base::socketpair();

		a=p.first;
		b=p.second;
	}

	size_t cnt=0;

	std::vector<LIBCXX_NAMESPACE::fdptr> fdarray;

	a->nonblock(true);
	b->nonblock(true);

	fdarray.resize(10);

	a->recv_fd(fdarray);

	if (fdarray.size() != 0)
		throw EXCEPTION("testsendrecvfd1: recv_fd() did not return 0");

	while (b->write("a", 1) > 0)
		++cnt;

	fdarray.resize(1);
	fdarray[0]=LIBCXX_NAMESPACE::fd::base::tmpfile();

	fdarray[0]->write("A", 1);

	{
		std::vector<LIBCXX_NAMESPACE::fd> fdrefarray(fdarray.begin(),
							   fdarray.end());

		if (b->send_fd(fdrefarray))
			throw EXCEPTION("testsendrecvfd1: send_fd() did not return false");
	}

	char buffer[cnt];

	if (a->read(buffer, cnt) != cnt)
		throw EXCEPTION("testsendrecvfd1: drain failed");

	fdarray[0]=LIBCXX_NAMESPACE::fd::base::tmpfile();
	fdarray[0]->write("B", 1);

	{
		std::vector<LIBCXX_NAMESPACE::fd> fdrefarray(fdarray.begin(),
							   fdarray.end());

		if (!b->send_fd(fdrefarray))
			throw EXCEPTION("testsendrecvfd1: send_fd() failed");
	}

	fdarray.clear();
	fdarray.resize(1);

	a->recv_fd(fdarray);

	if (fdarray.size() != 1)
		throw EXCEPTION("testsendrecvfd1: did not receive 1 fd");

	fdarray[0]->seek(0, 0);

	if (fdarray[0]->read(buffer, 1) != 1)
		throw EXCEPTION("testsendrecvfd1: could not read 1 byte");

	if (buffer[0] != 'B')
		throw EXCEPTION("testsendrecvfd1: received a temporary file that contained " + std::string(buffer, buffer+1));
}

int main(int argc, char **argv)
{
	try {
		alarm(30);
		testsendrecvfd1();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
