/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "fditer.H"
#include "fdbase.H"
#include "ref.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

fdbufferObj::fdbufferObj(const ptr<fdbaseObj> &fdArg,
			 size_t buffer_size)
	: fd(fdArg), buf_size(0), buf_ptr(0), requested_buf_size(buffer_size)
{
}

fdbufferObj::~fdbufferObj() noexcept
{
}

// ---

fdinputiter::fdinputiter()
	: buf(ref<fdbufferObj>::create(fdbaseptr(),
				       fdbaseObj::get_buffer_size()))
{
}


fdinputiter::fdinputiter(const fdbase &fdArg,
			 size_t buffer_size)
	: buf(ref<fdbufferObj>::create(fdArg, buffer_size))
{
}

fdinputiter::~fdinputiter() noexcept
{
}

void fdinputiter::update()
{
	buf->fd=fdbaseptr();
	buf->buf_ptr=buf->buf_size=0;
}

void fdinputiter::update(const fdbase &fdArg)
{
	buf->fd=fdArg;
}

fdbaseptr fdinputiter::buffer()
{
	return buf->fd;
}

bool fdinputiter::operator==(const fdinputiter &o)
	const
{
	operator*();
	o.operator *();
	return buf->fd.null() && o.buf->fd.null();
}

bool fdinputiter::operator!=(const fdinputiter &o)
	const
{
	return !operator==(o);
}

char fdinputiter::operator*() const
{
	fdbufferObj &o= *buf;

	if (!o.fd.null())
	{
		if (o.buf_ptr < o.buf_size)
			return o.buffer[o.buf_ptr];

		fdbaseptr cpy(o.fd);

		o.fd=fdbaseptr();

		o.buf_ptr=0;
		o.buffer.resize(o.requested_buf_size);

		o.buf_size=pubread(cpy, &o.buffer[0], o.buffer.size());

		if (o.buf_size > 0)
		{
			o.fd=cpy;
			return o.buffer[0];
		}
	}
	return 0;
}

size_t fdinputiter::pubread(const fdbase &fdref, char *ptr, size_t cnt)
	const
{
	return fdref->pubread(ptr, cnt);
}

fdinputiter &fdinputiter::operator++()

{
	operator*();

	fdbufferObj &o= *buf;

	if (!o.fd.null())
		++o.buf_ptr;
	return *this;
}

const char *fdinputiter::operator++(int)
{
	const char *p=NULL;

	operator*();

	fdbufferObj &o= *buf;

	if (!o.fd.null())
	{
		p=&o.buffer[o.buf_ptr];
		++o.buf_ptr;
	}

	return p;
}

// ---

fdoutputiter::fdoutputiter()
	: buf(ref<fdbufferObj>::create(fdbaseptr(),
				       fdbaseObj::get_buffer_size()))
{
}

fdoutputiter::fdoutputiter(const fdbase &fdArg,
			   size_t buffer_size)
	: buf(ref<fdbufferObj>::create(fdArg, buffer_size))
{
}

fdoutputiter::~fdoutputiter() noexcept
{
}

void fdoutputiter::update()
{
	buf->fd=fdbaseptr();
	buf->buf_ptr=0;
}

void fdoutputiter::update(const fdbase &fdArg)
{
	buf->fd=fdArg;
}

void fdoutputiter::flush()
{
	fdbufferObj &o= *buf;

	if (o.fd.null())
		return;

	//! Move the file descriptor to the stack. A thrown exceptions now
	//! results in the iterator being knocked out of action for all
	//! future flushes.

	fdbaseptr fdSave(o.fd);

	o.fd=fdbaseptr();

	const char *p=&o.buffer[0];
	size_t n=o.buf_ptr;

	o.buf_ptr=0;

	while (n)
	{
		size_t cnt=fdSave->pubwrite(p, n);

		p += cnt;
		n -= cnt;
	}

	if (o.buffer.empty())
		o.buffer.resize(o.requested_buf_size);

	o.fd=fdSave; // Restore, we're good now.
}

fdoutputiter &fdoutputiter::operator=(char c)
{
	fdbufferObj &o= *buf;

	if (!o.fd.null() && o.buf_ptr >= o.buffer.size())
		flush();

	if (!o.fd.null())
		o.buffer[o.buf_ptr++]=c;
	return *this;
}

#if 0
{
#endif
}
