/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_serverimpl_H
#define x_http_serverimpl_H

#include <x/http/messageimplfwd.H>
#include <x/http/receiverimpl.H>
#include <x/http/senderimpl.H>
#include <x/http/exception.H>
#include <x/mime/structured_content_header.H>
#include <x/http/form.H>
#include <iterator>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! An HTTP server implementation

//! This template class implements a generic HTTP server loop. This class
//! derives from receiverimpl and senderimpl. The default implementation
//! sends a 404 message in response to any request. The subclass is
//! responsible for implementing received() that does something useful.
//!
//! The template class takes an input iterator parameter and an output
//! iterator parameter.
//! HTTP messages are read from an input sequence defined by a
//! beginning and an ending iterator which are passed to the constructor.
//! The constructor also takes an output iterator where replies are sent.
//! run() is the main loop that reads HTTP messages from the
//! input sequence, and invokes received()
//!
//! The subclass must define received() to process each message. The subclass
//! may also override the following methods:
//!
//! - eof() checks whether the end of the HTTP messages have been reached.
//! The default implementation compares the input iterator with the ending
//! input iterator value.
//!
//! - message() retrieve an HTTP message from the input sequence and initializes
//! a requestimpl object
//!
//! \par Methods available to the subclass
//!
//! The subclass()'s received() method may also invoke the following methods
//! that it inherits:
//!
//! - install() to reset the input iterators and the output iterator.
//!
//! - report_exception() takes a thrown %exception and writes it out as a reply
//! to a message.
//!
//! - send() must be invoked by subclass's received() method. Alternatively
//! report_exception() must be invoked which throws an %exception. Either
//! approach sends a
//! reply to the received HTTP message. If received() throws a
//! responseimpl::response_exception, the main run() loop will invoke
//! report_exception() on its behalf.

template<typename input_iter, typename output_iter>
class serverimpl : protected receiverimpl<requestimpl, input_iter>,
		   protected senderimpl<output_iter> {

protected:
	//! The receiver superclass implementation type
	typedef receiverimpl<requestimpl, input_iter> receiver_t;

	//! The sender superclass implementation type
	typedef senderimpl<output_iter> sender_t;
public:
	//! Constructor
	serverimpl(//! Maximum number of headers accepted in a message
		   size_t maxlimit,

		   //! Beginning input iterator
		   input_iter iter=input_iter(),

		   //! Ending input iterator
		   input_iter iter_end=input_iter(),

		   //! Output iterator
		   output_iter out_iter=output_iter())
		: receiver_t(iter, iter_end, maxlimit),
		  sender_t(out_iter)
	{
	}

	//! Destructor
	~serverimpl() noexcept
	{
	}

	//! Install input and output iterators

	void install(//! Input iterator for a new HTTP message sequence
		     input_iter iterArg,

		     //! Ending input iterator
		     input_iter iter_endArg,

		     //! New output iterator
		     output_iter out_iter,

		     //! Maximum number of headers accepted in a message
		     size_t maxlimit)
	{
		receiver_t::install(iterArg, iter_endArg, maxlimit);
		sender_t::install(out_iter);
	}

	//! Check whether the input sequence has been reached

	virtual bool eof()
	{
		return receiver_t::iter == receiver_t::iter_end;
	}

	//! Import the iterator from the superclass
	typedef typename receiver_t::iterator iterator;
	using receiver_t::begin;
	using receiver_t::end;

protected:
	using receiver_t::discardbody;
	using receiver_t::check_body_status;
	using receiver_t::body_begin;
	using receiver_t::body_end;
	using receiver_t::message;

private:
	//! Flag - the response to the request was sent
	bool response_sent;

	//! Flag - the connection to the client should be closed now

	bool connection_closed;

	//! Register a response sent to a request

	void register_response(//! The response
			       responseimpl &resp,

			       //! The response is to this request
			       const requestimpl &req)
	{
		if (response_sent)
			responseimpl::throw_internal_server_error();

		response_sent=true;
		connection_closed=resp.getVersion() == httpver_t::http10 ||
			resp.connection_closed() ||
			req.getVersion() == httpver_t::http10 ||
			req.connection_closed();

		if (connection_closed && !resp.connection_closed())
			resp.append("Connection: close");
	}

protected:
	//! Send a response to the client

	//! The superclass's received() method should invoke send()
	//! to return a response to the sender.
	//!
	//! The superclass has prepared an entire responseimpl object.

	template<typename... Args_t>
	void send(//! The prepared responseimpl object

		  //! \note
		  //! send() may modify this object, for example to add the
		  //! "Connection: close" header.
		  responseimpl &resp,

		  //! The original request

		  const requestimpl &req,

		  //! Optional body
		  Args_t && ...args)
	{
		do_send(resp, req, std::forward<Args_t>(args)...);
	}

	//! Send a response to the client, of the given content-type

	//! \overload

	template<typename... Args_t>
	void send(//! Original request
		  const requestimpl &req,

		  //! The content-type of the response
		  const std::string &type,

		  //! Optional headers, original request, body
		  Args_t && ...args)
	{
		responseimpl resp;

		get_send_headers(resp, mime::structured_content_header::content_type,
				 type, req, std::forward<Args_t>(args)...);
	}

	//! Convenience function -- send a canned reply

	void send(//! Original request

		  const requestimpl &req,

		  //! The HTTP status code

		  int statusCode,

		  //! The reason phrase -- also the message body, in UTF-8.

		  const std::string &reasonPhrase)
	{
		responseimpl resp(statusCode, reasonPhrase);

		get_send_headers(resp, mime::structured_content_header::content_type,
				 "text/plain; charset=\"utf-8\"", req,
				 reasonPhrase.begin(), reasonPhrase.end());
	}

	//! Format an http exception

	void send(//! The error occured in response to this request
		  const requestimpl &req,

		  //! The exception
		  const response_exception &exception)
	{
		responseimpl resp=exception;

		if (req.responseHasMessageBody(resp))
		{
			std::string contents=exception.body;

			this->send(resp, req, contents.begin(), contents.end());
		}
		else
		{
			this->send(resp, req);
		}
	}

	//! Parse form parameters from a received request

	//! \return A pair consisting of form parameters, and boolean flag.
	//! The boolean flag contains \c true if the received request was a
	//! POSTed form, \c false otherwise. Even if it's not, the form
	//! parameters may still contain parameters from the URI's query string.

	std::pair<form::parameters, bool>
	getform(//! The current received request
		const requestimpl &req,

		//! Whether request body has been supplied
		bool hasbody)
	{
		form::parameters params=form::parameters
			::create(static_cast<receiver_t &>(*this), req,
				 hasbody);

		return std::make_pair(params, params->consumed());
	}

	//! Parse form parameters from a received request

	//! \overload

	template<typename factory_type>
	std::pair<form::parameters, bool>
	getform(//! The current received request
		const requestimpl &req,

		//! Whether request body has been supplied
		bool hasbody,

		//! Factory for receiving files
		factory_type &&factoryArg,

		//! Form's encoding
		const std::string &form_encoding)
	{
		form::parameters params=form::parameters
			::create(static_cast<receiver_t &>(*this), req,
				 hasbody,
				 std::forward<factory_type>(factoryArg),
				 form_encoding);

		return std::make_pair(params, params->consumed());
	}

private:

	//! Collect any extra headers for the response

	//! \internal
	template<typename... Args_t>
	void get_send_headers(responseimpl &resp,
			      const std::string &headername,
			      const std::string &headervalue,
			      Args_t && ...args)
	{
		resp.append(headername, headervalue);
		get_send_headers(resp, std::forward<Args_t>(args)...);
	}

	//! End of extra headers

	//! \internal
	template<typename... Args_t>
	void get_send_headers(responseimpl &resp,
			      const requestimpl &req,
			      Args_t && ...args)
	{
		do_send(resp, req, std::forward<Args_t>(args)...);
	}

	//! Send a response without a body

	void do_send(responseimpl &resp,
		     const requestimpl &req)
	{
		discardbody();
		begin_message_send();

		try {
			register_response(resp, req);
			sender_t::send(resp, req);
			flush_message_send();
		} catch (const exception &e)
		{
			end_message_send();
			throw;
		}
		end_message_send();
	}

	//! Send a response with a body defined by a container

	template<typename container_type>
	void do_send(responseimpl &resp,
		     const requestimpl &req,
		     const container_type &container)
	{
		do_send(resp, req, container.begin(), container.end());
	}

	//! Send a response with a body defined by iterators

	template<typename iter_type>
	void do_send(responseimpl &resp,
		     const requestimpl &req,
		     iter_type beg_iter,
		     iter_type end_iter)
	{
		discardbody();
		begin_message_send();

		try {
			typename sender_t::wait_continue never;

			register_response(resp, req);
			sender_t::send(resp, req, beg_iter, end_iter, never);
			flush_message_send();
		} catch (const exception &e)
		{
			end_message_send();
			throw;
		}
		end_message_send();
	}
public:
	//! Main loop

	//! This main loop continues to receive HTTP messages from the input
	//! sequence until one of the following conditions occurs:
	//!
	//! - the end of the input sequence has been reached, the client
	//! has disconnected.
	//!
	//! - an HTTP 1.0 message has been received from the client, and
	//! processed
	//!
	//! - an HTTP 1.1 message with a "Connection: close" has been received
	//! from the client, and processed
	//!
	//! - an HTTP 1.0 response has been sent to the client as a response.
	//!
	//! - an HTTP 1.1 response with a "Connection: close" has been sent to
	//! the client as a response.

	virtual void run()
	{
		connection_closed=false;

		while (!connection_closed && !eof())
		{
			requestimpl req;

			bool hasBody=this->message(req);

			response_sent=false;
			connection_closed=true;

			this->setPeerHttpVersion(req.getVersion());

			try {
				bool got_100_continue=false;

				if (req.getVersion() == httpver_t::http11)
				{
					messageimpl::keywordseti_t expectations;

					req.getCommaValuesToSet
						(messageimpl::expect,
						 expectations);

					for (messageimpl::keywordseti_t
						     ::const_iterator
						     b(expectations.begin()),
						     e(expectations.end());
					     b != e; ++b)
					{
						if (chrcasecmp::str_equal_to()
						    (*b, messageimpl::
						     expect_100_continue))
						{
							got_100_continue=true;
							continue;
						}
						responseimpl::
							throw_expectation_failed
							();
					}
				}

				if (got_100_continue)
				{
					responseimpl resp;

					resp.setStatusCode(100);
					sender_t::send(resp, req);
				}

				this->received(req, hasBody);

				if (!response_sent)
					responseimpl::throw_internal_server_error();
			} catch (const response_exception &e)
			{
				report_exception(req, e);
			}
		}
	}

protected:
	//! Process an HTTP message, must be defined by the subclass.

	//! The subclass must do one of the following:
	//!
	//! - invoke send() to send a reply to the server exactly once, or
	//!
	//! - invoke report_exception() to send a reply formatted as an HTTP
	//! error.
	//!
	//! - an alternative to invoking report_exception() is to just throw
	//! a response_exception

	virtual void received(//! The received HTTP request
			      const requestimpl &req,

			      //! This HTTP request contains a body.

			      //! The subclass should invoke begin() and end()
			      //!  receive the body part of the request.
			      bool bodyflag)
	{
		received_unknown_request(req, bodyflag);
	}

	//! Unknown request received by the subclass

	//! The subclass can call this method to properly dispose of a request
	//! that it does not know about, based solely on the contents of the
	//! \ref requestimpl "INSERT_LIBX_NAMESPACE::http:;requestimpl"
	//! object.
	//!
	//! This method throws an exception that gets caught by the
	//! main server loop.

	void received_unknown_request(const requestimpl &req,
				      bool bodyflag)
	{
		if (bodyflag)
			this->discardbody();
		responseimpl::throw_not_found();
	}

	//! Format a thrown exception as an HTTP error and send it to the client as a response

	//! If a response was already sent, the exception gets rethrown.
	//!
	virtual void report_exception(//! The message that caused the exception
				      const requestimpl &req,

				      //! A thrown exception
				      const response_exception &e)

	{
		if (response_sent
		    || e.getStatusCode() == 408) // Propagate timeout exceptions
			throw e;

		response_exception cpy(e);
		send(req, cpy);
	}

private:
	//! Stub - begin sending a message

	//! \internal
	//!
	virtual void begin_message_send()
	{
	}

	//! Stub - response has been sent, flush any output buffer

	//! \internal
	//!

	virtual void flush_message_send()
	{
	}

	//! Stub - check for a timeout after sending a message

	//! \internal
	//!

	virtual void end_message_send()
	{
	}
};

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/http/serverimpl_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
	{
#endif
	}
}
#endif
