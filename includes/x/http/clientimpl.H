/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_clientimpl_H
#define x_http_clientimpl_H

#include <x/http/receiverimpl.H>
#include <x/http/senderimpl.H>
#include <x/http/exception.H>
#include <iterator>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! An HTTP client implementation

//! This template class implements a generic HTTP client. The template
//! parameters are:
//!
//! - output iterator type. The output iterator writes a byte stream to the
//! HTTP server.
//!
//! - input iterator type. An input sequence defined by a beginning and an
//! ending input iterator defines the input byte stream from the HTTP
//! server.
//!
//! This class derives from receiverimpl and senderimpl.
//! An HTTP request gets sent by invoking send() and passing the following
//! parameters:
//!
//! - an HTTP request object. Note that this object is mutable. send()
//! modifies some headers in the request.
//!
//! - an HTTP response object.
//!
//! - an optional beginning and ending input iterator representing the body
//! of the HTTP request message.
//!
//! When send() returns,
//! the request object's responseHasMessageBody() will indicate whether the
//! response has a message body. If so, use begin() and end() to
//! to extract the response message's body.
//!
//! send() may be used to "try" to send another request to the server.
//! If send() returns \c false the message cannot be sent through the same
//! connection. Either the server is an HTTP 1.0 server, the message was an
//! HTTP 1.0 message, or either message has a "Connection: close" header.
//!
//! When the optional message body is specified with random access iterators,
//! the message body's length gets placed into the request message's
//! \c Content-Length: header, and the message body copied gets copied directly
//! to the output iterator to the HTTP server.
//!
//! If the message body is not specified by random access iterators, chunked
//! encoding gets used if the server support HTTP 1.1. Otherwise, the sequence
//! defined by the beginning and an ending iterator gets saved in a temporary
//! file, its size gets determined by the length of the file, placed into the
//! \c Content-Length: header, and the message body gets copied from the
//! temporary file
//!
//! Initially the HTTP server is presumed to be an HTTP 1.0 server, until the
//! server's response to the first request specifies its true version.
//! This does mean that non-random access iterators on the first request will
//! result in using an expensive intermediate temporary file.
//! After constructing the object, and before invoking send() for the first
//! time, setPeerHttp11() may be invoked if the server is known to support
//! HTTP 1.1. If so, the first message's body will be sent using chunked
//! encoding, if the body's beginning and ending iterators are not random
//! access iterators.

template<typename output_iter, typename input_iter>
class clientimpl : protected receiverimpl<responseimpl, input_iter>,
		   protected senderimpl<output_iter> {

	//! Flag - this is the first message to the server
	bool firstmsg;

	//! Flag - another message can be sent to the server.
	bool pipelined;

	//! Flag - whether the request should be formatted for a proxy
	bool proxyflag;

protected:
	//! The receiver superclass implementation type
	typedef receiverimpl<responseimpl, input_iter> receiver_t;

	//! The sender superclass implementation type
	typedef senderimpl<output_iter> sender_t;

public:
	using sender_t::setPeerHttp11;

	//! Import the iterator from the superclass
	typedef typename receiver_t::iterator iterator;

	using receiver_t::begin;
	using receiver_t::end;
	using receiver_t::discardbody;

	//! Constructor
	clientimpl(//! Maximum number of headers accepted in a message
		   size_t maxlimit,

		   //! Whether the given connection is to a proxy
		   bool proxyflagArg=false,

		   //! Output iterator
		   output_iter out_iter=output_iter(),

		   //! Beginning input iterator
		   input_iter iter=input_iter(),

		   //! Ending input iterator
		   input_iter iter_end=input_iter())
		: receiver_t(iter, iter_end, maxlimit),
		  sender_t(out_iter), firstmsg(true), pipelined(true),
		  proxyflag(proxyflagArg)
	{
	}

	//! Install input and output iterators

	void install(//! Whether the given connection is to a proxy
		     bool proxyflagArg,

		     //! New output iterator
		     output_iter out_iter,

		     //! Input iterator for a new HTTP message sequence
		     input_iter iterArg,

		     //! Ending input iterator
		     input_iter iter_endArg,

		     //! Maximum number of headers accepted in a message
		     size_t maxlimit)
	{
		receiver_t::install(iterArg, iter_endArg, maxlimit);
		sender_t::install(out_iter);
		pipelined=true;
		firstmsg=true;
		proxyflag=proxyflagArg;
	}

	//! Destructor
	~clientimpl()
	{
	}

protected:
	using receiver_t::check_body_status;

	//! Wrapper class for a request being sent

	//! \internal

	class sendmsg {
	public:
		//! The original request
		requestimpl &req;

		//! The sender superclass
		sender_t &sender;

		//! Constructor
		sendmsg(requestimpl &reqArg,
			sender_t &senderArg)
			: req(reqArg), sender(senderArg)
		{
		}

		//! Destructor
		~sendmsg()
		{
		}

		//! Send the request
		virtual void send()
		{
			sender.send(req);
		}
	};

private:
	//! Wrapper class for a request being sent with a body

	//! \internal

	template<typename iter_type> class sendmsgbody : public sendmsg {
	public:
		//! Beginning iterator.
		iter_type beg_iter;

		//! Ending iterator
		iter_type end_iter;

		//! Check whether the body should be sent
		senderimpl_encode::wait_continue &do_sendbody;

		//! Constructor
		sendmsgbody(requestimpl &reqArg,
			    const iter_type &begArg,
			    const iter_type &endArg,
			    sender_t &senderArg,
			    senderimpl_encode::wait_continue &do_sendbodyArg)

			: sendmsg(reqArg, senderArg),
			  beg_iter(begArg),
			  end_iter(endArg),
			  do_sendbody(do_sendbodyArg)
		{
		}

		//! Destructor
		~sendmsgbody()
		{
		}

		//! Send the request and the body
		virtual void send() override
		{
			this->sender.send(this->req, beg_iter, end_iter,
					  do_sendbody);
		}
	};

	//! Sanity checking on the request

	void validate_request(requestimpl &req)
	{
		discardbody(); // From the previous request

		const char *host_header="Host";

		const uriimpl &uri=req.getURI();
		const uriimpl::authority_t &auth=uri.getAuthority();

		messageimpl::keywordseti_t expectations;

		req.getCommaValuesToSet(messageimpl::expect, expectations);

		if (// This is for our internal use:
		    req.find(messageimpl::expect_100_continue) != req.end()

		    // An absolute URL is always required
		    || uri.getScheme().size() == 0
		    || auth.hostport.size() == 0
		    || req.find(host_header) != req.end())
			responseimpl::throw_bad_request();

		req.setProxyFormat(proxyflag);
	}

public:
	//! Check whether this client connection is no longer available

	//! HTTP/1.1 servers have optional pipelining, and may accept multiple
	//! message. In that case, available() returns \c true after processing
	//! a request. In all other cases, available() will return \c false.
	//!
	//! \note
	//! available() may only be called after completely retrieving any
	//! content of the request. available() drains any unretrieved
	//! body content, and terminates an HTTPS connection, if applicable.

	bool available() noexcept
	{
		discardbody();

		if (!pipelined)
			disconnect();

		return pipelined;
	}

	//! \return \c true if the request was sent and a response received,
	//! \c false if this request could not be sent. This may happen on a
	//! second request to an HTTP 1.0 server, or if the HTTP 1.1 server
	//! closed the pipelined connection.

	bool send(requestimpl &req,
		  responseimpl &resp)
	{
		if (!available())
			return false;

		validate_request(req);

		if (req.shouldHaveMessageBody())
			senderimpl_encode::expected_message_body();

		pipelined=false;

		bool isfirstmsg=firstmsg;

		firstmsg=false;

		try {
			{
				sendmsg msg(req, *this);
				internal_send(msg);
			}
			internal_recv(resp, req);
		} catch (...)
		{
			if (isfirstmsg)
				throw;
			pipelined=false;
			disconnect();
			return false;
		}
		return true;
	}

private:

	//! Subclass hook -- connection can be terminated

	//! \internal
	//! The connection is expected to be terminated. The subclass can
	//! perform any necessary cleanup. The TLS-based subclass performs
	//! a TLS session close.

	virtual void disconnect()
	{
	}

	//! Wait for the server to reply to the "Expect: 100-continue" header

	class do_continue_impl : public sender_t::wait_continue {

		//! The receiver of messages to the server.
		receiver_t &receiver;

		//! Original request message
		const requestimpl &req;

		//! The response received from the server
		responseimpl &resp;

		//! Whether the server agreed to continue
		bool continued_flag;

	public:
		//! Constructor
		do_continue_impl(//! Receiver
				 receiver_t &receiverArg,

				 //! The original request
				 const requestimpl &reqArg,

				 //! The response from the server
				 responseimpl &respArg)
			: receiver(receiverArg),
			  req(reqArg), resp(respArg),
			  continued_flag(false)
		{
		}

		//! Destructor
		~do_continue_impl()
		{
		}

		//! Check whether the server replied with a 100 message

		bool continued() noexcept
		{
			return continued_flag;
		}

		//! Wait for the server to response to the Expect: 100-continue message

		bool operator()() override
		{
			receiver.message(resp, req);
			if (resp.getStatusCode() != 100)
				return false;
			continued_flag=true;
			return true;
		}
	};

public:
	//! Send a request with a body, wait for a response

	//! \return \c true if the request was sent and a response received,
	//! \c false if this request could not be sent. This may happen on a
	//! second request to an HTTP 1.0 server, or if the HTTP 1.1 server
	//! closed the pipelined connection.

	template<typename iter_type>
	bool send(requestimpl &req,
		  iter_type beg_iter,
		  iter_type end_iter,
		  responseimpl &resp)
	{
		if (!pipelined)
			return false;

		validate_request(req);

		if (!req.shouldHaveMessageBody())
			senderimpl_encode::unexpected_message_body();

		pipelined=false;

		bool isfirstmsg=firstmsg;

		firstmsg=false;

		messageimpl::iterator expect_continue_header_iter;

		typename sender_t::wait_continue always_continue;
		do_continue_impl check_continue(*this, req, resp);

		if (!isfirstmsg)
		{
			expect_continue_header_iter=
				req.append(messageimpl::expect,
					   messageimpl::expect_100_continue);
		}

		try
		{
			sendmsgbody<iter_type> msg(req, beg_iter, end_iter,
						   *this,
						   isfirstmsg ? always_continue
						   : check_continue);
			internal_send(msg);
		} catch (...)
		{
			if (!isfirstmsg)
				req.erase(expect_continue_header_iter);
			if (!isfirstmsg && !check_continue.continued())
				return false;
			throw;
		}
		if (!isfirstmsg)
		{
			req.erase(expect_continue_header_iter);

			if (!check_continue.continued())
				return true;
		}

		internal_recv(resp, req);
		return true;
	}

protected:
	//! Hook for subclass to wrap sending the request

	virtual void internal_send(sendmsg &msg)
	{
		msg.send();
	}

	//! Hook for subclass to wrap receiving a response

	//! \internal
	//! If both the request and a response was an HTTP/1.1 message, and
	//! there was no Connection: close header in either the request or
	//! response, then set the pipelined flag to indicate another request
	//! may be sent.

	virtual void internal_recv(responseimpl &resp,
				   const requestimpl &req)
	{
		receiver_t::message(resp, req);

		this->setPeerHttpVersion(resp.getVersion());

		pipelined=req.getVersion() == httpver_t::http10 ||
			resp.getVersion() == httpver_t::http10 ||
			req.connection_closed() || resp.connection_closed()
			? false:true;
	}
};

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/http/clientimpl_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
	{
#endif
	}
}
#endif
