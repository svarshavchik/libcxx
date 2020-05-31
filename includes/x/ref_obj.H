#ifdef x_obj_H_included
#ifdef x_ref_H_included

//! Copy construct from a ref to a different object.

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline const_ref<objClass, baseClass>::const_ref(const const_ref<objClass2, baseClass2> &o)

{
	p.refP=convert<objClass2, objClass, true>::cast(o.p.refP);

	if (p.refP == 0)
		invalid_cast(typeid(*o.p.refP).name(),
			     typeid(objClass).name());

	obj &oo=*p.refP;

	++oo.refcnt;
}

//! Copy constructor from a nullable pointer to a different object.

template<typename objClass, typename baseClass>
template<typename objClass2>
inline const_ref<objClass, baseClass>::const_ref(const ptrImpl<objClass2> &o)
{
	if (!o.refP)
		null_ptr_deref();

	p.refP=convert<objClass2, objClass, true>::cast(o.refP);

	if (p.refP == 0)
		invalid_cast(typeid(*o.refP).name(),
			     typeid(objClass).name());

	obj &oo=*p.refP;

	++oo.refcnt;
}

//! Move constructor from a nullable pointer to a different object.

template<typename objClass, typename baseClass>
template<typename objClass2>
inline const_ref<objClass, baseClass>::const_ref(ptrImpl<objClass2> &&o)
{
	if (!o.refP)
		null_ptr_deref();

	p.refP=convert<objClass2, objClass, false>::cast(o.refP);

	if (p.refP == 0)
		invalid_cast(typeid(*o.refP).name(),
			     typeid(objClass).name());

	o.refP=nullptr;
}

//! The assignment operator

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

template<typename objClass, typename objClass2>
objClass *ref_to(objClass2 *p) LIBCXX_INLINE;

//! Helper used by the \ref constructor.
template<typename objClass, typename objClass2>
inline objClass *ref_to(objClass2 *p)
{
	auto q=convert<objClass2, objClass, false>::cast(p);

	if (!q)
		invalid_cast(typeid(*p).name(),
			     typeid(objClass).name());

	return q;
}

//! Construct from a ref to a different object.

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ref<objClass, baseClass>::ref(const ref<objClass2, baseClass2> &o)
	: const_ref<objClass, baseClass>(ref_to<objClass, objClass2>(o.p.refP))
{
}

#endif
#endif
