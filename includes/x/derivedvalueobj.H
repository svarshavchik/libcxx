#ifndef x_derivedvalueobj_H
#define x_derivedvalueobj_H

#include <x/weaklistfwd.H>
#include <x/mpobj.H>
#include <x/functional.H>
#include <x/vipobj.H>

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif

template<typename basedon_t, typename vipobj_type,
	 typename vipobj_arg_type,
	 typename ...vipobj_extra_type> class derivedvalueObj;

template<typename begin_calc_functor_t,
	 typename end_calc_functor_t>
class derivedvalue_type_calc;

//! A container for \ref derivedvalue "values based on which another value gets derived".

template<typename basedon_t>
class derivedvalueListObj : virtual public obj {

public:

	class value_fromObj;

	derivedvalueListObj();

	~derivedvalueListObj();

protected:

	//! Container for all values.

	typedef std::list<basedon_t> all_values_t;

	//! All current values from which a very important value is derived from.

	//! This list is accessed only while holding an update lock on the
	//! very important object.
	all_values_t all_values;

	//! Acquire an update lock, then invoke the closure.

	virtual void updated(const function<void (void)> &)=0;

public:
	friend class value_fromObj;

	ref<value_fromObj> create(const basedon_t &initial_value);

	ref<value_fromObj> create(basedon_t &&initial_value);

	//! Create a new value from which some other value is derived.

	template<typename ...Args>
	ref<value_fromObj> emplace(Args && ...args);

	template<typename begin_calc_functor_t,
		 typename calc_functor_t,
		 typename end_calc_functor_t,
		 typename vipobj_arg_type,
		 typename ...vipobj_type> class calcObj;
};

//! A single value on which a derived value is based on.

//! An instance of this object gets added, weakly, to all_values.

template<typename basedon_t>
class derivedvalueListObj<basedon_t>::value_fromObj : virtual public obj {

public:

	//! The collection of values this value is from.

	const ref<derivedvalueListObj> all_values;

private:

	//! My value.

	typename all_values_t::iterator iterator;

public:

	value_fromObj(const ref<derivedvalueListObj> &all_valuesArg,
		      all_values_t &all_values);

	~value_fromObj();

	void update(const basedon_t &new_basedonvalue);

	void update(basedon_t &&new_basedonvalue);

	friend class derivedvalueListObj;

};

//! A value derived from other values.

//! This inherits from derivedvalueListObj<basedon_t>, and a
//! \ref vipobj "very important object" with the derived value. The first
//! template parameter defines the types of values that the very important
//! object gets derived from, and the remaining template parameters are
//! forwarded to the very important object. Because the very important object's
//! first template parameter is the type of the very important object, the
//! first two parameters to this template are the type of the values derived
//! from, and the type of the derived value.

template<typename basedon_t, typename vipobj_type,
	 typename vipobj_arg_type,
	 typename ...vipobj_extra_type>
class derivedvalueObj : public derivedvalueListObj<basedon_t>,
			public vipobj<vipobj_type, vipobj_arg_type, true,
				      vipobj_extra_type...> {
public:

	//! The very important object this is a subclass of.
	typedef vipobj<vipobj_type, vipobj_arg_type, true,
		       vipobj_extra_type...> vipobj_t;

	//! Constructor
	template<typename ...Args>
	derivedvalueObj(Args && ...args)
		: vipobj_t(std::forward<Args>(args)...) {}

	//! Destructor
	~derivedvalueObj() {}
};

//! Calculate the type of the derived value.

//! Helper class that takes the begin-calculation and end-calculation
//! functors, passed to derivedvaluelist::create(), and figures out what
//! type gets derived.

template<typename begin_calc_functor_t,
	 typename end_calc_functor_t>
class derivedvalue_type_calc {

public:

	//! What the begin-calculation functor returns.
	typedef decltype( (*(begin_calc_functor_t *)nullptr)() ) begin_ret_t;

	//! What the end-calculation functor returns.
	typedef decltype((*((end_calc_functor_t *)nullptr))
			 (*(begin_ret_t *)nullptr)) type;
};

//! Helper class that implements updated().

//! Creates a subclass of \ref derivedvalueListObj that implements updated()
//! using three functors.
//!
//! 1) begin_calc_functor_t() gets called with no parameters, its return value
//!    gets stored temporarily.
//! 2) For each current value to derive from, calc_functor_t() gets called,
//!    passing a mutable native reference to the value returned from
//!    begin_calc_functor_t, together with the value to be used in the
//!    derivation calculation.
//! 3) end_calc_functor_t() gets called, its parameter is a CONST reference
//!    to the current value of whatever's returned from begin_calc_functor_t(),
//!    the return value from end_calc_functor_t() is the calculated value.
//!
//! calcObj figures out the type that end_calc_functor_t() returns, and uses
//! it to subclass derivedvalueObj, which inherits from
//! derivedvalueListObj, the actual list of values to derive from, and the
//! very important object that holds the derived value.
//!
//! Steps 1-3 are protected by a lock.
//!
//! The functors must support begin_calc_functor_t() followed immediately by
//! end_calc_functor_t() - empty list of values to derive from. This defines
//! the default derived value when there are no window configurations.

template<typename basedon_t>
template<typename begin_calc_functor_t,
	 typename calc_functor_t,
	 typename end_calc_functor_t,
	 typename vipobj_arg_type,
	 typename ...vipobj_type>
class derivedvalueListObj<basedon_t>::calcObj
	: public derivedvalueObj<basedon_t,
				 typename derivedvalue_type_calc
				 <begin_calc_functor_t, end_calc_functor_t>
				 ::type,
				 vipobj_arg_type,
				 vipobj_type...> {
public:

	//! Begin calculation functor
	begin_calc_functor_t begin_calc;

	//! Calculation functor
	calc_functor_t calc;

	//! End calculation functor
	end_calc_functor_t end_calc;

	//! The derivedvalueObj superclass of this class.
	typedef derivedvalueObj<basedon_t,
				typename derivedvalue_type_calc
				<begin_calc_functor_t, end_calc_functor_t>
				::type,
				vipobj_type...> derivedvalueObj_t;

	//! The very important object being derived.
	typedef typename derivedvalueObj_t::vipobj_t vipobj_t;

	template<typename B, typename C, typename E,
		 typename ...Args>
	calcObj(B &&b, C &&c, E &&e, Args && ...args);

	~calcObj();

	void updated(const function<void (void)> &) override;
};


#if 0
{
#endif
}
#endif
