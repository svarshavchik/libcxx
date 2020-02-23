/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_hms_H
#define x_hms_H

#include <x/hmsfwd.H>
#include <x/interval.H>
#include <x/strtok.H>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <list>
#include <vector>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename OutputIterator>
OutputIterator hms::to_string(OutputIterator iter,
			     const const_locale &localeRef,
			     const std::string_view &pattern) const
{
	std::string str(this->format_time(pattern, localeRef));

	return std::copy(str.begin(), str.end(), iter);
}

template<typename InputIterator>
void hms::construct(InputIterator beg_iter,
		    InputIterator end_iter,
		    const const_locale &l)
{
	typedef typename std::iterator_traits<InputIterator>
		::value_type char_type;

	std::basic_string<char_type> str(beg_iter, end_iter);

	if (str.find(':') != str.npos)
	{
		// Probably hh:mm:ss

		std::list<std::basic_string<char_type> > units;

		char_type delim[2];

		delim[0]=':';
		delim[1]=0;

		strtok_str(str, delim, units);

		if (units.size() > 0 && units.size() <= 3)
		{
			std::list<int32_t> units_n;

			while (!units.empty())
			{
				std::basic_string<char_type> &s=units.front();

				if (s.size() == 0)
					break;

				std::basic_istringstream<char_type> i(s);

				int32_t v;

				i >> v;

				if (i.fail())
					break;

				units_n.push_back(v);
				units.pop_front();
			}
			if (!units.empty())
				parsing_error();

			h=0;
			m=0;
			s=0;

			switch (units_n.size()) {
			case 1:
				s=units_n.front();
				break;
			case 2:
				h=units_n.front();
				units_n.pop_front();
				m=units_n.front();
				break;
			default:
				h=units_n.front();
				units_n.pop_front();
				m=units_n.front();
				units_n.pop_front();
				s=units_n.front();
				break;
			}

			normalize(h, m, s, 0, 0);
			return;
		}
	}
	else
	{
		interval<int32_t> parser(interval_descr, 2, libmsg());

		std::vector<int32_t> vec=parser.parse(str, l);

		normalize(vec[0], vec[1], vec[2], vec[3], vec[4]);
		h=vec[0];
		m=vec[1];
		s=vec[2];
		return;
	}

	parsing_error();
}

//! Overload the << operator.

std::ostream &operator<<(std::ostream &o, const hms &);

#ifndef DOXYGEN
extern template
void hms::construct(std::string::const_iterator,
		    std::string::const_iterator,
		    const const_locale &);
#endif

#if 0
{
#endif
}
#endif
