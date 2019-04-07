/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

struct tzfileObj::ttinfo_s {
	int32_t tt_gmtoff;
	bool tt_isdst;

	const char *tz_str;
	bool isstd;

	bool isgmt;

	ttinfo_s(const ttinfo_file &f) throw() LIBCXX_INTERNAL;

	ttinfo_s() throw() : tt_gmtoff(0), tt_isdst(0),
		tz_str(0), isstd(0), isgmt(0) {}
};


struct __attribute__((packed)) tzfileObj::leaps_s {
	time_t first;
	int32_t second;

	template<typename int_type>
		leaps_s(const leaps_file<int_type> &) throw() LIBCXX_INTERNAL;

	template<typename int_type>
		leaps_s &operator=(const leaps_file<int_type> &)
		throw() LIBCXX_INTERNAL;
};
