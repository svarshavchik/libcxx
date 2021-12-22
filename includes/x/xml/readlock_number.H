#ifdef x_xml_readlock_defined
#ifdef x_number_defined

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

template<LIBCXX_NAMESPACE::typesafe_number T>
T readlockObj::get_text() const
{
	return get_text<typename T::value_type>();
}

template<LIBCXX_NAMESPACE::typesafe_number T,
	 typename ...Args, typename>
T readlockObj::get_attribute(Args && ...args) const
{
	return get_attribute<typename T::value_type>(
		std::forward<Args>(args)...
	);
}

template<LIBCXX_NAMESPACE::typesafe_number T>
T LIBCXX_NAMESPACE::xml::readlockObj::get_any_attribute(
	const std::string_view &value
) const
{
	return get_any_attribute<typename T::value_type>(value);
}

#if 0
{
#endif
}

#endif
#endif
