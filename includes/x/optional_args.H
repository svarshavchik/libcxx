/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_optional_args_H
#define x_optional_args_H

#include <x/optional_argsfwd.H>
#include <optional>
#include <functional>
#include <type_traits>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<> class optional_args<> {

public:
	template<typename ...Args>
	optional_args(Args && ...)
	{
		static_assert(sizeof...(Args) == 0,
			      "optional_args constructor parameters did not match");
	}
};

template<typename Arg, typename ...Args>
class optional_args<Arg, Args...> : public optional_args<Args...> {

public:

	static_assert(std::is_same_v<Arg,
		      std::remove_const_t<std::remove_reference_t<Arg>>>,
		      "Template parameter to optional_args cannot be a "
		      "reference or a const-qualified type");

	//! A value was specified

	std::optional<Arg> value;

	//! Default constructor
	optional_args()=default;

	//! Copy-construct this parameter.

	template<typename ...OtherArgs>
	optional_args(const Arg &a,
		      OtherArgs && ...args)
		: optional_args<Args...>{std::forward<OtherArgs>(args)...},
		  value{a}
	{
	}

	//! Move construct this parameter.
	template<typename ...OtherArgs>
	optional_args(Arg &&a,
		      OtherArgs && ...args)
		: optional_args<Args...>{std::forward<OtherArgs>(args)...},
		  value{std::forward<Arg>(a)}
	{
	}

	//! In-place construct this parameter.

	//! If the next constructor parameter can be used to construct Arg,
	//! (but not std::optional<Arg>), then use it to construct it.
	template<typename FirstArg,
		 typename ...OtherArgs,
		 typename=std::enable_if_t<
			 std::conjunction_v<
				 std::is_constructible<Arg, FirstArg &&>,
				 std::negation<std::is_constructible
					       <std::optional<Arg>,
						FirstArg &&>>
				 >>>
	optional_args(FirstArg &&arg,
		      OtherArgs && ...args)
		: optional_args<Args...>{std::forward<OtherArgs>(args)...},
		  value{std::forward<FirstArg>(arg)}
	{
	}

	//! Will not construct this parameter.

	//! The next argument to our constructor cannot be used to construct
	//! this parameter, so forward it along to the next one.
	template<typename FirstArg,
		 typename ...OtherArgs,
		 typename=void,
		 typename=std::enable_if_t<
			 std::negation_v<std::is_constructible<Arg,
							       FirstArg &&>>>>
	optional_args(FirstArg &&arg,
		      OtherArgs && ...args)
		: optional_args<Args...>{std::forward<FirstArg>(arg),
						 std::forward<OtherArgs>
						 (args)...}
	{
	}
};

//! Helper for get_optional_arg that takes a type parameter.

//! The default template is for the case where the specified type does not
//! match the type of the first optional_args type. get() is implemented
//! by forwarding the call to the next get_optional_arg_helper, for the next
//! optional_args.

template<typename Arg, typename FirstArg, typename ...Args>
class get_optional_arg_helper {

public:

	//! Whether this is a unique optional_args<> type.

	static constexpr bool argument_type_is_unique=
		get_optional_arg_helper<Arg, Args...>::argument_type_is_unique;

	//! Implement get() for a const optional_args.

	static inline const auto &
	get(const optional_args<FirstArg, Args...> &a)
	{
		return get_optional_arg_helper<Arg, Args...>::get(a);
	}

	//! Implement get() for a mutable optional_args.

	static inline auto &
	get(optional_args<FirstArg, Args...> &a)
	{
		return get_optional_arg_helper<Arg, Args...>::get(a);
	}
};

//! Specialization of get_optional_arg_helper.

//! Defines argument_type_is_unique by searching the remaining ...Args for the
//! same type.
//!
//! Implements get().

template<typename Arg, typename ...Args>
class get_optional_arg_helper<Arg, Arg, Args...> {
public:

	//! Indicate whether the type is unique.
	static constexpr bool argument_type_is_unique=
		(std::negation_v<std::is_same<Arg, Args>> && ...);

	//! Implement get() for a const optional_args.

	static inline const std::optional<Arg> &
	get(const optional_args<Arg, Args...> &a)
	{
		return a.value;
	}

	//! Implement get() for a mutable optional_args.

	static inline std::optional<Arg> &
	get(optional_args<Arg, Args...> &a)
	{
		return a.value;
	}
};

//! Return the parsed \ref optional_args "optional argument".

//! The template parameter is a type, returns the corresponding
//! std::optional<type> from the optional_args.

template<typename Arg, typename ...Args>
inline const auto &optional_arg(const optional_args<Args...> &a)
{
	static_assert(get_optional_arg_helper<Arg, Args...>
		      ::argument_type_is_unique,
		      "optional_arg template type is not unique");

	return get_optional_arg_helper<Arg, Args...>::get(a);
}

//! Return the parsed \ref optional_args "optional argument".

//! The template parameter is a type, returns the corresponding
//! std::optional<type> from the optional_args.

template<typename Arg, typename ...Args>
inline auto &optional_arg(optional_args<Args...> &a)
{
	static_assert(get_optional_arg_helper<Arg, Args...>
		      ::argument_type_is_unique,
		      "optional_arg template type is not unique");

	return get_optional_arg_helper<Arg, Args...>::get(a);
}

//! Helper for get_optional_arg that takes an index value template parameter.

//! The default template is for the case where the index value is not 0.
//! get() is implemented by forwarding the call to the next
//! get_optional_arg_helper, for the next optional_args, and index value-1.

template<size_t n, typename Arg, typename ...Args>
class get_optional_arg_index_helper {

public:

	static inline const auto &get(const optional_args<Arg, Args...> &a)
	{
		return get_optional_arg_index_helper<n-1, Args...>::get(a);
	}

	static inline auto &get(optional_args<Arg, Args...> &a)
	{
		return get_optional_arg_index_helper<n-1, Args...>::get(a);
	}
};

//! Specialization for the 0 index value.

//! Return the corresponding parameter.

template<typename Arg, typename ...Args>
class get_optional_arg_index_helper<0, Arg, Args...> {

public:

	static inline const auto &get(const optional_args<Arg, Args...> &a)
	{
		return a.value;
	}

	static inline auto &get(optional_args<Arg, Args...> &a)
	{
		return a.value;
	}
};

//! Return the parsed \ref optional_args "optional argument".

//! The template parameter is a 0-based index of the
//! std::optional<type> from the optional_args.

template<size_t n, typename ...Args>
inline const auto &optional_arg(const optional_args<Args...> &a)
{
	return get_optional_arg_index_helper<n, Args...>::get(a);
}


//! Return the parsed \ref optional_args "optional argument".

//! The template parameter is a 0-based index of the
//! std::optional<type> from the optional_args.

template<size_t n, typename ...Args>
inline auto &optional_arg(optional_args<Args...> &a)
{
	return get_optional_arg_index_helper<n, Args...>::get(a);
}

template<typename ...Args>
class optional_argconstrefs : public optional_args<std::reference_wrapper
						   <const Args>...> {

public:

	static_assert((std::is_same_v<Args,
		       std::remove_const_t<std::remove_reference_t<Args>>>
		       && ...),
		      "Template parameter to optional_argconstrefs cannot be a "
		      "reference or a const-qualified type");

	//! The underlying optional_args.

	typedef optional_args<std::reference_wrapper<const Args>...> type;

	//! Constructor
	template<typename ...cArgs>
	inline optional_argconstrefs(const cArgs & ...args)
		: type{args...}
	{
	}

	//! Return the underlying optional_args
	inline type &get()
	{
		return *this;
	}

	//! Return the underlying optional_args
	inline const type &get() const
	{
		return *this;
	}
};

//! Return the optional reference wrapper.
template<typename Arg, typename ...Args>
inline const auto &optional_arg(const optional_argconstrefs<Args...> &a)
{
	return optional_arg<std::reference_wrapper<const Arg>>(a.get());
}

//! Return the optional reference wrapper.
template<typename Arg, typename ...Args>
inline auto &optional_arg(optional_argconstrefs<Args...> &a)
{
	return optional_arg<std::reference_wrapper<const Arg>>(a.get());
}

//! Return the optional reference wrapper.
template<size_t n, typename ...Args>
inline const auto &optional_arg(const optional_argconstrefs<Args...> &a)
{
	return optional_arg<n>(a.get());
}

//! Return the optional reference wrapper.
template<size_t n, typename ...Args>
inline auto &optional_arg(optional_argconstrefs<Args...> &a)
{
	return optional_arg<n>(a.get());
}

//! Return the parsed \ref optional_args "optional argument" or a default value.

//! Like \ref optional_arg "optional_arg", but takes an extra \c std::optional
//! parameter, that's presumably uninitialized. If the specified optional
//! paramer was not constructed, the \c std::optional gets emplaced(), using
//! any forwarded parameters, so that a reference to the requested type gets
//! always returned.

template<typename selected,
	 typename ...OptArgs,
	 typename optional_type,
	 typename ...Args>
inline optional_type
&optional_arg_or(optional_args<OptArgs...> &param,
		 std::optional<optional_type> &value,
		 Args && ...args)
{
	auto &explicit_option=optional_arg<selected>(param);

	if (explicit_option)
		return explicit_option.value();

	value.emplace(std::forward<Args>(args)...);

	return *value;
}

//! Return the parsed \ref optional_args "optional argument" or a default value.

//! \overload

template<typename selected,
	 typename ...OptArgs,
	 typename optional_type,
	 typename ...Args>
inline const optional_type
&optional_arg_or(const optional_args<OptArgs...> &param,
		 std::optional<optional_type> &value,
		 Args && ...args)
{
	const auto &explicit_option=optional_arg<selected>(param);

	if (explicit_option)
		return explicit_option.value();

	value.emplace(std::forward<Args>(args)...);

	return *value;
}

//! Deduce the type extracted by optional_arg_or

template<typename T> struct deduce_argconstref;

//! Deduce the type extracted by optional_arg_or

template<typename T>
struct deduce_argconstref<std::reference_wrapper<const T>> {

	typedef T type;
};

//! Deduce the type extracted by optional_arg_or

template<typename T>
using deduce_argconstref_t=
	typename deduce_argconstref<std::remove_cv_t
				    <std::remove_reference_t<T>>>::type;

//! Return the parsed \ref optional_args "optional argument" or a default value.

//! \overload

template<size_t selected,
	 typename ...OptArgs,
	 typename optional_type,
	 typename ...Args>
inline optional_type
&optional_arg_or(optional_args<OptArgs...> &param,
		 std::optional<optional_type> &value,
		 Args && ...args)
{
	auto &explicit_option=optional_arg<selected>(param);

	if (explicit_option)
		return explicit_option.value();

	value.emplace(std::forward<Args>(args)...);

	return *value;
}


//! Return the parsed \ref optional_args "optional argument" or a default value.

//! \overload

template<size_t selected,
	 typename ...OptArgs,
	 typename optional_type,
	 typename ...Args>
inline const optional_type
&optional_arg_or(const optional_args<OptArgs...> &param,
		 std::optional<optional_type> &value,
		 Args && ...args)
{
	const auto &explicit_option=optional_arg<selected>(param);

	if (explicit_option)
		return explicit_option.value();

	value.emplace(std::forward<Args>(args)...);

	return *value;
}


//! Return the parsed \ref optional_args "optional argument" or a default value.

//! \overload
//!
//! Overloaded for a \ref optional_argconstrefs "optional_argconstrefs" which
//! always constructs a \c std::reference_wrapper. This overload always
//! returns a \c const reference, since the \c std::reference_wrapper wraps
//! a \c const reference itself.

template<typename selected,
	 typename ...OptArgs,
	 typename optional_type,
	 typename ...Args>
inline const auto
&optional_arg_or(const optional_argconstrefs<OptArgs...> &param,
		 std::optional<optional_type> &value,
		 Args && ...args)
{
	auto &explicit_option=optional_arg<selected>(param);

	typedef deduce_argconstref_t<decltype(explicit_option.value())> ret_t;

	if (explicit_option)
		return static_cast<const ret_t &>(explicit_option.value());

	value.emplace(std::forward<Args>(args)...);

	return static_cast<const ret_t &>(*value);
}

//! Return the parsed \ref optional_args "optional argument" or a default value.

//! \overload
//!

template<size_t selected,
	 typename ...OptArgs,
	 typename optional_type,
	 typename ...Args>
inline const auto
&optional_arg_or(const optional_argconstrefs<OptArgs...> &param,
		 std::optional<optional_type> &value,
		 Args && ...args)
{
	auto &explicit_option=optional_arg<selected>(param);

	typedef deduce_argconstref_t<decltype(explicit_option.value())> ret_t;

	if (explicit_option)
		return static_cast<const ret_t &>(explicit_option.value());

	value.emplace(std::forward<Args>(args)...);

	return static_cast<const ret_t &>(*value);
}

#if 0
{
#endif
}

#endif
