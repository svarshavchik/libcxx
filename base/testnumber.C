/*
** Copyright 2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/number.H"

class foo;

typedef LIBCXX_NAMESPACE::number<int, foo> number_t;

void testnumber()
{
	number_t n1;
	number_t n2(4);

	n1=n2=n1;

	n2=5;
	n1=n2+4;
	n1=n2-4;
	n1=n2*4;
	n1=n2/4;
	n1=n2%4;

	n2=5;
	n1=n2+n2;
	n1=n2-n2;
	n1=n2*n2;
	n1=n2/n2;
	n1=n2%n2;

	n2=5;
	n1= n2 += 4;
	n1= n2 -= 4;
	n1= n2 *= 4;
	n1= n2 /= 4;
	n1= n2 %= 4;

	n2=5;
	n1= n2 += n2;
	n1= n2 -= n2;

	n2=5;
	n1= n2 *= n2;
	n1= n2 /= n2;
	n1= n2 %= n2;
}

int main()
{
	testnumber();
	return 0;
}
