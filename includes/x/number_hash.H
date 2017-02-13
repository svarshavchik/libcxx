/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_number_hash_H
#define x_number_hash_H

#include <functional>
#include <x/number.H>

namespace std {

	//! Specialize \c std::hash for integer \c INSERT_LIBX_NAMESPACE::number

	template<typename value_type_t, typename tag,
		 typename base_class>
	struct hash<LIBCXX_NAMESPACE::number<
			    value_type_t,
			    tag, base_class>> {

		typedef typename
		std::enable_if<std::is_integral<value_type_t>::value
			       >::type only_for_integers;

		inline size_t operator()(const
					 LIBCXX_NAMESPACE::number<value_type_t,
					 tag, base_class> &n) const noexcept
		{
			return n.n;
		}
	};
};


#endif
