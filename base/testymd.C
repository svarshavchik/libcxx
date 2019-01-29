/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ymd.H"
#include "x/hms.H"
#include "x/ymdhms.H"
#include "x/tostring.H"

#include <iostream>

bool have_fake_today=false;
LIBCXX_NAMESPACE::ymd fake_today;
#define TEST_2YEAR_PARSE() do {			\
		if (have_fake_today)		\
			today=fake_today;	\
	} while(0)

#include "ymd.C"

#define VERIFY(cond) do {						\
		if (!(cond)) throw EXCEPTION("Assertion failed: " #cond); \
	} while(0)

#define VERIFYFAIL(cond) do {						\
	bool fail=false;						\
	try {								\
		(cond);							\
	} catch (LIBCXX_NAMESPACE::exception &e)					\
	{								\
		fail=true;						\
	}								\
	if (!fail)							\
		throw EXCEPTION("Assertion failure: " #cond);		\
	} while(0)

static void CHECK_DATEDIFF_S(int y1, int m1, int d1,
			     int y2, int m2, int d2,
			     int diff)
{
	VERIFY( LIBCXX_NAMESPACE::ymd(y1,m1,d1).jd() + diff == LIBCXX_NAMESPACE::ymd(y2,m2,d2).jd());
	VERIFY( LIBCXX_NAMESPACE::ymd(y1,m1,d1)+diff == LIBCXX_NAMESPACE::ymd(y2,m2,d2));
	VERIFY( LIBCXX_NAMESPACE::ymd(y1,m1,d1)+(2000+diff)-2000 == LIBCXX_NAMESPACE::ymd(y2,m2,d2));
	VERIFY( LIBCXX_NAMESPACE::ymd(y1,m1,d1)-(2000-diff)+2000 == LIBCXX_NAMESPACE::ymd(y2,m2,d2));
	VERIFY( LIBCXX_NAMESPACE::ymd(y2,m2,d2)-LIBCXX_NAMESPACE::ymd(y1,m1,d1) == diff);
}

static void CHECK_DATEDIFF(int y1, int m1, int d1, int y2, int m2, int d2,
			   int diff)
{
	CHECK_DATEDIFF_S(y1,m1,d1,y2,m2,d2,diff);
	CHECK_DATEDIFF_S(y2,m2,d2,y1,m1,d1,-(diff));
}

static void CHECK_LY(int y)
{
	CHECK_DATEDIFF(y-1,12,31,y,1,1,1);
	CHECK_DATEDIFF(y,2,28,y,2,29,1);
	CHECK_DATEDIFF(y,2,29,y,3,1,1);
	CHECK_DATEDIFF(y,2,28,y,3,1,2);
	CHECK_DATEDIFF(y,12,31,y+1,1,1,1);
}

static void CHECK_NLY(int y)
{
	CHECK_DATEDIFF(y-1,12,31,y,1,1,1);
	CHECK_DATEDIFF(y,2,28,y,3,1,1);
	CHECK_DATEDIFF(y,12,31,y+1,1,1,1);
}

class Testymd {

public:

	static void test();

	static void testdateput();
};

void Testymd::test()
{
	CHECK_DATEDIFF(1752,9,2,1752,9,14,1);

	CHECK_NLY(1900);
	CHECK_NLY(1901);
	CHECK_NLY(1902);
	CHECK_NLY(1903);
	CHECK_LY (1904);

	CHECK_LY(2000);
	CHECK_NLY(2001);
	CHECK_NLY(2002);
	CHECK_NLY(2003);
	CHECK_LY (2004);

	CHECK_LY(1600);
	CHECK_NLY(1601);
	CHECK_NLY(1602);
	CHECK_NLY(1603);
	CHECK_LY(1604);

	CHECK_LY(1700);
	CHECK_NLY(1701);
	CHECK_NLY(1702);
	CHECK_NLY(1703);
	CHECK_LY(1704);

	VERIFY( LIBCXX_NAMESPACE::ymd(1969,8,3) + (LIBCXX_NAMESPACE::ymd(2009,5,18)-LIBCXX_NAMESPACE::ymd(1969,8,3))
		== LIBCXX_NAMESPACE::ymd(2009,5,18));

	VERIFYFAIL( LIBCXX_NAMESPACE::ymd(1,1,1)-1);

	VERIFYFAIL( LIBCXX_NAMESPACE::ymd(1,1,1)-1000);

	VERIFYFAIL( LIBCXX_NAMESPACE::ymd(9999,12,31)+1);

	VERIFYFAIL( LIBCXX_NAMESPACE::ymd(9999,12,31)+0x7FFFFFFF);

	VERIFY( LIBCXX_NAMESPACE::ymd(1, 1, 1).get_day_of_week() == 6);

	VERIFY( LIBCXX_NAMESPACE::ymd(1752, 9, 2).get_day_of_week() == 3);

	VERIFY( LIBCXX_NAMESPACE::ymd(1752, 9, 14).get_day_of_week() == 4);

	VERIFY( LIBCXX_NAMESPACE::ymd(1905, 1, 1).get_day_of_week() == 0);

	VERIFY( LIBCXX_NAMESPACE::ymd(1970, 1, 1).get_day_of_week() == 4);

	VERIFY( LIBCXX_NAMESPACE::ymd(2009, 5, 17).get_day_of_week() == 0);

	VERIFY( LIBCXX_NAMESPACE::ymd(2009, 1, 1) + LIBCXX_NAMESPACE::ymd::months(12) ==
		LIBCXX_NAMESPACE::ymd(2010, 1, 1));

	VERIFY( LIBCXX_NAMESPACE::ymd(2010, 1, 1) + LIBCXX_NAMESPACE::ymd::months(-12) ==
		LIBCXX_NAMESPACE::ymd(2009, 1, 1));

	VERIFY( LIBCXX_NAMESPACE::ymd(2009, 1, 1) + LIBCXX_NAMESPACE::ymd::years(1) ==
		LIBCXX_NAMESPACE::ymd(2010, 1, 1));

	VERIFY( LIBCXX_NAMESPACE::ymd(2010, 1, 1) + LIBCXX_NAMESPACE::ymd::years(-1) ==
		LIBCXX_NAMESPACE::ymd(2009, 1, 1));

	VERIFY( LIBCXX_NAMESPACE::ymd(2007, 2, 28) + LIBCXX_NAMESPACE::ymd::months(1) ==
		LIBCXX_NAMESPACE::ymd(2007, 3, 31));

	VERIFY( LIBCXX_NAMESPACE::ymd(2007, 2, 28) + LIBCXX_NAMESPACE::ymd::years(1) ==
		LIBCXX_NAMESPACE::ymd(2008, 2, 29));

	VERIFY( LIBCXX_NAMESPACE::ymd(2007, 3, 31) - LIBCXX_NAMESPACE::ymd::months(1) ==
		LIBCXX_NAMESPACE::ymd(2007, 2, 28));

	VERIFY( LIBCXX_NAMESPACE::ymd(2007, 3, 30) - LIBCXX_NAMESPACE::ymd::months(1) ==
		LIBCXX_NAMESPACE::ymd(2007, 2, 28));

	VERIFY( LIBCXX_NAMESPACE::ymd(1752, 10, 10) - LIBCXX_NAMESPACE::ymd::months(1) ==
		LIBCXX_NAMESPACE::ymd(1752, 9, 2));

	VERIFY( LIBCXX_NAMESPACE::ymd(1752, 8, 10) + LIBCXX_NAMESPACE::ymd::months(1) ==
		LIBCXX_NAMESPACE::ymd(1752, 9, 14));

	VERIFYFAIL(LIBCXX_NAMESPACE::hms(  ({LIBCXX_NAMESPACE::ymd::interval i;
					i.weeks=0x7FFFFFFF / 7; i.days=14; i; })));
}

void Testymd::testdateput()
{
	LIBCXX_NAMESPACE::locale en_us(LIBCXX_NAMESPACE::locale::create("en_US.UTF-8"));

	LIBCXX_NAMESPACE::ymd ymd(2009,1,1);

	std::ostringstream o;

	o << ymd;

	std::cout << ymd.format_date("std=%C %d %F %j %m %u %w %y %Y %a %A %b %B%n"
				     "alt%%=%EC %Od %Om %Ou %Ow %Ey %EY",
				    en_us) << std::endl;

	std::cout << (std::string)ymd << std::endl;

	std::cout <<
		ymd.format_date(std::string("std=%C %d %F %j %m %u %w %y %Y %a %A %b %B%n"
					   "alt%%=%EC %Od %Om %Ou %Ow %Ey %EY"),
			       en_us) << std::endl;

	std::cout << LIBCXX_NAMESPACE::ymd(2009,1,1).format_date("%U%n", en_us);
	std::cout << LIBCXX_NAMESPACE::ymd(2009,1,3).format_date("%U%n", en_us);
	std::cout << LIBCXX_NAMESPACE::ymd(2009,1,4).format_date("%U%n", en_us);
	std::cout << LIBCXX_NAMESPACE::ymd(2009,1,1).format_date("%g %G-W%V-%u%n", en_us);
	std::cout << LIBCXX_NAMESPACE::ymd(2008,1,1).format_date("%g %G-W%V-%u%n", en_us);
	VERIFY(LIBCXX_NAMESPACE::ymd(LIBCXX_NAMESPACE::ymd::iso8601(LIBCXX_NAMESPACE::ymd(2009,1,1))) == LIBCXX_NAMESPACE::ymd(2009,1,1));
	VERIFY(LIBCXX_NAMESPACE::ymd(LIBCXX_NAMESPACE::ymd::iso8601(LIBCXX_NAMESPACE::ymd(2008,1,1))) == LIBCXX_NAMESPACE::ymd(2008,1,1));

	static const int isochk[][7]={
		{2005,1,1,2004,53,6},
		{2005,1,2,2004,53,7},
		{2005,12,31,2005,52,6},
		{2007,1,1,2007,1,1},
		{2007,12,30,2007,52,7},
		{2007,12,31,2008,1,1},
		{2008,1,1,2008,1,2},
		{2008,12,29,2009,1,1},
		{2008,12,31,2009,1,3},
		{2009,1,1,2009,1,4},
		{2009,12,31,2009,53,4},
		{2010,1,3,2009,53,7},

		{2009,12,31,2009,53,4},
		{2010,1,1,2009,53,5},
		{2010,1,2,2009,53,6},
		{2010,1,3,2009,53,7},
		{2010,1,4,2010,1,1},

		{2008,12,28,2008,52,7},
		{2008,12,29,2009,1,1},
		{2008,12,30,2009,1,2},
		{2008,12,31,2009,1,3},
		{2009,1,1,2009,1,4}};


	for (size_t i=0; i<sizeof(isochk)/sizeof(isochk[0]); i++)
	{
		LIBCXX_NAMESPACE::ymd::iso8601 iso8601(LIBCXX_NAMESPACE::ymd(isochk[i][0],
					       isochk[i][1],
					       isochk[i][2]));

		if (iso8601.get_year() != isochk[i][3] ||
		    iso8601.get_week() != isochk[i][4] ||
		    iso8601.get_day_of_week() != isochk[i][5])
		{
			std::ostringstream errmsg;

			errmsg << "ISO 8601 check failed for "
			       << isochk[i][0] << "-"
			       << isochk[i][1] << "-"
			       << isochk[i][2];

			throw EXCEPTION(errmsg.str());
		}
	}

	LIBCXX_NAMESPACE::ymd::parser cp;

	std::cout << (std::string)cp.parse("3-aug-2009") << std::endl;

	auto time_get=en_us->get_facet<x::facets::time_get_facet<char> >();

	std::cout << LIBCXX_NAMESPACE::ymd("8/3/2009", en_us) << std::endl;
	std::cout << LIBCXX_NAMESPACE::ymd(std::string("2009-8-3"), en_us)
		  << std::endl;
	std::cout << (std::string)LIBCXX_NAMESPACE::ymd::iso8601(cp.parse("2009-W2-3")) << std::endl;
}

int main(int argc, char **argv)
{
	alarm(30);

	try {
		auto en_us=LIBCXX_NAMESPACE::locale::create("en_US.UTF-8");
		auto es_ES=LIBCXX_NAMESPACE::locale::create("es_ES.UTF-8");

		auto old_global=LIBCXX_NAMESPACE::locale::base::global();

		if (old_global != LIBCXX_NAMESPACE::locale::base::global())
			throw EXCEPTION("Should be global");

		en_us->global();
		if (old_global == LIBCXX_NAMESPACE::locale::base::global())
			throw EXCEPTION("Shouldn't be global");

		if (x::ymd(2017,1,1).format_date("%x")  != "01/01/2017")
			throw EXCEPTION("Global format_date() doesn't work");

		have_fake_today=true;
		fake_today=LIBCXX_NAMESPACE::ymd(2001,1,1);

		if (LIBCXX_NAMESPACE::ymd::parser().parse("2/3/04") !=
		    LIBCXX_NAMESPACE::ymd(2004,2,3))
			throw EXCEPTION("2 digit year parse #1 failed");

		if (LIBCXX_NAMESPACE::ymd::parser().parse("8/3/69") !=
		    LIBCXX_NAMESPACE::ymd(1969,8,3))
			throw EXCEPTION("2 digit year parse #2 failed");

		if (LIBCXX_NAMESPACE::ymd::parser(es_ES).parse("28/01/19") !=
		    LIBCXX_NAMESPACE::ymd(2019,1,28))
			throw EXCEPTION("2 digit year parse #3 failed");

		if (LIBCXX_NAMESPACE::ymd::parser(es_ES).parse("2019.28.01") !=
		    LIBCXX_NAMESPACE::ymd(2019,1,28))
			throw EXCEPTION("4 digit year parse #5 failed");

		if (LIBCXX_NAMESPACE::ymd::parser(es_ES).parse("2019-01-28") !=
		    LIBCXX_NAMESPACE::ymd(2019,1,28))
			throw EXCEPTION("4 digit year parse #6 failed");

		static const struct {
			int m,d;

			int today_y,today_m,today_d;

			int y;
		} md_tests[]={
			{1, 10, 2001, 3, 1, 2001},
			{4, 10, 2001, 3, 1, 2001},
			{12, 10, 2001, 3, 1, 2000},


			{1, 10, 2000, 12, 10, 2001},
			{10, 10, 2000, 12, 10,2000}
		};

		for (const auto &md_test:md_tests)
		{
			std::ostringstream o;

			o << md_test.m << "/" << md_test.d;

			fake_today=LIBCXX_NAMESPACE::ymd(md_test.today_y,
				       md_test.today_m,
				       md_test.today_d);

			auto res=LIBCXX_NAMESPACE::ymd::parser().parse(o.str());

			if (res.get_month() != md_test.m ||
			    res.get_day() != md_test.d ||
			    res.get_year() != md_test.y)
				throw EXCEPTION("Expected year "
						<< md_test.y
						<< " for "
						<< o.str()
						<< ", got "
						<< res);
		}

		have_fake_today=false;

		if (LIBCXX_NAMESPACE::ymd::parser().try_parse("13/1"))
			throw EXCEPTION("Shouldn't happen.");

		LIBCXX_NAMESPACE::locale de_DE{
			LIBCXX_NAMESPACE::locale::create("de_DE.UTF-8")};

		Testymd::testdateput();
		Testymd::test();

		LIBCXX_NAMESPACE::ymd::interval
			inter("years 4, 1 month, 2 w, 3 days");

		std::cout << LIBCXX_NAMESPACE::tostring
			(inter, LIBCXX_NAMESPACE::locale::create("en_US.UTF-8"))
			  << std::endl;

		LIBCXX_NAMESPACE::hms hms("hour 1, 59 minutes, 70 seconds");

		std::cout << hms.verboseString() << std::endl;

		std::cout << LIBCXX_NAMESPACE::hms("10 minute -7 seconds").verboseString()
			  << std::endl;

		std::cout << LIBCXX_NAMESPACE::hms("10 minute -77 seconds").verboseString()
			  << std::endl;

		std::cout << LIBCXX_NAMESPACE::hms("-10 minute -7 seconds").verboseString()
			  << std::endl;
		std::cout << LIBCXX_NAMESPACE::hms("67").verboseString()
			  << std::endl;

		std::cout << LIBCXX_NAMESPACE::hms("00:01").verboseString()
			  << std::endl;
		std::cout << LIBCXX_NAMESPACE::hms("04:01:30").verboseString()
			  << std::endl;
		std::cout << LIBCXX_NAMESPACE::hms("04:01:30")
			  << std::endl;

		std::cout << (LIBCXX_NAMESPACE::hms("00:58:30")+
			      LIBCXX_NAMESPACE::hms("00:01:40")).verboseString()
			  << std::endl;

		std::cout << (LIBCXX_NAMESPACE::hms("00:58:30")-
			      LIBCXX_NAMESPACE::hms("00:01:40")).verboseString()
			  << std::endl;
		std::cout << (LIBCXX_NAMESPACE::hms("00:58:30")-
			      LIBCXX_NAMESPACE::hms("01:00:40")).verboseString()
			  << std::endl;
		std::cout << LIBCXX_NAMESPACE::hms("1h1m1s").seconds()
			  << std::endl;

		std::cout <<
			LIBCXX_NAMESPACE::hms(LIBCXX_NAMESPACE::ymd::interval("1 week 2 days")).verboseString()
			  << std::endl;
		std::cout << LIBCXX_NAMESPACE::hms("1 week 2 days -1 hour").verboseString()
			  << std::endl;

		LIBCXX_NAMESPACE::hms testTime("19:23:30");

		testTime.toString(std::ostreambuf_iterator<char>
				  (std::cout.rdbuf()),
				  LIBCXX_NAMESPACE::locale::base::global(),
				  std::string("%l:%M:%S %p"));
		std::cout << std::endl;

		std::cout << testTime.format_time("%l:%M:%S %p")
			  << std::endl;

		std::cout << LIBCXX_NAMESPACE::tostring(testTime) << std::endl;

		{
			std::string s("1 week");

			LIBCXX_NAMESPACE::ymd::interval i(s);

			if (i.weeks != 1)
				throw EXCEPTION("1 week is not 1 week");
		}

		LIBCXX_NAMESPACE::ymd Feb1(2010, 02, 01);

		if (Feb1.format_date(U"%x", en_us) != U"02/01/2010")
			throw EXCEPTION("format_date(unicode) failed");


		if (LIBCXX_NAMESPACE::ymd("2010-02-01") != Feb1)
			throw EXCEPTION("YYYY-MM-DD is not parsed properly");

		if (LIBCXX_NAMESPACE::ymd("01-02-2010", de_DE) != Feb1)
			throw EXCEPTION("DD-MM-YYYY is not parsed properly");

		if (LIBCXX_NAMESPACE::ymd("02/01/2010", en_us) != Feb1)
			throw EXCEPTION("MM/DD/YYYY is not parsed properly");

		if (LIBCXX_NAMESPACE::ymd("01-Feb-2010") != Feb1)
			throw EXCEPTION("DD-MON-YYYY is not parsed properly");

		if (LIBCXX_NAMESPACE::ymd("February 1, 2010") != Feb1)
			throw EXCEPTION("MONTH DAY, YEAR is not parsed properly");
		if (LIBCXX_NAMESPACE::ymd("2010-W1-1") !=
		    LIBCXX_NAMESPACE::ymd(LIBCXX_NAMESPACE::ymd
					::iso8601(2010, 1, 1)))
			throw EXCEPTION("YYYY-W-D is not parsed properly");

		std::cout << (std::string)LIBCXX_NAMESPACE
			::ymdhms(std::string("Fri, 03 Aug 2012 19:30:31 GMT"),
				 LIBCXX_NAMESPACE::tzfile::base::utc())
			  << std::endl;

		std::cout << LIBCXX_NAMESPACE
			::ymdhms(std::string("Friday, 03-Aug-12 19:30:31 GMT"),
				 LIBCXX_NAMESPACE::tzfile::base::utc())
			  << std::endl;

		std::cout << LIBCXX_NAMESPACE
			::ymdhms(std::string("Fri Aug 3 19:30:31 2012"),
				 LIBCXX_NAMESPACE::tzfile::base::utc())
			  << std::endl;

		std::cout << LIBCXX_NAMESPACE::strftime::preferred(en_us)
			  << std::endl;

		std::cout << LIBCXX_NAMESPACE::strftime::preferred(de_DE)
			  << std::endl;

		std::cout << LIBCXX_NAMESPACE::strftime::preferred(es_ES)
			  << std::endl;
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
