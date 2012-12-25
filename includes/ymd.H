/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ymd_H
#define x_ymd_H

#include <x/ymdfwd.H>
#include <x/strftime.H>
#include <x/messagesfwd.H>
#include <x/interval.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename OutputIterator>
OutputIterator ymd::toString(OutputIterator iter,
			     const const_locale &localeRef,
			     const char *pattern)
	const
{
	std::string dt(formatDate(pattern, localeRef));

	return std::copy(dt.begin(), dt.end(), iter);
}

template<typename OutputIterator>
OutputIterator ymd::toString(OutputIterator iter,
			     const const_locale &localeRef,
			     const std::string &pattern)
	const
{
	return toString(iter, pattern.c_str(), localeRef);
}

template<typename OutputIterator>
OutputIterator ymd::toWideString(OutputIterator iter,
				 const const_locale &localeRef,
				 const wchar_t *pattern)
	const
{
	std::wstring dt(formatDate(pattern, localeRef));

	return std::copy(dt.begin(), dt.end(), iter);
}

template<typename OutputIterator>
OutputIterator ymd::toWideString(OutputIterator iter,
				 const const_locale &localeRef,
				 const std::wstring &pattern)
	const
{
	return toString(iter, pattern.c_str(), localeRef);
}

template<typename CharT>
std::basic_string<CharT> ymd::formatDate(const CharT *pattern,
					 const const_locale &localeRef)
	const
{
	CharT isodate[]={'%', 'd', '-', '%', 'b', '-', '%', 'Y', 0};

	if (!pattern)
		pattern=isodate;

	return basic_strftime<CharT>(*this, localeRef)(pattern);
}

template<typename CharT>
std::basic_string<CharT> ymd::formatDate(const std::basic_string<CharT>
					 &pattern,
					 const const_locale &localeRef)
	const
{
	return formatDate(pattern.c_str(), localeRef);
}

template<typename CharT>
ymd::operator std::basic_string<CharT>() const
{
	return formatDate<CharT>();
}

template<typename OutputIterator>
OutputIterator ymd::interval::toString(OutputIterator iter,
				       const const_locale &localeRef) const
{
	std::string str(toString<char>(localeRef));

	return std::copy(str.begin(), str.end(), iter);
}

template<typename InputIterator>
ymd::interval::interval(InputIterator beg_iter,
			InputIterator end_iter,
			const const_locale &localeArg)
	: years(0), months(0), weeks(0), days(0)
{
	construct(beg_iter, end_iter, localeArg);
}

template<typename InputIterator>
void ymd::interval::construct(InputIterator beg_iter,
			      InputIterator end_iter,
			      const const_locale &localeArg)
{
	LIBCXX_NAMESPACE::interval<sdaynum_t> parser(interval_descr, 0,
						   wlibmsg());

	const std::vector<sdaynum_t> &vec=parser.parse(beg_iter, end_iter);

	days=vec[0];
	weeks=vec[1];
	months=vec[2];
	years=vec[3];
}

//! Common template instantiation

extern template void ymd::interval::construct<std::string::iterator>
(std::string::iterator, std::string::iterator, const const_locale &)
;

template<typename CharT>
template<typename InputIterator>
ymd ymd::parser<CharT>::parse(InputIterator beg_iter,
			      InputIterator end_iter)
{
	basic_ctype<CharT> ct(loc);

	CharT digits[10];

	for (int i=0; i<10; i++)
		digits[i]=ct.widen('0' + i);

	int md[2];
	int md_cnt=0;

	bool md_str=false;
	std::basic_string<CharT> mstr;

	uint16_t year=0;
	bool yearfound=false;

	bool yearfirst=false;
	bool first=true;

	while (beg_iter != end_iter)
	{
		{
			size_t i;

			for (i=0; i<10; i++)
				if (*beg_iter == digits[i])
					break;

			if (i < 10)
			{
				uint16_t n=0;
				int ndigits=0;

				for (; beg_iter != end_iter; ++beg_iter)
				{
					for (i=0; i<10; i++)
						if (*beg_iter == digits[i])
							break;

					if (i >= 10)
						break;

					++ndigits;
					n=n*10 + i;

					if (n > 9999)
						goto bad;
				}


				if (ndigits == 4)
				{
					if (yearfound)
						goto bad;
					yearfound=true;
					yearfirst=first;
					year=n;
				}
				else
				{
					if (md_cnt >= 2)
						goto bad;

					if ((uint8_t)n != n)
						goto bad;

					md[md_cnt]=n;
					++md_cnt;
				}

				first=false;
				continue;
			}
		}
					
		if (!ct.is(*beg_iter, std::ctype_base::alpha))
		{
			++beg_iter;
			continue;
		}

		if (md_str)
			goto bad;

		while (beg_iter != end_iter &&
		       ct.is(*beg_iter, std::ctype_base::alnum))
			mstr.push_back(*beg_iter++);
		md_str=true;
	}

	uint16_t month, day;

	if (!yearfound)
		goto bad;

	if (md_str)
	{
		mstr=ct.toupper(mstr);

		if (md_cnt != 1)
			goto bad;

		if (mstr.size() > 1 && mstr[0] == 'W')
		{
			std::basic_istringstream<CharT> i(mstr.substr(1));

			uint16_t n=0;

			i >> n;

			if (!i.fail())
				return ymd(ymd::iso8601(year, n, md[0]));
		}

		for (month=0; month<12; month++)
			if (month_names[month] == mstr ||
			    month_names_long[month] == mstr)
				break;

		if (month >= 12)
			goto bad;
		++month;
		day=md[0];
	}
	else
	{
		if (md_cnt != 2)
		{
		bad:

			date_parsing_failed();
		}

		if (yearfirst || usmdy)
		{
			month=md[0];
			day=md[1];
		}
		else
		{
			day=md[0];
			month=md[1];
		}
	}

	return ymd(year, month, day);
}


template<typename CharT>
ymd ymd::parser<CharT>::parse(const CharT *string)
{
	size_t n;

	for (n=0; string[n]; ++n)
		;

	return parse(string, string+n);
}

#ifndef DOXYGEN

extern template class ymd::parser<char>;
extern template ymd ymd::parser<char>::parse(const char *, const char *);

extern template class ymd::parser<wchar_t>;
extern template ymd ymd::parser<wchar_t>::parse(const wchar_t *,
						const wchar_t *);
#endif

#if 0
{
#endif
}

#include <x/strftime.H>

#endif
