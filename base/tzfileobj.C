/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/dirwalk.H"
#include "x/singleton.H"
#include "x/tzfile.H"
#include "x/ymd.H"
#include "x/ymdhmsbase.H"
#include "x/exception.H"
#include "x/vector.H"
#include "x/fileattr.H"
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <set>
#if HAVE_ENDIAN_H
#include <endian.h>
#endif
#if HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif
namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

#include "tz_internal.H"

#ifndef TZ_LOCALFILE
#define TZ_LOCALFILE	"/etc/localtime"
#endif

#ifndef TZ_SYSTEM_CONFIG_SYSDATE
#define TZ_SYSTEM_CONFIG_SYSDATE	"/etc/sysconfig/clock"
#endif

const char tzfileObj::localtime[]=TZ_LOCALFILE;
const char tzfileObj::localnameid[]="[local]";

class tzfileBase::localtz : virtual public obj {

public:
	tzfile localInstance;

	localtz();
	~localtz() noexcept;
};

singleton<tzfileBase::localtz> tzfileBase::localtzInstance;

tzfileBase::localtz::localtz()
	: localInstance(tzfile::create(tzfile::base::localname()))
{
}

tzfileBase::localtz::~localtz() noexcept
{
}

class tzfileBase::utctz : virtual public obj {

public:
	tzfile utcInstance;

	utctz();
	~utctz() noexcept;
};

singleton<tzfileBase::utctz> tzfileBase::utctzInstance;

tzfileBase::utctz::utctz()
	: utcInstance(tzfileObj::utc())
{
}

tzfileBase::utctz::~utctz() noexcept
{
}

tzfile tzfileBase::local()
{
	ptr<localtz> l(localtzInstance.get());

	if (!l.null())
		return l->localInstance;

	return tzfile::create(tzfile::base::localname());
}

tzfile tzfileBase::utc()
{
	ptr<utctz> l(utctzInstance.get());

	if (!l.null())
		return l->utcInstance;

	return tzfileObj::utc();
}

template<>
class tzfileObj::frombe<int16_t> : public std::unary_function<uint16_t,
							      uint16_t> {

public:
	uint16_t operator()(uint16_t arg) __attribute__((const))
	{
		return be16toh(arg);
	}
};

template<>
class tzfileObj::frombe<int32_t> : public std::unary_function<uint32_t,
							      uint32_t> {

public:
	uint32_t operator()(uint32_t arg) __attribute__((const))
	{
		return be32toh(arg);
	}
};

template<>
class tzfileObj::frombe<int64_t> : public std::unary_function<uint64_t,
							      uint64_t> {

public:
	uint64_t operator()(uint64_t arg) __attribute__((const))
	{
		return be64toh(arg);
	}
};

ymd tzfileObj::tzinfo::getDayOfGivenYear(int32_t year) const
{
	if (start_format == 'M')
	{
		ymd curday(year, m, 1);

		curday += (d + 7 - curday.getDayOfWeek()) % 7;

		curday += (w-1) * 7;

		return curday;
	}

	ymd curday(year, 1, 1);

	if (start_format == 'J')
	{
		if (d <= 31+28)
		{
			curday += d-1;
		}
		else
		{
			curday += 31+28;

			if (curday.getDay() == 29)
				++curday;

			curday += d-31-28-1;
		}
	}
	else
	{
		curday += d-1;
	}

	return curday;
}

struct __attribute__((packed)) tzfileObj::ttinfo_file {
	int32_t tt_gmtoff;
	unsigned char tt_isdst;
	unsigned char tt_abbrind;
};

tzfileObj::ttinfo_s::ttinfo_s(const ttinfo_file &f) noexcept
{
	operator=(f);
}


template<typename int_type>
class __attribute__((packed)) tzfileObj::leaps_file {
public:
	int_type first;
	int32_t second;
};

template<typename int_type>
tzfileObj::leaps_s::leaps_s(const leaps_file<int_type> &ptr) noexcept
{
	operator=(ptr);
}

template<typename int_type>
tzfileObj::leaps_s &tzfileObj::leaps_s::operator=(const leaps_file<int_type>
						  &ptr)
	noexcept
{
	first=frombe<int_type>()(ptr.first);
	second=frombe<int32_t>()(ptr.second);
	return *this;
}


void tzfileObj::badtzfileObj(const std::string &tzname)
{
	throw EXCEPTION(tzname + ": corrupted timezone file");
}

tzfileObj::tzfileObj() : transitions(vector<time_t>::create()),
			 ttinfo_idx(vector<unsigned char>::create()),
			 ttinfo(vector<struct ttinfo_s>::create()),
			 tzstr(vector<char>::create()),
			 leaps(vector<leaps_s>::create()),
			 tz_alt_start(tzinfo()),
			 tz_alt_end(tzinfo())
{
	init_utc();
}

tzfile tzfileObj::utc()
{
	return tzfile::create();
}

void tzfileObj::init_utc()
{
	char UTC[4]="UTC";

	tzstr->clear();
	ttinfo->clear();

	tzstr->insert(tzstr->end(), UTC, UTC+4);

	ttinfo_s tt=ttinfo_s();

	tt.tz_str=&(*tzstr)[0];

	ttinfo->push_back(tt);

	tz_alt_start.tzname=tz_alt_end.tzname=UTC;
	name="";
}

std::string tzfileObj::localname()
{
	const char *tz=getenv("TZ");

	if (tz && *tz)
		return tz;

	{
		std::ifstream scd(TZ_SYSTEM_CONFIG_SYSDATE);

		std::string line;

		while (scd.is_open() && !std::getline(scd, line).eof())
		{
			size_t equal=line.find('=');

			if (equal == line.npos ||
			    line.substr(0, ++equal) != "ZONE=")
				continue;

			std::string::iterator b=line.begin() + equal,
				e=line.end();

			if (b != e)
			{
				if (*b == '"')
					++b;

				std::string::iterator
					p=std::find(b, e, '"');

				std::replace(b, p, ' ', '_');
				std::string zone(b, p);

				try {
					fileattr::create(tzfile::base::tzdir()
							 + "/" + zone,
							 O_RDONLY)
						->stat();
					return zone;

				} catch (exception &e)
				{
				}
			}
			break;
		}
	}

	// Maybe /etc/localtime is a symlink

	try {
		std::string link=
			fd::base::combinepath("/etc/localtime",
					      fileattr::create("/etc/localtime",
							       true)
					      ->readlink());

		auto pfix=tzfile::base::tzdir() + "/";

		if (link.substr(0, pfix.size()) == pfix)
			return link.substr(pfix.size());
	} catch (exception &e)
	{
	}

	return localnameid;
}

std::string tzfileBase::tzdir() noexcept
{
	const char *p=getenv("TZDIR");

	if (!p || !*p)
		p="/usr/share/zoneinfo";

	return p;
}

void tzfileBase::enumerate(std::set<std::string> &set)
{
	std::string dirname(tzdir());

	dirwalk dirnamewalk=dirwalk::create(dirname);

	for (auto direntry:*dirnamewalk)
	{
		if (direntry.filetype() != DT_REG)
			continue;

		if ( ((std::string)direntry).find('.') != std::string::npos)
			continue;

		set.insert(direntry.fullpath().substr(dirname.size()+1));
	}
}

tzfileObj::tzfileObj(const std::string &tzname)
	: transitions(vector<time_t>::create()),
	  ttinfo_idx(vector<unsigned char>::create()),
	  ttinfo(vector<struct ttinfo_s>::create()),
	  tzstr(vector<char>::create()),
	  leaps(vector<leaps_s>::create())
{
	load_file(tzname);
}

void tzfileObj::load_file(const std::string &tzname)
{
	if (tzname.find('.') != tzname.npos)
	{
	notfound:

		throw EXCEPTION(tzname + ": no such timezone");
	}

	if (tzname == "")
	{
		init_utc();
		return;
	}

	bool islocal=tzname == localnameid;

	std::string filename=islocal ? TZ_LOCALFILE:
		tzfile::base::tzdir() + "/" + tzname;

	bool opened=false;

	try {
		fd tzfileObj(fd::base::open(filename, O_RDONLY));
		opened=true;
		load_file(tzfileObj, tzname);

	} catch (const exception &e)
	{
		if (opened)
			throw;
		if (islocal)
		{
			init_utc();
			return;
		}
		goto notfound;
	}
}

void tzfileObj::load_file(fd &tzfileObj,
			  const std::string &tzname)
{
	tz_alt_start=tzinfo();
	tz_alt_end=tzinfo();

	istream i(tzfileObj->getistream());

	int v=parse_tzhead<int32_t>(tzname, i, true);

	if (v == '2')
	{
		v=parse_tzhead<int64_t>(tzname, i, sizeof(time_t) == 8);

		if (i->get() == '\n')
		{
			std::string tzstring;

			std::getline(*i, tzstring);
			parse_tzstring(tzstring);
		}
	}
}

void tzfileObj::parse_tzstring(const std::string &tzstring)
{
	std::string::const_iterator b=tzstring.begin(), e=tzstring.end();

	if (parse_tzname(b, e, tz_alt_end.tzname))
	{
		if (parse_tztod(b, e, tz_alt_end.offset))
		{
			tz_alt_end.offset *= -1;

			if (b == e)
			{
				tz_alt_start=tz_alt_end;
				return;
			}

			if (!parse_tzname(b, e, tz_alt_start.tzname))
			{
			bad:

				tz_alt_start=tzinfo();
				tz_alt_end=tzinfo();
				return;
			}

			tz_alt_start.offset=tz_alt_end.offset + 60 * 60;
			tz_alt_start.tod=2 * 60 * 60;
			tz_alt_start.start_format='M';
			tz_alt_start.m=3;
			tz_alt_start.w=2;
			tz_alt_start.d=0;
			tz_alt_end.tod=2 * 60 * 60;
			tz_alt_end.start_format='M';
			tz_alt_end.m=11;
			tz_alt_end.w=1;
			tz_alt_end.d=0;

			if (b == e)
				return;

			if (*b++ != ',')
				goto bad;

			if (!parse_tzwhen(b, e, tz_alt_start))
				goto bad;

			if (b == e || *b++ != ',')
				goto bad;

			if (!parse_tzwhen(b, e, tz_alt_end))
				goto bad;

			if (b != e)
				goto bad;
		}
	}
}

bool tzfileObj::parse_tzname(std::string::const_iterator &b,
			     std::string::const_iterator e,
			     std::string &s) noexcept
{
	if (b == e)
		return false;

	std::string::const_iterator p=b;

	if (*p == '<')
	{
		std::string::const_iterator q= ++p;

		while (p != e &&
		       ((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z') ||
			(*p >= '0' && *p <= '9') || *p == '+' || *p == '-'))
			++p;
		if (p == e || *p != '>' || p-q < 3)
			return false;

		s=std::string(q, p);
		b= ++p;
		return true;
	}

	while (p != e && ((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z')))
		++p;

	if (p != b && p-b >= 3)
	{
		s=std::string(b, p);
		b=p;
		return true;
	}
	return false;
}

bool tzfileObj::parse_tztod(std::string::const_iterator &b,
			    std::string::const_iterator e,
			    int32_t &n) noexcept
{
	n=1;

	if (b != e)
	{
		if (*b == '+')
			++b;
		else if (*b == '-')
		{
			n= -1;
			++b;
		}
	}

	int32_t h;

	if (!parse_tztodnum(b, e, h))
		return false;

	if (h > 23)
		return false;

	h *= 60 * 60;

	if (b != e && *b == ':')
	{
		++b;

		int32_t m;

		if (!parse_tztodnum(b, e, m))
			return false;

		if (m > 59)
			return false;
		h +=  m * 60;

		if (b != e && *b == ':')
		{
			++b;
			if (!parse_tztodnum(b, e, m))
				return false;
			h += m;
		}
	}

	n *= h;
	return true;
}

bool tzfileObj::parse_tztodnum(std::string::const_iterator &b,
			       std::string::const_iterator e,
			       int32_t &n) noexcept
{
	std::string::const_iterator p=b;

	if (p != e && *p >= '0' && *p <= '9')
	{
		n= *p - '0';

		++p;

		if (p != e && *p >= '0' && *p <= '9')
			n=n*10 + (*p++ - '0');

		b=p;
		return true;
	}
	return false;
}

bool tzfileObj::parse_tzwhen(std::string::const_iterator &b,
			     std::string::const_iterator e,
			     tzinfo &t) noexcept
{
	if (b == e)
		return false;

	switch (t.start_format=*b) {
	case 'M':
		++b;
		if (!parse_tztodnum(b, e, t.m) || t.m < 1 || t.m > 12)
			return false;
		if (b == e || *b++ != '.')
			return false;
		if (!parse_tztodnum(b, e, t.w) || t.w < 1 || t.w > 5)
			return false;
		if (b == e || *b++ != '.')
			return false;
		if (!parse_tztodnum(b, e, t.d) || t.d < 0 || t.d > 6)
			return false;
		break;
	case 'J':
		++b;
		if (!parse_tztodnum(b, e, t.d) || t.d < 1 || t.d > 365)
			return false;
		break;
	default:
		t.start_format=0;
		if (!parse_tztodnum(b, e, t.d) || t.d < 1 || t.d > 366)
			return false;
		break;
	}

	if (b == e || *b != '/')
		return true;
	++b;

	return parse_tztod(b, e, t.tod);
}

template<typename time_t_type>
int tzfileObj::parse_tzhead(const std::string &tzname,
			   istream &i,
			   bool parse)
{
	int version;

	uint32_t tzh_ttisgmtcnt,
		tzh_ttisstdcnt,
		tzh_leapcnt,
		tzh_timecnt,
		tzh_typecnt,
		tzh_charcnt;

	name=tzname;

	{
		struct  __attribute__((packed)) tzhdr_raw {
			char tzif[4];
			char tzh_version;
			char pad[15];
			int32_t counters[6];
		} tzhdr_rawBuf;

		i->read((char *)&tzhdr_rawBuf, sizeof(tzhdr_rawBuf));

		if (i->gcount() != sizeof(tzhdr_rawBuf) ||
		    memcmp(&tzhdr_rawBuf, "TZif", 4) != 0)
			badtzfileObj(tzname);

		version=tzhdr_rawBuf.tzh_version;

		std::transform(&tzhdr_rawBuf.counters[0],
			       &tzhdr_rawBuf.counters[6],
			       &tzhdr_rawBuf.counters[0],
			       frombe<int32_t>());

		tzh_ttisgmtcnt=tzhdr_rawBuf.counters[0];
		tzh_ttisstdcnt=tzhdr_rawBuf.counters[1];
		tzh_leapcnt=tzhdr_rawBuf.counters[2];
		tzh_timecnt=tzhdr_rawBuf.counters[3];
		tzh_typecnt=tzhdr_rawBuf.counters[4];
		tzh_charcnt=tzhdr_rawBuf.counters[5];
	}

	if (!parse)
	{
		size_t skip=
			tzh_timecnt * sizeof(time_t_type) +
			tzh_timecnt +
			tzh_typecnt * sizeof(ttinfo_file) +
			tzh_charcnt +
			tzh_leapcnt * (sizeof(time_t_type) + sizeof(int32_t)) +
			tzh_ttisstdcnt +
			tzh_ttisgmtcnt;

		while (skip)
		{
			if (i->get() == std::istream::traits_type::eof())
				return -1;
			--skip;
		}
		return version;
	}

	transitions=vector<time_t>::create();
	ttinfo_idx=vector<unsigned char>::create();

	if (tzh_timecnt > 0)
	{
		{
			time_t_type transitionsbuf[tzh_timecnt];

			i->read((char *)&transitionsbuf[0],
				tzh_timecnt*sizeof(time_t_type));

			if ((size_t)i->gcount() != tzh_timecnt*sizeof(time_t_type))
				badtzfileObj(tzname);

			std::transform(&transitionsbuf[0],
				       &transitionsbuf[tzh_timecnt],
				       &transitionsbuf[0],
				       frombe<time_t_type>());

			transitions=vector<time_t>
				::create(&transitionsbuf[0],
					 &transitionsbuf[tzh_timecnt]);
		}

		{
			unsigned char ttinfo_idxbuf[tzh_timecnt];

			i->read((char *)&ttinfo_idxbuf[0], tzh_timecnt);

			if ((uint32_t)i->gcount() != tzh_timecnt)
				badtzfileObj(tzname);

			ttinfo_idx=vector<unsigned char>
				::create(&ttinfo_idxbuf[0],
					 &ttinfo_idxbuf[tzh_timecnt]);
		}
	}

	if (tzh_typecnt == 0)
		badtzfileObj(tzname);

	{
		ttinfo_file ttinfo_buf[tzh_typecnt];

		i->read((char *)&ttinfo_buf[0],
			tzh_typecnt * sizeof(ttinfo_buf[0]));

		if ((size_t)i->gcount() != tzh_typecnt * sizeof(ttinfo_buf[0]))
			badtzfileObj(tzname);


		for (uint32_t ii=0; ii<tzh_typecnt; ii++)
			ttinfo_buf[ii].tt_gmtoff=
				frombe<int32_t>()
				(ttinfo_buf[ii].tt_gmtoff);

		if (tzh_charcnt == 0)
			tzstr=vector<char>::create();
		else
		{
			char tzstr_buf[tzh_charcnt+1];

			i->read(tzstr_buf, tzh_charcnt);

			if ((uint32_t)i->gcount() != tzh_charcnt)
				badtzfileObj(tzname);

			if (tzstr_buf[tzh_charcnt-1])
				tzstr_buf[tzh_charcnt++]=0;

			tzstr=vector<char>
				::create(&tzstr_buf[0],
					 &tzstr_buf[tzh_charcnt]);
		}

		ttinfo=vector<ttinfo_s>::create();

		ttinfo->resize(tzh_typecnt);

		ttinfo_file *ttinfo_buf_b=ttinfo_buf,
			*ttinfo_buf_e=ttinfo_buf + tzh_typecnt;
		std::vector<ttinfo_s>::iterator ttinfo_s_iter=ttinfo->begin();

		while (ttinfo_buf_b != ttinfo_buf_e)
		{
			ttinfo_s_iter->tt_gmtoff=ttinfo_buf_b->tt_gmtoff;
			ttinfo_s_iter->tt_isdst=ttinfo_buf_b->tt_isdst != 0;

			ttinfo_s_iter->tz_str=
				ttinfo_buf_b->tt_abbrind < tzstr->size()
				? &(*tzstr)[ttinfo_buf_b->tt_abbrind]:"GMT";

			ttinfo_s_iter->isstd=false;
			ttinfo_s_iter->isgmt=false;

			++ttinfo_buf_b;
			++ttinfo_s_iter;
		}
	}

	if (tzh_leapcnt == 0)
		leaps=vector<leaps_s>::create();
	else
	{
		leaps_file<time_t_type> leapsbuf[tzh_leapcnt];

		i->read((char *)&leapsbuf[0], tzh_leapcnt*sizeof(leapsbuf[0]));

		if ((size_t)i->gcount() != tzh_leapcnt*sizeof(leapsbuf[0]))
			badtzfileObj(tzname);

		leaps=vector<leaps_s>::create( &leapsbuf[0],
					       &leapsbuf[tzh_leapcnt]);
	}

	if (tzh_ttisstdcnt > 0)
	{
		char stdflag_buf[tzh_ttisstdcnt];

		i->read(stdflag_buf, tzh_ttisstdcnt);

		if ((uint32_t)i->gcount() != tzh_ttisstdcnt)
			badtzfileObj(tzname);

		size_t i;

		for (i=0; i<tzh_ttisstdcnt && i < ttinfo->size(); ++i)
			(*ttinfo)[i].isstd=stdflag_buf[i] != 0;
	}

	if (tzh_ttisgmtcnt > 0)
	{
		char gmtflag_buf[tzh_ttisgmtcnt];

		i->read(gmtflag_buf, tzh_ttisgmtcnt);

		if ((uint32_t)i->gcount() != tzh_ttisgmtcnt)
			badtzfileObj(tzname);

		size_t i;

		for (i=0; i<tzh_ttisgmtcnt && i < ttinfo->size(); ++i)
			(*ttinfo)[i].isgmt=gmtflag_buf[i] != 0;
	}
	return version;
}

tzfileObj::~tzfileObj() noexcept
{
}

class tzfileObj::leap_search : std::binary_function<leaps_s, time_t, bool>
{
public:
	bool operator()(const leaps_s &l,
			const time_t &r) const noexcept
	{
		return l.first < r;
	}

	bool operator()(const time_t &l,
			const leaps_s &r) const noexcept
	{
		return l < r.first;
	}
};

void tzfileObj::compute_transition(int32_t year,
				   time_t &alt_start,
				   time_t &alt_end) const
{
	if (tz_alt_end.tzname.size() == 0)
	{
		alt_start=alt_end=0;
		return;
	}

	ymd ymd_alt(tz_alt_start.getDayOfGivenYear(year));

	alt_start=compute_time_t(ymdhmsBase(ymd_alt,
					    hms(tz_alt_start.tod),
					    tz_alt_end.offset,
					    1,
					    tz_alt_start.tzname));

	ymd ymd_std(tz_alt_end.getDayOfGivenYear(year));

	alt_end=compute_time_t(ymdhmsBase(ymd_std,
					  hms(tz_alt_end.tod),
					  tz_alt_start.offset,
					  0,
					  tz_alt_end.tzname));
}

template<typename CallbackType>
void tzfileObj::compute_transition(time_t timeValue,
				   CallbackType &cb) const
{
	std::vector<time_t>::const_iterator tranb=transitions->begin(),
		trane=transitions->end(),
		tranptr=std::upper_bound(tranb, trane, timeValue);

	if (tranb == trane || (tranptr == trane && timeValue > tranptr[-1]))
	{
		int32_t year;

		ttinfo_s ttalt=ttinfo_s(), ttstd=ttinfo_s();

		ttalt.tt_gmtoff=tz_alt_start.offset;
		ttalt.tt_isdst=true;
		ttalt.tz_str=tz_alt_start.tzname.c_str();
		ttstd.tt_gmtoff=tz_alt_end.offset;
		ttstd.tt_isdst=false;
		ttstd.tz_str=tz_alt_end.tzname.c_str();

		if (ttalt.tt_gmtoff == ttstd.tt_gmtoff)
		{
			cb(ttstd);
			return;
		}

		{
			ymd get_year(ymdhmsBase::epoch());

			int dummy;

			get_year += (timeValue - compute_leapcnt(timeValue,
								dummy)
				     ) / (60 * 60 * 24);

			year=get_year.getYear();

		}

		time_t time_t_alt, time_t_std;

		compute_transition(year-1, time_t_alt, time_t_std);

		if (timeValue < time_t_alt)
		{
			cb(ttstd);
			return;
		}

		if (timeValue < time_t_std)
		{
			cb(ttalt);
			return;
		}
		compute_transition(year, time_t_alt, time_t_std);

		if (timeValue < time_t_alt)
		{
			cb(ttstd);
			return;
		}

		if (timeValue < time_t_std)
		{
			cb(ttalt);
			return;
		}

		compute_transition(year+1, time_t_alt, time_t_std);

		if (timeValue < time_t_alt)
		{
			cb(ttstd);
			return;
		}

		cb(ttalt);
		return;
	}

	cb(tranptr == tranb || (*ttinfo_idx)[--tranptr - tranb]
	   >= ttinfo->size() ? (*ttinfo)[0]
	   :(*ttinfo)[(*ttinfo_idx)[tranptr-tranb]]);
}

class tzfileObj::compute_ymdhms_getoffind {

public:
	int32_t gmtoff;
	signed char altzone;
	const char *tzstr;

	std::string tzname;

	compute_ymdhms_getoffind() noexcept : gmtoff(0), altzone(-1),
		tzstr("")
	{
	}

	void operator()(const ttinfo_s &s)
	{
		gmtoff=s.tt_gmtoff;
		altzone=s.tt_isdst;
		tzstr=s.tz_str;
	}
};

int32_t tzfileObj::compute_leapcnt(time_t timeValue,
				   int &leapcnt) const noexcept
{
	int32_t gmtoff=0;

	leapcnt=0;

	std::vector<leaps_s>::iterator
		leapb=leaps->begin(),
		leape=leaps->end(),
		leapp=std::lower_bound(leapb, leape, timeValue, leap_search());


	if (leapp > leapb)
		gmtoff = leapp[-1].second;

	if (leapp < leape && leapp->first == timeValue)
	{
		do
		{
			++leapcnt;

			if (leapp-- == leapb)
				break;

		} while (leapp[0].first + 1 == leapp[1].first &&
			 leapp[0].second + 1 == leapp[1].second);
	}

	return gmtoff;
}


ymdhmsBase tzfileObj::compute_ymdhms(time_t timeValue) const
{
	compute_ymdhms_getoffind tt;

	compute_transition(timeValue, tt);

	int32_t gmtoff=tt.gmtoff,
		orig_gmtoff=gmtoff,
		altzone=tt.altzone;

	int leapcnt;

	gmtoff -= compute_leapcnt(timeValue, leapcnt);

	timeValue += gmtoff-leapcnt;

	ymd::sdaynum_t ndays= timeValue / (60 * 60 * 24);

	int32_t nseconds= timeValue % (60 * 60 * 24);

	if (nseconds < 0)
	{
		--ndays;
		nseconds += 60 * 60 * 24;
	}

	hms hms_v(nseconds);

	hms_v.s += leapcnt;

	return ymdhmsBase( ymdhmsBase::epoch() + ymd::interval(ndays), hms_v,
			   orig_gmtoff, altzone, tt.tzstr);
}

ymdhmsBase tzfileObj::compute_ymdhms(const struct tm &timeValue)
	const
{
	return compute_ymdhms(compute_time_t(ymdhmsBase(ymd(timeValue.tm_year
							    + 1900,
							    timeValue.tm_mon+1,
							    timeValue.tm_mday),
							hms(timeValue.tm_hour,
							    timeValue.tm_min,
							    timeValue.tm_sec),
							timeValue.tm_gmtoff,
							timeValue.tm_isdst > 0,
							"UTC")));
}

ymdhmsBase tzfileObj::compute_ymdhms(const ymd &ymdVal,
				     const hms &hmsVal) const
{
	ymdhmsBase base(ymdVal, hmsVal, 0, 0, "GMT");

	time_t value=base.start_of_day_time_t_utc();

	time_t bias=(hmsVal.h * 60 + hmsVal.m) * 60;

	time_t tod=value + bias;

	if (bias > 0 ? tod < value:tod > value)
		base.internal_overflow();

	compute_ymdhms_getoffind tt;

	compute_transition(tod, tt);

	tod -= tt.gmtoff;

	compute_transition(tod, tt);

	return ymdhmsBase(ymdVal, hmsVal,
			  tt.gmtoff,
			  tt.altzone,
			  tt.tzstr);
}

time_t tzfileObj::compute_time_t(const ymdhmsBase &dt) const
{
	time_t nsecs=dt.start_of_day_time_t_utc();

	time_t bias=(dt.h * 60 + dt.m) * 60 - dt.getUTCoffset();

	time_t tod=nsecs + bias;

	if (bias > 0 ? tod < nsecs:tod > nsecs)
		dt.internal_overflow();

	int32_t prev_leaps=0;

	while (1)
	{
		std::vector<leaps_s>::iterator
			leapb=leaps->begin(),
			leape=leaps->end(),
			leapp=std::upper_bound(leapb, leape, tod,
					       leap_search());

		if (leapp == leapb)
			break;

		if ((--leapp)->second == prev_leaps)
			break;

		tod += leapp->second - prev_leaps;
		prev_leaps=leapp->second;
	}

	time_t timeValue = tod + dt.s;

	if (timeValue < tod)
		dt.internal_overflow();

	return timeValue;
}

#if 0
{
#endif
}
