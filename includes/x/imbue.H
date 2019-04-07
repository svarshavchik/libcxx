/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_imbue_H
#define x_imbue_H

#include <x/locale.H>
#include <x/globlocale.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class localeObj;

//! Imbue a locale into an object, for the duration of the current scope.

//! The constructor invokes the imbue() method of some object, setting its
//! locale from a locale object. The object's previous locale is saved, and
//! restored by the destructor.
//!
//! The template parameter is the class being imbued. The template instance
//! may only be on the stack, and the object must remain in scope for the
//! duration of the instance's scope.

template<typename obj_type> class imbue : public globlocale {

	//! The original object
	obj_type &obj;

	//! Object's previous locale
	std::locale prev_locale;

	//! Not implemented
	imbue(const imbue<obj_type> &);

	//! Not implemented
	imbue<obj_type> &operator=(const imbue<obj_type> &);

public:
	//! Constructor
	imbue(const const_locale &l, obj_type &objArg)
		: globlocale(l), obj(objArg), prev_locale(obj.imbue(l->locale))
	{
	}

	//! Destructor
	~imbue()
	{
		obj.imbue(prev_locale);
	}
};

#if 0
{
#endif
}
#endif

