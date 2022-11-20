/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_receiverimpl_H
#define x_http_receiverimpl_H

#include <sstream>
#include <iterator>
#include <x/getlinecrlf.H>
#include <x/http/responseimpl.H>
#include <x/http/requestimpl.H>
#include <x/http/discardoutput.H>
#include <x/fditer.H>
#include <x/namespace.h>

#include <stdint.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! Throw an exception: message stream is in a bad state

void throw_error_state() __attribute__((noreturn));

//! Returns
//! Base class for a receiverimpl specialization that parses HTTP messages from an input sequence.

//! This is a base class for parsing HTTP messages from an input sequence.
//! An HTTP message and HTTP message body get sequenced from an input iterator.
//! Two subclasses are specialized that implement processing of
//! \ref requestimpl "requestimpl"
//! and \ref responseimpl "responseimpl" messages:
//! receiverimpl<requestimpl, input_iter> and
//! receiverimpl<responseimpl, input_iter>.
//! The first parameter to this template subclass is either requestimpl or
//! responseimpl. The second
//! parameter to this template subclass is an input iterator type. The input
//! iterator defines a byte sequence from which HTTP messages and message bodies
//! get parsed out.
//!
//! The constructor takes a beginning and an ending input iterator that define
//! the sequence. message() parses out either an HTTP request or a response from
//! the input sequence. Once the message is parsed, invoke begin()
//! and end() to retrieve
//! the message body from the input sequence.
//!
//! This base class holds the input iterator and the ending input iterator,
//! and defines the begin() and end() methods that return the input sequence
//! for the body of the message. The specialized subclass implements message().

template<typename input_iter>
class receiverimplbase {

protected:
	//! Current input iterator
	input_iter iter;

	//! Ending input iterator
	input_iter iter_end;

	//! The current state of the receiver
	typedef enum {
		error_state,
		message_state,
		body_start_state,
		body_end_state
	} state_t;

	//! Current receiver state

	state_t state;

	//! Type of the message body that follows
	typedef enum {
		body_chunked,
		body_content_length,
		body_toeof
	} body_type_t;

	//! Upcoming body type

	body_type_t body_type;

	//! How many bytes given in the Content-Length: header
	uint64_t body_cnt;

	//! Maximum number of headers, set in the superclass. 0-unlimited

	size_t maxlimit;

public:
	class iterator;

	friend class iterator;

private:
	//! Current iterator that's iterating over the message body

	iterator *current_body_iterator;
public:

	//! Constructor
	receiverimplbase(//! Input iterator for the HTTP message sequence
			 input_iter iterArg,

			 //! Ending input iterator
			 input_iter iter_endArg,

			 //! Maximum number of headers in a message
			 size_t maxlimitArg)
		: iter(iterArg), iter_end(iter_endArg), state(message_state),
		  body_type(body_toeof), maxlimit(maxlimitArg),
		  current_body_iterator(NULL)// Shouldn't be needed
	{
	}

	//! Destructor
	~receiverimplbase()
	{
		delinkiter();
	}

	//! Install input iterators

	void install(//! Input iterator for the HTTP message sequence
		     input_iter iterArg,

		     //! Ending input iterator
		     input_iter iter_endArg,

		     //! Maximum number of headers in a message
		     size_t maxlimitArg)
	{
		delinkiter();

		iter=iterArg;
		iter_end=iter_endArg;
		state=message_state;
		maxlimit=maxlimitArg;
	}

	//! Retrieve the current value of the input iterator
	operator input_iter() const
	{
		return iter;
	}

	//! Message body iterator

	//! This input iterator gets returned by begin() and end() to iterate
	//! over the body of the message.

	class iterator {

		//! The receiver object this iterator is tied to

		mutable receiverimplbase<input_iter> *receiver;

	public:
		//! Iterator trait
		typedef std::input_iterator_tag iterator_category;

		//! Iterator trait
		typedef char value_type;

		//! Iterator trait
		typedef std::ptrdiff_t difference_type;

		//! Iterator trait
		typedef value_type *pointer;

		//! Iterator trait
		typedef value_type &reference;
		//! end() constructor

		iterator() : receiver(NULL)
		{
		}
	private:
		//! The begin() constructor
		iterator(receiverimplbase<input_iter> *receiverArg)

		{
			receiverArg->linkiter(this);
		}

		//! Buffer character for the postfix ++ operator

		char valueSave;

	public:
		friend class receiverimplbase<input_iter>;

		//! Public destructor

		//! The destructor discards any uniterated message body

		~iterator()
		{
			if (receiver)
				receiver->body_end_internal();
		}

		//! Compare two iterators

		//! Two ending iterators compare as equal. All others are
		//! not equal.

		bool operator==(const iterator &iter) const noexcept
		{
			return receiver == NULL && iter.receiver == NULL;
		}

		//! Compare two iterators

		//! Two ending iterators compare as equal. All others are
		//! not equal.

		bool operator!=(const iterator &iter) const noexcept
		{
			return !operator==(iter);
		}

		//! Copy constructor

		//! Only one non-ending iterator may exist. The other iterator
		//! gets invalidated (becomes equivalent to the ending iterator)
		//! and this iterator must be used to continue iterating over
		//! the message body.

		iterator(const iterator &iter)
			: receiver(NULL)
		{
			operator=(iter);
		}

		//! Assignment operator

		//! Only one non-ending iterator may exist. The other iterator
		//! gets invalidated (becomes equivalent to the ending iterator)
		//! and this iterator must be used to continue iterating over
		//! the message body.

		iterator &operator=(const iterator &iter)
		{
			if (&iter == this)
				return *this;

			if (receiver)
				receiver->body_end_internal();

			if (iter.receiver)
				iter.receiver->linkiter(this);

			return *this;
		}

		//! The * operator

		//! Returns the current character at the iterator position
		char operator*()
		{
			try {
				if (receiver)
					return *receiver->iter;
			} catch (...) {
				receiverimplbase<input_iter> *r=receiver;

				if (r)
				{
					r->delinkiter();
					r->body_end();
				}
				throw;
			}

			return 0;
		}

		//! Prefix ++ operator

		iterator &operator++()
		{
			if (receiver)
				try {
					receiver->advance();
				} catch (...)
				{
					receiverimplbase<input_iter>
						*r=receiver;

					if (r)
					{
						r->delinkiter();
						r->body_end();
					}
					throw;
				}
			return *this;
		}

		//! Postfix ++ operator
		const char *operator++(int)
		{
			valueSave=operator*();
			operator++();
			return &valueSave;
		}
	};

private:

	//! Deregister the iterator, making sure that the entire body gets consumed later
	void body_end_internal() noexcept
	{
		state=body_end_state;
		delinkiter();
	}

	//! Deregister the iterator to the current message body
	void delinkiter() noexcept
	{
		if (current_body_iterator)
		{
			current_body_iterator->receiver=NULL;
			current_body_iterator=NULL;
		}
	}

	//! Register an iterator to the current message body

	void linkiter(iterator *iter) noexcept
	{
		delinkiter();
		current_body_iterator=iter;
		iter->receiver=this;
	}

public:

	//! Return an input iterator for the body of the message.

	//! If message() returned \c true, return an input sequence iterator
	//! that retrieves the body of the message.
	//!
	//! Only one input sequence iterator can exist, begin() may be invoked
	//! only once per message body. An %exception gets thrown if the
	//! beginning iterator cannot be invoked.

	iterator begin()
	{
		if (state != body_start_state)
		{
			state=error_state;
			throw_error_state();
		}

		state=error_state;

		body_begin();

		try {
			switch (body_type) {
			case body_toeof:
				if (iter != iter_end)
					return this;
				break;
			case body_content_length:
				if (body_cnt)
				{
					if (iter == iter_end)
						responseimpl::throw_bad_request();

					return this;
				}
				break;
			case body_chunked:
				if (next_chunk())
					return this;
				break;
			}
		} catch (...) {
			state=error_state;
			delinkiter();
			body_end();
			throw;
		}

		state=error_state;
		delinkiter();
		body_end();
		state=message_state;

		return end();
	}

	//! Return an ending iterator for the message body

	iterator end()
	{
		return iterator();
	}

private:
	//! Number of bytes remaining in the current chunk

	uint64_t chunk_counter;

	//! Advance to the next character in the message body.

	void advance()
	{
		switch (body_type) {
		case body_toeof:
			if (++iter != iter_end)
				return;
			break;

		case body_content_length:
			++iter;

			if (--body_cnt > 0)
			{
				if (iter == iter_end)
					responseimpl::throw_bad_request();
				return;
			}
			break;

		case body_chunked:
			++iter;

			if (--chunk_counter > 0)
			{
				if (iter == iter_end)
					responseimpl::throw_bad_request();
				return;
			}

			if (iter == iter_end || *iter++ != '\r' ||
			    iter == iter_end || *iter++ != '\n')
				responseimpl::throw_bad_request();

			if (next_chunk())
				return;
			break;
		}

		delinkiter();
		body_end();
		state=message_state;
	}

	//! Advance to the next chunk in the input sequence

	//! \return \c false if the end of the message body has been reached,
	//! \c true if the next chunk is ready to be read.

	bool next_chunk()
	{
		std::string line;

		std::pair<input_iter, bool>
			res=getlinecrlf(iter, iter_end, line);

		if (!res.second)
			responseimpl::throw_bad_request();

		iter=res.first;

		std::istringstream i(line);

		chunk_counter=0;

		i >> std::hex >> chunk_counter;

		if (i.fail())
			responseimpl::throw_bad_request();

		if (chunk_counter > 0)
			return true;

		do
		{
			std::pair<input_iter, bool>
				res=getlinecrlf(iter, iter_end, line);

			if (!res.second)
				responseimpl::throw_bad_request();

			iter=res.first;
		} while (line.size());

		return false;
	}

protected:

	//! Hook for subclass to wrap header retrieval

	virtual void header_begin()
	{
	}

	//! Hook for subclass to wrap body retrieval

	virtual void body_begin()
	{
	}

	//! Hook for subclass to wrap body retrieval

	virtual void body_end()
	{
	}

protected:

	//! Did not finish iterating over the input sequence

	//! Finish the iteration now.
	//!
	virtual void check_body_status()
	{
		while (state == body_end_state)
			try {
				advance();
			} catch (...)
			{
				delinkiter();
				body_end();
				throw;
			}
	}
public:

	//! Discard the body of the current message

	void discardbody()
	{
		if (state == body_start_state)
			for (iterator b=begin(), e=end(); b != e; ++b)
				;

		check_body_status();

		if (state != message_state)
			responseimpl::throw_bad_request();
	}
};

//! Common functions used when parsing messages

//! \internal
//!
class receiverimplcommon {

public:

	//! Retrieve the contents of the Transfer-Encoding: header

	//! \return Whether the header was found.
	static
	bool get_transfer_encoding(//! A message
				   const messageimpl &msg,

				   //! Set to point to the Transfer-Encoding:
				   //! header.

				   messageimpl::const_iterator &p)
;

	//! We expect this Transfer-Encoding: header to contain "chunked"

	//! And since this is the only HTTP/1.1 encoding that's supported,
	//! an %exception gets thrown otherwise.

	static
	void is_chunked_encoding(messageimpl::const_iterator p)
;

	//! Retrieve the contents of the Content-Length: header

	//! \return Whether the header was found.

	static
	bool has_content_length(//! Message
				const messageimpl &msg,

				//! Parsed count gets placed here
				uint64_t &cnt);
};

template<typename msg_type, typename input_iter>
class receiverimpl;

//! Specialization for parsing HTTP request messages from an input sequence.

//! This specialization implements message() method which parses HTTP request
//! messages.
//!
//! Use message() to parse the next HTTP request message.
//! If message() returns \c true, invoke begin() and end() (inherited from
//! the superclass) to access the request message's body, if appropriate.

template<typename input_iter>
class receiverimpl<requestimpl, input_iter>
	: public receiverimplbase<input_iter> {

public:
	//! Constructor

	receiverimpl(//! Input iterator for the HTTP message sequence
		     input_iter iterArg,

		     //! Ending input iterator
		     input_iter iter_endArg,

		     //! Maximum number of headers in a message
		     size_t maxlimit)
		: receiverimplbase<input_iter>(iterArg, iter_endArg, maxlimit)
	{
	}

	//! Destructor
	~receiverimpl()
	{
	}


	//! Parse the next HTTP request in the input sequence

	//! \return \c true if the parsed request has a body, use begin()
	//! and end() to extract it; \c false otherwise.
	//!
	//! The presence of a body is determined according to section 4.3
	//! of RFC 2616: the presence of the Content-Length: or the
	//! Transfer-Encoding: header.

	virtual bool message(//! Parsed request message returned here
			     requestimpl &msg)
	{
		if (this->state == receiverimplbase<input_iter>::error_state)
			throw_error_state();

		if (this->state == receiverimplbase<input_iter>::body_start_state)
			this->discardbody();

		try {
			this->check_body_status();
		} catch (...) {
			this->state=receiverimplbase<input_iter>::error_state;
			throw;
		}

		this->header_begin();

		this->state=receiverimplbase<input_iter>::error_state;
		this->iter=msg.parse(this->iter, this->iter_end,
				     this->maxlimit);

		messageimpl::const_iterator p;

		if (receiverimplcommon::get_transfer_encoding(msg, p))
		{
			receiverimplcommon::is_chunked_encoding(p);
			this->body_type=receiverimplbase<input_iter>
				::body_chunked;
			this->state=receiverimplbase<input_iter>
				::body_start_state;
		}
		else if (receiverimplcommon::has_content_length(msg,
								this->body_cnt))
		{
			this->body_type=receiverimplbase<input_iter>
				::body_content_length;
			this->state=receiverimplbase<input_iter>
				::body_start_state;
		}
		else
		{
			this->state=receiverimplbase<input_iter>::message_state;
			this->body_end();
			return false;
		}
		return true;
	}
};

//! Specialization for parsing HTTP response messages

//! This specialization implements message() method which parses HTTP response
//! messages.
//!
//! Use message() to parse the next HTTP response message.
//! If message() returns \c true, invoke begin() and end() (inherited from
//! the superclass) to access the request message's body, if appropriate.

template<typename input_iter>
class receiverimpl<responseimpl, input_iter>
	: public receiverimplbase<input_iter> {

public:
	//! Constructor

	receiverimpl(//! Input iterator for the HTTP message sequence
		     input_iter iterArg,

		     //! Ending input iterator
		     input_iter iter_endArg,

		     //! Maximum number of headers in the message
		     size_t maxlimit)
		: receiverimplbase<input_iter>(iterArg, iter_endArg, maxlimit)
	{
	}

	//! Destructor
	~receiverimpl()
	{
	}


	//! Parse the next HTTP response in the input sequence

	//! \return \c true if the parsed request has a body, use begin() and
	//! end() to extract it; \c false otherwise.
	//!
	//! The presence of a body is determined according to section 4.3 and
	//! 4.4 of RFC 2616:
	//! A response to a HEAD request, or a 1xx, 204, or 304 responses
	//! do not have a message body. All other requests have a message body.
	//!
	//! A non-"identity" Transfer-Encoding: header indicates a chunked
	//! message body. Otherwise, Content-Length: gives the exact octet
	//! count extracted from the input sequence. Otherwise, the rest of the
	//! input sequence gets extracted.

	virtual bool message(//! The parsed response is placed here
			     responseimpl &msg,

			     //! This response comes as part of this request
			     const requestimpl &req)
	{
		if (this->state == receiverimplbase<input_iter>::error_state)
			throw_error_state();

		try {
			this->check_body_status();
		} catch (...) {
			this->state=receiverimplbase<input_iter>::error_state;
			throw;
		}

		if (this->state == receiverimplbase<input_iter>::body_start_state)
			this->discardbody();

		this->header_begin();
		this->state=receiverimplbase<input_iter>::error_state;
		internal_get_message(msg);

		if (!req.response_has_message_body(msg))
		{
			this->state=receiverimplbase<input_iter>::message_state;
			return false;
		}

		messageimpl::const_iterator p;

		if (receiverimplcommon::get_transfer_encoding(msg, p))
		{
			receiverimplcommon::is_chunked_encoding(p);
			this->body_type=receiverimplbase<input_iter>
				::body_chunked;
		}
		else
		{
			this->body_type=
				receiverimplcommon::
				has_content_length(msg, this->body_cnt)
				? receiverimplbase<input_iter>
				::body_content_length
				:
				receiverimplbase<input_iter>
				::body_toeof;
		}
		this->state=receiverimplbase<input_iter>::body_start_state;
		return true;
	}

protected:

	//! Retrieve a response message from the input stream

	//! This is overridden in
	//! \ref gnutls::http::fdtlsclientimpl "gnutls::http::fdtlsclientimpl".
	//! If it connects to an https proxy before
	//! sending the actual request,
	//! and the proxy fails the CONNECT message, the subclass returns the
	//! proxy response here.
	//!
	//! \internal

	virtual void internal_get_message(responseimpl &msg)

	{
		this->iter=msg.parse(this->iter, this->iter_end,
				     this->maxlimit);
	}
};


#ifndef DOXYGEN

extern template class
receiverimplbase<std::istreambuf_iterator<char> >;

extern template class
receiverimpl<requestimpl, std::istreambuf_iterator<char> >;

extern template class
receiverimpl<responseimpl, std::istreambuf_iterator<char> >;

extern template class receiverimplbase<fdinputiter>;

extern template class receiverimpl<requestimpl, fdinputiter>;

extern template class receiverimpl<responseimpl, fdinputiter>;

#endif


#if 0
{
#endif
}
#endif
