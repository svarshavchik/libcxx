/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_hier_H
#define x_hier_H

#include <x/ref.H>
#include <x/ptr.H>
#include <x/hierfwd.H>
#include <x/hierobj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for \ref hier

//! This base class provides convenient typedefs defined in hierObj.
//!
//! Refer to this class as \c %INSERT_LIBX_NAMESPACE:%hier<...>::%base.

template<typename hier_type, typename ref_type>
class hierBase : public ptrrefBase {

public:

	//! \c typedef alias
	typedef typename hierObj<hier_type, ref_type>::readlock readlock;

	//! \c typedef alias
	typedef typename hierObj<hier_type, ref_type>::readlockptr readlockptr;

	//! \c typedef alias
	typedef typename hierObj<hier_type, ref_type>::const_readlock const_readlock;

	//! \c typedef alias
	typedef typename hierObj<hier_type, ref_type>::const_readlockptr const_readlockptr;

	//! \c typedef alias
	typedef typename hierObj<hier_type, ref_type>::writelock writelock;

	//! \c typedef alias
	typedef typename hierObj<hier_type, ref_type>::writelockptr writelockptr;

	//! \c typedef alias
	typedef typename hierObj<hier_type, ref_type>::const_writelock const_writelock;

	//! \c typedef alias
	typedef typename hierObj<hier_type, ref_type>::const_writelockptr const_writelockptr;

	//! \c typedef alias
	typedef typename hierObj<hier_type, ref_type>::iterator iterator;
};

#if 0
{
#endif
}
#endif
