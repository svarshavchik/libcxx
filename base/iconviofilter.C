/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "iconviofilter.H"
#include "messages.H"
#include "sysexception.H"
#include "gettext_in.h"

#include <algorithm>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
const char iconviofilter::ucs4[]="UCS-4BE";
#else
#if __BYTE_ORDER == __LITTLE_ENDIAN
const char iconviofilter::ucs4[]="UCS-4LE";
#else
#error No more endians
#endif
#endif

iconviofilter::iconviofilter(const std::string &fromchset,
			     const std::string &tochset)
	: h((iconv_t)-1), x_outbuf_ptr(0)
{
	if ((h=iconv_open(tochset.c_str(), fromchset.c_str())) == (iconv_t)-1)
		throw SYSEXCEPTION(gettextmsg(libmsg(_txt("iconv(%1% to %2%)")),
					      fromchset, tochset));
}

iconviofilter::~iconviofilter() noexcept
{
	if (h != (iconv_t)-1)
		iconv_close(h);
}

void iconviofilter::filter()
{
	LOG_TRACE("Available output: " << avail_out
		  << ", buffered output: " << x_outbuf.size() - x_outbuf_ptr);

	while (x_outbuf_ptr < x_outbuf.size() && avail_out)
	{
		*next_out++ = x_outbuf[x_outbuf_ptr++];
		--avail_out;
	}

	if (x_outbuf_ptr < x_outbuf.size())
	{
		LOG_TRACE(x_outbuf.size() - x_outbuf_ptr
			  << " still buffered");
		return;
	}
 again:

	if (avail_out == 0)
	{
		LOG_TRACE("No more available output");
		return;
	}

	const size_t leftover=x_inbuf.size();

	LOG_TRACE("Available input: " << avail_in
		  << ", buffered input: " << leftover);

	if (leftover)
	{
		if (avail_in == 0)
		{
			x_inbuf.clear();
			throw EXCEPTION(_("iconv: invalid character sequence"));
		}

		size_t consume=avail_in;

		if (consume > 16)
			consume=16;

		size_t n=consume+leftover;
		char buf[n];

		std::copy(x_inbuf.begin(), x_inbuf.end(), &buf[0]);
		std::copy(next_in, next_in + avail_in, &buf[leftover]);

		const char *ptr=buf;

		if (!filter_handle_e2big(ptr, n))
		{
			if (errno != EINVAL)
				throw SYSEXCEPTION("iconv");

			LOG_TRACE("Buffering " << consume
				  << " additional input octets");

			x_inbuf.resize(consume+leftover);
			std::copy(buf, buf+consume+leftover, x_inbuf.begin());
			avail_in -= consume;
			next_in += consume;

			if (avail_in == 0)
				return;
			goto again;
		}

		LOG_TRACE(ptr - buf << " octets converted");

		if (ptr < buf+leftover) // How did this happen?
		{
			x_inbuf.resize(n);
			std::copy(ptr, ptr+n, x_inbuf.begin());
			goto again;
		}
		x_inbuf.clear();

		size_t swallowed=ptr - (buf+leftover);
		LOG_TRACE("Converted " << swallowed
			  << " octets from the original input");

		avail_in -= swallowed;
		next_in += swallowed;
		return;
	}

	if (x_outbuf_ptr < x_outbuf.size())
	{
		LOG_TRACE(x_outbuf.size() - x_outbuf_ptr
			  << " octets remain buffered");
		return;
	}

	if (!filter_handle_e2big(next_in, avail_in))
	{
		if (errno != EINVAL || avail_in == 0)
			throw SYSEXCEPTION("iconv");

		LOG_TRACE("Buffering " << avail_in
			  << " input octets");

		x_inbuf.resize(avail_in);
		std::copy(next_in, next_in+avail_in, x_inbuf.begin());
		next_in += avail_in;
		avail_in=0;
	}
}

bool iconviofilter::filter_handle_e2big(const char *&inp,
					size_t &inpsize)
{
	if (avail_out == 0)
	{
		LOG_TRACE("No available output octets");
		return true;
	}

	size_t save_inpsize=inpsize;

	if (iconv(h, inpsize ? const_cast<ICONV_INBUF_ARG>(&inp):NULL, &inpsize,
		  &next_out, &avail_out) != (size_t)-1 ||
	    (errno == E2BIG && save_inpsize != inpsize))
	{
		LOG_TRACE("Converted, remaining input: "
			  << inpsize
			  << ", remaining output: "
			  << avail_out);
		return true;
	}

	if (errno != E2BIG)
	{
		LOG_TRACE("Conversion error");
		return false;
	}

	LOG_TRACE("Insufficient output, buffering");

	x_outbuf.clear();
	x_outbuf_ptr=0;

	do
	{
		x_outbuf.resize(x_outbuf.size() + 16);

		char *p=&x_outbuf[0];
		size_t n=x_outbuf.size();

		LOG_TRACE("Trying buffer size " << n);

		if (iconv(h, inpsize ? const_cast<ICONV_INBUF_ARG>(&inp):NULL, &inpsize,
			  &p, &n) != (size_t)-1 ||
		    (errno == E2BIG && save_inpsize != inpsize))
		{
			x_outbuf.resize(x_outbuf.size() - n);
			LOG_TRACE("Converted, temporary output buffer: " <<
				  x_outbuf.size() << ", output buffer: " <<
				  avail_out);

			while (x_outbuf_ptr < x_outbuf.size() && avail_out)
			{
				*next_out++ = x_outbuf[x_outbuf_ptr++];
				--avail_out;
			}

			return true;
		}
	} while (errno == E2BIG);

	LOG_TRACE("Conversion error");
	return false;
}

extern void char32_t_better_be_4_bytes(char p[sizeof(char32_t) == 4 ? 1:-1]);

std::u32string iconviofilter::to_u32string(const std::string &string,
					   const std::string &charset)
{
	std::u32string u32string;

	iconviofilter to_ucs4(charset, iconviofilter::ucs4);

	to_ucs4.next_in=string.c_str();
	to_ucs4.avail_in=string.size();

	size_t taken, next_taken=0;

	do
	{
		taken=next_taken;

		u32string.resize(((taken ? taken*2:256)+3)/4);

		to_ucs4.next_out=reinterpret_cast<char *>(&u32string[0])+taken;
		to_ucs4.avail_out=u32string.size()*4-taken;

		to_ucs4.filter();

		next_taken=to_ucs4.next_out-
			reinterpret_cast<char *>(&u32string[0]);
	} while (taken != next_taken);

	u32string.resize(next_taken / 4);

	return u32string;
}

std::string iconviofilter::from_u32string(const std::u32string &u32string,
					  const std::string &charset)
{
	std::string string;

	iconviofilter from_ucs4(iconviofilter::ucs4, charset);

	from_ucs4.next_in=reinterpret_cast<const char *>(u32string.c_str());
	from_ucs4.avail_in=u32string.size()*sizeof(char32_t);

	size_t taken, next_taken=0;

	do
	{
		taken=next_taken;

		string.resize(taken ? taken*2:256);
		from_ucs4.next_out=reinterpret_cast<char *>(&string[0])+taken;
		from_ucs4.avail_out=string.size()-taken;

		from_ucs4.filter();

		next_taken=from_ucs4.next_out-
			reinterpret_cast<char *>(&string[0]);
	} while (taken != next_taken);

	string.resize(next_taken);

	return string;
}

#if 0
{
#endif
}

LOG_CLASS_INIT(LIBCXX_NAMESPACE::iconviofilter);
