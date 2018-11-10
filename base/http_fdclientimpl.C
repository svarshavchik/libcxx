/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/fdclientimpl.H"
#include "x/ref.H"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

property::value<hms>
fdclientimpl::response_timeout(LIBCXX_NAMESPACE_STR
			       "::http::client::response_timeout",
			       hms(0, 15, 0));

property::value<hms>
fdclientimpl::timeout(LIBCXX_NAMESPACE_STR "::http::client::timeout", 60);

property::value<size_t>
fdclientimpl::timeout_cnt(LIBCXX_NAMESPACE_STR "::http::client::timeout_cnt",
			  65536),
	fdclientimpl::headers_maxsize(LIBCXX_NAMESPACE_STR "::http::client::headers::maxsize",
				      1024 * 1024),
	fdclientimpl::headers_limit(LIBCXX_NAMESPACE_STR "::http::client::headers::limit",
				    1024);

fdclientimpl::fdclientimpl() : superclass_t(headers_limit.get())
{
}

// NOTE -- the TLS-based constructor is in the gnutls library

fdclientimpl::~fdclientimpl()
{
}

void fdclientimpl::install(const fd &filedescArg,
			   const fdptr &terminatorArg,
			   clientopts_t optsArg)
{
	fdimplbase::install(filedescArg, terminatorArg);
	superclass_t::install((optsArg & isproxy) != 0,
			      output_iter_t(filedesc()),
			      input_iter_t(filedesc()),
			      input_iter_t(),
			      headers_limit.get());
}

void fdclientimpl::install(const fd &filedescArg,
			   const fdptr &terminatorArg)
{
	install(filedescArg, terminatorArg, none);
}

void fdclientimpl::begin_write_message()
{
	filedesc_timeout->cancel_read_timer();
	filedesc_readlimit->cancel_read_limit();
	filedesc_timeout->set_write_timeout(timeout_cnt.get(),
					    timeout.get().seconds());
}

void fdclientimpl::end_write_message()
{
	sender_t::iter.flush();
	filedesc_timeout->cancel_write_timer();
}

void fdclientimpl::header_begin()
{
	sender_t::iter.flush();
	filedesc_timeout->cancel_write_timer();

	filedesc_readlimit->set_read_limit(headers_maxsize.get(),
					   "HTTP response header limit exceeded"
					   );
	filedesc_timeout->set_read_timeout(response_timeout.get()
					   .seconds());
}

void fdclientimpl::body_begin()
{
	sender_t::iter.flush();
	filedesc_timeout->cancel_write_timer();

	superclass_t::header_begin();

	filedesc_readlimit->cancel_read_limit();
	filedesc_timeout->set_read_timeout(timeout_cnt.get(),
					   timeout.get().seconds());
}

void fdclientimpl::body_end()
{
	filedesc_timeout->cancel_read_timer();
	filedesc_readlimit->cancel_read_limit();
}

void fdclientimpl::check_body_status()
{
	superclass_t::check_body_status();
}

void fdclientimpl::terminate()
{
}

#if 0
{
	{
#endif
	}
}
