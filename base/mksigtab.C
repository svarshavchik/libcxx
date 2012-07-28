/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

#include <signal.h>

int main()
{
	std::vector<const char *> s;

#define make(v,n) if (s.size() <= v) s.resize(v+1); \
	if (s[v] == 0) s[v]=n;

#include "mksiglist.h"

	std::cout << "static const char sigtab[]=\"";

	const char *z="";

	for (size_t i=0; i<s.size(); ++i)
		if (s[i])
		{
			std::cout << z << s[i];
			z="\\0";
		}

	std::cout << "\";\nstatic const int sigoff[]={";

	z="";

	size_t p=0;

	for (size_t i=0; i<s.size(); ++i)
	{
		std::cout << z;
		z=",";

		if (s[i])
		{
			std::cout << p;
			p += strlen(s[i])+1;
		}
		else
			std::cout << "-1";
	}

	std::cout << "};\nstatic const size_t nsigs=sizeof(sigoff)/sizeof(sigoff[0]);\n";
	return (0);
}



