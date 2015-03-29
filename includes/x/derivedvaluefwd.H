#ifndef x_derivedvaluefwd_H
#define x_derivedvaluefwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif

template<typename basedon_t> class derivedvalueListBase;

template<typename basedon_t> class derivedvalueListObj;

template<typename basedon_t,
	 typename vipobj_type,
	 typename vipobj_arg_type=const vipobj_type &,
	 typename ...vipobj_extra_type> class derivedvalueObj;

template<typename basedon_t,
	 typename vipobj_type,
	 typename vipobj_arg_type=const vipobj_type &,
	 typename ...vipobj_extra_type> class derivedvalueBase;

//////////////////////////////////////////////////////////////////////////////

//! A list of some values that some other value is derived from.

//! \see derivedvalue

template<typename basedon_t>
using derivedvaluelist=ref<derivedvalueListObj<basedon_t>,
			   derivedvalueListBase<basedon_t>>;

//! A nullable pointer reference to a list of some values that some other value is derived from.

//! \see derivedvalue

template<typename basedon_t>
using derivedvaluelistptr=ptr<derivedvalueListObj<basedon_t>,
			      derivedvalueListBase<basedon_t>>;

//! A non-mutable list of some values that some other value is derived from.

//! \see derivedvalue

template<typename basedon_t>
using const_derivedvaluelist=const_ref<derivedvalueListObj<basedon_t>,
				       derivedvalueListBase<basedon_t>>;

//! A non-mutable nullable pointer reference to a list of some values that some other value is derived from.

//! \see derivedvalue

template<typename basedon_t>
using const_derivedvaluelistptr=const_ptr<derivedvalueListObj<basedon_t>,
					  derivedvalueListBase<basedon_t>>;

//! A very important value that's derived from a collection of other values.

//! \code
//! typedef INSERT_LIBX_NAMESPACE::derivedvalue<int, std::string> derived_value_t;
//!
//! derived_value_t a_derived_value=INSERT_LIBX_NAMESPACE::derivedvalues<int>::create(
//!     []
//!     {
//!          return (int)0;
//!     },
//!     []
//!     (int &sum, int v)
//!     {
//!         sum += v;
//!     },
//!     []
//!     (const int &sum)
//!     {
//!         std::ostringstream o;
//!
//!         o << sum * 2;
//!         return o.str();
//!     });
//!
//! typedef derived_value_t::base::vipobj_t vipobj_t;
//!
//! {
//!     vipobj_t::handlerlock lock(*a_derived_value);
//!
//!     auto current_value=*vipobj_t::readlock(*a_derived_value);
//!     auto mcguffin=lock->install_back([]
//!                                      (const std::string &value)
//!                                      {
//!                                      },
//!                                      current_value);
//! }
//!
//! derived_value_t::base::current_value_t
//!      v=a_derived_value->create(1);
//!
//!  v->update(2);
//! \endcode
//!
//! Implement a design pattern where a value of a
//! \ref vipobj "very important object" is automatically calculated based on
//! a (possibly empty) collection of other values.
//!
//! \c INSERT_LIBX_NAMESPACE::derivedvalues<type>::create() constructs
//! a \ref vipobj "very important object" whose value depends on zero or
//! more other values. The type of the other values is given as
//! \c derivedvalues template parameter. In this example, the value is derived
//! from a list of \c ints.
//!
//! create() takes three functors as arguments, that specify how the derived
//! value is calculated.
//!
//! - The first functor returns the initial value for the calculation. The
//! type of its return value sets the type used for temporary calculations,
//! \c int in this case.
//!
//! - The second functor gets invoked for each value that enters the
//! calculation. Its first parameter is a mutable reference to the temporary
//! calculation value. Its second parameter is the next value for the
//! calculation.
//!
//! - After all values are passed to the second functor, the third functor
//! gets invoked to compute the final value.
//!
//! The return value from the third functor sets the type of the
//! \ref vipobj "very important object"'s value. In this example, the derived
//! value is a \c std::string of the sum of the list of \c int values,
//! multiplied by two.
//!
//! create() returns a \c INSERT_LIBX_NAMESPACE::derivedvalue<A, B>
//! which is a reference to a reference-counted object that's derived from
//! \ref vipobj "INSERT_LIBX_NAMESPACE::vipobj".
//!
//! \c INSERT_LIBX_NAMESPACE::derivedvalue's first template parameter is the
//! type of the values that make up the calculation, and the second
//! template parameter is the very important object's type. In this example
//! they are \c int and \c std::string, respectively.
//!
//! The \c INSERT_LIBX_NAMESPACE::derivedvalue's object is a subclass of
//! \c INSERT_LIBX_NAMESPACE::derivedvalue<A,B>::base::vipobj_t, an instance
//! if the \ref vipobj "INSERT_LIBX_NAMESPACE::vipobj" template.
//!
//! The list of values that make up the calculation of the derived values
//! is variable. It is a list of
//! \c INSERT_LIBX_NAMESPACE::derivedvalue<A,B>::base::current_value_t
//! type instances. This is a reference to a reference-counted object.
//! A new value gets added to the list by invoking
//! INSERT_LIBX_NAMESPACE::derivedvalue<A,B>->create().
//! This adds a new value to
//! the list of values that the derived value gets calculated from, and
//! returns a \c current_value_t handle for the new value.
//!
//! create()'s parameter sets the initial value. There's also an emplace(),
//! that constructs a new value.
//!
//! When the last reference to the \c current_value_t object goes out of
//! scope and gets destroyed, its automatically removed from the list of
//! values that the derived value gets calculated from.
//!
//! The derived value gets recalculated automatically when:
//!
//! - a new value its calculated from gets create()d.
//!
//! - a \c current_value_t handle for an existing value goes out of scope,
//! and gets destroyed.
//!
//! - a \c current_value_t handle's update() method gets invoked, updating
//! its corresponding value.
//!
//! Either of these events results in the derived value getting automatically
//! recalculated, and the very important object gets updated.
//!
//! Use \ref vipobj "vipobj"'s handler lock to register a lambda that gets
//! called each time the derived value gets recalculated.
//!
//! \note
//! The derived value object takes complete responsibility for using the
//! underlying update lock and write lock, to set the new calculated value.
//! These locks should not be used explicitly.
//!
//! \note
//! Each \c current_value_t holds a reference to its \c derivedvalue". It is
//! not specified in which order each value gets passed to the second functor,
//! during calculation.
//!
//! \note
//! The calculated derived value gets compared, using the == operator, with
//! the value of the very important object. Only if they are not compare
//! as qual, does this result in an update to the derived value, and
//! invocation of all the registered lambda callbacks.

template<typename basedon_t,
	 typename vipobj_type,
	 typename vipobj_arg_type=const vipobj_type &,
	 typename ...vipobj_extra_type>
using derivedvalue=ref<derivedvalueObj<basedon_t, vipobj_type,
				       vipobj_arg_type, vipobj_extra_type...>,
		       derivedvalueBase<basedon_t, vipobj_type,
					vipobj_arg_type, vipobj_extra_type...>>;

//! A nullable pointer reference to a \ref derivedvalue "derived value".
template<typename basedon_t,
	 typename vipobj_type,
	 typename vipobj_arg_type=const vipobj_type &, typename ...vipobj_extra_type>
using derivedvalueptr=ptr<derivedvalueObj<basedon_t, vipobj_type,
					  vipobj_arg_type, vipobj_extra_type...>,
			  derivedvalueBase<basedon_t, vipobj_type,
					   vipobj_arg_type, vipobj_extra_type...>>;

//! A non-mutable \ref derivedvalue "derived value".
template<typename basedon_t,
	 typename vipobj_type,
	 typename vipobj_arg_type=const vipobj_type &, typename ...vipobj_extra_type>
using const_derivedvalue=const_ref<derivedvalueObj<basedon_t, vipobj_type,
						   vipobj_arg_type, vipobj_extra_type...>,
				   derivedvalueBase<basedon_t, vipobj_type,
						    vipobj_arg_type, vipobj_extra_type...>>;

//! A nullable pointer reference to a non-mutable \ref derivedvalue "derived value".

template<typename basedon_t,
	 typename vipobj_type,
	 typename vipobj_arg_type=const vipobj_type &, typename ...vipobj_extra_type>
using const_derivedvalueptr=const_ptr<derivedvalueObj<basedon_t, vipobj_type,
						      vipobj_arg_type,
						      vipobj_extra_type...>,
				      derivedvalueBase<basedon_t, vipobj_type,
						       vipobj_arg_type,
						       vipobj_extra_type...>>;
#if 0
{
#endif
}
#endif
