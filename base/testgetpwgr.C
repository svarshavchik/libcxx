/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/grp.H"
#include "x/pwd.H"
#include "x/servent.H"
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::passwd pw(getuid());
	LIBCXX_NAMESPACE::group gr(getgid());

	std::cout << (pw->pw_name ? pw->pw_name:"unknown") << ":"
		  << (gr->gr_name ? gr->gr_name:"unknown") << std::endl;

	LIBCXX_NAMESPACE::servent http("http", "tcp");

	if (http->s_name)
		std::cout << "http is port " << ntohs(http->s_port) << std::endl;
	return 0;
}
