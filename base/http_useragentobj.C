/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/useragent.H"
#include "x/http/bodycallback.H"
#include "x/http/form.H"
#include "x/mime/structured_content_header.H"
#include "x/http/clientauth.H"
#include "x/http/clientauthimpl.H"
#include "x/http/clientauthcache.H"
#include "x/http/cookiejar.H"
#include "x/messages.H"
#include "x/netaddr.H"
#include "x/fdtimeoutconfig.H"
#include "x/singleton.H"
#include "gettext_in.h"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::http::useragentObj);

namespace LIBCXX_NAMESPACE::http {
#if 0
}
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
(LIBCXX_NAMESPACE_STR "::http::useragent::pool::maxconn", 20),
	useragentObj::maxhostconn
	(LIBCXX_NAMESPACE_STR
	 "::http::useragent::pool::maxhostconn", 4),
	useragentObj::maxredirects
	(LIBCXX_NAMESPACE_STR
	 "::http::useragent::maxredirects", 20);

property::value<std::string>
useragentObj::user_agent_header(LIBCXX_NAMESPACE_STR
				"::http::useragent",
				PACKAGE_NAME "/" PACKAGE_VERSION);

useragentObj::sip::sip() : epollfd(epoll::create()),
			   connectionlist_size(0),
			   idle_connections(idle_connections_t::create())
{
}

useragentObj::sip::~sip()
{
}

useragentObj::cache_key_t::cache_key_t()
{
}

useragentObj::cache_key_t::cache_key_t(const uriimpl &uri)
	: scheme(uri.get_scheme()), hostport(uri.get_authority().hostport)
{
}

useragentObj::cache_key_t::~cache_key_t()
{
}

useragentObj::idle_connectionlistObj::idle_connectionlistObj()

{
}

useragentObj::idle_connectionlistObj::~idle_connectionlistObj()
{
}

useragentObj::idleconnObj::idleconnObj()
{
}

useragentObj::idleconnObj::~idleconnObj()
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

useragentObj::epollCallbackObj::~epollCallbackObj()
{
}

void useragentObj::epollCallbackObj::event(const fd &fdRef,
					   event_t event)
{
	// The server must've closed the connection. First, take it out of
	// the
	idleconn c{conn};

	{
		meta_t::writelock lock(conn->uaObj->meta);
		conn->notidleanymore(lock);
	}

	try {
		c->client_terminate();
	} catch (const exception &e)
	{
		e->caught();
	}
}

useragentObj::request_sans_body::request_sans_body()
{
}

useragentObj::request_sans_body::~request_sans_body()
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
	  authcache(clientauthcache::create()),
	  cookies(cookiejar::create())
{
}

useragentObj::~useragentObj()
{
	try {
		terminate();
	} catch (const exception &e)
	{
		e->caught();
	}
}

void useragentObj::terminate()
{
	meta_t::writelock lock(meta);

	for (const auto &ic:(*lock->idle_connections))
	{
		auto c=ic.second.getptr();

		if (!c)
			continue;

		idle_connectionlistObj &obj{*c};

		while (!obj.idle_connection_list.empty())
		{
			idleconn conn{obj.idle_connection_list.front()};
			conn->notidleanymore(lock);
			conn->client_terminate();
		}
	}
}

useragentObj::responseObj::responseObj(const uriimpl &uriArg,
				       const cache_key_t &keyArg)
	: key(keyArg), uri(uriArg)
{
}

useragentObj::responseObj::~responseObj()
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
	if (req.get_method() == GET)
	{
		req.get_URI().set_query(form);
		return request_with_headers(terminate_fd, req);
	}

	req.replace(mime::structured_content_header::content_type,
		    mime::structured_content_header::application_x_www_form_urlencoded);

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
	bool initial=true;
	method_t meth=req.get_method();

	uriimpl redirected_uri;

	for (size_t i=0; ; ++i)
	{
		auto response=initial ?
			do_request_redirected(terminate_fd, req, impl)
			: ({
					requestimpl next(GET, redirected_uri);
					request_sans_body no_body;

					meth=GET;

					do_request_redirected(terminate_fd,
							      next,
							      no_body);
				});

		initial=false;

		if (i >= maxredirects.get())
			return response; // Too many redirections

		LOG_DEBUG("Processing redirect " + to_string(i+1));

		// Check for redirects that we should handle
		// See RFC 2616.

		switch (response->message.get_status_code()) {
		case 301:
		case 302:
		case 307:
			if (meth != HEAD && meth != GET)
				return response;
			break;
		case 303:
			break;
		default:
			return response;
		}

		// If there's no location header, we cannot redirect

		auto location=response->message.equal_range("Location");

		if (location.first == location.second)
			return response;

		try {
			redirected_uri=response->uri +
				uriimpl(std::string(location.first->
						    second.begin(),
						    location.first->
						    second.end()));
		} catch (...) {

			// Probably a bad URI

			return response;
		}

		LOG_TRACE("Processing redirect: "
			  + std::string(response->begin(), response->end()));

		response->discardbody();
	}
}

useragentObj::response
	useragentObj::do_request_redirected(const fd *terminate_fd,
					    requestimpl &req,
					    request_sans_body &impl)
{
	// Append a User-Agent header, unless there's already one

	{
		auto h=req.equal_range("User-Agent");

		if (h.first == h.second)
			req.append("User-Agent",
				   user_agent_header.get());
	}

	uriimpl &uri=req.get_URI();

	// If the URI has user_info, install it as a default basic
	// authentication scheme.

	{
		const uriimpl::authority_t &auth=uri.get_authority();

		if (auth.has_userinfo)
		{
			std::string useridcolonpassword=auth.userinfo;

			uriimpl::authority_t no_auth;

			no_auth.hostport=auth.hostport;

			uri.set_authority(no_auth);

			// Fake response to a www challenge.

			responseimpl resp;
			resp.set_status_code(resp.www_authenticate_code);

			authcache->
				save_basic_authorization(resp,
							 req.get_URI(),
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

cookiejar useragentObj::jar() const
{
	return cookies;
}

useragentObj::response
useragentObj::do_request_with_auth(const fd *terminate_fd,
				   requestimpl &req,
				   request_sans_body &impl)
{
	// Add the Cookie header, after removing any existing cookie headers.

	{
		auto old_cookies=req.equal_range("Cookie");

		req.erase(old_cookies.first, old_cookies.second);
	}

	{
		std::list<std::pair<std::string, std::string> > tray;

		cookies->find(req.get_URI(), tray);

		if (!tray.empty())
		{
			std::ostringstream o;
			const char *sep="";

			for (const auto &cookie:tray)
			{
				o << sep << cookie.first << "="
				  << cookie.second;
				sep="; ";
			}

			req.append("Cookie", o.str());
		}
	}

	LOG_DEBUG("Sending request to " + to_string(req.get_URI()));
	LOG_TRACE( ({
				std::ostringstream o;

				req.to_string(std::ostreambuf_iterator<char>(o));

				o.str();
			}));

	cache_key_t key(req.get_URI());

	while (1)
	{
		idleconn cl(findConn(key, terminate_fd));

		fdclientimpl &c=dynamic_cast<fdclientimpl &>(*cl);

		auto resp=response::create(req.get_URI(), key);

		if (!impl(c, req, resp->message))
			continue;

		LOG_TRACE( ({
					std::ostringstream o;

					resp->message.to_string
						(std::ostreambuf_iterator
						 <char>(o));

					o.str();
				}));

		if (req.response_has_message_body(resp->message))
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

class LIBCXX_HIDDEN useragentObj::challengeObj::basicObj : public challengeObj {

 public:

	uriimpl uri;
	std::string realm;

	basicObj(const uriimpl &uriArg,
		 const std::string &realmArg) : challengeObj(auth::basic, 0),
		uri(uriArg), realm(realmArg) {}
	~basicObj() {}

	clientauthimpl create(const std::string &username,
			      const std::string &password) override
	{
		return clientauthimpl::base::create_basic(uri, realm,
							  username, password);
	}

	clientauthimpl create_hash(const std::string &username,
				   const std::string &a1_hash) override
	{
		throw EXCEPTION("create_hash() invoked for a basic authentication factory object");
	}

	gcry_md_algos algorithm() override
	{
		throw EXCEPTION("algorithm() invoked for a basic authentication factory object");
	}
};

bool useragentObj::process_challenges(const clientauth &authorizations,
				      const requestimpl &req,
				      const response &resp)
{
	// While we're at it, store any new cookies, here.

	cookies->store(req.get_URI(), resp->message);

	process_authentication_info(resp, req,
				    responseimpl::authentication_info,
				    authorizations->www_authorizations);
	process_authentication_info(resp, req,
				    responseimpl::proxy_authentication_info,
				    authorizations->proxy_authorizations);

	// If the response was a challenge, see if we can fill it from
	// the authentication cache.

	bool authentication_required=false;

	resp->challenges.clear();

	resp->message
		.challenges([&, this]
			    (auth scheme,
			     const std::string &realm,
			     const responseimpl::scheme_parameters_t &params)
			    {
				    // Message had a challenge
				    authentication_required=true;

				    if (resp->message
					.www_authentication_required()
					&& scheme == auth::basic)
				    {
					    // If we supplied user:password in
					    // the URI, and we have a basic
					    // authorization failure, remove the
					    // default basic auth module that
					    // was installed in do_request().

					    this->authcache->
						    fail_authorization(req.get_URI(),
								       resp->message,
								       authorizations,
								       scheme,
								       "",
								       params);
				    }


				    if (this->authcache->
					fail_authorization(req.get_URI(),
							   resp->message,
							   authorizations,
							   scheme,
							   realm,
							   params))
					    return;

				    challengeptr p;

				    switch (scheme) {
				    case auth::basic:
					    p=ref<challengeObj::basicObj>
						    ::create(req.get_URI(),
							     realm);
					    break;
				    case auth::digest:

					    {
						    auto func=&challengeObj
							    ::create_digest;
						    if (func)
							    p=func(req.get_URI(),
								   realm,
								   params);
					    }
					    break;
				    default:
					    break;
				    }

				    if (p.null())
					    return;

				    challenge c(p);

				    // We did not find
				    // a cached authentication

				    auto iter=resp->challenges.find(realm);

				    if (iter != resp->challenges.end())
				    {
					    if (iter->second->strength >
						c->strength)
						    return;
					    // Already saved a better method.

					    iter->second=c;
					    return; // Found a better one.
				    }

				    resp->challenges
					    .insert(std::make_pair(realm, c));
			    });

	if (authentication_required && resp->challenges.empty())
		return true; // All challenges were filled from the cache.

	return false;
}

void useragentObj::process_authentication_info(const response &resp,
					       const requestimpl &req,
					       const char *header,
					       authorization_map_t
					       &authorizations)
{
	// Before doing anything, process the Authentication-Info header,
	// if one is present

	responseimpl::scheme_parameters_t params;

	if (!resp->message.get_authentication_info(header, params))
		return;

	auto b=authorizations.begin(),
		e=authorizations.end(),
		p=b;

	if (b != e && ++b == e)
		p->second->authentication_info(req, params);
	else
	{
		LOG_WARNING(libmsg(_txt("Cannot process the Authentication-Info header")));
	}
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
	fdptr terminate_fdptr=terminate_fd ? fdptr(*terminate_fd):fdptr();

	auto epollfd=({
			meta_t::writelock lock{meta};

			lock->epollfd;
		});

	// Check for idle connections closed by the server.

	// epollCallbackObj::event() locks the meta, that's why we make sure
	// the lock is released...

	epollfd->epoll_wait(0);

	// ... and reacquired
	meta_t::writelock lock{meta};

	idle_connectionlistptr connlist;

	for (std::pair<idle_connections_t::base::iterator,
		       idle_connections_t::base::iterator>
		     idleiter=lock->idle_connections->equal_range(key);
	     idleiter.first != idleiter.second; idleiter.first++)
	{
		connlist=idleiter.first->second.getptr();

		if (connlist.null())
			continue;

		idle_connectionlistObj &obj{*connlist};

		if (!obj.idle_connection_list.empty())
		{
			idleconn conn{obj.idle_connection_list.front()};
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

	if (!host.empty() && *host.begin() == '[' && *--host.end() == ']')
		host=host.substr(1, host.size()-2);

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

// ----------------------------------------------------------------

useragentObj::challengeObj::challengeObj(auth schemeArg,
					 int strengthArg)
	: scheme(schemeArg), strength(strengthArg)
{
}

useragentObj::challengeObj::~challengeObj()
{
}

void useragentObj::set_authorization(const response &resp,
				     const challenge &factory,
				     const std::string &userid,
				     const std::string &password)
{
	authcache->save_authorization(resp->message, resp->uri,
				      factory->create(userid, password));
}

void useragentObj::set_digest_authorization(const response &resp,
					    const challenge &factory,
					    const std::string &userid,
					    const std::string &a1_hash)
{
	authcache->save_authorization(resp->message, resp->uri,
				      factory->create_hash(userid, a1_hash));
}

class LIBCXX_HIDDEN useragentObj::request_with_form_upload
	: public request_sans_body {

 public:
	mime::encoder e;

	request_with_form_upload(const mime::encoder &eArg) : e(eArg) {}
	~request_with_form_upload() {}

	bool operator()(fdclientimpl &client,
			requestimpl &req,
			responseimpl &resp) override
	{
		return client.send(req, e->begin(), e->end(), resp);
	}
};

useragentObj::response
useragentObj::request_with_uploads(const fd *terminate_fd,
				   requestimpl &req,
				   const form::const_parameters &form,
				   std::list<mime::encoder> &form_uploads,
				   const std::string &charset)
{
	// Take form parameters, and prepend them to the list of MIME
	// uploads.

	for (auto b=form->begin(), e=form->end(); b != e; )
	{
		--e;

		headersimpl<headersbase::crlf_endl> headers;

		mime::structured_content_header("text/plain")
			("charset", charset)
			.append(headers, mime::structured_content_header
				::content_type, 0);
		mime::structured_content_header("form-data")
			.rfc2047("name", e->first, charset)
			.append(headers,
				mime::structured_content_header
				::content_disposition, 0);
		form_uploads.push_front(mime::make_plain_section(headers,
								 e->second));
	}

	std::string boundary=mime::encoderObj::new_boundary();

	mime::encoderObj::assemble_multipart_content_type(req, "form-data",
							  boundary,
							  0);

	auto e=mime::encoderObj::assemble_multipart_section<headersbase
							    ::crlf_endl>
		([]
		 (const mime::encoder &e)
		 {
		 },
		 0,
		 form_uploads.begin(),
		 form_uploads.end(),
		 boundary);

	request_with_form_upload cb(e);

	return do_request(terminate_fd, req, cb);
}

void useragentObj::add_file_upload(std::list<mime::encoder> &list,
				   const std::string &charset,
				   const std::string &name,
				   const std::string &filename)
{
	headersimpl<headersbase::crlf_endl> headers;

	std::string mimetype=mime::filetype(filename);

	if (mimetype.substr(0, 5) == "text/")
		mimetype=mime::structured_content_header(mimetype)
			("charset", charset);

	create_upload_headers(headers, charset, name, filename,
			      mimetype);

	list.push_back(mime::make_plain_section(headers,
						mime::from_file(filename)));
}

void useragentObj::add_file_upload(std::list<mime::encoder> &list,
				   const std::string &charset,
				   const std::string &name,
				   const fd &file,
				   const std::string &mimetype,
				   const std::string &filename)
{
	headersimpl<headersbase::crlf_endl> headers;

	create_upload_headers(headers, charset, name, filename,
			      mimetype);

	list.push_back(mime::make_plain_section(headers,
						mime::from_file(file)));
}

void useragentObj::create_upload_headers(headersimpl<headersbase::crlf_endl>
					 &headers,
					 const std::string &charset,
					 const std::string &name,
					 const std::string &filename,
					 const std::string &mime_type)
{
	headers.append(mime::structured_content_header::content_type,
		       mime_type);

	mime::structured_content_header("form-data")
		.rfc2047("name", name, charset)
		.rfc2231("filename", filename, charset)
		.append(headers,
			mime::structured_content_header::content_disposition,
			0);
}

#if 0
{
#endif
}
