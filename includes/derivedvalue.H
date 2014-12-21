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
	: all_values(all_values_t::create())
{
}

//! Destructor

template<typename basedon_t>
derivedvalueListObj<basedon_t>::~derivedvalueListObj() noexcept
{
}

//! Create a new value from which some other value is derived.

//! The derived value is immediately update()d, by the virtue of a
//! new value added to the list of values from which it is derived.

template<typename basedon_t>
ref<typename derivedvalueListObj<basedon_t>::value_fromObj>
derivedvalueListObj<basedon_t>::create(const basedon_t &initial_value)
{
	auto p=ref<value_fromObj>::create(ref<derivedvalueListObj>(this),
					   initial_value);

	all_values->push_back(p);

	updated();

	return p;
}

//! Constructor, constructed in derivedvalue->create().

template<typename basedon_t>
derivedvalueListObj<basedon_t>::value_fromObj
::value_fromObj(const ref<derivedvalueListObj> &all_valuesArg,
		const basedon_t &initial_value)
	: all_values(all_valuesArg), latest_value(initial_value)
{
}

//! The destructor also invokes all_values->updated()

template<typename basedon_t>
derivedvalueListObj<basedon_t>::value_fromObj::~value_fromObj() noexcept
{
	all_values->updated();
}

//! Calculate the derived value.

//! updated() gets invoked every time a new value is added, removed,
//! or updated.

template<typename basedon_t>
void derivedvalueListObj<basedon_t>::value_fromObj
::update(const basedon_t &new_basedonvalue)
{
	{
		typename latest_value_t::lock lock(latest_value);

		*lock=new_basedonvalue;
	}
	all_values->updated();
}

//! The derived value object

//! The constructor invoked by derivedvaluelist::create().

template<typename basedon_t>
template<typename begin_calc_functor_t,
	 typename calc_functor_t,
	 typename end_calc_functor_t,
	 typename ...vipobj_t>
template<typename B, typename C, typename E, typename ...Args>
derivedvalueListObj<basedon_t>::calcObj<begin_calc_functor_t,
					calc_functor_t,
					end_calc_functor_t,
					vipobj_t...>
::calcObj(B &&b, C &&c, E &&e, Args && ...args)
	: derivedvalueObj<basedon_t,
	typename derivedvalue_type_calc
	<begin_calc_functor_t, end_calc_functor_t>
	::type,
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
	 typename ...vipobj_t>
derivedvalueListObj<basedon_t>::calcObj<begin_calc_functor_t,
					calc_functor_t,
					end_calc_functor_t,
					vipobj_t...>
::~calcObj() noexcept
{
}

//! Some value has been updated.

//! Calculated a new derived value, puts it into the very important object.
template<typename basedon_t>
template<typename begin_calc_functor_t,
	 typename calc_functor_t,
	 typename end_calc_functor_t,
	 typename ...vipobj_type>
void derivedvalueListObj<basedon_t>::calcObj<begin_calc_functor_t,
					     calc_functor_t,
					     end_calc_functor_t,
					     vipobj_type...>::updated()
{
	typename vipobj_t::updatelock lock(*this);

	auto v=begin_calc();

	for (const auto &iter:*this->all_values)
	{
		auto p=iter.getptr();

		if (p.null())
			continue;

		typename mpobj<basedon_t>::lock lock(p->latest_value);
			
		calc(v, *lock);
	}

	auto final_value=end_calc(v);
	lock.update(final_value);
}

//! Base class for a \ref derivedvaluelist "derivedvaluelist<value_type>".

//! Refer to this class as "INSERT_LIBX_NAMESPACE::derivedvaluelist<value_type>".
//!
//! \see derivedvalue

template<typename basedon_t>
class derivedvalueListBase {

public:

	//! The wrapper for a value that some other value is derived from.

	//! Refer to this class as "INSERT_LIBX_NAMESPACE::derivedvaluelist<value_type>::base::current_value_t".
	//!
	//! \see derivedvalue

	typedef ref<typename derivedvalueListObj<basedon_t>
		    ::value_fromObj> current_value_t;

	//! Create a new derived value.

	//! Implements the create() method.
	//!
	//! \see derivedvalue
	template<typename T> class objfactory {

	public:

		//! Create a new derived value

		//!
		//! \see derivedvalue

		template<typename B, typename C, typename E, typename ...Args>
		static inline auto create(B &&b, C &&c, E &&e,
					  Args && ...args)
		{
			return ref<typename derivedvalueListObj<basedon_t>
				   ::template calcObj
				   <typename std::decay<B>::type,
				    typename std::decay<C>::type,
				    typename std::decay<E>::type>>
				::create(std::forward<B>(b),
					 std::forward<C>(c),
					 std::forward<E>(e),
					 std::forward<Args>(args)...);
		}
	};
};

//! Base class for a \ref derivedvalue "derivedvalue<value_type, derived_type>".

//! Refer to this class as "INSERT_LIBX_NAMESPACE::derivedvalue<value_type, derived_type>::base".
//!
//! \see derivedvalue

template<typename basedon_t, typename ...vipobj_type>
class derivedvalueBase : public derivedvalueListBase<basedon_t> {

public:

	//! The very important object type.

	//! Refer to this type as "INSERT_LIBX_NAMESPACE::derivedvalue<value_type, derived_type>::base::vipobj_t".
	typedef typename derivedvalueObj<basedon_t, vipobj_type...
					 >::vipobj_t vipobj_t;
};

#if 0
{
#endif
}
#endif
