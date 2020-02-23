/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "ftp_client.H"
#include "x/ftp/exceptionimpl.H"
#include "x/threads/timertask.H"
#include "x/threads/timer.H"
#include "x/netaddr.H"
#include "x/fd.H"
#include "x/fdbase.H"
#include "x/sockaddr.H"
#include "x/fdtimeoutconfig.H"
#include "x/fdtimeout.H"
#include "x/property_value.H"
#include "x/hms.H"
#include "x/messages.H"
#include "x/refiterator.H"
#include "x/chrcasecmp.H"
#include "gettext_in.h"
#include <sstream>
#include <chrono>
#include <cstring>

namespace LIBCXX_NAMESPACE {
	namespace ftp {
#if 0
	};
};
#endif

property::value<hms>
read_timeout LIBCXX_HIDDEN (LIBCXX_NAMESPACE_STR
		     "::ftp::client::timeout::read",
		     hms(0,5,0)),
	write_timeout LIBCXX_HIDDEN (LIBCXX_NAMESPACE_STR
		      "::ftp::client::timeout::write",
		      hms(0,5,0));

property::value<size_t>
read_timeout_bytes LIBCXX_HIDDEN (LIBCXX_NAMESPACE_STR
				  "::ftp::client::timeout::read_bytes", 8192),
	write_timeout_bytes LIBCXX_HIDDEN (LIBCXX_NAMESPACE_STR
					   "::ftp::client::timeout::write_bytes", 8192),

	read_response_max_lines LIBCXX_HIDDEN(LIBCXX_NAMESPACE_STR
					      "::ftp::client::response::maxlines", 16),
	read_response_max_linesize LIBCXX_HIDDEN(LIBCXX_NAMESPACE_STR
						 "::ftp::client::response::maxlinesize", 2048);

//
// Set up a default timeout configuration for a data channel.
//
// When the caller does not supply a timeout configuration, we create one
// here, using the above properties.

class clientObj::default_timeout_config : public fdtimeoutconfig {

public:
	default_timeout_config() {}
	~default_timeout_config() {}

	fdbase operator()(const fd &fdinst)
		const override
	{
		auto new_timeout=fdtimeout::create(fdinst);

		new_timeout->set_read_timeout(read_timeout_bytes.get(),
					      read_timeout.get()
					      .seconds());

		new_timeout->set_write_timeout(write_timeout_bytes.get(),
					       write_timeout.get()
					       .seconds());
		return new_timeout;
	}
};

clientObj::string_callback_base::string_callback_base()
{
}

clientObj::string_callback_base::~string_callback_base()
{
}

void clientObj::string_callback_base::operator()(const std::string &)
{
}

// Parameter to response(), receives each line of the response from the FTP
// server.

class LIBCXX_INTERNAL clientObj::response_collector {

 public:
	response_collector() {}
	~response_collector() {}

	virtual void operator()(const char *ptr) const {}

	class LIBCXX_INTERNAL str;
};

// Collect the response into a single string, up to read_response_max_lines
// lines.

class LIBCXX_INTERNAL clientObj::response_collector::str
	: public response_collector {

 public:
	mutable std::string s;
	mutable size_t n;

	str() : n(0) {}
	~str() {}

	void operator()(const char *ptr) const override
	{
		if (n >= read_response_max_lines.get())
			return;

		++n;

		if (!s.empty())
			s.push_back('\n');
		s += ptr;
	}
};

// Helper class used by make_response_collector(). Turns a lambda into a
// response collector.

template<typename lambda_type>
class LIBCXX_INTERNAL clientObj::response_collector_impl
	: public response_collector {
 public:
	lambda_type lambda;

	response_collector_impl(lambda_type &&lambdaArg)
		: lambda(std::move(lambdaArg))
	{
	}

	~response_collector_impl()
	{
	}

	void operator()(const char *ptr) const override
	{
		lambda(ptr);
	}
};

template<typename lambda_type>
clientObj::response_collector_impl<lambda_type>
clientObj::make_response_collector(lambda_type &&lambda)
{
	return response_collector_impl<lambda_type>(std::move(lambda));
}

// Critical lock -- unless ok() is called, the destructor send ABOR

class LIBCXX_HIDDEN clientObj::lock_abor : public lock {

	// Ok was called

	bool ok_flag;

 public:
	lock_abor(const conn_info &c) : lock(c), ok_flag(false)
	{
	}

	void ok(bool flag=true) { ok_flag=flag; }

	~lock_abor() {
		if (ok_flag)
			return;

		try {
			std::string ignore;

			(*this)->command_raw("\xFF\xF4" // Telnet IP, see RFC 959
					     "ABOR");

			while (1)
			{
				response_collector::str str;

				(*this)->response(str);

				if (str.s.substr(0, 3) == "226")
					break;
			}
		} catch (...)
		{
		}
	};
};

// Constructors for the reference-counted connection information object.

// The arguments get forwarded to the mutex-protected instance.

clientObj::conn_infoObj::conn_infoObj(const fdbase &connArg)
	: instance(connArg)
{
}

clientObj::conn_infoObj::conn_infoObj(const fdbase &connArg,
				      const fdtimeout &timeoutArg)
	: instance(connArg, timeoutArg)
{
}

clientObj::conn_infoObj::~conn_infoObj()
{
}

// Constructor for the base class of the connection information object.
//
// Captures the socket/peer name. Sets up a default timeout handler for the
// connection file descriptor

clientObj::impl_common::impl_common(const fdbase &connArg)
	: socket(connArg),
	  sockname(fd::base::getsockname(connArg->get_fd())),
	  peername(fd::base::getpeername(connArg->get_fd())),
	  timeout(fdtimeout::create(connArg))
{
}

clientObj::impl_common::~impl_common()
{
}

// Use the default connection timeout implementation.
clientObj::impl::impl(const fdbase &connArg)
	: impl_common(connArg),
	  current_timeout(timeout), // Default timeout implementation
	  stream(current_timeout->getiostream()) // Construct its stream
{
}

// User supplied the actual connection, and pre-cooked timeout implementation

clientObj::impl::impl(const fdbase &connArg, const fdtimeout &timeoutArg)
	: impl_common(connArg),

	  // Construct the stream from the timeout implementation.
	  current_timeout(timeoutArg),
	  stream(current_timeout->getiostream())
{
}

clientObj::impl::~impl()
{
}

// Don't care about the response
char clientObj::impl::response(const std::string &command)
{
	return response(command, response_collector());
}

char clientObj::impl::response(const std::string &command,
			       const response_collector &collector)
{
	response_collector::str response_str;

	// Capture the response in response_str, pass it through, too.

	response(make_response_collector([&response_str, &collector]
					 (const char *str)
					 {
						 response_str(str);
						 collector(str);
					 }));

	char c=*response_str.s.c_str();

	switch (c) {
	case '1':
	case '2':
	case '3':
		return c;
	}

	throw_error_from_response_str(command, response_str.s);
}

void clientObj::throw_error_from_response_str(const std::string &command,
					      const std::string &response_str)
{
	int error_code=0;

	// Get the error code from the response message.

	std::istringstream(response_str) >> error_code;

	custom_exception<ftp_custom_exception> e(error_code);

	e << command <<  ": " << response_str;

	throw e;
}

void clientObj::impl::response(const response_collector &collector)
{
	auto max_linesize=read_response_max_linesize.get()+2;

	// First response line received
	char resp1[max_linesize];

	// Current response line received
	char resp2[max_linesize];

	// First time through the loop, save line received to resp1
	char *next_line=resp1;
	char *line;

	set_default_timeouts();

	do
	{
		size_t i=0;
		int ch;
		int last_ch=0;

		line=next_line;

		while (1)
		{
			ch=stream->get();

			if (ch == EOF)
			{
				cancel_default_timeouts();
				broken=true;
				if (stream->fail())
					throw EXCEPTION(_("Connection to FTP server timed out"));

				throw EXCEPTION(_("FTP server has closed the connection"));
			}
			if (last_ch == '\r' && ch == '\n')
				break;
			last_ch=ch;
			if (ch == 0)
				continue; // RFC 2640

			if (i < max_linesize-1)
				line[i++]=ch;
		}

		if (i > 0 && line[i-1] == '\r')
			--i;
		line[i]=0;

		// Save 2nd and subsequent lines in resp2
		next_line=resp2;

		collector(line);

		// Keep looping if the first line received had 4 or more chars,
		// and
	} while (strlen(resp1) > 3 &&

		 // The first three characters of the current line do not
		 // match the first three characters of the first line, or
		 // the fourth character is not a space.
		 //
		 // So, when we receive the first line, resp1 and line are the
		 // same, but the 4th char is ' '

		 (strncmp(resp1, line, 3) || line[3] != ' '));

	cancel_default_timeouts();
}

char clientObj::impl::ok_response(const std::string &command)
{
	return ok_response(command, response_collector());
}

char clientObj::impl::ok_response(const std::string &command,
				  const response_collector &collector)
{
	char c;

	while ((c=response(command, collector)) == '1')
		;

	return c;
}

void clientObj::impl::command(const std::string &command)
{
	std::string escaped_command;

	escaped_command.reserve(command.size()*2);

	// FTP is supposed to use telnet. Telnet escape.

	auto ins_iter=std::back_insert_iterator<std::string>(escaped_command);

	for (char c:command)
	{
		if ((unsigned char)c == (unsigned char)0xFF)
		{
			*ins_iter=0xFF;
			++ins_iter;
		}

		*ins_iter=c;
		++ins_iter;

		if (c == '\r')
		{
			*ins_iter=0; // RFC 2640
			++ins_iter;
		}
	}

	command_raw(escaped_command);
}

void clientObj::impl::command_raw(const std::string &command)
{
	set_default_timeouts();
	(*stream) << command << "\r\n" << std::flush;
	cancel_default_timeouts();
}

void clientObj::impl::set_default_timeouts()
{
	// TLS negotiation failed?
	if (broken)
		throw EXCEPTION(_("FTP connection no longer usable due to a fatal error"));

	broken=true;
	// We set the default timeouts for both read and write operations,
	// which can be used during TLS.
	timeout->set_read_timeout(read_timeout_bytes.get(),
				  read_timeout.get().seconds());
	timeout->set_write_timeout(write_timeout_bytes.get(),
				   write_timeout.get().seconds());
}

void clientObj::impl::cancel_default_timeouts()
{
	broken=false;
	timeout->cancel_read_timer();
	timeout->cancel_write_timer();
}

// Periodically keep the connection alive by sending it NOOPs.

timertask clientObj::create_noop_task(const conn_info &conn)
{
	return timertask::base::make_timer_task
		([conn]
		 {
			 lock l(conn);

			 l->command("NOOP");
			 l->ok_response("NOOP");
		 });
}

clientObj::clientObj(const fdbase &connArg, bool passiveArg)
	: conn(conn_info::create(connArg)),
	  noop_timer_thread(timer::create()),
	  noop_task(create_noop_task(conn)),
	  noop_task_mcguffin(noop_task->autocancel()),
	  passive(passiveArg)
{
	greeting();
}

clientObj::clientObj(const fdbase &connArg,
		     const fdtimeout &timeoutArg,
		     bool passiveArg)
	: conn(conn_info::create(connArg, timeoutArg)),
	  noop_timer_thread(timer::create()),
	  noop_task(create_noop_task(conn)),
	  noop_task_mcguffin(noop_task->autocancel()),
	  passive(passiveArg)
{
	greeting();
}

// After waiting for a greeting, set up a periodic NOOP task.
void clientObj::greeting()
{
	lock l(conn);

	l->stream->exceptions(std::ios::badbit);
	l->ok_response("connect");

	noop_timer_thread->setTimerName("ftp_noop_keepalive");
	noop_timer_thread->scheduleAtFixedRate(noop_task,
					       LIBCXX_NAMESPACE_STR
					       "::ftp::client::noop_interval",
					       std::chrono::minutes(1));
}

void clientObj::auth_tls(lock &l)
{
	l->command("AUTH TLS");
	l->ok_response("AUTH TLS");
}

// Set a new user-supplied timeout handler.
void clientObj::timeout(const fdbase &timeoutArg)
{
	lock l(conn);
	l->current_timeout=timeoutArg;
	current_timeout_updated(l);
}

// Reset to a default timeout handler.
void clientObj::timeout()
{
	lock l(conn);

	reset_default_timeout(l);
}

void clientObj::reset_default_timeout(lock &l)
{
	// Replace user-supplied timeout with the default one.
	l->current_timeout=l->timeout;
	current_timeout_updated(l);
}

// After a new timeout handler has been set, for TLS connections hand it off
// to the TLS session object, to install it. Then reinitialize the read/write
// stream.
void clientObj::current_timeout_updated(lock &l)
{
	auto current_timeout=l->current_timeout;

	if (!l->tls.null())
		current_timeout=l->tls->setTransport(current_timeout);

	l->stream=current_timeout->getiostream();
	l->stream->exceptions(std::ios::badbit);
}

// chdir command

void clientObj::chdir(const std::string &str)
{
	check_unprintable(str, _("NUL character in directory name for CWD")
			  );

	lock l(conn);

	l->command("CWD " + str);
	l->ok_response("CWD");
}

// cdup command
void clientObj::cdup()
{
	lock l(conn);

	l->command("CDUP");
	l->ok_response("CDUP");
}

// smnt command
void clientObj::smnt(const std::string &str)
{
	check_unprintable(str, _("NUL character in parameter for SMNT"));

	lock l(conn);

	l->command("SMNT " + str);
	l->ok_response("SMNT");
}

// logout command
void clientObj::logout()
{
	lock l(conn);

	l->command("QUIT");
	l->ok_response("QUIT");
	if (!l->tls.null())
		l->tls->shutdown();
}

// reinit command
void clientObj::reinit()
{
	lock l(conn);

	l->command("REIN");
	l->ok_response("REIN");
}


void clientObj::login(const std::string &userid,
		      const std::string &password)
{
	std::string userpass=userid+password;

	check_unprintable(userpass, _("FTP login ID or password cannot contain spaces or control characters"));

	lock l(conn);

	l->command("USER " + userid);
	if (l->ok_response("USER") == '3')
	{
		l->command("PASS " + password);
		l->ok_response("PASS");
	}

	if (!l->tls.null())
	{
		// RFC 4217

		l->command("PBSZ 0");
		l->ok_response("PBSZ");

		l->command("PROT P");
		l->ok_response("PROT");
	}
}

clientObj::~clientObj()
{
}

fd clientObj::port(lock &l)
{
	auto family=l->sockname->family();
	std::list<fd> fds;

	std::string address=l->sockname->address();

	netaddr::create(address, 0)->domain(family)->bind(fds, false);

	fds.front()->listen();
	auto port=fds.front()->getsockname()->port();

	std::ostringstream cmd;

	if (family == AF_INET)
	{
		std::replace(address.begin(), address.end(), '.', ',');

		cmd << "PORT " << address
		    << "," << (port / 256) << "," << (port % 256);
	}
	else if (family == AF_INET6)
	{
		cmd << "EPRT |2|" << address << "|" << port << "|";
	}

	auto s=cmd.str();
	l->command(s);
	l->ok_response(s);
	return fds.front();
}

sockaddr clientObj::pasv(lock &l)
{
	auto family=l->peername->family();

	response_collector::str resp;

	if (family == AF_INET)
	{
		l->command("PASV");
		l->ok_response("PASV", resp);
		auto e=resp.s.end();

		auto p=std::find(resp.s.begin(), e, '(');

		if (p != e) ++p;

		std::vector<int> octets;

		while (p != e)
		{
			auto q=p;

			while (*p >= '0' && *p <= '9')
			{
				if (++p == e)
					break;
			}

			int n=0;

			std::istringstream(std::string(q,p)) >> n;

			if (n > 255)
				break;
			octets.push_back(n);

			if (p == e || *p != ',')
				break;

			++p;
		}

		if (p != e && *p == ')' && octets.size() == 6)
		{
			std::ostringstream o;

			o << octets[0] << "." << octets[1] << "." << octets[2]
			  << "." << octets[3];

			return sockaddr::create(AF_INET, o.str(),
						octets[4] * 256 + octets[5]);
		}
	}
	else if (family == AF_INET6)
	{
		l->command("EPSV");
		l->ok_response("EPSV", resp);

		if (resp.s.substr(0, 3) != "229")
			throw EXCEPTION("EPSV: " + resp.s);

		auto e=resp.s.end();

		auto p=std::find(resp.s.begin(), e, '(');

		if (p != e) ++p;

		if (p != e)
		{
			auto q=p;

			size_t i;

			for (i=0; i<3; ++i)
			{
				if (p != e && *p == *q)
					++p;
				else
					break;
			}

			if (i == 3 && p != e)
			{
				q=std::find(p, e, *q);

				if (q != e)
				{
					int n=0;

					std::istringstream i(std::string(p, q));

					i >> n;

					if (!i.fail() && n >= 0 && n <= 65535
					    && ++q != e && *q == ')')
					{
						return sockaddr
							::create(AF_INET6,
								 l->peername
								 ->address(),
								 n);
					}
				}
			}
		}
	}
	else
	{
		throw EXCEPTION(_("Unknown server address protocol family"));
	}

	throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot parse server response: %1%")), resp.s));
}

clientObj::tls_socket clientObj::transfer(lock_abor &l, bool flag,
					  const fdtimeoutconfig &timeout,
					  const std::string &allo_cmd,
					  const std::string &cmd,
					  std::string &response_str) const
{
	l.ok(); // Who cares if TYPE fails.
	l->command(flag ? "TYPE I":"TYPE A");
	l->ok_response("TYPE");

	return transfer(l, timeout, allo_cmd, cmd, response_str);
}

clientObj::tls_socket clientObj::transfer(lock_abor &l,
					  const fdtimeoutconfig &timeout,
					  const std::string &allo_cmd,
					  const std::string &cmd,
					  std::string &response_str) const
{
	l.ok(false);

	if (passive)
	{
		auto address=pasv(l);

		auto netaddr=netaddr::base::result::create();

		netaddr->add(address);

		auto sock=netaddr->connect(timeout);

		if (!allo_cmd.empty())
		{
			l->command(allo_cmd);
			l->ok_response(allo_cmd);
		}

		l->command(cmd);

		response_collector::str str;
		l->response(str);
		response_str=str.s;

		if (*response_str.c_str() == '1')
			return enable_tls(l, std::move(sock), timeout);
	}
	else
	{
		auto fd=port(l);

		if (!allo_cmd.empty())
		{
			l->command(allo_cmd);
			l->ok_response(allo_cmd);
		}

		l->command(cmd);

		response_collector::str str;
		l->response(str);
		response_str=str.s;

		if (*response_str.c_str() == '1')
			return enable_tls(l, timeout(fd)->pubaccept(),
					  timeout);
	}

	throw_error_from_response_str(cmd, response_str);
}

clientObj::tls_socket::tls_socket(fdbase &&socketArg,
				  fdbase &&origsocketArg,
				  const impl_tlsptr &tlsptrArg)
	: fdbase(std::move(socketArg)), tlsptr(tlsptrArg),
	  origsocket(std::move(origsocketArg))
{
}

clientObj::tls_socket::~tls_socket()
{
}

void clientObj::tls_socket::shutdown()
{
	if (tlsptr.null())
		return;

	tlsptr->shutdown(*this);
}

clientObj::tls_socket clientObj::enable_tls(lock &lock,
					    fdbase &&new_connection,
					    const fdtimeoutconfig &config)
	const
{
	if (lock->tls.null())
		return tls_socket(config(std::move(new_connection)),
				  std::move(new_connection), lock->tls);

	return tls_socket(lock->tls->handshake(config(std::move(new_connection)
						      )),
			  std::move(new_connection), lock->tls);
}

void clientObj::nlst_or_list_output(const char *cmd,
				    string_callback_base &output,
				    const std::string &directory) const
{
	nlst_or_list_output(cmd, output, directory, default_timeout_config());
}

void clientObj::check_unprintable(const std::string &argument,
				  const char *what)
{
	if (std::find_if(argument.begin(),
			 argument.end(), [](unsigned char ch)
			 {
				 return ch == 0;
			 }) != argument.end())
		throw EXCEPTION(what);
}

void clientObj::nlst_or_list_output(const char *cmd,
				    string_callback_base &output,
				    const std::string &directory,
				    const fdtimeoutconfig &timeout) const
{
	std::string ignore;

	check_unprintable(directory, _("NUL character in directory name"));

	std::ostringstream o;

	o << cmd;

	if (directory.size() > 0)
		o << " " << directory;

	lock_abor l(conn);

	{
		auto sock=transfer(l, false, timeout, "", o.str(), ignore);

		auto conn=sock->getistream();

		std::string line;

		while (!(std::getline(*conn, line)).eof()
		       || !line.empty())
		{
			if (!line.empty() && *--line.end() == '\r')
				line.pop_back();

			output(line);
			line.clear();
		}
		sock.shutdown();
	}
	l.ok();
	l->ok_response(o.str());
}

clientObj::retr_callback_base::retr_callback_base()
{
}

clientObj::retr_callback_base::~retr_callback_base()
{
}

void clientObj::retr_impl(retr_callback_base &callback,
			  const std::string &file,
			  const fdtimeoutconfig &config,
			  bool binary) const
{
	std::string ignore;

	check_unprintable(file, _("NUL character in filename"));

	std::string cmd="RETR " + file;

	lock_abor l(conn);

	{
		auto conn=transfer(l, binary, config, "", cmd, ignore);

		size_t n=fdbaseObj::get_buffer_size();

		char buffer[n];
		size_t i;

		while ((i=conn->pubread(buffer, n)) > 0)
			callback.chunk(buffer, i);
		conn.shutdown();
	}
	l.ok();
	l->ok_response(cmd);
}

void clientObj::retr_impl(retr_callback_base &callback,
			  const std::string &file,
			  bool binary) const
{
	return retr_impl(callback, file, default_timeout_config(), binary);
}

clientObj::stor_callback_base::stor_callback_base()
{
}

clientObj::stor_callback_base::~stor_callback_base()
{
}

std::string clientObj::stor_command(const std::string &file)
{
	check_unprintable(file, _("NUL character in filename"));

	return "STOR " + file;
}

std::string clientObj::appe_command(const std::string &file)
{
	check_unprintable(file, _("NUL character in filename"));

	return "APPE " + file;
}

void clientObj::do_stor(const std::string &cmd,
			bool binary,
			stor_callback_base &callback)
{
	do_stor(cmd, default_timeout_config(), binary, callback);
}

void clientObj::do_stor(const std::string &cmd,
			const fdtimeoutconfig &config,
			bool binary,
			stor_callback_base &callback)
{
	std::ostringstream allo;

	if (callback.can_allo())
		allo << "ALLO " << callback.allo_size();

	lock_abor l(conn);

	std::string resp;

	{
		auto conn=transfer(l, binary, config, allo.str(), cmd, resp);
		callback.do_stor(*conn);
		conn.shutdown();
		conn.origsocket->pubclose();
	}
	l.ok();
	l->ok_response(cmd);

	auto p=resp.find(' ');

	if (p < resp.size())
		resp=resp.substr(p+1);

	callback.init_response=resp;
}

void clientObj::unlink(const std::string &file)
{
	check_unprintable(file, _("NUL character in filename"));

	lock l(conn);

	l->command("DELE " + file);
	l->ok_response("DELE");
}

void clientObj::rename(const std::string &from_file,
		       const std::string &to_file)
{
	check_unprintable(from_file,
			  _("NUL character in \"from\" filename"));
	check_unprintable(to_file, _("NUL character in \"to\" filename"));

	lock l(conn);

	l->command("RNFR " + from_file);
	l->ok_response("RNFR");

	l->command("RNTO " + to_file);
	l->ok_response("RNTO");
}

std::string clientObj::parse_257(const std::string &reply)
{
	auto b=reply.begin(), e=reply.end();

	b=std::find(b, e, '"');

	if (b != e)
		++b;

	std::stringstream o;

	while (b != e)
	{
		if (*b == '"')
		{
			if (++b == e || *b != '"')
				break;
		}
		o << *b;
		++b;
	}

	return o.str();
}

std::string clientObj::mkdir(const std::string &dir)
{
	response_collector::str resp;

	check_unprintable(dir,
			  _("NUL character in directory filename"));

	lock l(conn);

	l->command("MKD " + dir);
	l->ok_response("MKD", resp);

	return parse_257(resp.s);
}

void clientObj::rmdir(const std::string &dir)
{
	check_unprintable(dir,
			  _("NUL character in directory filename"));

	lock l(conn);

	l->command("RMD " + dir);
	l->ok_response("RMD");
}

std::string clientObj::pwd() const
{
	response_collector::str resp;

	lock l(conn);

	l->command("PWD");
	l->ok_response("PWD", resp);

	return parse_257(resp.s);
}

std::string clientObj::site(const std::string &cmd)
{
	check_unprintable(cmd,
			  _("NUL character in SITE command"));

	response_collector::str resp;

	lock l(conn);

	l->command("SITE " + cmd);
	l->ok_response("SITE", resp);

	return resp.s;
}

ymdhms clientObj::parse_timeval(const std::string &s)
{
	if (s.size() < 14 ||
	    std::find_if(s.begin(), s.begin() + 14, []
			 (char c)
			 {
				 return c < '0' || c > '9';
			 }) != s.begin() + 14)
		throw EXCEPTION(_("Cannot parse timestamp from FTP server"));

	int n[7];

	for (size_t i=0; i<7; i++)
		n[i]=(s[i*2]-'0')*10 + s[i*2+1]-'0';

	return ymdhms(ymd(n[0] * 100 + n[1], n[2], n[3]),
		      hms(n[4], n[5], n[6]),
		      tzfile::base::utc());
}

ymdhms clientObj::timestamp(const std::string &filename)
{
	check_unprintable(filename, _("NUL character in filename"));

	response_collector::str resp;

	lock l(conn);

	l->command("MDTM " + filename);
	l->ok_response("MDTM", resp);

	auto e=resp.s.end(), b=std::find(resp.s.begin(), e, ' ');
	if (b != e)
		++b;

	return parse_timeval(std::string(b, e));
}

off64_t clientObj::size(const std::string &filename)
{
	check_unprintable(filename, _("NUL character in filename"));

	response_collector::str resp;

	lock l(conn);

	l->command("SIZE " + filename);
	l->ok_response("SIZE", resp);

	auto e=resp.s.end(), b=std::find(resp.s.begin(), e, ' ');
	if (b != e)
		++b;

	off64_t s=0;

	std::istringstream(std::string(b, e)) >> s;
	return s;
}

clientObj::stat_callback::stat_callback()
{
}

clientObj::stat_callback::~stat_callback()
{
}

clientBase::stat::stat()
{
}

clientBase::stat::~stat()
{
}

clientBase::stat::stat(const std::string &str) : stat()
{
	std::list<std::string> facts;

	strtok_str(str, ";", facts);

	for (const std::string &fact:facts)
	{
		size_t equal=fact.find('=');

		if (equal >= facts.size())
			continue;

		std::string fact_name=fact.substr(0, equal);
		std::string fact_value=fact.substr(equal+1);
		std::string fact_value_l=fact;

		for (char &c:fact_name)
			c=chrcasecmp::tolower(c);

		for (char &c:fact_value_l)
			c=chrcasecmp::tolower(c);

		if (fact_name == "type")
		{
			type=fact_value_l;
			continue;
		}

		if (fact_name == "unique")
		{
			unique=fact_value;
			continue;
		}

		if (fact_name == "modify")
		{
			modify=clientObj::parse_timeval(fact_value);
			continue;
		}

		if (fact_name == "create")
		{
			create=clientObj::parse_timeval(fact_value);
			continue;
		}

		if (fact_name == "perm")
		{
			perm=fact_value;
			continue;
		}

		if (fact_name == "lang")
		{
			lang=fact_value_l;
			continue;
		}

		if (fact_name == "size")
		{
			std::istringstream(fact_value) >> size;
			continue;
		}

		if (fact_name == "media-type")
		{
			mime_type=fact_value;
			continue;
		}

		if (fact_name == "charset")
		{
			charset=fact_value;
			continue;
		}

		if (fact_name == "unix.mode")
		{
			std::istringstream(fact_value) >> std::oct >> mode;
			continue;
		}

		if (fact_name == "unix.owner")
		{
			std::istringstream(fact_value) >> uid;
			continue;
		}

		if (fact_name == "unix.group")
		{
			std::istringstream(fact_value) >> gid;
			continue;
		}
	}
}

void clientObj::do_filestat(stat_callback &callback,
			    const std::string &filename) const
{
	check_unprintable(filename, _("NUL character in filename"));

	lock l(conn);

	std::ostringstream o;

	o << "MLST";

	if (!filename.empty())
		o << ' ' << filename;
	l->command(o.str());
	l->ok_response("MLST", make_response_collector
		       ([&callback]
			(const char *line)
			{
				if (*line++ != ' ')
					return;

				const char *n=strchr(line, ' ');

				if (!n || !*n) return;
				callback(n+1,
					 client::base::stat(std::string
							    (line, n)));
			}));
}

void clientObj::do_dirstat(stat_callback &callback,
			   const std::string &dirname) const
{
	do_dirstat(callback, dirname, default_timeout_config());
}

void clientObj::do_dirstat(stat_callback &callback,
			   const std::string &dirname,
			   const fdtimeoutconfig &config) const
{
	check_unprintable(dirname, _("NUL character in directory name"));

	std::ostringstream o;

	o << "MLSD";

	if (!dirname.empty())
		o << ' ' << dirname;

	lock_abor l(conn);

	std::string ignore;

	{
		auto sock=transfer(l, config, "", o.str(), ignore);

		auto conn=sock->getistream();

		std::string line;

		while (!(std::getline(*conn, line)).eof() || !line.empty())
		{
			if (!line.empty() && *--line.end() == '\r')
				line.pop_back();

			size_t p=line.find(' ');
			if (p >= line.size())
				continue;

			callback(line.c_str() + p + 1,
				 client::base::stat(line.substr(0, p)));
			line.clear();
		}
		sock.shutdown();
	}
	l.ok();
	l->ok_response(o.str());
}

struct LIBCXX_HIDDEN clientObj::put_fd_callback : public stor_callback_base {

 public:
	fd filedesc;
	struct ::stat stat;

	put_fd_callback(const std::string &filename)
		: filedesc(fd::base::open(filename, O_RDONLY)),
		stat(filedesc->stat())
	{
	}

	~put_fd_callback()
	{
	}

	void do_stor(fdbaseObj &o) override
	{
		o.write(filedesc);
	}

	bool can_allo() override
	{
		return S_ISREG(stat.st_mode);
	}

	off64_t allo_size() override
	{
		return stat.st_size;
	}
};

void clientObj::put_file(const std::string &localfile,
			 const std::string &remotefile,
			 bool binary)
{
	put_fd_callback callback(localfile);

	do_stor(stor_command(remotefile), binary, callback);
}

void clientObj::put_file(const std::string &localfile,
			 const std::string &remotefile,
			 const fdtimeoutconfig &config,
			 bool binary)
{
	put_fd_callback callback(localfile);

	do_stor(stor_command(remotefile), config, binary, callback);
}

std::string clientObj::put_unique_file(const std::string &localfile,
				       bool binary)
{
	put_fd_callback callback(localfile);

	do_stor("STOU", binary, callback);
	return callback.init_response;
}

std::string clientObj::put_unique_file(const std::string &localfile,
				       const fdtimeoutconfig &config,
				       bool binary)
{
	put_fd_callback callback(localfile);

	do_stor("STOU", config, binary, callback);
	return callback.init_response;
}

void clientObj::append_file(const std::string &localfile,
			    const std::string &remotefile,
			    bool binary)
{
	put_fd_callback callback(localfile);

	do_stor(appe_command(remotefile), binary, callback);
}

void clientObj::append_file(const std::string &localfile,
			    const std::string &remotefile,
			    const fdtimeoutconfig &config,
			    bool binary)
{
	put_fd_callback callback(localfile);

	do_stor(appe_command(remotefile), config, binary, callback);
}

#if 0
{
	{
#endif
	}
}
