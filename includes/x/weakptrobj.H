/*
** Copyright 2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakptrobj_H
#define x_weakptrobj_H

#include <x/namespace.h>
#include <x/weakptrfwd.H>
#include <x/exceptionfwd.H>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif
//! A weak pointer.

//! When all \ref ptr "pointers" to an object go out of scope,
//! the object gets destroyed even if some weak pointers to the object still
//! exist.
//!
//! \code
//! class myObj : virtual public obj {
//!
//! ...
//! \endcode
//!
//! Regular, strong, references, are defined and used normally:
//!
//! \code
//! typedef INSERT_LIBX_NAMESPACE::ptr<myObj> myPtr;
//!
//! myPtr r=myPtr::create();
//!
//! \endcode
//!
//! Use an existing "strong" pointer to create a weak one, by passing the
//! strong pointer to the constructor:
//!
//! \code
//! weakptr<myPtr> wr(r);
//! \endcode
//!
//! Note that the template parameter is a nullable pointer, but
//! the constructor parameter is a strong pointer.
//!
//! The original strong pointer, \c r, is unaffected, and a new weak pointer
//! gets instantiated.
//!
//! The weak pointer does not define a \c * or a \c -> operator, unlike
//! regular strong pointers. It only defines a
//! getptr() function that recovers the original strong
//! pointer, if the underlying object has not been destroyed.
//!
//! \par Recovering the strong pointer.
//!
//! \code
//! myPtr r2(wr.getptr());
//!
//! if (!r2.null())
//! {
//!   // ...
//! }
//! \endcode
//!
//! The only operation allowed on a weak pointer is the getptr() method, which
//! returns the reference to the underlying object. If all strong pointers
//! went out of scope and the object got destroyed, getptr() returns a null
//! pointer.

template<typename objClass, typename baseType>
class weakptr<ptr<objClass, baseType>> : private ptr<weakinfo> {

public:
	//! The strong pointer type

	typedef ptr<objClass, baseType> ptrType;

	//! Default constructor

	//! This weak pointer is not bound to anything.

	weakptr();

	//! Create a weak pointer

	//! A new weak pointer gets created from an existing regular,
	//! strong pointer.

	weakptr(//! A strong pointer to an object.

		//! This weak pointer gets bound to the given
		//! object.

		const ptrType &refArg);

	//! Create a weak pointer

	//! \overload
	weakptr(const ref<objClass, baseType> &refArg);

	//! Create a weak pointer

	//! \overload
	template<typename objClass2, typename baseType2>
		weakptr(const ptr<objClass2, baseType2> &ptrArg);

	//! Create a weak pointer

	//! \overload
	template<typename objClass2, typename baseType2>
		weakptr(const ref<objClass2, baseType2> &refArg);

	//! Default destructor

	~weakptr() noexcept;

	//! Assignment operator

	//! Construct a weak pointer by assignment

	weakptr<ptrType> &operator=(//! A strong pointer
				    const ptrType &r);

	//! Assignment operator

	//! Construct a weak pointer by assignment
	template<typename objClass2, typename baseType2>
	weakptr<ptrType> &operator=(//! A strong pointer
				    const ptr<objClass2, baseType2> &r);

	//! Assignment operator

	//! Construct a weak pointer by assignment

	weakptr<ptrType> &operator=(//! A strong pointer
				    const ref<objClass, baseType> &r);

	//! Assignment operator

	//! Construct a weak pointer by assignment

	template<typename objClass2, typename baseType2>
	weakptr<ptrType> &operator=(//! A strong pointer
				    const ref<objClass2, baseType2> &r);

	//! Return a strong pointer to the underlying object.

	//! \return a strong pointer. The pointer's
	//! null() method returns
	//! \c true if the underlying object is destroyed
	ptrType getptr() const;
};

#if 0
{
#endif
}
#endif
