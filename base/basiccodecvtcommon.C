/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "ptr.H"
#include "exceptionfwd.H"
#include "facetobj.H"
#include "locale.H"
#include "basiccodecvtcommon.H"
#include "basiciofiltercodecvtin.H"
#include "basiciofiltercodecvtout.H"
#include "basicstreamcodecvtobj.H"
#include "basicstreamcodecvttype.H"
#include "codecvtiter.H"
#include "property_value.H"
#include "exception.H"
#include "messages.H"
#include <locale>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void basic_codecvt_common::in_failed(const std::string &localeName)

{
	std::ostringstream o;

	o << gettextmsg(libmsg("Invalid character set sequence for a "
			       "\"%1%\" locale"), localeName);

	throw EXCEPTION(o.str());
}

property::value<size_t> iofilter_bufsizeprop LIBCXX_INTERNAL
(LIBCXX_NAMESPACE_WSTR L"::iofilter::defaultsize", 1024);

size_t iofilter_bufsize()
{
	return iofilter_bufsizeprop.getValue();
}

property::value<size_t> iofilter_codecvtbufsizeprop LIBCXX_INTERNAL
(LIBCXX_NAMESPACE_WSTR L"::iofilter::codecvtsize", 1024);

size_t iofilter_codecvtbufsize()
{
	return iofilter_codecvtbufsizeprop.getValue();
}

template class iofilter<char, wchar_t>;
template class iofilter<wchar_t, char>;
template class iofilter<char, char>;
template class iofilteradapterObj<char, wchar_t>;
template class iofilteradapterObj<wchar_t, char>;
template class iofilteradapterObj<char, char>;
template class obasiciofilteriterbase<char, wchar_t>;
template class obasiciofilteriterbase<wchar_t, char>;
template class obasiciofilteriterbase<char, char>;

template class basic_codecvtout<std::codecvt<wchar_t, char, mbstate_t> >;
template class basic_codecvtin< std::codecvt<wchar_t, char, mbstate_t> >;

template class basic_iofiltercodecvtcommon<std::codecvt<wchar_t, char,
							mbstate_t>,
					   wchar_t, char,
					   basic_codecvtout_impl
					   <std::codecvt<wchar_t, char,
							 mbstate_t> > >;

template class basic_iofiltercodecvtcommon<std::codecvt<wchar_t, char,
							mbstate_t>,
					   char, wchar_t,
					   basic_codecvtin_impl
					   <std::codecvt<wchar_t, char,
							 mbstate_t> > >;

template class basic_ostreambufcodecvt<char, wchar_t>;
template class basic_ostreambufcodecvt<wchar_t, char>;

template class basic_istreambufcodecvt<char, wchar_t>;
template class basic_istreambufcodecvt<wchar_t, char>;

template class basic_iostreambufcodecvt<char, wchar_t>;
template class basic_iostreambufcodecvt<wchar_t, char>;

template class basic_streambufcodecvtObj<ctow_ostreambuf>;
template class basic_streambufcodecvtObj<wtoc_ostreambuf>;
template class basic_streambufcodecvtObj<ctow_istreambuf>;
template class basic_streambufcodecvtObj<wtoc_istreambuf>;

template class basic_convstream<ctow_ostreambuf>;
template class basic_convstream<wtoc_ostreambuf>;
template class basic_convstream<ctow_istreambuf>;
template class basic_convstream<wtoc_istreambuf>;

template class basic_ostreambufiofilter<char, wchar_t>;
template class basic_ostreambufiofilter<wchar_t, char>;
template class basic_istreambufiofilter<char, wchar_t>;
template class basic_istreambufiofilter<wchar_t, char>;
template class basic_iostreambufiofilter<char, wchar_t>;
template class basic_iostreambufiofilter<wchar_t, char>;

template class codecvtiter_bufObj<iofilteradapterObj<char, wchar_t>,
				  basic_codecvtin<std::codecvt<wchar_t, char,
							       mbstate_t> > >;

template class codecvtiter_bufObj<iofilteradapterObj<wchar_t, char>,
				  basic_codecvtout<std::codecvt<wchar_t,
								char,
								mbstate_t
								> > >;

#if 0
{
#endif
}
