/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/serialization.H"
#include "x/deserialize.H"
#include "x/messages.H"
#include "x/fd.H"
#include <cstdlib>
#include <sstream>

#include "gettext_in.h"

namespace LIBCXX_NAMESPACE::serialization {
#if 0
}
#endif

const char *tname(datatype value) noexcept
{
	switch (value) {
	case tbool:
		return "BOOL";
	case tint8:
		return "SIGNED BYTE";
	case tuint8:
		return "UNSIGNED BYTE";
	case tint16:
		return "SIGNED INT2";
	case tuint16:
		return "UNSIGNED INT2";
	case tint32:
		return "SIGNED INT4";
	case tuint32:
		return "UNSIGNED BYTE4";
	case tint64:
		return "SIGNED INT8";
	case tuint64:
		return "UNSIGNED BYTE8";
	case tsequence:
		return "SEQUENCE";
	case tpair:
		return "PAIR";
	case ttuple:
		return "TUPLE";
	case tobject:
		return "OBJECT";
	case treference:
		return "REFERENCE";
	default:
		break;
	}
	return "UNKNOWN";
}

std::string txtget(const char *str) LIBCXX_INTERNAL;

std::string txtget()
{
	return "";
}

void container_toolong()
{
	throw EXCEPTION(libmsg(_txt("Container size exceeds the allowed maximum")));
}

void serialization_conversion_error()
{
	throw EXCEPTION(libmsg(_txt("Unexpected serialization conversion error")));
}

void classname_toolong(const char *str)
{
	throw EXCEPTION(gettextmsg(libmsg(_txt("%1%: object name is too long")),
				   str));
}

void unknown_class(obj *o)
{
	std::string name;

	obj::demangle(typeid(o).name(), name);

	unknown_class(name);
}

void unknown_class(const std::string &name)
{
	throw EXCEPTION(gettextmsg(libmsg(_txt("%1%: class is not enumerated by classlist()")),
				   name));
}

void classname_empty()
{
	throw EXCEPTION(libmsg(_txt("Internal error: object name is an empty string")));
}

#if 0
{
#endif
}

namespace LIBCXX_NAMESPACE::deserialize {
#if 0
}
#endif


void floatmismatch(const std::string &value)

{
	mismatch(libmsg("floating point value"), value);
}

void mismatch(const std::string &expected_type,
	      const std::string &actual_type)

{
	throw EXCEPTION(gettextmsg(libmsg(_txt("Deserialization failure: expected %1%, received %2%...")),
				   expected_type, actual_type));
}

void serialization_truncated()
{
	throw EXCEPTION(libmsg(_txt("Truncated serialized bytestream")));
}


void deserialization_failure(const std::string &type)
{
	throw EXCEPTION(gettextmsg(libmsg(_txt("Deserialization failure: insert failure for %1%")),
				   type));
}

void deserialization_nullobj(const std::string &type)
{
	throw EXCEPTION(gettextmsg(libmsg(_txt("Deserialization failure: %1%: deserialized a null object")),
				   type));
}

#if 0
{
#endif
}
