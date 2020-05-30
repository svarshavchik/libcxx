/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_serialize_H
#define x_serialize_H

#include <stdint.h>

#include <string>
#include <cstring>
#include <locale>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <functional>

#include <x/exception.H>
#include <x/serialization.H>
#include <x/container_traits.H>
#include <x/namespace.h>

//! Object serializing

//! This namespace defines the \ref serialize::iterator "iterator"
//! template,
//! an %iterator that %serializes objects into any STL output
//! %iterator that iterators over chars.
//! an %iterator specialization that consumes the serialized bytestream
//! and counts how many bytes were produced; and
//! a pair of object() convenience functions,
//!
//! Other classes in this namespace are internal classes.

namespace LIBCXX_NAMESPACE::serialize {
#if 0
};
#endif

using namespace serialization;

//! Helper for serializing a sequence defined by beginning/ending random access iterators

//! This object takes a beginning and an ending iterator, and turns them into
//! something that pretends to be a container well enough to be serializable
//! so that
//! \c serialize_value<T> does the right thing.

template<typename iter_type_t, typename value_type_t>
class serialize_be_helper {

public:
	//! The iterator type
	typedef iter_type_t const_iterator;

	//! The character type
	typedef value_type_t value_type;

	//! The size type
	typedef size_t size_type;

private:

	//! The beginning iterator
	const_iterator b;

	//! The ending iterator
	const_iterator e;

public:

	//! The constructor saves the iterators

	serialize_be_helper(//! Beginning iterator
			    const const_iterator &bArg,

			    //! Ending iterator
			    const const_iterator &eArg) noexcept
		: b(bArg), e(eArg)
	{
	}

	//! Destructor
	~serialize_be_helper()
	{
	}

	//! Size of this container

	size_t size() const
	{
		size_t n=(size_t)(e-b);

		if (b + n != e)
			serialization::container_toolong();

		return n;
	}

	//! Return the beginning iterator
	const_iterator begin() const noexcept
	{
		return b;
	}

	//! Return the ending iterator
	const_iterator end() const noexcept
	{
		return e;
	}
};

//! %Serialize a native int value in network byte order

//! Invoking
//! <tt>%serialize_bytestream<T,iter_type,sizeof(T)>::%serialize(i, val)</tt>
//! serializes a native int type T into the output iterator, most significant
//! byte first.

template<typename value_type, typename iter_type,
	 int byte_cnt> class serialize_bytestream {

public:
	//! Default function implementation

	//! %Serialize the most significant bytes first,
	//! then %serialize the last byte.

	static inline void serialize(//! Output iterator
				     iter_type &i,
				     //! Value to %serialize
				     value_type val)

	{
		serialize_bytestream<value_type, iter_type, byte_cnt-1>
			::serialize(i, val >> 8);
		serialize_bytestream<value_type, iter_type, 1>
			::serialize(i, val);
	}
};

//! Specialization for serializing the first, most significant byte

//! \see serialize_bytestream<value_type,iter_type,byte_cnt>.
//!

template<typename value_type, typename iter_type>
class serialize_bytestream<value_type, iter_type, 1> {
public:

	//! %Serialize the most significant bytes
	static inline void serialize(iter_type &i, value_type val)

	{
		*i++ = (uint8_t)val;
	}
};

//! %Serialize an object

template<typename value_type>
class serialize_value_nonconst {

	//! %Serialize the name of a class

	//! The 16 bit class name length is serialized, followed by the
	//! bytes of the class name.

	template <typename iter_type>
	static void serialize_typename(//! Class name
				       const char *cp,

				       //! Output iterator
				       iter_type &i)
	{
		size_t l=strlen(cp);
		uint16_t l16=l;

		if (l == 0)
			serialization::classname_empty();
		if (l16 != l)
			serialization::classname_toolong(cp);

		*i++ = (uint8_t)serialization::tobject;
		serialize_bytestream<uint16_t, iter_type,
			sizeof(uint16_t)>::serialize(i, l16);
		while (*cp)
		{
			*i++ = (uint8_t)*cp++;
		}
	}

public:

	//! %Serialize the type of an object

	template <typename iter_type>
	static void serialize_type(//! Output iterator
				   iter_type &i)
	{
		serialize_typename(serialization::object_name<value_type>(), i);
	}

	//! %Serialize the value of an object

	template <typename iter_type, typename serialize_toiter_type>
	static void serialize_bytes(//! Output iterator
				    iter_type &iter,

				    //! The object to serialize
				    const value_type &value,

				    //! Serialization_object
				    serialize_toiter_type &toiter

				    )
	{
		serialization::serialize_object_bytes<value_type>
			::serialize(const_cast<value_type &>(value), toiter);
	}
};


//! %Serialize an object

//! Invoke \c serialize_value<T>::serialize_type(iter_type) to %serialize T's
//! %signature, or a type definition. Then, invoke
//! serialize_value<T>::serialize_bytes(iter, value, serialize_iter)
//! to %serialize an instance of T itself.

template<typename value_type>
using serialize_value=serialize_value_nonconst<std::remove_cv_t<value_type>>;

//! Try serializing something that looks like a native integer type

//! \see serialize_value
//!
template<serializable_integer value_type>
class serialize_value_nonconst<value_type> {
public:
	typedef typename serialization::serialize_integer_type<value_type>
	::int_t native_value_type;

	//! %Serialize a native integer type

	template<typename iter_type>
	static void serialize_type(//! Output iterator
				   iter_type &i)
	{
		*i++ = (uint8_t)serialization::basic_type<native_value_type>
			::type;
	}

	//! %Serialize a native integer value

	template<typename iter_type, typename serialize_toiter_type>
	static void serialize_bytes(//! Output iterator
				    iter_type &i,
				    //! Value to %serialize
				    const value_type &value,

				    //! Serialization_object
				    serialize_toiter_type &toiter)
	{
		serialize_bytestream<native_value_type, iter_type,
				     sizeof(value_type)>
			::serialize(i, (native_value_type)value);
	}
};

//! %Serialize a container

//! \see serialize_value
//!
template<serializable_container container_type>
class serialize_value_nonconst<container_type> {

	//! The container's value
	typedef typename container_type::value_type value_type;

public:

	//! %Serialize the container's type

	template<typename iter_type>
	static void serialize_type(//! Output iterator
				   iter_type &i)

	{
		*i++ = (uint8_t)serialization::tsequence;
		serialize_value<value_type>::serialize_type(i);
	}

	//! %Serialize the contents of the container

	template<typename iter_type,
		 typename container_value_type,
		 typename serialize_toiter_type>
	static void serialize_bytes(//! Output iterator
				    iter_type &i,

				    //! The container
				    container_value_type &&value,

				    //! Serialization_object
				    serialize_toiter_type &toiter

				    )

	{
		auto s=value.size();

		uint32_t seqsize(s);

		if (seqsize != s)
			serialization::container_toolong();

		serialize_bytestream<uint32_t, iter_type, sizeof(uint32_t)>
			::serialize(i, seqsize);

		auto b=value.begin(), e=value.end();

		while (b != e)
		{
			serialize_value<std::remove_cvref_t<decltype(*b)>
					>::serialize_bytes(i, *b++, toiter);
		}
	}
};

//! %Serialize a native float value

//! floats are serialized as strings, and are type-compatible with std::string.
//! \see serialize_value
//!
template<serializable_float value_type>
class serialize_value_nonconst<value_type> {
public:

	//! %Serialize the native float type

	//! This is the same type as a \c std::string because the float
	//! value is converted to a string, for serializing
	template<typename iter_type>
	static void serialize_type(//! Output iterator
				   iter_type &i)
	{
		serialize_value<std::string>::serialize_type(i);
	}

	//! %Serialize the float's value

	template<typename iter_type, typename serialize_toiter_type>
	static void serialize_bytes(//! Output iterator
				    iter_type &i,

				    //! The float value
				    const value_type &value,

				    //! Serialization_object
				    serialize_toiter_type &toiter

				    )
	{
		std::ostringstream s;

		// I found that digits10 is often one less than the real value,
		// so that the results of calculations are rounded off.
		// Furthermore there's usually a partial digit as a result
		// of base 2 digits not fitting evenly into a base 10 value.
		// Therefore, to capture all of the available precision, add
		// 2 to digits10.

		s << std::setprecision(std::numeric_limits<value_type>::digits10+2);

		std::use_facet<std::num_put<char> >(std::locale("C"))
			.put(std::ostreambuf_iterator<char>(s.rdbuf()), s,
			     ' ', value);

		// Use std::string for the heavy lifting

		serialize_value<std::string>
			::serialize_bytes(i, s.str(), toiter);
	}

};

//! Specialization for serializing a \c std::pair

//! \see serialize_value
//!
template<typename first, typename second>
class serialize_value_nonconst< std::pair<first, second> > {
public:

	//! %Serialize the type for \c std::pair

	//! It's simply first's type, the second's type.

	template<typename iter_type>
	static void serialize_type(//! Output iterator
				   iter_type &i)
	{
		*i++ = (uint8_t)serialization::tpair;
		serialize_value<first>::serialize_type(i);
		serialize_value<second>::serialize_type(i);
	}

	//! %Serialize the contents of a \c std::pair

	//! It's simply the contents of first, followed by contents of
	//! second.

	template<typename iter_type, typename serialize_toiter_type>
	static void serialize_bytes(//! Output iterator
				    iter_type &i,

				    //! Output value
				    const std::pair<first, second> &pair,

				    //! Serialization_object
				    serialize_toiter_type &toiter

				    )

	{
		serialize_value<first>::serialize_bytes(i, pair.first, toiter);
		serialize_value<second>::serialize_bytes(i, pair.second,
							 toiter);
	}
};

//! Specialization for serializing a \c std::tuple

//! \see serialize_value
//!
template<typename ...Types>
class serialize_value_nonconst< std::tuple<Types...> > {
public:

	//! %Serialize the type for \c std::tuple

	template<typename iter_type>
	static void serialize_type(//! Output iterator
				   iter_type &i)
	{
		*i++ = (uint8_t)serialization::ttuple;

		serialize_bytestream<uint32_t, iter_type, sizeof(uint32_t)>
			::serialize(i, sizeof...(Types));

		(serialize_value<Types>::serialize_type(i), ...);
	}

	//! %Serialize the contents of a \c std::tuple

	template<typename iter_type, typename serialize_toiter_type>
	static void serialize_bytes(//! Output iterator
				    iter_type &i,

				    //! Output value
				    const std::tuple<Types...> &tuple,

				    //! Serialization_object
				    serialize_toiter_type &toiter

				    )
	{
		std::apply([&]
			   (const Types & ...values)
			   {
				   (serialize_value<Types>::serialize_bytes
				    (i, values, toiter), ...);
			   }, tuple);
	}
};

//! Specialization for serializing a \c std::reference_wrapper

//! \see serialize_value
//!
template<typename value_type>
class serialize_value_nonconst< std::reference_wrapper<value_type> > {
public:

	//! %Serialize the type for the referneced object.

	template<typename iter_type>
	static void serialize_type(//! Output iterator
				   iter_type &i)
	{
		serialize_value<value_type>::serialize_type(i);
	}

	//! %Serialize the contents of a \c std::tuple

	template<typename iter_type, typename serialize_toiter_type>
	static void serialize_bytes(//! Output iterator
				    iter_type &i,

				    //! Output value
				    const std::reference_wrapper<value_type> &r,

				    //! Serialization_object
				    serialize_toiter_type &toiter)
	{
		serialize_value<value_type>::serialize_bytes(i, r, toiter);
	}
};

//! Specialization for serializing a native string type

//! \see serialize_value
//!
template<>
class serialize_value_nonconst<const char *> {

public:
	//! %Serialize the type of a literal string

	//! Instantiate the helper class, and let it figure it out.
	//!

	template <typename iter_type>
	static void serialize_type(//! Output iterator
				   iter_type &iter)
	{
		serialize_value<std::string>::serialize_type(iter);
	}

	//! %Serialize the value of a literal string

	//! Instantiates a helper object that makes it look like a
	//! \c std::basic string, then serializes that as a container.

	template <typename iter_type, typename serialize_toiter_type>
	static void serialize_bytes(//! Output iterator
				    iter_type &iter,

				    //! Native string
				    const char *str,

				    //! Serialization_object
				    serialize_toiter_type &toiter

				    )
	{
		serialize_be_helper<const char *, char>
			helper(str, str+strlen(str));

		serialize_value<std::string>
			::serialize_bytes(iter, helper, toiter);
	}
};


//! Specialization for serializing a native string type

//! \see serialize_value
//!
template<size_t n>
class serialize_value_nonconst<const char[n]>
	: public serialize_value_nonconst<const char *> {
};

//! Specialization for serializing a native string type

//! \see serialize_value
//!
template<size_t n>
class serialize_value_nonconst<char[n]>
	: public serialize_value_nonconst<const char *> {
};

//! %Serialize a pointer to an object

//! \see serialize_value
//!
template<typename obj_type, typename base_type>
class serialize_value_nonconst<ptr<obj_type, base_type> > {

public:

	//! %Serialize the referenced type.

	template <typename iter_type>
	static void serialize_type(//! Output iterator
				   iter_type &i)
	{
		*i++=(uint8_t)serialization::treference;
	}

	//! %Serialize the value of a referenced object.

	//! It's either 0, for a null reference, or 1 followed by the
	//! type signature and a value of the referenced object.
	template<typename iter_type, typename serialize_toiter_type>
	static void serialize_bytes(//! Output iterator
				    iter_type &i,

				    //! Output value
				    const const_ptr<obj_type, base_type> &value,

				    //! Serialization_object
				    serialize_toiter_type &toiter

				    )

	{
		if (value.null())
		{
			*i++=(uint8_t)0;
			return;
		}
		*i++=(uint8_t)1;

		toiter(*value);
	}
};

//! %Serialize a reference to an object, similar to ptr, but we know we always have the object.

template<typename obj_type, typename base_type>
class serialize_value_nonconst<ref<obj_type, base_type> > {

public:

	//! %Serialize the reference type

	template <typename iter_type>
	static void serialize_type(//! Output iterator
				   iter_type &i)
	{
		*i++=(uint8_t)serialization::treference;
	}

	//! %Serialize the value of a reference.

	//! It's either 0, for a null reference, or 1 followed by the
	//! type signature and a value of the referenced object.
	template<typename iter_type, typename serialize_toiter_type>
	static void serialize_bytes(//! Output iterator
				    iter_type &i,

				    //! Output value
				    const const_ref<obj_type, base_type> &value,

				    //! Serialization_object
				    serialize_toiter_type &toiter

				    )

	{
		*i++=(uint8_t)1;

		toiter(*value);
	}
};

template<typename obj_type, typename base_type>
class serialize_value_nonconst<const_ptr<obj_type, base_type> >
	: public serialize_value_nonconst<ptr<obj_type, base_type> > {};

template<typename obj_type, typename base_type>
class serialize_value_nonconst<const_ref<obj_type, base_type> >
	: public serialize_value_nonconst<ref<obj_type, base_type> > {};

//! A minimal output iterator that counts the serialized bytes

//! Use this output iterator with serialize<traits_type>::iterator
//! to count the serialized bytes.
//!
//! The serialized contents of an object are thrown away, but the number
//! of serialized bytes are counted. Use this iterator to serialize an object,
//! invoke counter(), preallocate a buffer, then reserialize the same object
//! into the preallocated buffer. Although the object gets serialized twice,
//! some benefit may be gained from preallocating a single output buffer.

class sizeof_iter {

	//! Count of bytes reached.

	size_t cnt;
public:
	//! Iterator trait
	typedef std::output_iterator_tag iterator_category;

	//! Iterator trait
	typedef void value_type;

	//! Iterator trait
	typedef void difference_type;

	//! Iterator trait
	typedef void pointer;

	//! Iterator trait
	typedef void reference;

	//! Constructor
	sizeof_iter() noexcept : cnt(0)
	{
	}

	//! Destructor

	~sizeof_iter()=default;

	//! Dummy * operator does nothing

	sizeof_iter &operator*() noexcept
	{
		return *this;
	}

	//! Dummy ++ operator does nothing

	sizeof_iter &operator++() noexcept
	{
		return *this;
	}

	//! Dummy ++ operator does nothing

	sizeof_iter &operator++(int) noexcept
	{
		return *this;
	}

	//! The assignment operator increments the count

	void operator=(uint8_t value) noexcept
	{
		++cnt;
	}

	//! Retrieve the count of serialized bytes

	size_t counter() const noexcept
	{
		return cnt;
	}

};

//! %Serialize an object into an output %iterator

//! The constructor takes a reference to an output iterator.
//! Invoking \c operator() serializes the passed object into the output
//! %iterator.

template<typename iter_type>
class iterator {

	//! The output iterator
	iter_type &iter;

public:

	//! This is a %serializing iterator

	static constexpr bool serializing=true;

	//! The underlying iterator

	typedef iter_type raw_iter_t;

	//! Return the underlying iterator, when You Know What You Are Doing

	iter_type &getiter() noexcept
	{
		return iter;
	}

	//! Constructor

	iterator(//! Output iterator
		 iter_type &iterArg) noexcept : iter(iterArg)
	{
	}

	//! Destructor

	~iterator()
	{
	}

	//! %Serialize an object

	//! The object is serialized by serializing its type followed by
	//! a value.
	//!
	//! \return a reference to \c *this, for a shorthand:
	//!
	//! \c iter(obj1)(obj2)(obj3)...

	template<typename value_type>
	iterator<iter_type> &operator()(//! Value to %serialize
					const value_type &value)

	{
		typedef serialize_value<value_type> serialize_value_t;

		serialize_value_t::serialize_type(iter);
		serialize_value_t::serialize_bytes(iter, value, *this);
		return *this;
	}

	//! The serialization iterator ignores the maximum sequence size

	//! The sequence, as is, gets serialized
	//!

	template<typename value_type>
	iterator<iter_type> &operator()(//! Value to %serialize
					const value_type &value,

					//! Ignored
					uint32_t maxseqsize)

	{
		return operator()(value);
	}

	//! Serialize a range defined by a separate beginning/ending iterator

	template<typename seq_iter_type>
	iterator<iter_type> &range(//! The beginning iterator
				   const seq_iter_type &b,

				   //! The ending iterator
				   const seq_iter_type &e)

	{
		typedef typename
			std::iterator_traits<seq_iter_type>::value_type
			value_type;

		typedef serialize_be_helper<seq_iter_type, value_type
					    > helper_t;

		typedef serialize_value<std::vector<value_type>
					> serialize_value_t;

		serialize_value_t::serialize_type(iter);

		helper_t helper{b, e};

		serialize_value_t::serialize_bytes(iter, helper, *this);

		return *this;
	}
};

//! Convenience function for serializing some object into an output %iterator

//! Constructs a new iterator
//! object, then invokes its operator() to
//! %serialize the specified object.
//!
//! \return the new value of the output %iterator

template<typename object_type, typename iter_type>
static iter_type object(const object_type &obj, iter_type iter)

{
	iterator<iter_type> i(iter);

	i(obj);

	return iter;
}

//! Base class of sizeof_iterator

//! Define a %serialization %iterator that counts the number of
//! serialized bytes.

typedef iterator<sizeof_iter> sizeof_iter_iterator_t;

//! An all-in-one %serialized object size counter

//! This object derives from sizeof_iter and
//! iterator<sizeof_iter>
//! to produce
//! a single object for %serializing which counts the number of bytes to
//! serialize.
//!
//! Instantiate the object, use the same API that's documented by
//! iterator, then invoke counter() to figure out how many bytes
//! %serializing the objects produces.

class sizeof_iterator : public sizeof_iter,
			public sizeof_iter_iterator_t {

public:
	//! Constructor
	sizeof_iterator() noexcept
	: sizeof_iter_iterator_t((sizeof_iter &)*this)
	{
	}

	//! Destructor
	~sizeof_iterator()
	{
	}

	using sizeof_iter_iterator_t::operator();
};

//! Convenience function for computing the size of a serialized object

//! Constructs a sizeof_iterator object, then invokes its operator() to
//! %serialize the specified object.
//!
//! Computing the size of a %serialized object is an expensive
//! proposition.
//! The object gets to be %serialized twice, once to compute its size,
//! and
//! presumably a second time to actually %serialize it. Furthermore, the
//! object cannot change between its first and second %serialization,
//! of course.
//!
//! \return the count of the number of bytes produced by %serializing
//! the given object.

template<typename object_type>
static size_t object(const object_type &obj)

{
	sizeof_iterator s_iter;

	s_iter(obj);

	return s_iter.counter();
}

#if 0
{
#endif
}
#endif
