/*
** Copyright 2014-2021 Double Precision, Inc.
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
		LIBCXX_NAMESPACE::derivedvalues<int>
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
			b=derived->create((int)2);

		a->update(3);
		a->update(3);
	}

	return values;
}

void make_sure_this_compiles(const LIBCXX_NAMESPACE::derivedvalue<int,
			     std::string> &derived,
			     LIBCXX_NAMESPACE::derivedvaluelist<int>::base
			     ::current_value_t &value)

{
	int n=1;
	derived->create(n);
	derived->emplace(1);

	value->update(n);
	value->update(1);

	typedef LIBCXX_NAMESPACE::derivedvalue<int, std::string>::base::vipobj_t
		vipobj_t;

	vipobj_t::handlerlock lock(*derived);

	lock.install_back([]
			  (const std::string &new_value)
			  {
			  },

			  (std::string)*vipobj_t::readlock(*derived));





	auto ptr=LIBCXX_NAMESPACE::derivedvalues<int>
		::create([]
			 {
				 return 0;
			 },
			 []
			 (int &sum, const int &v)
			 {
			 },
			 []
			 (const int &sum)
			 {
				 return sum;
			 });

	decltype(ptr)::base::vipobj_t *ptr_p;

	ptr_p=&*ptr;

	++ptr_p;
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
