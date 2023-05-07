/*
** Copyright 2012-2021 Double Precision, Inc.
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
OutputIterator tzfileObj::tzinfo::to_string(OutputIterator iter)
	const
{
	std::ostringstream o;

	o << tzname
	  << ':'
	  << std::setiosflags(std::ios_base::showpos)
	  << offset
	  << std::resetiosflags(std::ios_base::showpos)
	  << '@';

	switch (start_format) {
	case 'J':
		o << "J" << d;
		break;
	case 'M':
		o << "M" << m << "." << w
		  << "." << d;
		break;
	default:
		o << d;
	}
	o << "/"
	  << std::setw(2) << std::setfill('0')
	  << tod / (60 * 60)
	  << ":"
	  << std::setw(2) << std::setfill('0')
	  << (tod % (60 * 60)) / 60
	  << ":"
	  << std::setw(2) << std::setfill('0')
	  << tod % 60;

	std::string os(o.str());

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
					::to_string(date.short_format());
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

				o << LIBCXX_NAMESPACE::to_string
					(datechk.short_format());

				o << ", expected ";
				o << LIBCXX_NAMESPACE::to_string
					(date.short_format());
				throw EXCEPTION(o.str());
			}

			auto s=LIBCXX_NAMESPACE::to_string(date.short_format());

			return std::copy(s.begin(), s.end(), iter);
		}
	};

	std::string endl;

	{
		std::ostringstream ss;

		ss << std::endl;

		endl=ss.str();
	}

	{
		std::vector<struct ttinfo_s>::iterator b, e;
		size_t i;

		for (b=ttinfo->begin(), e=ttinfo->end(), i=0; b != e;
		     ++b, ++i)
		{
			std::ostringstream o;

			o << "tz[" << i
			  << "]: gmtoff=" << b->tt_gmtoff
			  << ", dst=" << b->tt_isdst
			  << ", name=" << b->tz_str
			  << ", isstd=" << b->isstd
			  << ", isgmt=" << b->isgmt << std::endl;

			std::string os(o.str());

			iter=std::copy(os.begin(), os.end(), iter);
		}
	}

	{
		std::vector<leaps_s>::iterator b, e;

		std::string pfix("leaps=");

		iter=std::copy(pfix.begin(), pfix.end(), iter);

		pfix="";

		for (b=leaps->begin(), e=leaps->end(); b != e; ++b)
		{
			std::ostringstream o;

			o << b->first << "=" << b->second;

			pfix += o.str();

			iter=std::copy(pfix.begin(), pfix.end(), iter);
			pfix = ", ";
		}

		iter=std::copy(endl.begin(), endl.end(), iter);
	}

	{
		size_t i, n=transitions->size();

		for (i=0; i<n; i++)
		{
			std::ostringstream o;

			o << "transition[" << i
			  << "]=" << (*transitions)[i]
			  << ", " << (int)(*ttinfo_idx)[i]
			  << std::endl;

			std::string os(o.str());

			iter=std::copy(os.begin(), os.end(), iter);
		}
	}

	if (tz_alt_end.tzname.size() > 0)
	{
		const char *tzseq="TZ=";

		iter=std::copy(tzseq, tzseq+3, iter);

		iter=tz_alt_start.to_string(iter);

		if (tz_alt_start.offset != tz_alt_end.offset)
		{
			*iter++ = ',';
			iter=tz_alt_end.to_string(iter);
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
					.get_year()+1;
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
				*iter++ = '\t';
				iter=dump_time_t::dump(iter, ios,
						       bb->first-1, ltz);
				iter=std::copy(endl.begin(), endl.end(), iter);

				*iter++ = '\t';
				iter=dump_time_t::dump(iter, ios,
						       bb->first, ltz);
				iter=std::copy(endl.begin(), endl.end(), iter);

				ymdhms date_chk(bb->first, ltz);

				*iter++ = '\t';

				if (date_chk.s > 59)
				{
					iter=dump_time_t::dump(iter, ios,
							       bb->first+1,
							       ltz);
				}
				else
				{
					auto s=LIBCXX_NAMESPACE::to_string
						(date_chk.short_format());

					iter=std::copy(s.begin(), s.end(),
						       iter);

					*iter++ =  ' ';
					*iter++ =  '(';
					*iter++ =  '*';
					*iter++ =  ')';
				}
				iter=std::copy(endl.begin(), endl.end(), iter);
			}
			else
			{
				if (bb->first == (decltype(bb->first))
				    0xf800000000000000)
					continue;

				iter=dump_time_t::dump(iter, ios,
						       bb->first-1, utc);
				*iter++ = ' ';
				*iter++ = '=';
				*iter++ = ' ';
				iter=dump_time_t::dump(iter, ios,
						       bb->first-1, ltz);
				iter=std::copy(endl.begin(), endl.end(), iter);

				iter=dump_time_t::dump(iter, ios,
						       bb->first, utc);
				*iter++ = ' ';
				*iter++ = '=';
				*iter++ = ' ';
				iter=dump_time_t::dump(iter, ios,
						       bb->first, ltz);
				iter=std::copy(endl.begin(), endl.end(), iter);
			}
		}
	}

	return iter;
}

std::string tzfileObj::debugDump(const const_locale &locale)
	const
{
	std::ostringstream o;

	debugDump(std::ostreambuf_iterator<char>(o.rdbuf()), o, locale);

	return o.str();
}

std::ostream &tzfileObj::debugDump(std::ostream &o,
				   const const_locale &locale)
	const
{
	debugDump(std::ostreambuf_iterator<char>(o.rdbuf()), o, locale);
	return o;
}

#if 0
{
#endif
}

static void dump(const std::string &tzname, std::ostream &str)

{
	std::string hdr("NAME: " + tzname);

	std::string whdr(hdr.begin(), hdr.end());

	str << whdr << std::endl;

	LIBCXX_NAMESPACE::tzfile tz(LIBCXX_NAMESPACE::tzfile::create(tzname));

	try {
		tz->debugDump(str);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
	}
	LIBCXX_NAMESPACE::ymdhms now( time(NULL), tz);

	LIBCXX_NAMESPACE::ymd now_ymd(now);
	LIBCXX_NAMESPACE::hms now_hms(now);
	str << std::string(now) << std::endl;
}

static void testtzfile(int argc, char **argv)
{
	const char rfc822[]="%a, %d %b %Y %H:%M:%S %z";

	LIBCXX_NAMESPACE::tzfile nyctz(LIBCXX_NAMESPACE::tzfile::create("America/New_York"));

	LIBCXX_NAMESPACE::ymdhms nyc20090101(LIBCXX_NAMESPACE::ymd(2009, 1, 1), LIBCXX_NAMESPACE::hms(0, 0, 0), nyctz);
	std::cout << LIBCXX_NAMESPACE::strftime(nyc20090101)(rfc822) << std::endl;

	LIBCXX_NAMESPACE::ymdhms nyc20090801(LIBCXX_NAMESPACE::ymd(2009, 8, 1), LIBCXX_NAMESPACE::hms(12, 0, 0), nyctz);

	std::cout << LIBCXX_NAMESPACE::to_string(nyc20090101.short_format(nyc20090801))
		  << std::endl;

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

		auto timezones=LIBCXX_NAMESPACE::tzfile::base::enumerate();

		for (auto &b:timezones)
		{
			std::ostringstream o;

			try {
				dump(b, o);
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
