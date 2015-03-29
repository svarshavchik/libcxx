/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/messages.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

messages libmsg()
{
	return messages::create(locale::base::environment(), LIBCXX_DOMAIN);
}

wmessages wlibmsg()
{
	return wmessages::create(locale::base::environment(), LIBCXX_DOMAIN);
}

std::string libmsg(const char *str)
{
	return libmsg()->get(str);
}

std::wstring wlibmsg(const char *str)
{
	return wlibmsg()->get(str);
}

template class basic_messagescommonObj<char>;
template class basic_messagescommonObj<wchar_t>;
template class basic_messagesObj<char>;
template class basic_messagesObj<wchar_t>;
template void gettext_fmt_advance(std::ostream &, const char *&);
template size_t gettext_fmt_getn(const char *&);
template void gettext_fmt_advance(std::wostream &, const wchar_t *&);
template size_t gettext_fmt_getn(const wchar_t *&);

#if 0
{
#endif
}
