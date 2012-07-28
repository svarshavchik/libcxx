/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "ymd2.C"

#include <iostream>

int main(int argc, char **argv)
{
	std::cout <<
		LIBCXX_NAMESPACE::ymd
		::compute_daynum(LIBCXX_NAMESPACE::ymd::gregorian_reformation_date_year,
				 LIBCXX_NAMESPACE::ymd::gregorian_reformation_date_month,
				 LIBCXX_NAMESPACE::ymd::gregorian_reformation_date_day)
		  << std::endl;
	return (0);
}
