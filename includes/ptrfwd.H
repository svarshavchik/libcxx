/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_reffwd_H
#define x_reffwd_H

#include <utility>
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

class ptrrefBase {

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
			return ptrrefType(new typename ptrrefType
					  ::obj_type(std::forward<Args_t>
						     (args)...));
		}
	};
};

template<typename objClass, typename baseType=ptrrefBase> class ptr;
template<typename objClass, typename baseType=ptrrefBase> class const_ptr;
template<typename objClass, typename baseType=ptrrefBase> class ref;
template<typename objClass, typename baseType=ptrrefBase> class const_ref;


#if 0
{
#endif
}
#endif

