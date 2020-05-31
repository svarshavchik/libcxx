#ifdef x_obj_H_included
#ifdef x_ref_H_included
#ifdef x_ondestroy_H_included

template<typename functor_type>
inline ondestroy_cb_t obj::create_ondestroy(functor_type &&functor)
{
	// If the passed functor is itself a reference, it is
	// a near certainty that this is wrong, except in few specific
	// cases.

	static_assert(
		std::is_same_v<functor_type, void (&)()> ||
		std::is_same_v<std::remove_cvref_t<functor_type>,
			       functionref<void ()>> ||
		(! std::is_lvalue_reference<functor_type>::value &&
		 ! std::is_rvalue_reference<functor_type>::value),
		"Functor cannot be a reference");

	return std::forward<functor_type>(functor);
}

#endif
#endif
#endif
