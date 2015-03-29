/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fdstreambufobj.H"
#include "x/fdbase.H"
#include <cstdio>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

fdstreambufObj::fdstreambufObj(const fdbase &fdrefArg,
			       size_t defaultBufSizeArg,
			       bool syncflagArg) noexcept
	: fdref(fdrefArg), defaultBufSize(defaultBufSizeArg),
	  read_buf_ptr(0), read_buf_size(0),
	  write_buf_ptr(0), write_buf_size(0),
	  syncflag(syncflagArg)
{
}

fdstreambufObj::~fdstreambufObj() noexcept
{
	try {
		sync();
	} catch(...) {
	}
}

std::streambuf *fdstreambufObj::setbuf(char_type *s, std::streamsize n)
{
	if (sync() < 0)
		return NULL;

	if (n < 2 || s == 0)
	{
		s=0;
		n=0;
	}

	readbuffer.clear();
	writebuffer.clear();

	if (syncflag || s == 0)
	{
		read_buf_ptr=s;
		read_buf_size=n;
		write_buf_ptr=s;
		write_buf_size=n;
	}
	else
	{
		read_buf_ptr=s;
		read_buf_size=n/2;
		write_buf_ptr=s + n/2;
		write_buf_size=n/2;
	}
	setg(read_buf_ptr, read_buf_ptr, read_buf_ptr);
	return this;
}

fdstreambufObj::int_type fdstreambufObj::underflow()
{
	if (syncflag && sync() < 0)
		return traits_type::eof();

	if (!read_buf_ptr || read_buf_size == 0)
	{
		readbuffer.resize(defaultBufSize);
		read_buf_ptr=&readbuffer[0];
		read_buf_size=readbuffer.size();

		if (syncflag)
		{
			write_buf_ptr=read_buf_ptr;
			write_buf_size=read_buf_size;
		}
	}

	size_t n=fdref->pubread(read_buf_ptr, read_buf_size);

	setg(read_buf_ptr, read_buf_ptr, read_buf_ptr + n);
	if (n == 0)
		return traits_type::eof();

	return traits_type::to_int_type(*read_buf_ptr);
}

//! Implement the \c std::basic_streambuf::showmanyc() method

std::streamsize fdstreambufObj::showmanyc()
{
	char *p=this->gptr(), *e=this->egptr();

	if (p && e > p)
		return e-p;

	if (std::char_traits<char>::eq_int_type(underflow(),
						std::char_traits<char>::eof()))
		return 0;

	return egptr() - gptr();
}

fdstreambufObj::int_type fdstreambufObj::overflow(int_type c)
{
	if (sync() < 0)
		return traits_type::eof();

	if (syncflag)
	{
		setg(read_buf_ptr, read_buf_ptr, read_buf_ptr);
	}

	if (!write_buf_ptr || write_buf_size == 0)
	{
		writebuffer.resize(defaultBufSize);
		write_buf_ptr=&writebuffer[0];
		write_buf_size=writebuffer.size();

		if (syncflag)
		{
			read_buf_ptr=write_buf_ptr;
			read_buf_size=write_buf_size;
		}
	}

	setp(write_buf_ptr, write_buf_ptr + write_buf_size);

	if (!traits_type::eq_int_type(c, traits_type::eof()))
	{
		*pptr()=c;
		pbump(1);
	}
	return 0;
}

int fdstreambufObj::sync()
{
	char *pb=pbase();
	char *pp=pptr();

	while (pb < pp)
	{
		size_t n=fdref->pubwrite(pb, pp-pb);

		if (n == 0)
			return -1;

		pb += n;
	}

	setp(write_buf_ptr, write_buf_ptr);

	if (syncflag) // Using the same buffer
	{
		char *gp=gptr();
		char *eg=egptr();

		if (gp && gp < eg)
			fdref->pubseek(gp-eg, SEEK_CUR);
		setg(read_buf_ptr, read_buf_ptr, read_buf_ptr);
	}

	return 0;
}

fdstreambufObj::pos_type
fdstreambufObj::seekoff(off_type off, std::ios_base::seekdir way,
			std::ios_base::openmode which)
{
	if (sync() < 0)
		return pos_type(off_type(-1));

	return pos_type(fdref->pubseek(off, way == std::ios_base::beg
				       ? SEEK_SET:way == std::ios_base::end
				       ? SEEK_END:SEEK_CUR));
}

fdstreambufObj::pos_type
fdstreambufObj::seekpos(pos_type sp,
			std::ios_base::openmode which)
{
	if (sync() < 0)
		return pos_type(off_type(-1));

	return pos_type(fdref->pubseek(sp, SEEK_SET));
}

#if 0
{
#endif
}
