/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxx_config.h"
#include "testftp.H"
#include "x/fd.H"

bool plain()
{
	return true;
}

LIBCXX_NAMESPACE::fdbase new_server_socket(const LIBCXX_NAMESPACE::fdbase
					   &socket)
{
	return socket;
}

void shutdown_server_socket(const LIBCXX_NAMESPACE::fdbase &socket)
{
}

LIBCXX_NAMESPACE::ftp::client new_client(const LIBCXX_NAMESPACE::fdbase
					 &connArg, bool passive)
{
	return LIBCXX_NAMESPACE::ftp::client::create(connArg, passive);
}

LIBCXX_NAMESPACE::ftp::client new_client(const LIBCXX_NAMESPACE::fdbase
					 &connArg,
					 const LIBCXX_NAMESPACE::fdtimeout
					 &config,
					 bool passive)
{
	return LIBCXX_NAMESPACE::ftp::client::create(connArg, config, passive);
}
