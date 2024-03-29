/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/fdtlsclientimpl.H"
#include "x/http/responseimpl.H"
#include "x/servent.H"
#include "x/timerfd.H"
#include "x/hms.H"
#include <sstream>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

using http::httpver_t;

property::value<hms>
gnutls::http::fdtlsclientimpl::handshake_timeout(LIBCXX_NAMESPACE_STR
						 "::https::client::handshake_timeout",
						 hms(0,5,0));

property::value<hms>
gnutls::http::fdtlsclientimpl::bye_timeout(LIBCXX_NAMESPACE_STR
					   "::https::client::bye_timeout",
					   30);

gnutls::http::fdtlsclientimpl::fdtlsclientimpl()
	: handshake_required(true), opts(none)
{
}

gnutls::http::fdtlsclientimpl::~fdtlsclientimpl()
{
}

void gnutls::http::fdtlsclientimpl::install(const session &sessArg,
					    const fd &filedescArg,
					    const fdptr &terminatorArg,
					    clientopts_t optsArg)
{
	sess=sessArg;
	handshake_required=true;

	// We handle the proxy connection below. The http client should not
	// format URIs with scheme://host:port

	LIBCXX_NAMESPACE::http::fdclientimpl::install(filedescArg,
						    terminatorArg,
						    optsArg & ~isproxy);
	opts=optsArg;
}

void gnutls::http::fdtlsclientimpl::filedesc_installed(const fdbase &transport)
{
	sess->set_transport(transport);
}

void gnutls::http::fdtlsclientimpl::install(const fd &filedescArg,
					    const fdptr &terminatorArg)
{
	LIBCXX_NAMESPACE::http::responseimpl::throw_bad_gateway();
}

void gnutls::http::fdtlsclientimpl::install(const fd &filedescArg,
					    const fdptr &terminatorArg,
					    clientopts_t options)
{
	LIBCXX_NAMESPACE::http::responseimpl::throw_bad_gateway();
}


fdbase gnutls::http::fdtlsclientimpl::filedesc()
{
	return sess;
}

void gnutls::http::fdtlsclientimpl::internal_send(sendmsg &msg)
{
	init(msg);
	if (proxyfailed)
		return;
	LIBCXX_NAMESPACE::http::fdclientimpl::internal_send(msg);
}

void gnutls::http::fdtlsclientimpl::init(sendmsg &msg)
{
	proxyfailed=false;

	if (!handshake_required)
		return;

	if (sess.null())
		LIBCXX_NAMESPACE::http::responseimpl::throw_bad_gateway();

	std::pair<std::string, int> hostport(msg.req.get_URI().get_host_port());

	session save_session(sess);

	sess=sessionptr();

	if (opts & isproxy)
	{
		std::ostringstream proxyurl;

		proxyurl << "http://" << hostport.first << ":"
			 << hostport.second;

		LIBCXX_NAMESPACE::http::fdclientimpl proxyconn;

		proxyconn.borrow(*this);

		LIBCXX_NAMESPACE::http::requestimpl connect;

		connect.set_method(LIBCXX_NAMESPACE::http::CONNECT);
		connect.set_URI(proxyurl.str());

		proxyerror=LIBCXX_NAMESPACE::http::responseimpl();
		proxyconn.send(connect, proxyerror);

		if (proxyerror.get_status_code() != 200)
		{
			proxyfailed=true;
			return;
		}
	}

	set_readwrite_timeout(handshake_timeout.get().seconds());

	save_session->set_alpn("http/1.1,http/1.0");

	int dummy;

	bool flag=save_session->handshake(dummy);

	cancel_readwrite_timeout();

	if (!flag)
		throw EXCEPTION("TLS negotiation failed");


	try {
		if (!(opts & noverifycert))
		{
			if (!(opts & noverifypeer))
				save_session->verify_peer(hostport.first);
			else
				save_session->verify_peer();
		}
	} catch (const exception &e)
	{
		LIBCXX_NAMESPACE::http::responseimpl
			::throwResponseException(502, (std::string)*e);
	}

	auto protocol=save_session->get_alpn();

	if (protocol)
	{
		if (*protocol == "http/1.1")
		{
			set_peer_http_version(httpver_t::http11);
		}
		else if (*protocol == "http/1.0")
		{
			set_peer_http_version(httpver_t::http10);
		}
	}

	handshake_required=false;

	sess=save_session;

}

void gnutls::http::fdtlsclientimpl::clear()
{
	sess=sessionptr();
	fdclientimpl::clear();
}

void gnutls::http::fdtlsclientimpl
::internal_get_message(LIBCXX_NAMESPACE::http::responseimpl &msg)

{
	if (!proxyfailed)
	{
		fdclientimpl::internal_get_message(msg);
		return;
	}

	msg=proxyerror;
	msg.replace(msg.connection, "close");
}

void gnutls::http::fdtlsclientimpl::terminate()
{
	if (sess.null())
		return;

	session s(sess);

	sess=sessionptr();

	set_readwrite_timeout(bye_timeout.get().seconds());
	try {
		int dummy;

		s->bye(dummy);
	} catch (const exception &e)
	{
		e->caught();
	} catch (...) {
	}
	cancel_readwrite_timeout();
}

#if 0
{
#endif
};
