/*
** Copyright 2012-2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/exception.H"
#include "x/sysexception.H"
#include "x/logger.H"
#include "x/namespace.h"
#include "x/property_value.H"
#include "x/ptr.H"
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

exception::exception(const ref<exceptionObj> &refp) noexcept
: ref<exceptionObj>(refp)
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

sysexceptionObj::sysexceptionObj(int errcodeArg) : errcode(errcodeArg)
{
}

sysexceptionObj::~sysexceptionObj() noexcept
{
}

void sysexceptionObj::rethrow()
{
	throw sysexception(this);
}

sysexception::sysexception()
	: exception(ref<sysexceptionObj>::create(0))
{
}

sysexception::sysexception(const char *file, int line)
	: exception(ref<sysexceptionObj>::create(errno))
{
	(*this)->fileline(file, line);
}

sysexception::sysexception(sysexceptionObj *obj)
	: exception(obj)
{
}

sysexception::~sysexception() noexcept
{
}

int sysexception::getErrorCode() const noexcept
{
	return dynamic_cast<sysexceptionObj &>(**this).errcode;
}

void sysexception::done()
{
	int errcode=getErrorCode();

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
