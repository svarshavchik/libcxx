/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_senderimpl_H
#define x_http_senderimpl_H

#include <iterator>
#include <sstream>
#include <iomanip>
#include <x/http/requestimpl.H>
#include <x/http/responseimpl.H>
#include <x/http/discardoutput.H>
#include <x/property_value.H>
#include <x/fd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! Classes for encoding message bodies

//! \internal

namespace senderimpl_encode {

	//! Client callback: ok to proceed with sending the body of the message.

	//! clientimpl subclasses and implements this class
	class wait_continue {

	public:
		//! Constructor
		wait_continue();

		//! Destructor
		~wait_continue();

		//! Whether to proceed and send the body of this message

		//! If \c false gets returned, send() returns immediately,
		//! otherwise it proceeds to send the message.

		virtual bool operator()();
	};

	//! Specified a body on an HTTP message that shouldn't have one.

	//! \internal
	//!
	void unexpected_message_body()
		__attribute__((noreturn));

	//! Did not specify a body on an HTTP message that should have one.

	//! \internal
	//!
	void expected_message_body()
		__attribute__((noreturn));

	//! Size of message body chunks.

	extern property::value<size_t> chunksize;

	//! The size of a message body is known, use a Content-Length: header

	class content_length {

	public:
		//! Encode a message and message body

		//! \return the output iterator after encoding the message.

		template<typename output_iter,
			 typename input_iter, typename int_type,
			 typename req_type>
		static output_iter send(//! The HTTP message.

					//! An appropriate Content-Length:
					//! header gets added to this message
					//! before it gets encoded.

					req_type &req,

					//! Output iterator
					output_iter iter,

					//! Input iterator for a sequence that defines the message body.

					input_iter beg_iter,

					//! Ending input iterator for a sequence that defines the message body.

					input_iter end_iter,

					//! Number of octets in the sequence.

					int_type length,

					//! Callback - ok to proceed and send the body
					wait_continue &do_sendbody,

					//! Flag - message body is expected
					bool body_expected)

		{
			// Just to be sure:
			req.erase(messageimpl::transfer_encoding);

			std::ostringstream o;

			o << length;
			req.replace(messageimpl::content_length, o.str());

			iter=req.to_string(iter);

			if (body_expected && do_sendbody())
			{
				while (length)
				{
					if (beg_iter == end_iter)
						responseimpl::throw_bad_request();

					*iter++ = *beg_iter++;
					--length;
				}
			}
			return iter;
		}
	};

	//! Encode an HTTP request message for HTTP 1.0

	//! The message's body gets written into a temporary file in order
	//! to obtain its byte count, which gets placed in the Content-Length:
	//! header.

	template<typename iterator_type>
	class http10_request_encoding {

	public:
		//! Encode a message and message body

		//! \return the output iterator after encoding the message.
		//!
		//! The input sequence gets saved in a temporary file, then
		//! saved byte count gets placed in the Content-Length: header,
		//! then the message and the message body is written to the
		//! output iterator.

		template<typename output_iter, typename input_iter,
			 typename req_type>
		static output_iter send(//! An HTTP request
					req_type &req,

					//! The output iterator
					output_iter iter,

					//! Message body beginning input iterator
					input_iter beg_iter,

					//! Message body ending input iterator
					input_iter end_iter,

					//! Callback - ok to proceed and send the body
					wait_continue &do_sendbody)

		{
			fd t(fd::base::tmpfile());

			iostream o(t->getiostream());

			std::copy(beg_iter, end_iter,
				  std::ostreambuf_iterator<char>(*o));

			o->seekg(0);

			if (o->fail())
				requestimpl::tmpfile_error();

			iter=content_length::send(req, iter,
						  std::istreambuf_iterator<char> (*o),
						  std::istreambuf_iterator<char>(),
						  t->stat().st_size,
						  do_sendbody, true);
			if (o->fail())
				requestimpl::tmpfile_error();
			return iter;
		}
	};

	//! Encode an HTTP response message for HTTP 1.0

	//! The message's body gets written directly to the output sequence.
	//! The caller is expected to close the output sequence immediately
	//! after sending the message.

	template<typename iterator_type>
	class http10_response_encoding {

	public:
		//! Encode a message and message body

		//! \return the output iterator after encoding the message.
		//!

		template<typename output_iter, typename input_iter,
			 typename req_type>
		static output_iter send(//! An HTTP response
					req_type &resp,

					//! The output iterator
					output_iter iter,

					//! Message body beginning input iterator
					input_iter beg_iter,

					//! Message body ending input iterator
					input_iter end_iter,

					//! Callback - ok to proceed and send the body
					wait_continue &do_sendbody,

					//! Flag - message body is expected
					bool body_expected)

		{
			// Just to be sure:
			resp.erase(messageimpl::transfer_encoding);
			resp.erase(messageimpl::content_length);

			iter=resp.to_string(iter);
			if (body_expected && do_sendbody())
				iter=std::copy(beg_iter, end_iter, iter);
			return iter;
		}
	};

	//! Encode an HTTP message for HTTP/1.1 clients

	//! The message body is defined using a beginning and ending input
	//! iterator. The template parameter is the input iterator type.
	//!
	//! The message body gets encoded using chunked encoding.

	template<typename iterator_type>
	class http11_encoding {

	public:
		//! Encode a message and message body using chunked encoding

		//! \return the output iterator after encoding the message.
		//!

		template<typename output_iter, typename input_iter,
			 typename req_type>
		static output_iter send(//! An HTTP request or response

					//! A Transfer-Encoding: header gets
					//! added to the message.
					req_type &req,

					//! Output iterator
					output_iter iter,

					//! Message body input iterator start
					input_iter beg_iter,

					//! Message body input iterator end
					input_iter end_iter,

					//! Callback - ok to proceed and send the body
					wait_continue &do_sendbody,

					//! Flag - message body is expected
					bool body_expected)

		{
			req.replace(messageimpl::transfer_encoding,
				    messageimpl::transfer_encoding_chunked);

			req.erase(messageimpl::content_length); // Just in case

			iter=req.to_string(iter);

			if (!body_expected || !do_sendbody())
				return iter;

			size_t n=chunksize.get();

			if (n <= 0)
				n=1; // Sanity check

			std::vector<char> chunkbuf;

			chunkbuf.resize(n);

			size_t i=0;

			while (1)
			{
				if (beg_iter == end_iter || i >= n)
				{
					if (i)
					{
						std::ostringstream o;

						o << std::hex << i
						  << "\r\n";
						std::string s(o.str());

						iter=std::copy(&chunkbuf[0],
							       &chunkbuf[0]+i,
							       std::copy
							       (s.begin(),
								s.end(), iter)
							       );
						*iter++='\r';
						*iter++='\n';
						i=0;
					}

					if (beg_iter == end_iter)
						break;
				}

				chunkbuf[i++]=*beg_iter++;
			}
			for (const char *p="0\r\n\r\n"; *p; p++)
				*iter++ = *p;
			return iter;
		}
	};

	//! Encode an HTTP request message for HTTP 1.0

	//! \return the output iterator after encoding the message.
	//! This specialization for forward iterators obtains the
	//! Content-Length: by using std::distance().

	template<>
	class http10_request_encoding<std::forward_iterator_tag> {
	public:

		//! Encode a message and message body

		//! \return the output iterator after encoding the message.

		template<typename output_iter, typename input_iter,
			 typename req_type>
		static output_iter send(//! An HTTP request or reply.
					req_type &req,

					//! The output iterator
					output_iter iter,

					//! Message body beginning input iterator
					input_iter beg_iter,

					//! Message body ending input iterator
					input_iter end_iter,

					//! Callback - ok to proceed and send the body
					wait_continue &do_sendbody)

		{
			return content_length::send(req, iter,
						    beg_iter, end_iter,
						    std::distance(beg_iter,
								  end_iter),
						    do_sendbody, true);
		}
	};

	//! Encode an HTTP request message for HTTP 1.0

	//! Bidirectional iterators can use the optimized path for forward
	//! iterators

	template<>
	class http10_request_encoding<std::bidirectional_iterator_tag>
		: public http10_request_encoding<std::forward_iterator_tag>
	{
	};

	//! Encode an HTTP request message for HTTP 1.0

	//! Random access iterators can use the optimized path for forward
	//! iterators

	template<>
	class http10_request_encoding<std::random_access_iterator_tag>
		: public http10_request_encoding<std::forward_iterator_tag>
	{
	};

	//! Encode an HTTP response message for HTTP 1.0

	//! \return the output iterator after encoding the message.
	//! This specialization for forward iterators obtains the
	//! Content-Length: by using std::distance().

	template<>
	class http10_response_encoding<std::random_access_iterator_tag> {
	public:

		//! Encode a message and message body

		//! \return the output iterator after encoding the message.

		template<typename output_iter, typename input_iter,
			 typename req_type>
		static output_iter send(//! An HTTP request or reply.
					req_type &req,

					//! The output iterator
					output_iter iter,

					//! Message body beginning input iterator
					input_iter beg_iter,

					//! Message body ending input iterator
					input_iter end_iter,

					//! Callback - ok to proceed and send the body
					wait_continue &do_sendbody,

					//! Flag - message body is expected
					bool body_expected)

		{
			return content_length::send(req, iter,
						    beg_iter, end_iter,
						    std::distance(beg_iter,
								  end_iter),
						    do_sendbody,
						    body_expected);
		}
	};

	//! Encode an HTTP message for HTTP/1.1 clients

	//! This specialization for random access iterator foregoes
	//! chunked encoding. The number of bytes in the message body is
	//! known, so the Content-Length header gets used instead.

	template<>
	class http11_encoding<std::random_access_iterator_tag> {
	public:

		//! Encode a message and message body

		//! \return the output iterator after encoding the message.

		template<typename output_iter, typename input_iter,
			 typename req_type>
		static output_iter send(//! An HTTP request or reply.
					req_type &req,

					//! The output iterator
					output_iter iter,

					//! Message body beginning input iterator
					input_iter beg_iter,

					//! Message body ending input iterator
					input_iter end_iter,

					//! Callback - ok to proceed and send the body
					wait_continue &do_sendbody,

					//! Flag - message body is expected
					bool body_expected)

		{
			return content_length::send(req, iter,
						    beg_iter, end_iter,
						    end_iter-beg_iter,
						    do_sendbody,
						    body_expected);
		}
	};
}

//! Write an HTTP message and optional body to an output iterator

//! The message and message body get sequenced to an output iterator. The
//! parameter to this template class is the output iterator type.
//!
//! The constructor takes an output iterator. Invoke send_request() to
//! sequence an HTTP message, and an optional message body, to the output
//! iterator.

template<typename output_iter>
class senderimpl {

	//! Invoke begin_write_message and end_write_message;

	class write_message_scope {

		//! The sender object that's writing HTTP messages

		senderimpl<output_iter> &sender;

		//! Whether end_write_message() has been invoked.

		bool done_flag;

	public:
		//! Constructor
		write_message_scope(senderimpl<output_iter> &senderArg)
			: sender(senderArg), done_flag(false)
		{
			sender.begin_write_message();
		}

		//! Destructor
		~write_message_scope()
		{
			try {
				done();
			} catch (...)
			{
			}
		}

		//! Finished writing the message
		void done()
		{
			if (done_flag)
				return;

			done_flag=true;
			sender.end_write_message();
		}

	};

protected:
	//! Current output iterator
	output_iter iter;

	//! Alias -- whether it's ok to proceed with the body

	typedef typename senderimpl_encode::wait_continue wait_continue;

private:
	//! Peer's known HTTP version support

	httpver_t peerhttpver;

public:
	friend class write_message_scope;

	//! Constructor
	senderimpl(//! Output sequence for HTTP messages and bodyes
		   output_iter iterArg=output_iter())
		: iter(iterArg), peerhttpver(httpver_t::http10)
	{
	}

	//! Destructor
	~senderimpl()
	{
	}

	//! Install the output iterator

	void install(output_iter iterArg)
	{
		iter=iterArg;
		peerhttpver=httpver_t::http10;
	}

	//! Set peer's HTTP version

	void set_peer_http_version(httpver_t level)
	{
		peerhttpver=level;
	}

	//! Set the peer's HTTP version as HTTP 1.1

	void set_peer_http11()
	{
		set_peer_http_version(httpver_t::http11);
	}

	//! Return the current value of the output iterator
	operator output_iter() const
	{
		return iter;
	}

	//! Sequence an HTTP request message without a message body

	void send(//! The request.

		  //! Note that this object may get modified in order
		  //! to set or remove certain headers.
		  requestimpl &req)
	{
		if (req.should_have_message_body())
			senderimpl_encode::expected_message_body();
		sendimpl(req);
	}

	//! Sequence an HTTP request message with a message body

	//! The message body is specified by a beginning and an ending input
	//! iterator.
	//!
	//! If the input iterators are random access iterators, the
	//! exact byte count gets used to create the
	//! "Content-Length:" header and the message body
	//! is written directly to the output sequence.
	//!
	//! Otherwise, for \c httpver_t::http11 requests, a chunked
	//! sequence gets encodes the message body.
	//! For \c http10 request, the input sequence gets
	//! written into a temporary file. The temporary
	//! file's size goes into the "Content-Length:"
	//! header.
	//!
	//! \note An %exception may get thrown if an error occurs while encoding
	//! the message and the body.

	template<typename input_iter>
	void send(//! The request.

		  //! Note that this object may get modified in order
		  //! to set or remove certain headers.
		  requestimpl &req,

		  //! The message body's beginning input iterator
		  input_iter beg_iter,

		  //! The message body's ending iterator
		  input_iter end_iter,

		  //! Whether it's ok to proceed with the body of the request
		  wait_continue &do_sendbody)
	{
		if (!req.should_have_message_body())
			senderimpl_encode::unexpected_message_body();

		write_message_scope writing(*this);

		if (req.get_version() == httpver_t::http10 ||
		    peerhttpver == httpver_t::http10)
		{
			iter=senderimpl_encode::http10_request_encoding
				<typename std::iterator_traits<input_iter>
				 ::iterator_category>::
				send(req, iter, beg_iter, end_iter,
				     do_sendbody);
		}
		else
		{
			iter=senderimpl_encode::http11_encoding
				<typename std::iterator_traits<input_iter>
				 ::iterator_category>::
				send(req, iter, beg_iter, end_iter,
				     do_sendbody, true);
		}
		writing.done();
	}

	//! Sequence an HTTP response message without a message body

	void send(//! The response message.

		  //! Note that this object may get modified in order
		  //! to set or remove certain headers.
		  responseimpl &resp,

		  //! Original request
		  const requestimpl &req)
	{
		if (req.response_has_message_body(resp))
			senderimpl_encode::expected_message_body();

		sendimpl(resp);
	}

	//! Sequence an HTTP response message with a message body

	//! The message body is specified by a beginning and an ending input
	//! iterator.
	//!
	//! If the input iterators are random access iterators, the
	//! exact byte count sets the "Content-Length:" header and the
	//! message body gets written directly to the output sequence.
	//!
	//! Otherwise, for \c httpver_t::http11 responses, a chunked
	//! sequence encodes the message body.
	//!
	//! For \c httpver_t::http10 versions, since the connection is
	//! going to be closed, the body gets simply copied
	//! to the output sequence, and we expect the caller
	//! to proceed and close the connection when we're
	//! done.
	//!
	//! \note An %exception may get thrown if an error occurs while encoding
	//! the message and the body.

	template<typename input_iter>
	void send(//! The response message.

		  //! Note that this object may get modified in order
		  //! to set or remove certain headers.
		  responseimpl &resp,

		  //! Original request
		  const requestimpl &req,

		  //! The message body's beginning input iterator
		  input_iter beg_iter,

		  //! The message body's ending iterator
		  input_iter end_iter,

		  //! Whether it's ok to proceed with the body of the request

		  wait_continue &do_sendbody)
	{
		write_message_scope writing(*this);

		if (resp.get_version() == httpver_t::http10 ||
		    peerhttpver == httpver_t::http10)
		{
			iter=senderimpl_encode::http10_response_encoding
				<typename std::iterator_traits<input_iter>
				 ::iterator_category>::
				send(resp, iter, beg_iter, end_iter,
				     do_sendbody,
				     req.response_has_message_body(resp));
		}
		else
		{
			iter=senderimpl_encode::http11_encoding
				<typename std::iterator_traits<input_iter>
				 ::iterator_category>::
				send(resp, iter, beg_iter, end_iter,
				     do_sendbody,
				     req.response_has_message_body(resp));
		}
		writing.done();
	}

private:
	//! Sequence an HTTP message that does not have a message body

	//! \note An %exception may get thrown if an error occurs while encoding
	//! the message.

	template<typename req_type>
	void sendimpl(//! An HTTP request or response.

		      //! Note that this object may get modified in order
		      //! to set or remove certain headers.
		      req_type &req)
	{
		// Just to be sure:
		req.erase(messageimpl::content_length);
		req.erase(messageimpl::transfer_encoding);

		write_message_scope writing(*this);

		iter=req.to_string(iter);
		writing.done();
	}

protected:
	//! Subclass hook to set any timeouts.
	virtual void begin_write_message()
	{
	}

	//! Subclass hook to clear any timeouts
	virtual void end_write_message()
	{
	}
};

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/http/senderimpl_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
#endif
}
#endif
