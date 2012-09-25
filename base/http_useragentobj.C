/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/useragent.H"
#include "x/http/bodycallback.H"
#include "x/http/form.H"
#include "x/http/content_type_header.H"
#include "x/http/clientauth.H"
#include "x/http/clientauthcache.H"
#include "x/messages.H"
#include "x/netaddr.H"
#include "x/fdtimeoutconfig.H"
#include "x/singleton.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

singleton<http::useragentObj> global_useragent LIBCXX_HIDDEN;

http::useragent useragentBase::global()
{
	http::useragentptr p=global_useragent.get();

	if (p.null())
		p=http::useragent::create();

	return p;
}

property::value<size_t> useragentObj::maxconn
(LIBCXX_NAMESPACE_WSTR L"::http::useragent::pool::maxconn", 20),
	useragentObj::maxhostconn
	(LIBCXX_NAMESPACE_WSTR
	 L"::http::useragent::pool::maxhostconn", 4);

useragentObj::sip::sip() : epollfd(epoll::create()),
			   connectionlist_size(0),
			   idle_connections(idle_connections_t::create())
{
}

useragentObj::sip::~sip() noexcept
{
}

useragentObj::cache_key_t::cache_key_t()
{
}

useragentObj::cache_key_t::cache_key_t(const uriimpl &uri)
	: scheme(uri.getScheme()), hostport(uri.getAuthority().hostport)
{
}

useragentObj::cache_key_t::~cache_key_t() noexcept
{
}

useragentObj::idle_connectionlistObj::idle_connectionlistObj()

{
}

useragentObj::idle_connectionlistObj::~idle_connectionlistObj() noexcept
{
}

useragentObj::idleconnObj::idleconnObj()
{
}

useragentObj::idleconnObj::~idleconnObj() noexcept
{
}

void useragentObj::idleconnObj::notidleanymore(meta_t::writelock &lock)
{
	--lock->connectionlist_size;
	lock->connectionlist.erase(connectionlist_iter);
	idlelist->idle_connection_list.erase(idlelist_iter);
	socket->epoll_mod();
}

useragentObj::epollCallbackObj::epollCallbackObj(idleconnObj *connArg)
	: conn(connArg)
{
}

useragentObj::epollCallbackObj::~epollCallbackObj() noexcept
{
}

void useragentObj::epollCallbackObj::event(const fd &fdRef,
					   event_t event)
{
	idleconn c(conn);

	meta_t::writelock lock(conn->uaObj->meta);
	conn->notidleanymore(lock);
}

useragentObj::request_sans_body::request_sans_body()
{
}

useragentObj::request_sans_body::~request_sans_body() noexcept
{
}

bool useragentObj::request_sans_body::operator()(fdclientimpl &client,
						 requestimpl &req,
						 responseimpl &resp)

{
	return client.send(req, resp);
}

useragentObj::useragentObj(clientopts_t optsArg,
			   size_t connectionlist_maxsizeArg,
			   size_t hostconnectionlist_maxsizeArg)

	: opts(optsArg),
	  connectionlist_maxsize(connectionlist_maxsizeArg),
	  hostconnectionlist_maxsize(hostconnectionlist_maxsizeArg),
	  authcache(clientauthcache::create())
{
}

useragentObj::~useragentObj() noexcept
{
}

useragentObj::responseObj::responseObj(const uriimpl &uriArg,
				       const cache_key_t &keyArg)
	: key(keyArg), uri(uriArg)
{
}

useragentObj::responseObj::~responseObj() noexcept
{
	try {
		if (!ua.null())
		{
			fdclientimpl &cl=
				dynamic_cast<fdclientimpl &>(*conn);

			cl.discardbody();

			cl.cancel_terminate_fd();

			if (cl.available())
				ua->recycle(key, conn);
		}
	} catch (...) {
	}
}

void useragentObj::responseObj::discardbody()
{
	auto b=begin(), e=end();

	while (b != e)
		++b;
}

useragentObj::response
useragentObj::request_with_headers(const fd *terminate_fd,
				   requestimpl &req,
				   const form::const_parameters &form)

{
	if (req.getMethod() == GET)
	{
		req.getURI().setQuery(form);
		return request_with_headers(terminate_fd, req);
	}

	req.replace(content_type_header::name,
		    content_type_header::application_x_www_form_urlencoded);

	return request_with_headers(terminate_fd, req,
				    useragent::base::body(form->encode_begin(),
							  form->encode_end()));
}

useragentObj::response
useragentObj::request_with_headers(const fd *terminate_fd,
				   requestimpl &req)

{
	request_sans_body do_req;

	return do_request(terminate_fd, req, do_req);
}

useragentObj::response useragentObj::do_request(const fd *terminate_fd,
						requestimpl &req,
						request_sans_body &impl)
{
	uriimpl &uri=req.getURI();

	// If the URI has user_info, install it as a default basic
	// authentication scheme.

	{
		const uriimpl::authority_t &auth=uri.getAuthority();

		if (auth.has_userinfo)
		{
			std::string useridcolonpassword=auth.userinfo;

			uriimpl::authority_t no_auth;

			no_auth.hostport=auth.hostport;

			uri.setAuthority(no_auth);

			// Fake response to a www challenge.

			responseimpl resp;
			resp.setStatusCode(resp.www_authenticate_code);

			authcache->
				save_basic_authorization(resp,
							 req.getURI(),
							 "",
							 useridcolonpassword);
		}
	}

	// Search for any cached authorizations that we know this request
	// will need.

	auto authorizations=clientauth::create();
	authcache->search_authorizations(req, authorizations);

	// Add authorizations to the request headers.

	authorizations->add_headers(req);

	{
		auto resp=do_request_with_auth(terminate_fd, req, impl);

		if (!process_challenges(authorizations, req, resp))
			return resp;

		// Discard the response, and make sure that the response object
		// goes completely out of scope, so that the same persistent
		// HTTP connection can be used, if possible.
		resp->discardbody();
	}

	// Install updated authorization headers.

	authorizations->add_headers(req);

	auto resp=do_request_with_auth(terminate_fd, req, impl);

	// If there's still a challenge, make sure to return it.

	(void)process_challenges(authorizations, req, resp);
	return resp;
}

useragentObj::response
useragentObj::do_request_with_auth(const fd *terminate_fd,
				   requestimpl &req,
				   request_sans_body &impl)
{
	cache_key_t key(req.getURI());

	while (1)
	{
		idleconn cl(findConn(key, terminate_fd));

		fdclientimpl &c=dynamic_cast<fdclientimpl &>(*cl);

		auto resp=response::create(req.getURI(), key);

		if (!impl(c, req, resp->message))
			continue;

		if (req.responseHasMessageBody(resp->message))
		{
			resp->content_begin_iter=c.begin();
			resp->content_end_iter=c.end();
			resp->conn=cl;
			resp->ua=useragent(this);
		}
		else if (c.available())
			recycle(key, cl);

		return resp;
	}
}

bool useragentObj::process_challenges(const clientauth &authorizations,
				      const requestimpl &req,
				      const response &resp)
{
	// If the response was a challenge, see if we can fill it from
	// the authentication cache.

	bool authentication_required=false;
	bool try_again_flag=true;

	resp->challenges.clear();

	resp->message
		.challenges([&, this]
			    (auth scheme,
			     const std::string &realm,
			     const responseimpl::scheme_parameters_t &params)
			    {
				    // Message had a challenge
				    authentication_required=true;

				    if (this->authcache->
					fail_authorization(req.getURI(),
							   resp->message,
							   authorizations,
							   scheme,
							   realm,
							   params))
					    return;

				    // We did not find
				    // a cached authentication

				    try_again_flag=false;

				    auto iter=resp->challenges.find(realm);

				    if (iter != resp->challenges.end())
				    {
					    if (iter->second.first > scheme)
						    return;
					    // Already saved a better method.

					    resp->challenges.erase(iter);
				    }

				    resp->challenges
					    .insert(std::make_pair
						    (realm, std::make_pair
						     (scheme, params)));
			    });

	if (authentication_required)
		return try_again_flag;

	return false;
	// No authentication challenges, nothing was filled from the cache
}

void useragentObj::recycle(const cache_key_t &key,
			   const idleconn &idle)

{
	auto cb=ref<epollCallbackObj>::create(&*idle);

	meta_t::writelock lock(meta);

	idle_connectionlistObj &i=*idle->idlelist;

	idle->socket->epoll_add(POLLIN, lock->epollfd, cb);

	lock->connectionlist.push_back(idle);
	idle->connectionlist_iter= --lock->connectionlist.end();
	++lock->connectionlist_size;

	i.idle_connection_list.push_back(&*idle);
	idle->idlelist_iter=--i.idle_connection_list.end();

	if (i.idle_connection_list.size() > hostconnectionlist_maxsize
	    || lock->connectionlist_size > connectionlist_maxsize)
	{
		idleconn(i.idle_connection_list.front())->notidleanymore(lock);
	}
}

useragentObj::idleconn useragentObj::findConn(const cache_key_t &key,
					      const fd *terminate_fd)
{
 again:
	std::pair<epoll, size_t>
		idle_cnt=({
				meta_t::writelock lock(meta);

				std::make_pair(lock->epollfd,
					       lock->connectionlist_size);
			});

	fdptr terminate_fdptr=terminate_fd ? fdptr(*terminate_fd):fdptr();

	idle_cnt.first->epoll_wait(0);

	meta_t::writelock lock(meta);

	if (idle_cnt.second != lock->connectionlist_size)
		goto again;

	idle_connectionlistptr connlist;

	for (std::pair<idle_connections_t::base::iterator,
		       idle_connections_t::base::iterator>
		     idleiter=lock->idle_connections->equal_range(key);
	     idleiter.first != idleiter.second; idleiter.first++)
	{
		connlist=idleiter.first->second.getptr();

		if (connlist.null())
			continue;

		idle_connectionlistObj &obj(*connlist);

		if (!obj.idle_connection_list.empty())
		{
			idleconn conn(obj.idle_connection_list.front());
			conn->notidleanymore(lock);

			return conn;
		}
		break;
	}

	if (connlist.null())
	{
		connlist=idle_connectionlist::create();
		lock->idle_connections->insert(std::make_pair(key, connlist));
	}

	std::string service(key.scheme);

	idleconn (useragentObj::*create_func)(clientopts_t,
					      const std::string &,
					      const fd &,
					      const fdptr &);

	chrcasecmp::str_equal_to strcasecmp;

	if (strcasecmp(service, "http"))
	{
		create_func=&useragentObj::init_http_socket;
	}
	else if (strcasecmp(service, "https"))
	{
		create_func=&useragentObj::init_https_socket;
	}
	else
	{
	unknown:

		throw EXCEPTION(gettextmsg(libmsg(_txt("Unknown protocol: %1")),
					   service));
	}

	std::string host(key.hostport);
	std::string port(service);

	size_t p=host.rfind(':');

	if (p != std::string::npos && host.find(']',p+1) == std::string::npos)
	{
		port=host.substr(p+1);
		host=host.substr(0, p);
	}

	fd socket(({
			netaddr addr(netaddr::create(host, port));

			!terminate_fd
				? addr->connect()
				: addr->connect(fdtimeoutconfig
						::terminate_fd(*terminate_fd));
			}));

	idleconnptr conn;

	if (create_func)
		conn=(this->*create_func)(opts, host, socket, terminate_fdptr);

	if (conn.null())
		goto unknown;

	conn->socket=socket;
	conn->uaObj=this;
	conn->idlelist=connlist;

	return conn;
}

useragentObj::idleconn useragentObj::init_http_socket(clientopts_t opts,
						      const std::string &host,
						      const fd &socket,
						      const fdptr &terminator)
{
	ref<connObj<fdclientObj> >
		newclient(ref<connObj<fdclientObj> >::create());

	newclient->install(socket, terminator);
	return newclient;
}

void useragentObj::set_authorization(const response &resp,
				     const std::pair<std::string,
				     std::pair<auth,
				     responseimpl::scheme_parameters_t> >
				     &challenge,
				     const std::string &userid,
				     const std::string &password)
{
	if (challenge.second.first == auth::basic)
		authcache->save_basic_authorization(resp->message,
						    resp->uri,
						    challenge.first,
						    userid,
						    password);
}

#if 0
{
	{
#endif
	}
}
