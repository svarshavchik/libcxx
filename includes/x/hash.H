/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_hash_H
#define x_hash_H

#include <functional>

//! std::hash<void> specialization is a transparent hash.

template<>
struct std::hash<void> {

	template<typename T>
	decltype(std::hash<std::remove_cvref_t<T>>{}(
			 std::declval<std::remove_cvref_t<T> &>()
		 )) operator()(T && t) const
	{
		return std::hash<std::remove_cvref_t<T>>{}(std::forward<T>(t));
	}

	typedef void is_transparent;
};

#endif
