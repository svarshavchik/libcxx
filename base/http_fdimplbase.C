/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/fdimplbase.H"
#include "x/http/responseimpl.H"
#include "x/fd.H"
#include "x/ref.H"
#include "x/sysexception.H"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

fdimplbase::input_iter_t::input_iter_t(const fdbase &fdArg)
	: fdinputiter(fdArg)
{
}

fdimplbase::input_iter_t::input_iter_t()
{
}

fdimplbase::input_iter_t::~input_iter_t()
{
}

size_t fdimplbase::input_iter_t::pubread(const fdbase &fdref, char *ptr,
					 size_t cnt)
	const
{
	size_t n;

	n=fd::base::inputiter::pubread(fdref, ptr, cnt);

	if (n == 0 && errno)
	{
		if (errno == ETIMEDOUT)
			throw_request_timeout();
		throw SYSEXCEPTION(errno);
	}
	return n;
}

fdimplbase::output_iter_t::output_iter_t(const fdbase &fdArg)
	: fdoutputiter(fdArg)
{
}

fdimplbase::output_iter_t::output_iter_t()
{
}

fdimplbase::output_iter_t::~output_iter_t()
{
}

void fdimplbase::output_iter_t::flush()
{
	try {
		fd::base::outputiter::flush();
	} catch (const sysexception &e)
	{
		if (e.getErrorCode() == EPIPE)
			throw_request_timeout();
		throw;
	}
}

void fdimplbase::throw_request_timeout()
{
	responseimpl::throw_request_timeout();
}

fdimplbase::fdimplbase()
{
}

fdbase fdimplbase::filedesc()
{
	return filedesc_timeout;
}

void fdimplbase::install(const fd &orig_filedescArg,
			 const fdptr &terminatefdArg)
{
	orig_filedesc=orig_filedescArg;
	orig_filedesc->nonblock(true);
	filedesc_readlimit=fdreadlimit::create(orig_filedesc);
	filedesc_timeout=fdtimeout::create(filedesc_readlimit);
	if (!terminatefdArg.null())
		set_terminate_fd(terminatefdArg);
	filedesc_installed(filedesc_timeout);
}

void fdimplbase::filedesc_installed(const fdbase &transport)
{
}

void fdimplbase::clear()
{
	orig_filedesc=fdptr();
	filedesc_readlimit=fdreadlimitptr();
	filedesc_timeout=fdtimeoutptr();
}

fdimplbase::~fdimplbase()
{
}

void fdimplbase::borrow(const fdimplbase &other)
{
	install(other.orig_filedesc,
		other.filedesc_timeout->get_terminate_fd());
}

void fdimplbase::set_readwrite_timeout(time_t seconds)
{
	filedesc_timeout->set_read_timeout(seconds);
	filedesc_timeout->set_write_timeout(seconds);
}

void fdimplbase::cancel_readwrite_timeout()
{
	filedesc_timeout->cancel_read_timer();
	filedesc_timeout->cancel_write_timer();
}

#if 0
{
	{
#endif
	}
}
