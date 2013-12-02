#include "x/ftp/client.H"

LIBCXX_NAMESPACE::fdbase
new_server_socket(const LIBCXX_NAMESPACE::fdbase &socket);

void shutdown_server_socket(const LIBCXX_NAMESPACE::fdbase &socket);

LIBCXX_NAMESPACE::ftp::client new_client(const LIBCXX_NAMESPACE::fdbase
					 &connArg, bool passive=true);
LIBCXX_NAMESPACE::ftp::client new_client(const LIBCXX_NAMESPACE::fdbase
					 &connArg,
					 const LIBCXX_NAMESPACE::fdtimeout
					 &config,
					 bool passive=true);
bool plain();
