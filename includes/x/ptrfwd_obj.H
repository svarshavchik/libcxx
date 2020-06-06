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
template<typename... Args_t, bool has_constructor>
inline ptrrefType ptrref_base::objfactory<ptrrefType>::create(Args_t &&...args)
	requires derived_from_obj<typename ptrrefType::obj_type>
{
	// Sadly, cannot use uniform initialization syntax here because
	// it warns about narrowing conversion when passing, say a literal
	// 100 constant int to a constructor that takes a size_t parameter.

	auto p=new typename ptrrefType
		::obj_type(std::forward<Args_t>(args)...);

	++p->obj::refcnt; // Set refcnt's constructor

	if constexpr (std::is_base_of<with_constructorObj,
		      typename ptrrefType::obj_type>::value)
        {
		ptrrefType r{p};

		p->constructor(std::forward<Args_t>(args)...);

		return r;
	}
	else
	{
		static_assert(!has_constructor,
			      "Objects that implement constructor() must "
			      "derive from with_constructorObj");

		return ptrrefType{p};
	}
}

#endif
#endif
