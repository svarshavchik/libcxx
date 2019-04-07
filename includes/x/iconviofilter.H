/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_iconviofilter_H
#define x_iconviofilter_H

#include <x/exception.H>
#include <x/logger.H>
#include <x/basiciofilter.H>
#include <vector>
#include <iconv.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! An interface to iconv(3) implemented as an iofilter

class iconviofilter : virtual public iofilter<char, char> {

	//! The logger class

	LOG_CLASS_SCOPE;

	//! iconv(3) handle
	iconv_t h;

	//! Buffered input
	std::vector<char> x_inbuf;

	//! Buffered output
	std::vector<char> x_outbuf;

	//! Index into outbuf
	size_t x_outbuf_ptr;

public:
	//! Constructor
	iconviofilter(const std::string &fromchset,
		      const std::string &tochset);

	//! Destructor
	~iconviofilter();

	//! Convert between character sets
	void filter() override;

private:

	//! Invoke iconv(), if get errno=E2BIG, try again with a bigger outbuf.

	//! \internal

	bool filter_handle_e2big(const char *&inp,
				 size_t &inpsize);

};

#if 0
{
#endif
}
#endif
