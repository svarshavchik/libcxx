/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_useragentobj_H
#define x_http_useragentobj_H

#include <x/http/fdclient.H>
#include <x/http/clientauthfwd.H>
#include <x/http/clientauthimplfwd.H>
#include <x/http/clientauthcachefwd.H>
#include <x/http/cookiejarfwd.H>
#include <x/http/formfwd.H>
#include <x/http/uploadfwd.H>
#include <x/http/useragentfwd.H>
#include <x/mime/encoder.H>
#include <x/gcrypt/md.H>
#include <x/epoll.H>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/fdfwd.H>
#include <x/property_properties.H>
#include <x/weakmultimap.H>
#include <x/sipobj.H>
#include <x/logger.H>

#include <list>
#include <map>
#include <iterator>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

class clientauthcacheObj;

//! A reference to an internal object that caches client authorizations

typedef ref<clientauthcacheObj> clientauthcache;


//! Helper class for compile-time verification of the proper iterator type passed to useragentObj::request()

//! \internal
//!
template<typename iterator_category>
class useragent_iterator_check {

public:
	//! This is a valid iterator type

	static constexpr int check_value=0;
};

//! Helper class for compile-time verification of the proper iterator type passed to useragentObj::request()

//! \internal
//!

template<>
class useragent_iterator_check<std::input_iterator_tag> {

public:
	//! This is not a valid iterator type

	static constexpr int check_value= -1;
};

//! Helper class for compile-time verification of the proper iterator type passed to useragentObj::request()

//! \internal
//!
template<>
class useragent_iterator_check<std::output_iterator_tag> {

public:
	//! This is not a valid iterator type

	static constexpr int check_value= -1;
};

//! An HTTP user agent implementation

//! A high level HTTP client that caches persistent HTTP 1.1 connections.
//! Thread safe.
//!
class useragentObj : virtual public obj {

	//! The internal logger object

	LOG_CLASS_SCOPE;

	class idleconnObj;

	//! Client options

	int opts;

	//! List of cached connections

	//! \internal
	//!
	typedef std::list< ptr<idleconnObj> > connectionlist_t;

	//! Connection cache key

	//! \internal
	//!
	struct cache_key_t {

		//! The scheme of the cached connection
		std::string scheme;

		//! The host:port of the cached connection
		std::string hostport;

		//! Default constructor
		cache_key_t();

		//! Constructor -- extract the key for a URI

		cache_key_t(const uriimpl &uri);

		//! Destructor
		~cache_key_t();

		//! Comparison function for an associative container

		bool operator<(const cache_key_t &a) const
		{
			int rc=chrcasecmp::compare(scheme, a.scheme);

			if (rc == 0)
				rc=chrcasecmp::compare(hostport, a.hostport);

			return rc < 0;
		}
	};

	//! A list of all cached connections to the same destination

	//! \internal
	//!
	struct idle_connectionlistObj : virtual public obj {

	public:
		//! Constructor
		idle_connectionlistObj();

		//! Destructor
		~idle_connectionlistObj();

		//! The list of all cached idle objects that hold a reference to this object

		std::list<idleconnObj *> idle_connection_list;
	};

	//! A reference to a list of all idle connections to the same destination

	//! \internal
	//!

	typedef ref<idle_connectionlistObj> idle_connectionlist;

	//! A nullable pointer reference to a list of all idle connections to the same destination

	//! \internal
	//!

	typedef ptr<idle_connectionlistObj> idle_connectionlistptr;

	class idleconnObj;

	//! An epoll object that's used to clean up closed connections

	//! When the HTTP server closes the socket for a cached connection,
	//! this callback object helps in deleting it from the useragent.

	//! \internal
	//!

	class epollCallbackObj : public epoll::base::callbackObj {
	public:
		//! This idle connection
		idleconnObj *conn;

		//! Constructor
		epollCallbackObj(idleconnObj *connArg);

		//! Destructor
		~epollCallbackObj();

		//! The connection must've been closed. Delink it.

		void event(const fd &fdRef,
			   event_t event) override;
	};

	//! A reference to a cached idle connection

	//! \internal
	//!

	typedef ref<idleconnObj> idleconn;

	//! A nullable pointer reference to a cached idle connection

	//! \internal
	//!
	typedef ptr<idleconnObj> idleconnptr;

	template<typename client_type> class connObj;

	//! Maximum size of connectionlist

	//! \internal
	//!
	size_t connectionlist_maxsize;

	//! Maximum number of cached connections to the same host.

	//! \internal
	//!
	size_t hostconnectionlist_maxsize;

	//! A map that holds all cached connections to a destination

	//! \internal
	//!

	typedef weakmultimap<cache_key_t, idle_connectionlistObj>
	idle_connections_t;

	//! The following data is somewhat important

	class sip {

	public:
		sip() LIBCXX_HIDDEN;
		~sip() LIBCXX_HIDDEN;

		//! An epoll file descriptor that monitors open cached connections

		//! \internal
		//!
		epoll epollfd;

		//! A list of all cached connections

		//! \internal
		//!
		connectionlist_t connectionlist;

		//! Keep track of the size of connectionlist

		//! \internal
		//!
		size_t connectionlist_size;

		//! All cached connections, per a given destination

		//! \note
		//! An active connection, a connection that some thread is currently
		//! using, for a pending HTTP request, also holds a reference to the
		//! stored idle connection list.
		//!
		//! \internal
		//!

		idle_connections_t idle_connections;
	};

	//! Thread-safe metadata

	typedef sipobj<sip> meta_t;

	//! Safely-locked connection metadata
	meta_t meta;

	//! A cached connection

	//! \internal
	//!

	class idleconnObj : virtual public obj {

	public:
		//! Constructor
		idleconnObj();

		//! Destructor
		~idleconnObj();

		//! This cached connection is owned by this user agent object
		useragentObj *uaObj;

		//! My iterator in useragent's idle_connections list

		connectionlist_t::iterator connectionlist_iter;

		//! A reference to a list of all cached connections to the same destination
		idle_connectionlistptr idlelist;

		//! My iterator in idlelist->idle_connection_list.

		std::list<idleconnObj *>::iterator idlelist_iter;

		//! The connected socket
		fdptr socket;

		//! This connection is not idle anymore, remove it from useragent's cached list.
		void notidleanymore(meta_t::writelock &lock) LIBCXX_HIDDEN;

		//! Being called from terminate().
		virtual void client_terminate()=0;
	};

	//! Multiply inherit from fdclient and idleconnObj

	//! \internal
	//!

	template<typename client_type>
	class connObj : public idleconnObj, public client_type {

	public:
		connObj() LIBCXX_HIDDEN {}
		~connObj() LIBCXX_HIDDEN {}

		void client_terminate() override LIBCXX_HIDDEN
		{
			this->terminate();
		}
	};

	//! Internal authentication cache

	clientauthcache authcache;

	//! Internal cookie jar

	cookiejar cookies;

public:

	//! Maximum number of persistent connections cached by a useragent

	static property::value<size_t> maxconn;

	//! Maximum number of persistent connections to the same host cached by useragent
	static property::value<size_t> maxhostconn;

	//! Maximum number of redirections

	static property::value<size_t> maxredirects;

	//! My user agent header

	static property::value<std::string> user_agent_header;

	friend class useragentBase;

	class bodycallbackObj;

	//! Response body receiver callback

	//! This callback object's body() method receives the beginning and
	//! the ending iterator that defines the input sequence of the body
	//! of a response to a request.
	//!
	//! Use "http::useragent::base::bodycallback to reference this typedef.
	typedef ref<bodycallbackObj> bodycallback;

	//! A nullable pointer to a response body receiver callback.

	//! Use "http::useragent::base::bodycallbackptr to reference this
	//! typedef.

	typedef ptr<bodycallbackObj> bodycallbackptr;

	//! Default constructor
	useragentObj(//! Client options.

		     //! Allowable options are:
		     //!
		     //! - \c http::noverifypeer
		     //!
		     //! - \c http::noverifycert

		     clientopts_t optsArg=http::none,

		     //! Maximum number of persistent connections cached by useragent

		     size_t connectionlist_maxsizeArg=maxconn.get(),

		     //! Maximum number of persistent connections to the same host cached by useragent

		     size_t hostconnectionlist_maxsizeArg
		     =maxhostconn.get());

	//! Default destructor
	~useragentObj();

	//! Terminate all connections.

	void terminate();

	class responseBase;
	class responseObj;
	class challengeObj;

	//! A reference to an authentication challenge factory

	typedef ref<challengeObj> challenge;

	//! A nullable pointer reference to an authentication challenge factory

	typedef ptr<challengeObj> challengeptr;

	//! An authentication handle factory object

	class challengeObj : virtual public obj {

	public:
		//! The authentication scheme
		const auth scheme;

		//! The relative strength of the authentication scheme
		const int strength;

		//! Constructor

		challengeObj(auth schemeArg,
			     int strengthArg);

		//! Destructor
		~challengeObj()=0;

		//! Construct an authentication handler for the given userid and password

		virtual clientauthimpl create(const std::string &username,
					      const std::string &password)=0;

		//! Construct an authentication handler with a precomputed A1 hash

		//! This is implemented by the digest authentication scheme only
		virtual clientauthimpl create_hash(const std::string &username,
						   const std::string &a1_hash)
		=0;

		//! Return the digest algorith.
		//! This is implemented by the digest authentication scheme only

		virtual gcry_md_algos algorithm()=0;

		class basicObj;
		class digestObj;

		//! Create a digest challenge factory

		//! Implemented in libcxxtls
		//!
		static challengeptr
		create_digest(const uriimpl &request_uri,
			      const std::string &realm,
			      const responseimpl::scheme_parameters_t &params)
			__attribute__((weak));
	};

	//! A reference to a response to an HTTP request.
	typedef ref<responseObj, responseBase> response;

	//! A nullable pointer reference to a response to an HTTP request.
	typedef ptr<responseObj, responseBase> responseptr;

	//! A reference to a const response to an HTTP request.
	typedef const_ref<responseObj, responseBase> const_response;

	//! A nullable pointer reference to a const response to an HTTP request.
	typedef const_ptr<responseObj, responseBase> const_responseptr;

	//! The base class defines an iterator over the response contents.
	class responseBase : public ptrref_base {

	public:

		//! Objects returned by http::useragent::base::response::begin() and end().

		typedef fdclientimpl::iterator iterator;
	};

	//! A response to an HTTP request.

	//! A reference to this object gets returned by request(). This is
	//! a reference-counted responseimpl object, that also holds internal
	//! metadata relating to the request.
	//!
	class responseObj : virtual public obj {

		//! The underlying connection to the HTTP server, where the content is retrieved from

		idleconnptr conn;

		//! The user agent object that constructed this response.
		useragentptr ua;

		//! The cache key for the underlying connection in the useragent

		cache_key_t key;

	public:
		//! Internal constructor, invoked by useragent
		responseObj(const uriimpl &uriArg,
			    const cache_key_t &keyArg) LIBCXX_HIDDEN;

	private:
		//! Response content beginning iterator, returned by begin().

		responseBase::iterator content_begin_iter;

		//! Response content ending iterator, returned by end().
		responseBase::iterator content_end_iter;

	public:
		friend class useragentObj;

		//! The request's original URI
		const uriimpl uri;

		//! The received HTTP response message
		responseimpl message;

		//! A container for the new authentication challenges

		//! Map key is the authentication realm. The value is a
		//! a factory for the authentication implementation.

		typedef std::map<std::string, challenge> challenges_t;

		//! New authentication challenges

		challenges_t challenges;

		//! Destructor

		//! If the response had no content, the destruction is
		//! immediate. If the response contained content, which has
		//! not been fully retrieved, there may be a delay to drain
		//! the unconsumed content, before the underlying connection
		//! gets returned to the useragent object.

		~responseObj();

		//! Return an indication if this response has content.
		bool has_content() const
		{
			return content_begin_iter != content_end_iter;
		}

		//! Return the response's content beginning iterator

		//! The beginning iterator may be retrieved only once.
		//! The returned iterator type should be referenced as
		//! \c http::useragent::base::body_iterator;

		responseBase::iterator begin() const noexcept
		{
			return content_begin_iter;
		}

		//! Return the response's content ending iterator

		//! The returned iterator type should be referenced as
		//! \c http::useragent::base::body_iterator;
		responseBase::iterator end() const noexcept
		{
			return content_end_iter;
		}

		//! Discard the message body

		//! This also happens in the destructor, but with no error
		//! handling. Here, if there was a network error, an exception
		//! gets thrown.

		void discardbody();
	};

	friend class responseObj;

	//! Send a request with a terminatable file descriptor.

	//! The extra parameter, terminate_fd, is the terminatable file
	//! descriptor.
	//! If this file descriptor becomes readable, the request is aborted.

	template<typename... Args_t>
	response request(//! The terminatable file descriptor

			 const fd &terminate_fd,

			 //! Arguments
			 Args_t && ...args)
	{
		return request_terminate_fd(&terminate_fd,
					    std::forward<Args_t>(args)...);
	}

	//! Send a request with or without a terminatable file descriptor

	template<typename... Args_t>
	response request(//! The terminatable file descriptor

			 const fdptr &terminate_fd,

			 //! Arguments
			 Args_t && ...args)
	{
		if (terminate_fd.null())
			return request_terminate_fd(nullptr,
						    std::forward<Args_t>
						    (args)...);

		fd rfd(terminate_fd);

		return request_terminate_fd(&rfd,
					    std::forward<Args_t>(args)...);
	}

	//! Send a request without a terminatable file descriptor parameter.

	//! The request gives an explicit requestimpl object.
	//!
	template<typename... Args_t>
	response request(requestimpl &req,
			 Args_t && ...args)
	{
		return request_terminate_fd(nullptr, req,
					    std::forward<Args_t>(args)...);
	}

	//! Send a request without a terminatable file descriptor parameter.

	//! A request that starts with an explicit HTTP method.
	//!
	template<typename... Args_t>
	response request(method_t method,
			 Args_t && ...args)
	{
		return request_terminate_fd(nullptr, method,
					    std::forward<Args_t>(args)...);
	}

	//! Set the authorization in response to a challenge

	void set_authorization(//! Failed response
			       const response &response,

			       //! The challenge
			       const challenge &factory,

			       //! Userid
			       const std::string &userid,

			       //! Password
			       const std::string &password);

	//! Set the authorization in response to a Digest scheme challenge, using a precomputed A1 hash

	void set_digest_authorization(//! Original response
				      const response &resp,

				      //! The digest challenge
				      const challenge &factory,

				      //! The userid
				      const std::string &userid,

				      //! The precomputed 'A1' hash
				      const std::string &a1_hash);

	friend class uploadObj;

private:

	//! A request with headers given in an explicit requestimpl object.

	template<typename... Args_t>
	response request_terminate_fd(const fd *terminate_fd,
				      requestimpl &req,
				      Args_t && ...args)
	{
		return request_with_headers(terminate_fd, req,
					    std::forward<Args_t>(args)...);
	}

	//! A request that starts with an explicit HTTP method.

	template<typename... Args_t>
	response request_terminate_fd(const fd *terminate_fd,
				      method_t method,
				      Args_t && ...args)
	{
		return request_terminate_fd_method(terminate_fd, method,
						   std::forward<Args_t>(args)...
						   );
	}

	//! A URI after the request method

	template<typename... Args_t>
	response request_terminate_fd_method(const fd *terminate_fd,
					     method_t method,
					     const uriimpl &uri,
					     Args_t && ...args)
	{
		requestimpl req(method, uri);

		return request_with_headers(terminate_fd, req,
					    std::forward<Args_t>(args)...);
	}

	//! A URI and an HTTP version, after the request method

	template<typename... Args_t>
	response request_terminate_fd_method(const fd *terminate_fd,
					     method_t method,
					     const uriimpl &uri,
					     httpver_t httpver,
					     Args_t && ...args)
	{
		requestimpl req(method, uri, httpver);

		return request_with_headers(terminate_fd, req,
					    std::forward<Args_t>(args)...);
	}

	//! Add header name and value to the request

	template<typename... Args_t>
	response request_with_headers(const fd *terminate_fd,
				      requestimpl &req,
				      const std::string &hdrname,
				      const std::string &hdrvalue,
				      Args_t && ...args)

	{
		req.append(hdrname, hdrvalue);

		return request_with_headers(terminate_fd, req,
					    std::forward<Args_t>(args)...);
	}

	//! A request with form parameters
	response request_with_headers(const fd *terminate_fd,
				      requestimpl &req,
				      const form::const_parameters &form);

	//! A request with form parameters and file uploads

	template<typename ...Args>
	response request_with_headers(const fd *terminate_fd,
				      requestimpl &req,
				      const form::const_parameters &form,
				      const std::string &charset,
				      Args && ...args)
	{
		std::list<mime::encoder> form_uploads;

		request_form_uploads(form_uploads, charset,
				     std::forward<Args>(args)...);

		return request_with_uploads(terminate_fd, req,
					    form, form_uploads, charset);
	}

	//! Process a request with form parameters and file uploads

	response request_with_uploads(const fd *terminate_fd,
				      requestimpl &req,
				      const form::const_parameters &form,
				      std::list<mime::encoder> &form_uploads,
				      const std::string &charset);

	//! No more file uploads
	void request_form_uploads(std::list<mime::encoder> &list,
				  const std::string &charset)
	{
	}

	//! File upload

	template<typename ...Args>
	void request_form_uploads(std::list<mime::encoder> &list,
				  const std::string &charset,
				  const std::string &name,
				  const std::string &filename,
				  Args && ...args)
	{
		add_file_upload(list, charset, name, filename);
		request_form_uploads(list, charset,
				     std::forward<Args>(args)...);
	}

	//! File upload

	template<typename ...Args>
	void request_form_uploads(std::list<mime::encoder> &list,
				  const std::string &charset,
				  const std::string &name,
				  const fd &file,
				  const std::string &mimetype,
				  const std::string &filename,
				  Args && ...args)
	{
		add_file_upload(list, charset, name, file, mimetype, filename);
		request_form_uploads(list, charset,
				     std::forward<Args>(args)...);
	}

	template<typename ...Args>
	void request_form_uploads(std::list<mime::encoder> &list,
				  const std::string &charset,
				  const upload &upload_list,
				  Args && ...args);

	//! File upload

	template<typename ...Args, typename iter_type>
	void request_form_uploads(std::list<mime::encoder> &list,
				  const std::string &charset,
				  const std::string &name,
				  const std::pair<iter_type, iter_type> &iter,
				  const std::string &filename,
				  const std::string &mime_type,
				  Args && ...args)
	{
		add_file_upload(list, charset, name, iter, filename, mime_type);
		request_form_uploads(list, charset,
				     std::forward<Args>(args)...);
	}

	//! Add an upload

	template<typename container_type>
	void add_upload(std::list<mime::encoder> &list,
			const std::string &charset,
			const std::string &name,
			container_type &&container,
			const std::string &filename,
			const std::string &mime_type)
	{
		headersimpl<headersbase::crlf_endl> headers;

		create_upload_headers(headers, charset,
				      name, filename, mime_type);
		list.push_back(mime::make_plain_section(headers, container));
	}

	//! Add a file upload

	template<typename iter_type>
	void add_file_upload(std::list<mime::encoder> &list,
			     const std::string &charset,
			     const std::string &name,
			     const std::pair<iter_type, iter_type> &iter,
			     const std::string &filename,
			     const std::string &mime_type)
	{
		// A compiler error on the following line means that a pair of
		// input or output iterators were passed to request().
		//
		// Output iterators are obviously wrong. Input iterators are,
		// as documented, not allowed; at the minimum the iterators
		// should be forward iterators.

		extern char invalid_iterator_type[useragent_iterator_check<typename std::iterator_traits<iter_type>::iterator_category>::check_value]
			__attribute__((unused));

		struct container {
			iter_type b;
			iter_type e;

			container(iter_type bArg,
				  iter_type eArg) : b(bArg), e(eArg) {}

			iter_type begin() const { return b; }
			iter_type end() const { return e; }
		};

		add_upload(list, charset, name,
			   container(iter.first, iter.second),
			   filename, mime_type);

	}

	//! Add a file upload

	void add_file_upload(std::list<mime::encoder> &list,
			     const std::string &charset,
			     const std::string &name,
			     const std::string &filename);

	//! Add a file upload

	void add_file_upload(std::list<mime::encoder> &list,
			     const std::string &charset,
			     const std::string &name,
			     const fd &file,
			     const std::string &mimetype,
			     const std::string &filename);

	//! Create headers for the upload's MIME section

	void create_upload_headers(headersimpl<headersbase::crlf_endl> &headers,
				   const std::string &charset,
				   const std::string &name,
				   const std::string &filename,
				   const std::string &mime_type);

	//! A request without a body

	response request_with_headers(const fd *terminate_fd,
				      requestimpl &req);

	//! A request with an arbitrary body

	//! \internal
	template<typename iter_type>
	response request_with_headers(const fd *terminate_fd,
				      requestimpl &req,
				      const std::pair<iter_type, iter_type>
				      &req_body)

	{
		// A compiler error on the following line means that a pair of
		// input or output iterators were passed to request().
		//
		// Output iterators are obviously wrong. Input iterators are,
		// as documented, not allowed; at the minimum the iterators
		// should be forward iterators.

		extern char invalid_iterator_type[useragent_iterator_check<typename std::iterator_traits<iter_type>::iterator_category>::check_value]
			__attribute__((unused));

		// This function requires forward iterators (or bidirectional,
		// or random).

		request_with_body<iter_type> do_req(req_body.first,
						    req_body.second);

		return do_request(terminate_fd, req, do_req);
	}


	//! Send a request without a body

	class request_sans_body {

	public:
		//! Constructor

		request_sans_body();

		//! Destructor
		~request_sans_body();

		//! Send the request

		virtual bool operator()(fdclientimpl &client,
					requestimpl &req,
					responseimpl &resp);
	};

	//! Send a request together with the body

	template<typename iter_type>
	class request_with_body : public request_sans_body {

		//! Beginning iterator that defines the body
		iter_type beg_iter;

		//! Ending iterator that defines the body
		iter_type end_iter;

	public:
		//! Constructor
		request_with_body(const iter_type &beg_iterArg,
				  const iter_type &end_iterArg)
			: beg_iter(beg_iterArg),
			  end_iter(end_iterArg)
		{
		}

		//! Destructor
		~request_with_body()
		{
		}

		//! Send the request

		bool operator()(fdclientimpl &client,
				requestimpl &req,
				responseimpl &resp) override
		{
			return client.send(req, beg_iter, end_iter, resp);
		}
	};

	class request_with_form_upload;

	//! Implement the request

	//! If the URI specifies authentication credentials, remove them,
	//! and install them as default basic authentication scheme
	//! credentials.
	//!
	//! Call do_request_with_auth. If it sets try_again to true,
	//! discard the response, and try again.

	response do_request(const fd *terminate_fd,
			    requestimpl &req,
			    request_sans_body &impl);

	//! Implement the request

	response do_request_redirected(const fd *terminate_fd,
				       requestimpl &req,
				       request_sans_body &impl)
		LIBCXX_INTERNAL;

	//! Implement the request

	response do_request_with_auth(const fd *terminate_fd,
				      requestimpl &req,
				      request_sans_body &impl)
		LIBCXX_INTERNAL;

	//! Process any authentication challenges.

	//! Returns true if there were authentication challenges, and they
	//! were all filled from the cache.

	bool process_challenges(const clientauth &authorizations,
				const requestimpl &req,
				const response &resp)
		LIBCXX_INTERNAL;

	//! Check the Authentication-Info and Proxy-Authentication-Info headers

	//! \internal

	void process_authentication_info(const response &resp,
					 const requestimpl &req,
					 const char *header,
					 std::map<std::string,
					 clientauthimpl> &authorizations)
		LIBCXX_INTERNAL;

	//! Find a connection for the given destination

	//! \internal
	//!
	idleconn findConn(const cache_key_t &key,
			  const fd *terminate_fd)
		LIBCXX_INTERNAL;

	//! Recycle an idle connection

	//! \internal
	void recycle(const cache_key_t &key,
		     const idleconn &conn)
		LIBCXX_INTERNAL;

	//! Initialize an http socket

	//! \internal
	//!
	idleconn init_http_socket(clientopts_t opts,
				  const std::string &host,
				  const fd &socket,
				  const fdptr &terminator)
		LIBCXX_HIDDEN;

	//! Initialize an https socket

	//! \internal
	//!

	idleconn init_https_socket(clientopts_t opts,
				   const std::string &host,
				   const fd &socket,
				   const fdptr &terminator)
		__attribute__((weak));

public:
	//! Return this user agent's cookie jar

	cookiejar jar() const;
};

#if 0
{
	{
#endif
	}
}


#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/http/useragentobj_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#endif
