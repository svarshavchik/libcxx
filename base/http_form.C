/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http/form.H"
#include "http/useragent.H"
#include "property_value.H"
#include "gettext_in.h"

#define LIBCXX_TEMPLATE_DECL
#include <http/useragentobj_t.H>
#undef LIBCXX_TEMPLATE_DECL

namespace LIBCXX_NAMESPACE {
	namespace http {
		namespace form {
#if 0
		};
	};
};
#endif

#define LIBCXX_TEMPLATE_DECL
#include "http/form_t.H"
#undef LIBCXX_TEMPLATE_DECL

void parametersBase::parameters_too_large()
{
	throw EXCEPTION(_("Form parameters too big"));
}

property::value<size_t> LIBCXX_HIDDEN
formmaxsize(LIBCXX_NAMESPACE_WSTR "::http::form::maxsize", 1024 * 1024 * 10);

size_t getformmaxsize()
{
	return formmaxsize.getValue();
}

const char hex[]="0123456789ABCDEFabcdef";

parametersObj::parametersObj()
{
}

parametersObj::~parametersObj() noexcept
{
}

parametersObj::parametersObj(const std::string &query_string)
{
	decode_params(query_string.begin(), query_string.end());
}

template<>
void
parametersObj::decode_params(std::string::iterator b, std::string::iterator e)

{
	return decode_params(static_cast<std::string::const_iterator>(b),
			     static_cast<std::string::const_iterator>(e));
}

// -------------------------------------------------------------------------

const char *parametersObj::word_iter::curchar() noexcept
{
	if (!*ptr)
		return ptr;

	switch (escape) {
	case do_escape:
		// Do not encode only unreserved characters, RFC 3986.

		if ((*ptr >= '0' && *ptr <= '9') ||
		    (*ptr >= 'A' && *ptr <= 'Z') ||
		    (*ptr >= 'a' && *ptr <= 'z') ||
		    (*ptr == '-' || *ptr == '_' || *ptr == '.' || *ptr == '~'))
			return ptr;
		return "%";
	case escape_next1:
		return &hex[ ((*ptr) >> 4) & 15];
	case escape_next2:
		return &hex[*ptr & 15];
	default:
		break;
	}
	return ptr;
}

void parametersObj::word_iter::advance()
{
	switch (escape) {
	case do_escape:
		if (*curchar() == '%')
		{
			escape=escape_next1;
			return;
		}
		break;
	case escape_next1:
		escape=escape_next2;
		return;
	case escape_next2:
		escape=do_escape;
		break;
	default:
		break;
	}
	++ptr;
}

bool parametersObj::encode_iter::eof() const
{
	while (begin_iter != end_iter)
	{
		if (*cur_word.curchar())
			return false;

		switch (next_word) {
		case next_do_equals:
			cur_word=word_iter("=", false);
			next_word=next_do_value;
			return false;
		case next_do_value:
			cur_word=word_iter(begin_iter->second.c_str(), true);
			next_word=next_do_advance;
			break;
		case next_do_advance:
			if (++begin_iter == end_iter)
				return true;
			cur_word=word_iter("&", false);
			next_word=next_do_param;
			return false;
		case next_do_param:
			cur_word=word_iter(begin_iter->first.c_str(), true);
			next_word=next_do_equals;
			break;
		}
	}
	return true;
}

const char *parametersObj::encode_iter::curchar() noexcept
{
	(void)eof();
	return cur_word.curchar();
}

void parametersObj::encode_iter::advance()
{
	if (!eof())
		cur_word.advance();
}

parametersObj::encode_iter::encode_iter(const const_parameters &parametersArg,
					bool endingIterator)
 : begin_iter(endingIterator ?
				      parametersArg->end() :
				      parametersArg->begin()),
			   end_iter(parametersArg->end()),
			   params(parametersArg)
{
	if (begin_iter != end_iter)
		cur_word=word_iter(begin_iter->first.c_str(), true);
	next_word=next_do_equals;
}

parametersObj::encode_iter::~encode_iter() noexcept
{
}

parametersObj::encode_iter parametersObj::encode_begin() const
{
	return encode_iter(const_parameters(this), false);
}

parametersObj::encode_iter parametersObj::encode_end() const
{
	return encode_iter(const_parameters(this), true);
}

#if 0
{
	{
		{
#endif
		}
	}
}
