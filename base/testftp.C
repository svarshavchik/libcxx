/*
** Copyright 2013-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ftp/client.H"
#include "x/ftp/exception.H"
#include "x/options.H"
#include "x/netaddr.H"
#include "x/fdtimeoutconfig.H"
#include "x/destroy_callback.H"
#include "x/threads/run.H"
#include "x/sysexception.H"
#include "x/strtok.H"
#include "x/join.H"

#include <iterator>
#include <set>
#include <sstream>
#include <algorithm>
#include <cstdio>

#include "testftp.H"

std::pair<LIBCXX_NAMESPACE::fd, std::string> create_server_socket()
{
	std::list<LIBCXX_NAMESPACE::fd> fd_list;

	LIBCXX_NAMESPACE::netaddr::create("localhost", 0)->domain(AF_INET)
		->bind(fd_list, true);

	std::ostringstream o;

	o << fd_list.front()->getsockname()->port();

	fd_list.front()->listen();
	fd_list.front()->nonblock(true);
	return std::make_pair(fd_list.front(), o.str());
}

typedef std::pair<LIBCXX_NAMESPACE::fdbaseptr, LIBCXX_NAMESPACE::sockaddrptr>
accept_server_socket_ret_t;

accept_server_socket_ret_t
accept_server_socket(LIBCXX_NAMESPACE::fd &sock,
		     LIBCXX_NAMESPACE::fd &terminator)
{
	LIBCXX_NAMESPACE::fdptr newsock;

	sock->nonblock(true);
	terminator->nonblock(true);
	do
	{
		struct pollfd pfd[2];

		pfd[0].fd=sock->get_fd();
		pfd[1].fd=terminator->get_fd();
		pfd[0].events=POLLIN;
		pfd[1].events=POLLIN;

		if (poll(pfd, 2, -1) < 0)
		{
			if (errno != EINTR)
				throw EXCEPTION("poll");
		}

		if (pfd[1].revents & (POLLIN|POLLHUP|POLLERR))
			return accept_server_socket_ret_t();

		newsock=sock->accept();
	} while (newsock.null());

	sock->nonblock(false);
	terminator->nonblock(false);

	return std::make_pair(newsock,
			      newsock->getsockname());
}

void dummy_server_process(const accept_server_socket_ret_t &sock,
			  const LIBCXX_NAMESPACE::fd &terminator,
			  bool passive,
			  bool ignore_port=false)
{
	auto sock_terminated=
		LIBCXX_NAMESPACE::fdtimeoutconfig::terminate_fd(terminator)
		(sock.first);

	auto stream=sock_terminated->getiostream();

	std::string cmd,line;
	LIBCXX_NAMESPACE::fdptr dataconn;
	LIBCXX_NAMESPACE::sockaddrptr dataconnaddr;
	LIBCXX_NAMESPACE::fdptr openconn;

	bool received_allo=false;

	auto make_conn=[&]
		{
			if (!dataconn.null())
			{
				LIBCXX_NAMESPACE::fd newsock=dataconn->accept();

				return new_server_socket(newsock);
			}

			auto sock=LIBCXX_NAMESPACE::fd::base
			::socket(dataconnaddr->family(), SOCK_STREAM, 0);

			sock->connect(dataconnaddr);

			return new_server_socket(sock);
		};

	do
	{
		line.clear();
		line.reserve(cmd.size());

		for (auto b=cmd.begin(), e=cmd.end(); b != e; ++b)
		{
			if ((unsigned char)*b == 255) // telnet escape
			{
				if (++b == e)
					break;
				continue;
			}
			line.push_back(*b);
		}

		if (line.substr(0, 8) == "AUTH TLS")
		{
			(*stream) << "226 Ok\r\n" << std::flush;

			stream=new_server_socket(sock_terminated)
				->getiostream();
			continue;
		}

		if (line.substr(0, 4) == "PASV" && passive)
		{
			received_allo=false;
			dataconn=({
					std::list<LIBCXX_NAMESPACE::fd> fds;

					LIBCXX_NAMESPACE::netaddr
						::create(sock.second
							 ->address(),
							 0)->bind(fds, false);

					fds.front();
				});

			dataconn->listen();
			dataconnaddr=LIBCXX_NAMESPACE::sockaddrptr();
			auto sockname=dataconn->getsockname();

			std::string portname=({
					std::ostringstream o;

					int portnum=sockname->port();
					o << sockname->address() << "."
					  << portnum / 256 << "."
					  << portnum % 256;

					o.str();
				});

			std::replace(portname.begin(), portname.end(),
				     '.', ',');
			(*stream) << "229 Ok (" << portname << ")\r\n"
				  << std::flush;
			continue;
		}

		if (line.substr(0, 4) == "EPSV" && passive)
		{
			received_allo=false;
			dataconn=({
					std::list<LIBCXX_NAMESPACE::fd> fds;

					LIBCXX_NAMESPACE::netaddr
						::create(sock.second
							 ->address(),
							 0)->bind(fds, false);

					fds.front();
				});

			dataconn->listen();
			dataconnaddr=LIBCXX_NAMESPACE::sockaddrptr();

			auto portnum=dataconn->getsockname()->port();

			(*stream) << "229 Ok (|||" << portnum << ")\r\n"
				  << std::flush;
			continue;
		}

		if (line.substr(0, 4) == "PORT" && !passive)
		{
			received_allo=false;
			std::vector<std::string> words;

			LIBCXX_NAMESPACE::strtok_str(line.substr(5),
						     ",", words);

			words.resize(6);

			std::string ip_address=
				LIBCXX_NAMESPACE::join(words.begin(),
						       words.begin()+4,
						       ".");
			int h=0, l=0;

			std::istringstream(words[4]) >> h;
			std::istringstream(words[5]) >> l;

			dataconnaddr=
				LIBCXX_NAMESPACE::sockaddr
				::create(AF_INET, ip_address,
					 h*256+l);
			dataconn=LIBCXX_NAMESPACE::fdptr();
			(*stream) << "200 Ok\r\n" << std::flush;

			if (ignore_port)
				dataconnaddr=LIBCXX_NAMESPACE::sockaddrptr();
			ignore_port=false;
			continue;
		}

		if (line.substr(0, 4) == "EPRT" && !passive)
		{
			received_allo=false;
			std::vector<std::string> words;

			LIBCXX_NAMESPACE::strtok_str(line.substr(6),
						     line.substr(5, 1).c_str(),
						     words);

			words.resize(2);

			int n=0;

			std::istringstream(words[1]) >> n;
			dataconnaddr=
				LIBCXX_NAMESPACE::sockaddr
				::create(AF_INET6, words[0], n);

			dataconn=LIBCXX_NAMESPACE::fdptr();
			(*stream) << "200 Ok\r\n" << std::flush;

			if (ignore_port)
				dataconnaddr=LIBCXX_NAMESPACE::sockaddrptr();
			ignore_port=false;
			continue;
		}

		if (line.substr(0, 4) == "NLST")
		{
			if (line.substr(4, 6) == " error")
			{
				(*stream) << "400 Error\r\n" << std::flush;
				continue;
			}

			(*stream) << "100 Ok\r\n" << std::flush;

			auto conn=make_conn();
			auto fd=conn->getostream();

			(*fd) << "alpha\r\nbeta\r\n" << std::flush;
			shutdown_server_socket(conn);
		}

		if (line.substr(0, 4) == "RETR")
		{
			if (line.substr(4, 6) == " error")
			{
				(*stream) << "400 Error\r\n" << std::flush;
				continue;
			}

			(*stream) << "100 Ok\r\n" << std::flush;

			auto conn=make_conn();
			auto fd=conn->getiostream();

			if (line.substr(4, 8) == " timeout")
			{
				char dummy;

				fd->get(dummy);
				continue;
			}
			(*fd) << std::string(10000, 'a') << std::flush;
			shutdown_server_socket(conn);
		}

		if (line.substr(0, 4) == "STOR")
		{
			bool error=false;

			if (line.substr(4, 5) == " allo")
			{
				if (!received_allo)
				{
					error=true;
					(*stream) << "500 ALLO not received\r\n"
						  << std::flush;
				}
			}
			if (line.substr(4, 7) == " noallo")
			{
				if (received_allo)
				{
					error=true;
					(*stream) << "500 ALLO received\r\n"
						  << std::flush;
				}
			}

			if (!error)
			{
				(*stream) << "100 Ok\r\n" << std::flush;

				auto conn=make_conn();
				auto fd=conn->getiostream();

				if (line.substr(4, 8) == " timeout")
				{
					// Wait for the ABOR
					openconn=conn;
					continue;
				}

				std::ostringstream ignore;

				auto buf=fd->rdbuf();

				ignore << &*buf;
				shutdown_server_socket(conn);
			}
		}

		if (line.substr(0, 4) == "ABOR")
		{
			openconn=LIBCXX_NAMESPACE::fdptr();
			(*stream) << "226 Ok\r\n" << std::flush;
		}
		else
		{
			(*stream) << "200 Ok\r\n" << std::flush;
		}

		if (line.substr(0, 4) == "ALLO")
			received_allo=true;
		else
		{
			dataconn=LIBCXX_NAMESPACE::fdptr();
			dataconnaddr=LIBCXX_NAMESPACE::sockaddrptr();
		}
	}
	while (!std::getline(*stream, cmd).eof());
}

void connect_test(const char *ip, bool passive)
{
	std::cout << ip << " " << passive << std::endl;

	auto server_socket=create_server_socket();

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto rwsock=LIBCXX_NAMESPACE::fd::base::pipe();

	auto thread=LIBCXX_NAMESPACE::run_lambda
		([passive]
		 (LIBCXX_NAMESPACE::fd &sock,
		  LIBCXX_NAMESPACE::fd &terminator)
		 {
			 accept_server_socket_ret_t newsock;

			 while (!(newsock=accept_server_socket(sock,
							       terminator))
				.first.null())
			 {
				 dummy_server_process(newsock, terminator,
						      passive);
			 }
		 }, server_socket.first, rwsock.first);

	guard(thread);

	auto conn=new_client(LIBCXX_NAMESPACE::netaddr
			     ::create(ip, server_socket.second)
			     ->connect(), passive);

	conn->login("anonymous", "nobody@example.com");
	conn->chdir("pub");
	conn->cdup();

	std::set<std::string> list;

	conn->nlst(std::insert_iterator<std::set<std::string>>(list,
							       list.end()),
		   "x");

	if (list != std::set<std::string>({"alpha", "beta"}))
		throw EXCEPTION("Unexpected NLST response");

	if (!passive)
	{
		bool caught=false;

		try {
			conn->nlst(std::insert_iterator<std::set<std::string>>
				   (list,
				    list.end()),
				   "error");
		} catch (const LIBCXX_NAMESPACE::ftp::exception &e)
		{
			if (e->status_code == 400)
				caught=true;
		}

		if (!caught)
			throw EXCEPTION("Did not receive expected 400 error");
	}

	std::string s;

	conn->get(std::back_insert_iterator<std::string>(s),
		  "file");

	if (s != std::string(10000, 'a'))
		throw EXCEPTION("Did not receive dummy file");

	unlink("ftpfile.tst");
	conn->get_file("ftpfile.tst", "file");

	{
		auto localfile=LIBCXX_NAMESPACE::fd::base
			::open("ftpfile.tst", O_RDONLY);

		auto stream=localfile->getistream();

		if (std::string(std::istreambuf_iterator<char>(*stream),
				std::istreambuf_iterator<char>())
		    != std::string(10000, 'a'))
			throw EXCEPTION("Did not receive dummy file via get_file()");
	}
	unlink("ftpfile.tst");
	s.clear();
	bool caught=false;

	try {
		conn->get(std::back_insert_iterator<std::string>(s),
			  "error");
	} catch (const LIBCXX_NAMESPACE::ftp::exception &e)
	{
		if (e->status_code == 400)
			caught=true;
	}

	if (!caught)
		throw EXCEPTION("Did not receive expected 400 error from retr");

	{
		std::string dummy(10000, 'a');

		conn->put("allo", dummy.begin(), dummy.end());

		std::istringstream i(dummy);

		conn->put("noallo", std::istreambuf_iterator<char>(i),
			  std::istreambuf_iterator<char>());

		{
			auto fd=LIBCXX_NAMESPACE::fd::create("put.tst");

			auto o=fd->getostream();

			*o << "x\n" << std::flush;
			fd->close();

			conn->put_file("put.tst", "allo");
			unlink("put.tst");
		}
	}
}

void testrfc2640()
{
	auto server_socket=create_server_socket();

	std::cout << "testrfc2640" << std::endl;

	std::string whatwegot;

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto terminator=LIBCXX_NAMESPACE::fd::base::pipe();

	auto thread=LIBCXX_NAMESPACE::run_lambda
		([&whatwegot]
		 (LIBCXX_NAMESPACE::fd &sock,
		  LIBCXX_NAMESPACE::fd &terminator)
		 {
			 accept_server_socket_ret_t newsock;

			 while (!(newsock=accept_server_socket(sock,
							       terminator))
				.first.null())
			 {
				 std::string line;

				 auto stream=newsock.first->getiostream();

				 while (1)
				 {
					 (*stream) << "200 Ok\r\n"
						   << std::flush;

					 int last_ch=0;
					 int ch;

					 while ((ch=stream->get()) != EOF)
					 {
						 if (last_ch == '\r' &&
						     ch == '\n')
							 break;
						 last_ch=ch;

						 if (ch == '\r')
						 {
							 whatwegot.push_back('\\');
							 whatwegot.push_back('r');
						 }
						 else if (ch == '\n')
						 {
							 whatwegot.push_back('\\');
							 whatwegot.push_back('n');
						 }
						 else if (ch == 0)
						 {
							 whatwegot.push_back('\\');
							 whatwegot.push_back('0');
						 }
						 else
							 whatwegot.push_back(ch);
					 }
					 if (ch == EOF)
						 break;
				 }
			 }
		 }, server_socket.first, terminator.first);

	{
		auto conn_fd=LIBCXX_NAMESPACE::netaddr
			::create("localhost",
				 server_socket.second)
			->connect();

		auto conn=new_client(conn_fd);

		conn->chdir("dir\r\n\xFF");
	}
	terminator.second->close();
	thread->wait();

	if (whatwegot != "CWD dir\\r\\0\\n\xFF\xFF\\r")
		throw EXCEPTION("Received " + whatwegot + " instead of what we expected");
}

void port_timeout_test()
{
	std::cout << "port_timeout_test" << std::endl;

	auto server_socket=create_server_socket();

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto rwsock=LIBCXX_NAMESPACE::fd::base::pipe();

	auto thread=LIBCXX_NAMESPACE::run_lambda
		([]
		 (LIBCXX_NAMESPACE::fd &sock,
		  LIBCXX_NAMESPACE::fd &terminator)
		 {
			 accept_server_socket_ret_t newsock;

			 while (!(newsock=accept_server_socket(sock,
							       terminator))
				.first.null())
			 {
				 dummy_server_process(newsock, terminator,
						      false, true);
			 }
		 }, server_socket.first, rwsock.first);

	guard(thread);

	auto conn=new_client(LIBCXX_NAMESPACE::netaddr
			     ::create("localhost",
				      server_socket.second)
			     ->connect(), false);

	conn->login("anonymous", "nobody@example.com");

	bool caught=false;
	try {
		std::set<std::string> list;

		conn->nlst(std::insert_iterator<std::set<std::string>>(list,
								       list.end()),
			   LIBCXX_NAMESPACE::make_fdtimeoutconfig
			   ([]
			    (const LIBCXX_NAMESPACE::fdbase &fd)
			    {
				    auto timeout=
					    LIBCXX_NAMESPACE::fdtimeout::create
					    (fd);

				    timeout->set_read_timeout(2);
				    timeout->set_write_timeout(2);
				    return timeout;
			    }));
	} catch (const LIBCXX_NAMESPACE::sysexception &e)
	{
		if (e.getErrorCode() == ETIMEDOUT)
			caught=true;
	}

	if (!caught)
		throw EXCEPTION("Did not catch expected timeout exception");
}

void connect_timeout_test()
{
	auto server_socket=create_server_socket();

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto rwsock=LIBCXX_NAMESPACE::fd::base::pipe();

	auto thread=LIBCXX_NAMESPACE::run_lambda
		([]
		 (LIBCXX_NAMESPACE::fd &sock,
		  LIBCXX_NAMESPACE::fd &terminator)
		 {
			 accept_server_socket_ret_t newsock;

			 while (!(newsock=accept_server_socket(sock,
							       terminator))
				.first.null())
			 {
				 auto i=newsock.first->getiostream();

				 if (!plain())
				 {
					 std::string line;

					 do
					 {
						 (*i) << "200 Ok\r\n"
						      << std::flush;
					 } while (!std::getline((*i), line)
						  .eof() &&
						  line.substr(0, 4) != "AUTH");
				 }

				 while (i->get() != EOF)
					 ;
			 }
		 }, server_socket.first, rwsock.first);

	guard(thread);

	std::cout << "connect_timeout_test" << std::endl;

	auto conn_fd=LIBCXX_NAMESPACE::netaddr::create("localhost",
						       server_socket.second)
		->connect();

	auto timeout_fd=LIBCXX_NAMESPACE::fdtimeout::create(conn_fd);

	timeout_fd->set_write_timeout(3);
	timeout_fd->set_read_timeout(3);

	try {
		auto conn=new_client(conn_fd, timeout_fd);

	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Exception caught: " << e << std::endl;
	}
}

LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>
timeout_thread(const LIBCXX_NAMESPACE::fd &sock,
	       const LIBCXX_NAMESPACE::fd &terminator)
{
	return LIBCXX_NAMESPACE::run_lambda
		([]
		 (LIBCXX_NAMESPACE::fd &sock,
		  LIBCXX_NAMESPACE::fd &terminator)
		 {
			 accept_server_socket_ret_t newsock;

			 while (!(newsock=accept_server_socket(sock,
							       terminator))
				.first.null())
			 {
				 dummy_server_process(newsock, terminator, true);
			 }
		 }, sock, terminator);
}

void retr_timeout_test()
{
	auto server_socket=create_server_socket();

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto rwsock=LIBCXX_NAMESPACE::fd::base::pipe();

	auto thread=timeout_thread(server_socket.first, rwsock.first);

	guard(thread);

	std::cout << "retr_timeout_test" << std::endl;

	auto conn_fd=LIBCXX_NAMESPACE::netaddr::create("localhost",
						       server_socket.second)
		->connect();

	auto timeout_fd=LIBCXX_NAMESPACE::fdtimeout::create(conn_fd);

	timeout_fd->set_write_timeout(3);
	timeout_fd->set_read_timeout(3);

	auto conn=new_client(conn_fd, timeout_fd);

	try {
		errno=0;

		std::string s;
		conn->get(std::back_insert_iterator<std::string>(s),
			  "timeout",
			  LIBCXX_NAMESPACE::make_fdtimeoutconfig
			  ([]
			   (const LIBCXX_NAMESPACE::fdbase &fd)
			   {
				   auto timeout=
					   LIBCXX_NAMESPACE::fdtimeout::create
					   (fd);

				   timeout->set_read_timeout(2);
				   timeout->set_write_timeout(2);
				   return timeout;
			   }));
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		if (errno != ETIMEDOUT)
			throw e;
		std::cout << "Caught RETR timeout exception" << std::endl;
	}
}

class stor_timeout_iter : public std::iterator<std::input_iterator_tag,
						 char, void, void, void>
{
public:

	char operator*() const { return 'a'; }
	stor_timeout_iter &operator++() { return *this; }
	stor_timeout_iter operator++(int) { return *this; }

	bool operator==(const stor_timeout_iter &o) const { return false; }
	bool operator!=(const stor_timeout_iter &o) const { return true; }
};

void stor_timeout_test()
{
	auto server_socket=create_server_socket();

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto rwsock=LIBCXX_NAMESPACE::fd::base::pipe();

	auto thread=timeout_thread(server_socket.first, rwsock.first);

	guard(thread);

	std::cout << "stor_timeout_test" << std::endl;

	auto conn_fd=LIBCXX_NAMESPACE::netaddr::create("localhost",
						       server_socket.second)
		->connect();

	auto timeout_fd=LIBCXX_NAMESPACE::fdtimeout::create(conn_fd);

	timeout_fd->set_write_timeout(3);
	timeout_fd->set_read_timeout(3);

	auto conn=new_client(conn_fd, timeout_fd);

	try {
		errno=0;

		conn->put("timeout", stor_timeout_iter(),
			  stor_timeout_iter(),
			  LIBCXX_NAMESPACE::make_fdtimeoutconfig
			  ([]
			   (const LIBCXX_NAMESPACE::fdbase &fd)
			   {
				   auto timeout=
					   LIBCXX_NAMESPACE::fdtimeout::create
					   (fd);

				   timeout->set_read_timeout(2);
				   timeout->set_write_timeout(2);
				   return timeout;
			   }));
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Caught STOR timeout exception: " << e << std::endl;
	}
}

void noop_test()
{
	auto server_socket=create_server_socket();

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto terminator=LIBCXX_NAMESPACE::fd::base::pipe();

	auto commands_received=LIBCXX_NAMESPACE::fd::base::pipe();

	auto thread=LIBCXX_NAMESPACE::run_lambda
		([]
		 (LIBCXX_NAMESPACE::fd &sock,
		  LIBCXX_NAMESPACE::fd &terminator,
		  LIBCXX_NAMESPACE::fd &commands_received)
		 {
			 accept_server_socket_ret_t newsock;

			 while (!(newsock=accept_server_socket(sock,
							       terminator))
				.first.null())
			 {
				 std::string line;

				 auto stream=newsock.first->getiostream();
				 auto cmd_channel=
					 commands_received->getostream();
				 do
				 {
					 (*stream) << (line.substr(0, 4)
						       == "CDUP" ?
						       "400 Fake error\r\n":
						       "200 Ok\r\n")
						 << std::flush;

					 (*cmd_channel) << line.substr(0, 4)
							<< "\n"
							<< std::flush;
				 }
				 while (!std::getline(*stream, line).eof());
			 }
		 }, server_socket.first, terminator.first,
		 commands_received.second);

	guard(thread);

	std::cout << "noop_test" << std::endl;

	LIBCXX_NAMESPACE::property::load_property
		(LIBCXX_NAMESPACE_STR "::ftp::client::noop_interval",
		 "2", true, true);

	auto conn_fd=LIBCXX_NAMESPACE::netaddr::create("localhost",
						       server_socket.second)
		->connect();

	auto conn=new_client(conn_fd);

	std::string dummy;

	auto cmd_channel=commands_received.first->getistream();

	while (!std::getline(*cmd_channel, dummy).eof())
		if (dummy == "NOOP")
			break;

	bool received=false;

	try {
		conn->cdup();
	} catch (const LIBCXX_NAMESPACE::ftp::exception &e)
	{
		if (e->status_code == 400)
			received=true;
	}

	if (!received)
		throw EXCEPTION("Did not receive expected exception");
}

void internaltest(const char *ip, bool passive)
{
	std::cout << (passive ? "passive:":"active:") << std::endl;
	auto conn=new_client(LIBCXX_NAMESPACE::netaddr::create(ip, "ftp")
			     ->connect(), passive);

	conn->login("anonymous", "nobody@example.com");

	{
		std::vector<std::string> dir;

		conn->list(std::insert_iterator<std::vector<std::string>>
			   (dir, dir.end()));

		for (const auto &n:dir)
		{
			std::cout << "    " << n << std::endl;
		}
	}
	{
		std::set<std::string> dir;

		conn->nlst(std::insert_iterator<std::set<std::string>>
			   (dir, dir.end()));

		for (const auto &n:dir)
		{
			std::cout << "    " << n << std::endl;
		}
	}
	conn->chdir("pub");
	conn->cdup();
	conn->get(std::ostreambuf_iterator<char>(std::cout),
		  "welcome.msg", true);
	std::cout << "Timestamp: "
		  << (std::string)conn->timestamp("welcome.msg")
		  << ", size: " << conn->size("welcome.msg") << std::endl;
	try {
		conn->get(std::ostreambuf_iterator<char>(std::cout),
			  "nonexistent_file");
	} catch (const LIBCXX_NAMESPACE::ftp::exception &e)
	{
		std::cout << e << std::endl;
	}
	conn->chdir("scratch");

	try {
		conn->unlink("testfile");
		conn->unlink("testfile");
	} catch (const LIBCXX_NAMESPACE::ftp::exception &e)
	{
		std::cout << e << std::endl;
	}

	{
		std::string dummy="Sample file\n";

		conn->put("testfile", dummy.begin(), dummy.end());
		std::string resp=conn->put_unique(dummy.begin(), dummy.end());

		std::cout << "STOU: " << resp << std::endl;

		conn->rename(std::string(std::find(resp.begin(), resp.end(),
						   ' ')+1, resp.end()),
			     "testfile");
		conn->unlink("testfile");
	}

	{
		std::istringstream i("Sample file\n");

		conn->put("testfile",
			  std::istreambuf_iterator<char>(i),
			  std::istreambuf_iterator<char>());
		conn->unlink("testfile");
		conn->rmdir(conn->mkdir("newdir"));
	}
	std::cout << "Current directory: " << conn->pwd() << std::endl;
	std::cout << conn->site("HELP") << std::endl;
	conn->cdup();

	conn->filestat([]
		       (const char *p,
			const LIBCXX_NAMESPACE::ftp::client::base::stat &stat)
		       {
			       std::cout << p << ": "
					 << (std::string)(stat.modify)
					 << ", "
					 << (std::string)(stat.create)
					 << std::endl;
		       });

	conn->dirstat([]
		       (const char *p,
			const LIBCXX_NAMESPACE::ftp::client::base::stat &stat)
		       {
			       std::cout << p << ": "
					 << (std::string)(stat.modify)
					 << ", "
					 << (std::string)(stat.create)
					 << std::endl;
		       });
	conn->logout();
}

void make_sure_this_compiles(LIBCXX_NAMESPACE::ftp::client conn,
			     const LIBCXX_NAMESPACE::fdtimeoutconfig &config)
{
	conn->dirstat([]
		      (const char *p,
		       const LIBCXX_NAMESPACE::ftp::client::base::stat &stat)
		       {
		       }, config);
}

void internaltest(const char *ip)
{
	std::cout << ip << ":" << std::endl;
	internaltest(ip, false);
	internaltest(ip, true);
}

int main(int argc, char **argv)
{
	alarm(120);
	try {
		LIBCXX_NAMESPACE::property::load_property
			(LIBCXX_NAMESPACE_STR
			 "::gnutls::ignore_premature_termination_error",
			 "true", true, true);

		LIBCXX_NAMESPACE::option::bool_value
			test(LIBCXX_NAMESPACE::option::bool_value::create());

		LIBCXX_NAMESPACE::option::list
			options(LIBCXX_NAMESPACE::option::list::create());

		options->add(test, 't', "test", 0, "Run internal test");
		options->addDefaultOptions();

		LIBCXX_NAMESPACE::option::parser
			opt_parser(LIBCXX_NAMESPACE::option::parser::create());

		opt_parser->setOptions(options);
		int err=opt_parser->parseArgv(argc, argv);

		if (err == 0) err=opt_parser->validate();

		if (err)
		{
			if (err == LIBCXX_NAMESPACE::option::parser::base
			    ::err_builtin)
				exit(0);

			std::cerr << opt_parser->errmessage();
			std::cerr.flush();
			exit(1);
		}

		if (test->isSet())
		{
			internaltest("127.0.0.1");
			internaltest("localhost");
		}
		else
		{
			connect_test("127.0.0.1", true);
			connect_test("127.0.0.1", false);
			connect_test("localhost", true);
			connect_test("localhost", false);
			if (plain())
				testrfc2640();
			port_timeout_test();
			connect_timeout_test();
			retr_timeout_test();
			stor_timeout_test();
			if (plain())
				noop_test(); // Must be last one
		}
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}

	return 0;
}
