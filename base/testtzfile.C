/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/tzfile.H"
#include "x/ymdhms.H"
#include "x/strftime.H"
#include "x/vector.H"
#include <cstring>
#include <iostream>
#include <map>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif
#include "tz_internal.H"

template<typename OutputIterator>
OutputIterator tzfileObj::tzinfo::toString(OutputIterator iter,
					   basic_ctype<typename
					   OutputIterator::char_type> ct)
	const
{
	typedef typename OutputIterator::char_type CharT;

	std::basic_ostringstream<CharT> o;

	o << ct.widen(tzname)
	  << (CharT)':'
	  << std::setiosflags(std::ios_base::showpos)
	  << offset
	  << std::resetiosflags(std::ios_base::showpos)
	  << ct.widen("@");

	switch (start_format) {
	case 'J':
		o << ct.widen("J") << d;
		break;
	case 'M':
		o << ct.widen("M") << m << ct.widen(".") << w
		  << ct.widen(".") << d;
		break;
	default:
		o << d;
	}
	o << ct.widen("/")
	  << std::setw(2) << std::setfill((CharT)'0')
	  << tod / (60 * 60)
	  << ct.widen(":")
	  << std::setw(2) << std::setfill((CharT)'0')
	  << (tod % (60 * 60)) / 60
	  << ct.widen(":")
	  << std::setw(2) << std::setfill((CharT)'0')
	  << tod % 60;

	std::basic_string<CharT> os(o.str());

	return std::copy(os.begin(), os.end(), iter);
}

template<typename OutputIterator>
OutputIterator tzfileObj::debugDump(OutputIterator iter,
				    std::ios_base &ios,
				    const const_locale &locale)
		const
{
	class dump_time_t {

	public:
		static OutputIterator dump(OutputIterator iter,
					   std::ios_base &ios,
					   time_t val,
					   LIBCXX_NAMESPACE::const_tzfile tz)

		{
			ymdhms date(val, tz);

			time_t date_time_t=date;

			if (date_time_t != val)
			{
				std::ostringstream o;

				o << "Error converting ";

				o << LIBCXX_NAMESPACE
					::tostring(date.format());
				o << " to a time_t value, got "
				  << date_time_t
				  << ", expecting " << val;

				throw EXCEPTION(o.str());
			}

			ymdhms datechk(date, date, tz);

			if (static_cast<ymd &>(datechk) != date ||
			    static_cast<hms &>(datechk) != date)
			{
				std::ostringstream o;

				o << "Error verifying date/time, got ";

				o << LIBCXX_NAMESPACE::tostring
					(datechk.format());

				o << ", expected ";
				o << LIBCXX_NAMESPACE::tostring
					(date.format());
				throw EXCEPTION(o.str());
			}

			auto s=LIBCXX_NAMESPACE::tostring(date.format());

			return std::copy(s.begin(), s.end(), iter);
		}
	};

	typedef typename OutputIterator::char_type CharT;

	std::basic_string<CharT> endl;

	{
		std::basic_ostringstream<CharT> ss;

		ss << std::endl;

		endl=ss.str();
	}

	basic_ctype<CharT> ct(locale);

	{
		std::vector<struct ttinfo_s>::iterator b, e;
		size_t i;

		for (b=ttinfo->begin(), e=ttinfo->end(), i=0; b != e;
		     ++b, ++i)
		{
			std::basic_ostringstream<CharT> o;

			o << ct.widen("tz[") << i
			  << "]: gmtoff=" << b->tt_gmtoff
			  << ", dst=" << b->tt_isdst
			  << ", name=" << b->tz_str
			  << ", isstd=" << b->isstd
			  << ", isgmt=" << b->isgmt << std::endl;

			std::basic_string<CharT> os(o.str());

			iter=std::copy(os.begin(), os.end(), iter);
		}
	}

	{
		std::vector<leaps_s>::iterator b, e;

		std::basic_string<CharT> pfix(ct.widen("leaps="));

		iter=std::copy(pfix.begin(), pfix.end(), iter);

		{
			CharT null=0;

			pfix= &null;
		}

		for (b=leaps->begin(), e=leaps->end(); b != e; ++b)
		{
			std::basic_ostringstream<CharT> o;

			o << b->first << ct.widen("=") << b->second;

			pfix += o.str();

			iter=std::copy(pfix.begin(), pfix.end(), iter);
			pfix = ct.widen(", ");
		}

		iter=std::copy(endl.begin(), endl.end(), iter);
	}

	{
		size_t i, n=transitions->size();

		for (i=0; i<n; i++)
		{
			std::basic_ostringstream<CharT> o;

			o << ct.widen("transition[") << i
			  << ct.widen("]=") << (*transitions)[i]
			  << ct.widen(", ") << (int)(*ttinfo_idx)[i]
			  << std::endl;

			std::basic_string<CharT> os(o.str());

			iter=std::copy(os.begin(), os.end(), iter);
		}
	}

	if (tz_alt_end.tzname.size() > 0)
	{
		const char *tzseq="TZ=";

		iter=std::copy(tzseq, tzseq+3, iter);

		iter=tz_alt_start.toString(iter, ct);

		if (tz_alt_start.offset != tz_alt_end.offset)
		{
			*iter++ = ',';
			iter=tz_alt_end.toString(iter, ct);
		}

		iter=std::copy(endl.begin(), endl.end(), iter);
	}

	{
		std::map<time_t, bool> breaks;

		{
			std::vector<leaps_s>::const_iterator lb, le;

			for (lb=leaps->begin(), le=leaps->end(); lb != le; ++lb)
				breaks.insert(std::make_pair(lb->first, true));

			std::vector<time_t>::const_iterator tb, te;

			for (tb=transitions->begin(), te=transitions->end();
			     tb != te; ++tb)
				breaks.insert(std::make_pair(*tb, false));

			int32_t year=1900;

			if (!transitions->empty() &&
			    tz_alt_start.offset != tz_alt_end.offset)
			{
				year=compute_ymdhms(transitions->end()[-1])
					.getYear()+1;
				while (year < 2100)
				{
					time_t alt_start, alt_end;

					compute_transition(year, alt_start,
							   alt_end);
					breaks.insert(std::make_pair(alt_start,
								     false));
					breaks.insert(std::make_pair(alt_end,
								     false));
					++year;
				}
			}
		}

		std::map<time_t, bool>::const_iterator bb, be;

		LIBCXX_NAMESPACE::const_tzfile ltz(this);
		LIBCXX_NAMESPACE::const_tzfile utc(LIBCXX_NAMESPACE::tzfile::create("UTC"));

		for (bb=breaks.begin(), be=breaks.end(); bb != be; ++bb)
		{
			if (bb->second)
			{
				*iter++ = (CharT)'\t';
				iter=dump_time_t::dump(iter, ios,
						       bb->first-1, ltz);
				iter=std::copy(endl.begin(), endl.end(), iter);

				*iter++ = (CharT)'\t';
				iter=dump_time_t::dump(iter, ios,
						       bb->first, ltz);
				iter=std::copy(endl.begin(), endl.end(), iter);

				ymdhms date_chk(bb->first, ltz);

				*iter++ = (CharT)'\t';

				if (date_chk.s > 59)
				{
					iter=dump_time_t::dump(iter, ios,
							       bb->first+1,
							       ltz);
				}
				else
				{
					auto s=LIBCXX_NAMESPACE::tostring
						(date_chk.format());

					iter=std::copy(s.begin(), s.end(),
						       iter);

					*iter++ = (CharT) ' ';
					*iter++ = (CharT) '(';
					*iter++ = (CharT) '*';
					*iter++ = (CharT) ')';
				}
				iter=std::copy(endl.begin(), endl.end(), iter);
			}
			else
			{
				iter=dump_time_t::dump(iter, ios,
						       bb->first-1, utc);
				*iter++ = (CharT)' ';
				*iter++ = (CharT)'=';
				*iter++ = (CharT)' ';
				iter=dump_time_t::dump(iter, ios,
						       bb->first-1, ltz);
				iter=std::copy(endl.begin(), endl.end(), iter);

				iter=dump_time_t::dump(iter, ios,
						       bb->first, utc);
				*iter++ = (CharT)' ';
				*iter++ = (CharT)'=';
				*iter++ = (CharT)' ';
				iter=dump_time_t::dump(iter, ios,
						       bb->first, ltz);
				iter=std::copy(endl.begin(), endl.end(), iter);
			}
		}
	}

	return iter;
}

template<typename CharT>
std::basic_string<CharT> tzfileObj::debugDump(const const_locale &locale)
	const
{
	std::basic_ostringstream<CharT> o;

	debugDump(std::ostreambuf_iterator<CharT>(o.rdbuf()), o, locale);

	return o.str();
}

template<typename CharT>
std::basic_ostream<CharT> &tzfileObj::debugDump(std::basic_ostream<CharT> &o,
						const const_locale &locale)
	const
{
	debugDump(std::ostreambuf_iterator<CharT>(o.rdbuf()), o, locale);
	return o;
}

#if 0
{
#endif
}

template<typename stream_type>
static void dump(const std::string &tzname, stream_type &str)

{
	std::string hdr("NAME: " + tzname);

	std::basic_string<typename stream_type::char_type>
		whdr(hdr.begin(), hdr.end());

	str << whdr << std::endl;

	LIBCXX_NAMESPACE::tzfile tz(LIBCXX_NAMESPACE::tzfile::create(tzname));

	tz->debugDump(str);

	LIBCXX_NAMESPACE::ymdhms now( time(NULL), tz);

	LIBCXX_NAMESPACE::ymd now_ymd(now);
	LIBCXX_NAMESPACE::hms now_hms(now);
	str << std::basic_string<typename stream_type::char_type>(now)
	    << std::endl;
}

static void testtzfile(int argc, char **argv)
{
	const char rfc822[]="%a, %d %b %Y %H:%M:%S %z";

	LIBCXX_NAMESPACE::tzfile nyctz(LIBCXX_NAMESPACE::tzfile::create("America/New_York"));

	LIBCXX_NAMESPACE::ymdhms nyc20090101(LIBCXX_NAMESPACE::ymd(2009, 1, 1), LIBCXX_NAMESPACE::hms(0, 0, 0), nyctz);
	std::cout << LIBCXX_NAMESPACE::strftime(nyc20090101)(rfc822) << std::endl;

	LIBCXX_NAMESPACE::ymdhms nyc20090801(LIBCXX_NAMESPACE::ymd(2009, 8, 1), LIBCXX_NAMESPACE::hms(12, 0, 0), nyctz);

	std::cout << LIBCXX_NAMESPACE::strftime(nyc20090801)(rfc822) << std::endl;

	LIBCXX_NAMESPACE::ymdhms nyc19600101(LIBCXX_NAMESPACE::ymd(1960, 1, 1), LIBCXX_NAMESPACE::hms(0, 0, 0), nyctz);

	std::cout << LIBCXX_NAMESPACE::strftime(nyc19600101)(rfc822) << std::endl;

	LIBCXX_NAMESPACE::strftime();

	time_t nyc20090101_time_t=nyc20090101;

	LIBCXX_NAMESPACE::strftime(localtime(&nyc20090101_time_t));

	LIBCXX_NAMESPACE::strftime( nyc20090101_time_t, nyctz)
		(std::string(rfc822), std::cout);

	std::cout << std::endl;

	struct tm tmValue=nyc20090101;

	LIBCXX_NAMESPACE::ymdhms nyc20090101utc(tmValue, LIBCXX_NAMESPACE::tzfile::base::utc());

	std::cout << LIBCXX_NAMESPACE::strftime(tmValue)(rfc822) << std::endl;
	std::cout << LIBCXX_NAMESPACE::strftime(nyc20090101utc)(rfc822) << std::endl;
	std::cout << (std::string)nyc20090101 << std::endl;

	if (argc > 1 && strcmp(argv[1], "all") == 0)
	{
		std::string local(LIBCXX_NAMESPACE::tzfile::base::localname());

		std::cout << "Local timezone is " << local << std::endl;

		std::set<std::string> timezones;

		LIBCXX_NAMESPACE::tzfile::base::enumerate(timezones);

		std::set<std::string>::const_iterator b, e;

		for (b=timezones.begin(), e=timezones.end(); b != e; ++b)
		{
			std::ostringstream o;

			try {
				dump(*b, o);
			} catch (LIBCXX_NAMESPACE::exception &e)
			{
				std::cerr << o.str() << std::endl;
				std::cerr << std::endl << e << std::endl;
				exit(1);
			}
		}

		LIBCXX_NAMESPACE::tzfile::base::local()->debugDump(std::cout);

		LIBCXX_NAMESPACE::ymdhms now( time(NULL),
			       LIBCXX_NAMESPACE::tzfile::create("America/New_York"));

		std::cout << (std::string)now << std::endl;
		std::cout << (std::string)
			LIBCXX_NAMESPACE::ymdhms(now, LIBCXX_NAMESPACE::tzfile::create("America/Los_Angeles"))
		  << std::endl;
	}

	time_t now_t=time(NULL);

	LIBCXX_NAMESPACE::ymdhms
		now( now_t, LIBCXX_NAMESPACE::tzfile::create("America/New_York"));

	std::string now_str=now;

	LIBCXX_NAMESPACE::ymdhms
		now2( now_str, LIBCXX_NAMESPACE::tzfile::create("America/New_York"));

	if (now != now2)
		throw EXCEPTION("Conversion from string failed for " + now_str);
}

int main(int argc, char **argv)
{
	try {
		testtzfile(argc, argv);
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}
