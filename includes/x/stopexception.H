/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_stopexception_H
#define x_stopexception_H

#include <x/exception.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! An %exception that gets thrown by msgdispatcherObj::stop()


//! You can throw it yourself, if you like.
//!

class stopexception : public exception {

public:
	stopexception() noexcept;
	~stopexception();

	using exception::operator->;
	using exception::operator*;
};

#if 0
{
#endif
}
#endif
