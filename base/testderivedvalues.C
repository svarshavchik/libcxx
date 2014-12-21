/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/derivedvalue.H"

#include <iostream>
#include <vector>
#include <string>

static std::vector<std::string> testderivedvalue()
{

	std::vector<std::string> values;

	LIBCXX_NAMESPACE::derivedvalue<int, std::string> derived=
		LIBCXX_NAMESPACE::derivedvaluelist<int>
		::create([]
			 {
				 return 0;
			 },
			 []
			 (int &sum, const int &v)
			 {
				 sum += v;
			 },
			 []
			 (const int &sum)
			 {
				 std::ostringstream o;

				 o << sum*2;
				 return o.str();
			 });

	auto handler=({

			LIBCXX_NAMESPACE::derivedvalue<int, std::string>
				::base::vipobj_t
				::handlerlock lock(*derived);

			lock.install_front([&]
					   (const std::string &new_value)
					   {
						   std::cout << new_value
							     << std::endl;
						   values.push_back(new_value);
					   }, "9");
		});

	LIBCXX_NAMESPACE::derivedvaluelist<int>::base::current_value_t
		a=derived->create(1);

	{
		LIBCXX_NAMESPACE::derivedvaluelist<int>::base::current_value_t
			b=derived->create(2);

		a->update(3);
	}

	return values;
}

int main(int argc, char **argv)
{
	try {
		if (testderivedvalue() !=
		    std::vector<std::string>({"9", "2", "6", "10", "6", "0"}))
			throw EXCEPTION("failed");
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testderivedvalues: "
			  << e << std::endl;
	}
	return (0);
}

