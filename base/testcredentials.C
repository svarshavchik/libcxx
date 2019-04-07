/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fd.H"
#include <vector>
#include <unistd.h>

static void testsendcredentials()
{
	LIBCXX_NAMESPACE::fdptr a,b;

	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p=LIBCXX_NAMESPACE::fd::base::socketpair();

		a=p.first;
		b=p.second;
	}

	a->nonblock(true);
	b->nonblock(true);

	if (!a->send_credentials())
		throw EXCEPTION("testsendcredentials: send failed");

	b->recv_credentials(true);

	LIBCXX_NAMESPACE::fd::base::credentials_t c;

	if (!b->recv_credentials(c))
		throw EXCEPTION("testsendcredentials: recv failed");
}

int main(int argc, char **argv)
{
	try {
		alarm(30);
		testsendcredentials();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
	}
	return 0;
}
