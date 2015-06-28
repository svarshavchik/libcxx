/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/fdserverimpl.H"
#include "x/hms.H"
#include "x/ref.H"

#include <iomanip>

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

property::value<hms>
fdserverimpl::pipeline_timeout(LIBCXX_NAMESPACE_STR
			       "::http::server::pipeline_timeout",
			       30),
	fdserverimpl::headers_timeout(LIBCXX_NAMESPACE_STR
				     "::http::server::headers::timeout", 60),
	fdserverimpl::body_timeout(LIBCXX_NAMESPACE_STR
				   "::http::server::body_timeout", 60);

property::value<size_t>
fdserverimpl::headers_maxsize(LIBCXX_NAMESPACE_STR "::http::server::headers::maxsize",
			   1024 * 1024),
	fdserverimpl::headers_limit(LIBCXX_NAMESPACE_STR "::http::server::headers::limit",
			   1024),
	fdserverimpl::body_timeout_cnt(LIBCXX_NAMESPACE_STR
				       "::http::server::body_timeout_cnt",
				       65536);

fdserverimpl::fdserverimpl() : superclass_t(headers_limit.getValue())
{
}

fdserverimpl::~fdserverimpl() noexcept
{
}

class fdserverimpl::run_helper {

public:

	fdserverimpl *p;

	run_helper(fdserverimpl *pArg) : p(pArg) {}
	~run_helper() noexcept
	{
		p->clear();
	}
};

void fdserverimpl::run(const fd &connection,
		       const fd &terminator)
{
	run_helper helper(this);

	fdimplbase::install(connection, terminator);
	superclass_t::install(input_iter_t(filedesc()),
			      input_iter_t(),
			      output_iter_t(filedesc()),
			      headers_limit.getValue());

	try {
		superclass_t::run();
	} catch (response_exception &e)
	{
	}

	ran();
}

void fdserverimpl::ran()
{
}
void fdserverimpl::clear()
{
	fdimplbase::clear();
	superclass_t::install(input_iter_t(),
			      input_iter_t(),
			      output_iter_t(), 0);
}

bool fdserverimpl::eof()
{
	sender_t::iter.flush();
	filedesc_timeout->cancel_write_timer();

	filedesc_timeout->set_read_timeout(pipeline_timeout.getValue()
					   .seconds());

	bool flag;

	try {
		flag=superclass_t::eof();
	} catch (exception &e)
	{
		flag=true;
	}

	filedesc_timeout->cancel_read_timer();
	return flag;

}

void fdserverimpl::header_begin()
{
	sender_t::iter.flush();
	filedesc_timeout->cancel_write_timer();

	superclass_t::header_begin();

	filedesc_readlimit->set_read_limit(headers_maxsize.getValue(),
					   "HTTP request header limit exceeded"
					   );
	filedesc_timeout->set_read_timeout(headers_timeout.getValue()
					   .seconds());
}

void fdserverimpl::body_begin()
{
	// A server writes after getting an Expect: 100-continue, then starts
	// reading message body. Need to flush here.
	sender_t::iter.flush();
	filedesc_timeout->cancel_write_timer();
	filedesc_timeout->set_read_timeout(body_timeout_cnt.getValue(),
					   body_timeout.getValue().seconds());

	superclass_t::body_begin();
}

void fdserverimpl::body_end()
{
	filedesc_timeout->cancel_read_timer();
	superclass_t::body_end();
}

void fdserverimpl::begin_write_message()
{
	filedesc_timeout->cancel_read_timer();
	filedesc_timeout->set_write_timeout(body_timeout_cnt.getValue(),
					    body_timeout.getValue().seconds());
}

void fdserverimpl::end_write_message()
{
	sender_t::iter.flush();
	filedesc_timeout->cancel_write_timer();
}

#if 0
{
	{
#endif
	}
}
