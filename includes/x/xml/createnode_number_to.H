/*
** Copyright 2020-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN
#ifdef x_number_H
#ifdef x_xml_createnodeobj_H

//! Specialization for \ref number "typesafe numerical aliases".

//! Implements writelock->text(n) for all number<>s.

template<typename value_type_t, typename tag, typename base_class>
struct createnode_ret_t<number<value_type_t, tag, base_class>, void> {

	//! Return type for SFINAE
	typedef createnode type_t;

	//! Generate a text node from a numeric value.
	static inline createnode text(createnodeObj &me,
				      number<value_type_t, tag, base_class> n)
	{
		return createnode_ret_t<value_type_t>::text(me, n.n);
	}
};

#endif
#endif
#endif
