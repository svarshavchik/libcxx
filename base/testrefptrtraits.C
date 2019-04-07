/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/refptr_traits.H"
#include "x/obj.H"
#include <type_traits>

class obj : public LIBCXX_NAMESPACE::obj {};

class base : public LIBCXX_NAMESPACE::ptrref_base {};

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

	is_same1 *p1=nullptr;

	typedef typename std::enable_if<
		std::is_same<typename traits_t::ptr_t,
			     ptr<obj, base>>::value>::type is_same2;

	is_same2 *p2=nullptr;

	typedef typename std::enable_if<
		std::is_same<typename traits_t::const_ref_t,
			     const_ref<obj, base>>::value>::type is_same3;

	is_same3 *p3=nullptr;
	typedef typename std::enable_if<
		std::is_same<typename traits_t::const_ptr_t,
			     const_ptr<obj, base>>::value>::type is_same4;

	is_same4 *p4=nullptr;

	if (p1 && p2 && p3 && p4)
		;
}

void check_all()
{
	check_me<ref<obj, base>>();
	check_me<ptr<obj, base>>();
	check_me<const_ref<obj, base>>();
	check_me<const_ptr<obj, base>>();
}

using LIBCXX_NAMESPACE::ref_cast;
using LIBCXX_NAMESPACE::ptr_cast;

typedef typename std::enable_if<
	std::is_same<ref<obj, base>, ref_cast<ref<obj, base>>
		     >::value>::type cast1;

typedef typename std::enable_if<
	std::is_same<ref<obj, base>, ref_cast<ptr<obj, base>>
		     >::value>::type cast2;

typedef typename std::enable_if<
	std::is_same<const_ref<obj, base>, ref_cast<const_ref<obj, base>>
		     >::value>::type cast1;

typedef typename std::enable_if<
	std::is_same<const_ref<obj, base>, ref_cast<const_ptr<obj, base>>
		     >::value>::type cast2;


typedef typename std::enable_if<
	std::is_same<ptr<obj, base>, ptr_cast<ref<obj, base>>
		     >::value>::type cast1;

typedef typename std::enable_if<
	std::is_same<ptr<obj, base>, ptr_cast<ptr<obj, base>>
		     >::value>::type cast2;

typedef typename std::enable_if<
	std::is_same<const_ptr<obj, base>, ptr_cast<const_ref<obj, base>>
		     >::value>::type cast1;

typedef typename std::enable_if<
	std::is_same<const_ptr<obj, base>, ptr_cast<const_ptr<obj, base>>
		     >::value>::type cast2;

int main()
{
	return 0;
}
