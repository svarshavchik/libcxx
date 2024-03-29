/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_listfwd_H
#define x_option_listfwd_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>

namespace LIBCXX_NAMESPACE:: option {
#if 0
}
#endif

class listObj;
class listBase;

//! A list of defined options

//! This is a reference to a reference-counted %option
//! definition list.
//! This object holds a list of all defined options.
//! \see option

typedef ref<listObj, listBase> list;

//! A pointer to an %option definition list.

//! \see list

typedef ptr<listObj, listBase> listptr;


//! Refer to this class as \c option::list::base or \c option::listptr::base

//! Base class for the %option list reference

class listBase : public ptrref_base {

public:
	//! This %option takes a value

	static constexpr int hasvalue=1;

	//! Required %option

	static constexpr int required=2;

	//! Repeated non-option argument

	static constexpr int repeated=4;
};

#if 0
{
#endif
}
#endif
