#ifdef x_ptrfwd_H_included
#ifdef x_obj_H_included

//! The default %create() method.

//! This is a default implementation for the %create() method:
//!
//! \code
//! typedef ptr<widgetObj> widget;
//!
//! widget myWidget=widget::create();
//! \endcode

template<typename ptrrefType>
template<typename... Args_t>
inline ptrrefType ptrref_base::objfactory<ptrrefType>::create(Args_t &&...args)
{
	return create_and_init<ptrrefType,
			       create_args<Args_t...>
			       >::create(std::forward<Args_t>(args)...);
}

//! SFINAE-resolved create() for a class without a suitable constructor().

template<typename ptrrefType, typename args >
template<typename ...Args>
inline ptrrefType ptrref_base::create_and_init<ptrrefType, args, false>
::create(Args && ...argsValue)
	requires is_derived_from_obj<typename ptrrefType::obj_type>
{
	// Sadly, cannot use uniform initialization syntax here because
	// it warns about narrowing conversion when passing, say a literal
	// 100 constant int to a constructor that takes a size_t parameter.

	auto p=new typename ptrrefType
		::obj_type(std::forward<Args>(argsValue)...);

	++p->obj::refcnt; // Set refcnt's constructor

	return ptrrefType{p};
}

//! SFINAE-resolved create() for a class with a suitable constructor().

template<typename ptrrefType, typename args >
template<typename ...Args>
inline ptrrefType ptrref_base::create_and_init<ptrrefType, args, true>
::create(Args && ...argsValue)
	requires is_derived_from_obj<typename ptrrefType::obj_type>
{
	// Sadly, cannot use uniform initialization syntax here because
	// it warns about narrowing conversion when passing, say a literal
	// 100 constant int to a constructor that takes a size_t parameter.
	auto p=new typename ptrrefType
		::obj_type(std::forward<Args>(argsValue)...);

	++p->obj::refcnt; // Set refcnt's constructor

	auto r=ref{p};

	r->constructor(std::forward<Args>(argsValue)...);

	return ptrrefType{r};
}

#endif
#endif
