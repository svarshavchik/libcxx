/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_reffwd_H
#define x_reffwd_H

#include <utility>
#include <type_traits>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Create a reference or a nullable pointer reference to a new object.

//! This is the default second parameter to
//! \ref ref "INSERT_LIBX_NAMESPACE::ref" and
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr", which becomes
//! \ref ref "INSERT_LIBX_NAMESPACE::ref<class>::base" and
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr<class>::base".
//!
//!
class obj;

class ptrref_base {

public:

	//! Instantiate a new class intance, put it into a reference or a pointer.

	//! \ref ref "INSERT_LIBX_NAMESPACE::ref<class>" and
	//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr<class>" define a %create()
	//! that calls \c %base::factory&lt;TYPE&gt;::%create(), where \c TYPE
	//! is the \c ref or the \c ptr type, with the arguments to
	//! %create() getting forwarded.

	template<typename ptrrefType> class objfactory
	{
	public:


		//! The default %create() method.

		//! This is a default implementation for the %create() method:
		//!
		//! \code
		//! typedef ptr<widgetObj> widget;
		//!
		//! widget myWidget=widget::create();
		//!
		//! \endcode
		template<typename... Args_t>
		static inline ptrrefType
		create(Args_t &&...args)
		{
			auto p=new typename ptrrefType
				::obj_type(std::forward<Args_t>(args)...);


			p->obj::refadd(1); // Set refcnt's constructor

			return ptrrefType(p);
		}
	};
};

//! Define the default base class for a \ref ptr "ptr" or a \ref ref "ref".

//! The default base class is ptrref_base

template<typename>
struct base_type {
public:

	typedef ptrref_base type;
};

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class ptr;

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class const_ptr;

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class ref;

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class const_ref;

//! Deduction guide for a \ref ptr "ptr".

template<typename objClass> ptr(objClass *) -> ptr<objClass>;

//! Deduction guide for a \ref const_ptr "const_ptr".

template<typename objClass> const_ptr(objClass *) -> const_ptr<objClass>;

//! Deduction guide for a \ref const_ptr "const_ptr".

template<typename objClass> const_ptr(const objClass *) -> const_ptr<objClass>;

//! Deduction guide for a \ref ref "ref".

template<typename objClass> ref(objClass *) -> ref<objClass>;

//! Deduction guide for a \ref const_ref "const_ref".

template<typename objClass> const_ref(objClass *) -> const_ref<objClass>;

//! Deduction guide for a \ref const_ref "const_ref".

template<typename objClass> const_ref(const objClass *) -> const_ref<objClass>;

#if 0
{
#endif
}
#endif
