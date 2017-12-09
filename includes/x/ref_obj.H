#ifdef x_obj_H_included
#ifdef x_ref_H_included

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
