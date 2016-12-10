/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/refptr_traits.H"
#include "x/obj.H"
#include <type_traits>

class obj : public LIBCXX_NAMESPACE::obj {};

class base : public LIBCXX_NAMESPACE::ptrrefBase {};

using LIBCXX_NAMESPACE::ref;
using LIBCXX_NAMESPACE::ptr;
using LIBCXX_NAMESPACE::const_ref;
using LIBCXX_NAMESPACE::const_ptr;

template<typename check_type>
void check_me()
{
	typedef LIBCXX_NAMESPACE::refptr_traits<check_type> traits_t;

	typedef typename std::enable_if<
		std::is_same<typename traits_t::ref_t,
			     ref<obj, base>>::value>::type is_same1;

	typedef typename std::enable_if<
		std::is_same<typename traits_t::ptr_t,
			     ptr<obj, base>>::value>::type is_same2;

	typedef typename std::enable_if<
		std::is_same<typename traits_t::const_ref_t,
			     const_ref<obj, base>>::value>::type is_same3;

	typedef typename std::enable_if<
		std::is_same<typename traits_t::const_ptr_t,
			     const_ptr<obj, base>>::value>::type is_same4;
}

void check_all()
{
	check_me<ref<obj, base>>();
	check_me<ptr<obj, base>>();
	check_me<const_ref<obj, base>>();
	check_me<const_ptr<obj, base>>();
}

int main()
{
	return 0;
}
