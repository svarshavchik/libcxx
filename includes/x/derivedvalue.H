#ifndef x_derivedvalue_H
#define x_derivedvalue_H

#include <x/derivedvaluefwd.H>
#include <x/derivedvalueobj.H>
#include <x/weaklist.H>
#include <x/ref.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif

//! Constructor

template<typename basedon_t>
derivedvalueListObj<basedon_t>::derivedvalueListObj()
{
}

//! Destructor

template<typename basedon_t>
derivedvalueListObj<basedon_t>::~derivedvalueListObj()
{
}

//! Create a new value from which some other value is derived.

template<typename basedon_t>
ref<typename derivedvalueListObj<basedon_t>::value_fromObj>
derivedvalueListObj<basedon_t>::create(const basedon_t &initial_value)
{
	ptr<value_fromObj> p;

	updated(make_function<void (void)>([&initial_value, &p, this]
		{
			auto n=ref<value_fromObj>::create
				(ref<derivedvalueListObj>(this),
				 this->all_values);

			p=n;

			this->all_values.push_back(initial_value);

			n->iterator=--all_values.end();

		}));
	return p;
}

//! Create a new value from which some other value is derived.

template<typename basedon_t>
ref<typename derivedvalueListObj<basedon_t>::value_fromObj>
derivedvalueListObj<basedon_t>::create(basedon_t &&initial_value)
{
	ptr<value_fromObj> p;

	updated(make_function<void (void)>([&initial_value, &p, this]
		{
			auto n=ref<value_fromObj>::create
				(ref<derivedvalueListObj>(this),
				 this->all_values);

			p=n;

			this->all_values
				.push_back(std::move(initial_value));

			n->iterator=--all_values.end();
		}));
	return p;
}

//! Create a new value from which some other value is derived.

template<typename basedon_t>
template<typename ...Args>
ref<typename derivedvalueListObj<basedon_t>::value_fromObj>
derivedvalueListObj<basedon_t>::emplace(Args && ...args)
{
	ptr<value_fromObj> p;

	updated(make_function<void (void)>([&]
		{
			auto n=ref<value_fromObj>::create
				(ref<derivedvalueListObj>(this),
				 this->all_values);

			p=n;

			this->all_values
				.emplace_back(std::forward<Args>(args)...);

			n->iterator=--all_values.end();
		}));
	return p;
}

//////////////////////////////////////////////////////////////////////////////


//! Constructor, only invoked from derivedvalue->create().

template<typename basedon_t>
derivedvalueListObj<basedon_t>::value_fromObj
::value_fromObj(const ref<derivedvalueListObj> &all_valuesArg,
		all_values_t &all_values_list)
	: all_values(all_valuesArg),
	  iterator(all_values_list.end())
{
}

//! The destructor also invokes all_values->updated()

template<typename basedon_t>
derivedvalueListObj<basedon_t>::value_fromObj::~value_fromObj()
{
	all_values->updated
		(make_function<void (void)>
		 ([this]
		 {
			 if (iterator != all_values->all_values.end())
				 all_values->all_values.erase(iterator);
		 }));
}

//! Calculate the derived value.

//! updated() gets invoked every time a new value is added, removed,
//! or updated.

template<typename basedon_t>
void derivedvalueListObj<basedon_t>::value_fromObj
::update(const basedon_t &new_basedonvalue)
{
	all_values->updated
		(make_function<void (void)>
		 ([this, &new_basedonvalue]
		 {
			 *iterator=new_basedonvalue;
		 }));
}

//! Calculate the derived value.

//! updated() gets invoked every time a new value is added, removed,
//! or updated.

template<typename basedon_t>
void derivedvalueListObj<basedon_t>::value_fromObj
::update(basedon_t &&new_basedonvalue)
{
	all_values->updated
		(make_function<void (void)>
		 ([this, &new_basedonvalue]
		 {
			 *iterator=std::move(new_basedonvalue);
		 }));
}

//! The derived value object

//! The constructor invoked by derivedvaluelist::create().

template<typename basedon_t>
template<typename begin_calc_functor_t,
	 typename calc_functor_t,
	 typename end_calc_functor_t,
	 typename vipobj_arg_type,
	 typename ...vipobj_t>
template<typename B, typename C, typename E, typename ...Args>
derivedvalueListObj<basedon_t>::calcObj
<begin_calc_functor_t, calc_functor_t,
 end_calc_functor_t, vipobj_arg_type,
 vipobj_t...>::calcObj(B &&b, C &&c, E &&e, Args && ...args)
	: derivedvalueObj<basedon_t,
	typename derivedvalue_type_calc
	<begin_calc_functor_t, end_calc_functor_t>
	::type,
	vipobj_arg_type,
	vipobj_t...>( ({
				// Initialize default value.

				auto v=b();

				e(v);
			}),
		std::forward<Args>(args)...),
	begin_calc(std::forward<B>(b)),
	calc(std::forward<C>(c)),
	end_calc(std::forward<E>(e))
{
}

//! Destructor

template<typename basedon_t>
template<typename begin_calc_functor_t,
	 typename calc_functor_t,
	 typename end_calc_functor_t,
	 typename vipobj_arg_type,
	 typename ...vipobj_t>
derivedvalueListObj<basedon_t>::calcObj<begin_calc_functor_t,
					calc_functor_t,
					end_calc_functor_t,
					vipobj_arg_type,
					vipobj_t...>
::~calcObj()
{
}

//! Some value has been updated.

//! Calculated a new derived value, puts it into the very important object.
template<typename basedon_t>
template<typename begin_calc_functor_t,
	 typename calc_functor_t,
	 typename end_calc_functor_t,
	 typename vipobj_arg_type,
	 typename ...vipobj_type>
void derivedvalueListObj<basedon_t>::calcObj<begin_calc_functor_t,
					     calc_functor_t,
					     end_calc_functor_t,
					     vipobj_arg_type,
					     vipobj_type...>
::updated(const function<void (void)> &f)
{
	typename vipobj_t::updatelock lock(*this);

	f();

	auto v=begin_calc();

	for (const auto &value:this->all_values)
	{
		calc(v, value);
	}

	auto final_value=end_calc(v);

	{
		typename vipobj_t::writelock writelock(*this);

		if (*writelock == final_value)
			return;

		*writelock=final_value;
	}
	lock.notify(final_value);
}

//! Base class for a \ref derivedvaluelist "derivedvaluelist<value_type>".

//! Refer to this class as "INSERT_LIBX_NAMESPACE::derivedvaluelist<value_type>".
//!
//! \see derivedvalue

template<typename basedon_t>
class derivedvalueListBase : public ptrref_base {

public:

	//! The wrapper for a value that some other value is derived from.

	//! Refer to this class as "INSERT_LIBX_NAMESPACE::derivedvaluelist<value_type>::base::current_value_t".
	//!
	//! \see derivedvalue

	typedef ref<typename derivedvalueListObj<basedon_t>
		    ::value_fromObj> current_value_t;

};

//! Base class for a \ref derivedvalue "derivedvalue<value_type, derived_type>".

//! Refer to this class as "INSERT_LIBX_NAMESPACE::derivedvalue<value_type, derived_type>::base".
//!
//! \see derivedvalue

template<typename basedon_t,
	 typename vipobj_type,
	 typename vipobj_arg_type, typename ...vipobj_extra_type>
class derivedvalueBase : public derivedvalueListBase<basedon_t> {

public:

	//! The very important object type.

	//! Refer to this type as "INSERT_LIBX_NAMESPACE::derivedvalue<value_type, derived_type>::base::vipobj_t".
	typedef typename derivedvalueObj<basedon_t, vipobj_type,
					 vipobj_arg_type,
					 vipobj_extra_type...
					 >::vipobj_t vipobj_t;
};

//! Calculate the type of a \ref derivedvalue "derived value", that the its very important object notifies.

//! If the second, optional parameter to
//! \ref derivedvalues "derivedvalues<class>::create"()
//! is specified, it overrides the default type, of the value that's returned
//! from the \c e_type functor.

template<typename b_type, typename e_type, typename override_arg_type>
class calculate_vipobj_arg_type {

public:

	//! Explicitly overriden type
	typedef override_arg_type type;
};

//! Specialization.

//! The default value of the second parameter to the
//! \ref derivedvalues "derivedvalues" template is void. This results in the
//! default type of the value, that the
//! \ref vipobj "very important object" notifies the
//! registered handler, being "const \c TYPE &", where TYPE is the type of the
//! return value from the \c e_type functor.

template<typename b_type, typename e_type>
class calculate_vipobj_arg_type<b_type, e_type, void> {

public:

	//! Default very important
	typedef const typename derivedvalue_type_calc<b_type,
						      e_type>::type &type;
};

//! Create a new derived value.

//! Implements the create() method.
//!
//! \see derivedvalue

template<typename basedon_t,
	 typename vipobj_arg_type=void> class derivedvalues {

public:

	//! Calculate the very important object that the functors calculate.

	template<typename B, typename C, typename E, typename ...Args>
	static inline auto create(B &&b, C &&c, E &&e, Args && ...args)
	{
		typedef typename calculate_vipobj_arg_type
			<typename std::decay<B>::type,
			 typename std::decay<E>::type,
			 vipobj_arg_type>::type vipobj_arg_type_t;


		typedef typename derivedvalueListObj<basedon_t>
			::template calcObj
			<typename std::decay<B>::type,
			 typename std::decay<C>::type,
			 typename std::decay<E>::type,
			 vipobj_arg_type_t> derived_obj_t;

		return ref<derived_obj_t,
			   derivedvalueBase<basedon_t,
					    typename derived_obj_t::vipobj_t
					    ::obj_t,
					    vipobj_arg_type_t>

			   >::create(std::forward<B>(b),
				     std::forward<C>(c),
				     std::forward<E>(e),
				     std::forward<Args>(args)...);
	}
};

#if 0
{
#endif
}
#endif
