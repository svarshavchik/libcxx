/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_fdclientimpl_H
#define x_http_fdclientimpl_H

#include <x/http/fdimplbase.H>
#include <x/http/clientimpl.H>
#include <x/property_value.H>
#include <x/hms.H>
#include <iterator>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	class timerfdObj;

	namespace http {
#if 0
	};
};
#endif

//! An HTTP client on a file descriptor

//!
//! Several properties govern the limits and timeouts for a file
//! descriptor-based HTTP client:
//!
//! - \c INSERT_LIBX_NAMESPACE::http::client::response_timeout - the timeout for
//! receiving the initial response to a request.
//!
//! - The \c INSERT_LIBX_NAMESPACE::http::client::header_limit - the maximum
//! size of the message from the server.
//!
//! - When receiving the body of the HTTP request response, or sending the
//! HTTP request message if
//! \c INSERT_LIBX_NAMESPACE::http::client::timeout_cnt bytes are not
//! received/sent in
//! \c INSERT_LIBX_NAMESPACE::http::client::timeout seconds, an internal
//! %exception gets thrown.

class fdclientimpl : public fdimplbase,
		     public clientimpl<fdimplbase::output_iter_t,
				       fdimplbase::input_iter_t> {

	//! Client superclass type
	typedef clientimpl<output_iter_t, input_iter_t> superclass_t;

	//! The \c INSERT_LIBX_NAMESPACE::http::client::timeout property.
	static property::value<hms> timeout LIBCXX_HIDDEN;

	//! The \c INSERT_LIBX_NAMESPACE::http::client::headers::maxsize property.

	static property::value<size_t> headers_maxsize LIBCXX_HIDDEN;

	//! The \c INSERT_LIBX_NAMESPACE::http::client::headers::limit property.

	static property::value<size_t> headers_limit LIBCXX_HIDDEN;

	//! The \c INSERT_LIBX_NAMESPACE::http::client::response_timeout property
	static property::value<hms> response_timeout LIBCXX_HIDDEN;

	//! The \c INSERT_LIBX_NAMESPACE::http::client::timeout_cnt property
	static property::value<size_t> timeout_cnt LIBCXX_HIDDEN;

public:
	using fdimplbase::set_terminate_fd;
	using fdimplbase::cancel_terminate_fd;

	//! Constructor
	fdclientimpl();

	//! Destructor
	~fdclientimpl();

	//! Recycle the HTTP client

	//! Invoking install() allows the same client object to be recycled,
	//! to send HTTP requests on another file descriptor.
	virtual void install(//! The file descriptor
			     const fd &filedescArg,

			     //! The terminator file descriptor
			     const fdptr &terminatorArg,

			     //! Client options

			     //! fdclientimpl knows the isproxy option,
			     //! which gets forwarded to the superclass,
			     //! and it governs how URLs get formatted in
			     //! requests.
			     clientopts_t optsArg);

	//! Override superclass

	//! If this is called via the fdimplbase superclass, intercept it,
	//! and call our install(), with the default client options.
	void install(const fd &filedescArg,
		     const fdptr &terminatorArg) override;

	//! Abnormal termination.

	virtual void terminate();

protected:
	//! Subclass hook to set a timeout.
	void begin_write_message() override;

	//! Subclass hook to clear any timeouts
	void end_write_message() override;

private:
	//! Begin header retrieval with a timeout
	void header_begin() override;

	//! Begin body retrieval with a timeout
	void body_begin() override;

	//! End body retrieval with a timeout
	void body_end() override;

	//! Finish up iterating over the body
	void check_body_status() override;
};


#if 0
{
	{
#endif
	}
}
#endif
