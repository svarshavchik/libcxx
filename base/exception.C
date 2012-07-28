/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "exception.H"
#include "sysexception.H"
#include "logger.H"
#include "namespace.h"
#include "property_value.H"
#include "ptr.H"
#include <cstring>
#include <cerrno>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

exception::exception()
	: ref<exceptionObj>(ref<exceptionObj>::create())
{
}

exception::exception(const char *file, int line)
	: ref<exceptionObj>(ref<exceptionObj>::create())
{
	(*this)->fileline(file, line);
}

exception::exception(exceptionObj *obj) noexcept : ref<exceptionObj>(obj)
{
}

exception::~exception() noexcept
{
}


std::ostream &operator<<(std::ostream &o, const exception &e)
{
	o << e->operator std::string();

	return o;
}

const char *exception::what() const noexcept
{
	return (*this)->what();
}

void exception::done()
{
	(*this)->log();
}

sysexception_base::sysexception_base() : errcode(errno)
{
}

sysexception::sysexception()
{
	errcode=0; // Reset it.
}

sysexception::sysexception(const char *file, int line)
	: exception(file, line)
{
}

sysexception::~sysexception() noexcept
{
}

void sysexception::done()
{
	if (errcode == EAGAIN || errcode == EWOULDBLOCK)
	{
		(*this)->save();
		// Too much noise
		return;
	}

	(*this) << ": " << strerror(errcode);
	(*this)->log();
}

#if 0
{
#endif
}
