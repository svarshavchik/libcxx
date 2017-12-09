#ifdef x_obj_H_included
#ifdef x_ref_H_included

template<typename functor_type>
ref<obj::destroyCallbackObj> obj::create_ondestroy(functor_type &&functor)
{
	// If the passed functor is itself a reference, it is
	// a near certainty that this is wrong.

	typedef typename std::enable_if<
		! std::is_lvalue_reference<functor_type>::value &&
		! std::is_rvalue_reference<functor_type>::value,
		functor_type>::type functor_cannot_be_a_reference
		__attribute__((unused));

	return ref<functorCallbackObj< functor_type > >
		::create(std::forward<functor_type>(functor));
}

template<typename objClass, typename baseClass>
inline const_ref<objClass, baseClass> &const_ref<objClass, baseClass>
::operator=(const const_ref<objClass, baseClass> &o) noexcept
{
	obj *prev=p.refP;

	p.refP=o.p.refP;

	++p.refP->obj::refcnt;

	if (--prev->refcnt == 0)
		prev->destroy();

	return *this;
}

#endif
#endif
