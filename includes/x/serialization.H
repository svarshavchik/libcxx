/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_serialization_H
#define x_serialization_H

#include <stdint.h>
#include <limits>
#include <typeinfo>
#include <type_traits>
#include <x/exception.H>
#include <x/serializationfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	//! Common classes used for serializing and deserializing objects

	//! %Serialization is the process of converting an object to a sequence
	//! of bytes, which can be saved to a file or a buffer, and subsequently
	//! recreating the object from the saved byte sequence.
	//!
	//! The \c serialize namespace defines templates for serializing
	//! objects, the \c deserialize namespace defines templates for
	//! deserializing objects. Objects may be serializing and deserialized
	//! using any STL iterator that iterates over chars.
	//!
	//! \par %Serializing
	//!
	//! The \ref serialize "serialize" namespace
	//! defines classes for %serializing objects
	//! that are bound to a given traits class.
	//!
	//! serialize::iterator handles object %serialization. Its constructor
	//! takes a reference to an output iterator.
	//! serialize::iterator saves a reference to the output iterator,
	//! which must exist as long as
	//! serialize::iterator itself remains in scope.
	//! The output iterator's value type must be either \c char or
	//! \c unsigned \c char.
	//!
	//! \c serialize::iterator::operator()
	//! serializes the passed object into the underlying output iterator.
	//!
	//! serialize::sizeof_iterator is a subclass of
	//! serialize::iterator that throws
	//! away the serialized bytes, after counting them.
	//! Its \c counter() method returns the byte count of the serialized
	//! object. After allocating a suitably-sized buffer, the object
	//! can be serialize again into the newly-allocated buffer.
	//!
	//! It goes without saying that the
	//! serialized objects cannot be modified between
	//! the first and the second serialization pass in a manner that
	//! affects their serialized representation.
	//!
	//! serialize::object() is a convenience function.
	//! The single argument version of serialize::object() constructs
	//! an instance of serialize::sizeof_iterator,
	//! %serializes the given object,
	//! and returns the resulting byte count.
	//!
	//! The two argument version of serialize::object() constructs an
	//! instance of serialize::iterator and serializes the given object.
	//!
	//! \par Deserializing
	//!
	//! The \ref deserialize "deserialize"
	//! namespace defines classes for %deserializing objects
	//! that are bound to a given traits class. The first template parameter
	//! is an input iterator class that iterates over chars or unsigned
	//! chars. The second optional template parameter is a traits class.
	//! The traits class defines a static classcreate() function which is
	//! used to initialize a reference to an object before deserializing.
	//! The default implementation checks if the reference is already
	//! initialized, if not a new object gets created using its default
	//! constructor. A specialization for \ref fd "file descriptor"
	//! object references uses \ref fdBase::tmpfile() "fd::tmpfile()" to create
	//! a temporary file unless the file descriptor object passed to the
	//! iterator is already open.
	//!
	//! deserialize::iterator gets constructed to %deserialize a previously
	//! %serialized byte stream. The constructor
	//! receives references to the beginning and
	//! an ending input iterators that specify the byte sequence which
	//! represent the serialized object or objects.
	//!
	//! These iterators must remain in scope as long as
	//! deserialize::iterator remains in scope.
	//!
	//! After constructing deserialize::iterator, pass the objects to
	//! %deserialize to \c deserialize::operator().
	//!
	//! An %exception gets thrown if the type of objects that were
	//! %serialized do not match the type of objects to
	//! %deserialize (but see below).
	//!
	//! Note that objects being %deserialized must be constructed beforehand
	//! and then %deserialized.
	//!
	//! This deserialize::object() convenience function takes a reference to
	//! an object, a beginning iterator, and an ending iterator.
	//! A deserialize::iterator object gets constructed from the two
	//! iterators, then the object referenced by the first argument
	//! to deserialize::object() gets %deserialized.
	//!
	//! The two argument version of deserialize::object() takes a container
	//! for its second argument. The container's begin() and end()
	//! method define the beginning and the ending iterator, which are
	//! %deserialized accordingly into the object referenced by its
	//! first argument.

	namespace serialization {
#if 0
	};
};
#endif

//! Serialized datatypes

enum datatype {
	tbool=0,
	tint8,
	tuint8,
	tint16,
	tuint16,
	tint32,
	tuint32,
	tint64,
	tuint64,

	tsequence=32,
	tpair,

	tobject=64,
	treference
};

template<bool is_integer, typename value_type> class basic_int_type;

template<bool is_signed, size_t size> class basic_int_type_bysize;

//! A specialization that defines a datatype for a given C++ type

//! \c basic_type<T>::type returns a datatype that corresponds to the
//! given type.
//!
//! \c basic_type<int>::type would be \c tint32 (if sizeof(int) is 4).

template<typename value_type> class basic_type {

public:

	//! The corresponding datatype
	static constexpr datatype type=
		basic_int_type<std::numeric_limits<std::remove_const_t
						   <value_type>>::is_integer,
			       std::remove_const_t<value_type>>::type;
};

//! Specialization for bools

//! \see basic_type
//!
template<> class basic_type<bool> {

public:

	//! This is a \c bool

	static constexpr datatype type=tbool;
};

//! Specialization for bools

//! \see basic_type
//!
template<> class basic_type<const bool> {

public:
	//! This is a \c bool

	static constexpr datatype type=tbool;
};

//! Specialization for native integer types

//! \see basic_type
//!
template<typename value_type>
class basic_int_type<true, value_type> {

public:
	//! Define the integer type based on its size and sign-ness

	static constexpr datatype type=
		basic_int_type_bysize<std::numeric_limits<value_type>
				      ::is_signed,
				      sizeof(value_type)>::type;
};

//! A signed 8 bit datatype

//! \see basic_type
//!
template<>
class basic_int_type_bysize<true, 1> {

public:
	//! A signed 8 bit datatype

	static constexpr datatype type=tint8;
};

//! An unsigned 8 bit datatype

//! \see basic_type
//!
template<>
class basic_int_type_bysize<false, 1> {

public:

	//! An unsigned 8 bit datatype

	static constexpr datatype type=tuint8;
};

//! A signed 16 bit datatype

//! \see basic_type
//!
template<>
class basic_int_type_bysize<true, 2> {

public:

	//! A signed 16 bit datatype

	static constexpr datatype type=tint16;
};

//! An unsigned 16 bit datatype

//! \see basic_type
//!
template<>
class basic_int_type_bysize<false, 2> {

public:

	//! An unsigned 16 bit datatype

	static constexpr datatype type=tuint16;
};

//! A signed 32 bit datatype

//! \see basic_type
//!
template<>
class basic_int_type_bysize<true, 4> {

public:

	//! A signed 32 bit datatype

	static constexpr datatype type=tint32;
};

//! An unsigned 32 bit datatype

//! \see basic_type
//!
template<>
class basic_int_type_bysize<false, 4> {

public:

	//! An unsigned 32 bit datatype

	static constexpr datatype type=tuint32;
};

//! A signed 64 bit datatype

//! \see basic_type
//!
template<>
class basic_int_type_bysize<true, 8> {

public:

	//! A signed 64 bit datatype

	static constexpr datatype type=tint64;
};

//! An unsigned 64 bit datatype

//! \see basic_type
//!
template<>
class basic_int_type_bysize<false, 8> {

public:
	//! An unsigned 64 bit datatype

	static constexpr datatype type=tuint64;
};

//! Return a printable label for the given datatype

const char *tname(datatype value) noexcept;

//! Number of objects in a container exceeds the maximum value

//! Throw an %exception.
//!

void container_toolong() __attribute__((noreturn));

//! The name of a class cannot be empty

//! Throw an %exception.
//!
void classname_empty() __attribute__((noreturn));

//! The name of a class is too long

//! Throw an %exception.
//!
void classname_toolong(const char *)
	__attribute__((noreturn));

//! Helper class for serializing enumerated class

//! Provide a specialization for serialize_integer_type<enumeration_type>
//! that defines int_t as a natural integer type, in order to be able to
//! serialize and deserialize enumerated values.

template<typename int_type> class serialize_integer_type {

public:

	//! Native integer type for serialization of enumerated type.

	//! If the template parameter is not an enumerated type, the default
	//! template, this is just the template parameter class.
	typedef int_type int_t;
};

//! Helper macro for implemented serialization of enumerated classes

#define LIBCXX_SERIALIZE_ENUMERATION(enum_t,native_int_t)			\
	namespace LIBCXX_NAMESPACE {					\
		namespace serialization {				\
			template<> class serialize_integer_type<enum_t> { \
			public:						\
			typedef native_int_t int_t;			\
			};						\
		};							\
	}

//! \c ptr<obj> references a class that we do not know how to serialize

//! Throw an %exception.
//!

void unknown_class(obj *)
	__attribute__((noreturn));

//! \c Do not know how to deserialize on object, for a ptr<obj>,

//! Throw an %exception.
//!

void unknown_class(const std::string &classname)
	__attribute__((noreturn));


//! Default class that gives the name of a type

//! Instantiating \c object_name<T> gives a class that's convertable to a
//! \c const \c char \c *.
//!
//! The default name is the demangled type name.
//! This class can be specialized to use an explicit name of a class.

template<typename object>
class object_name {

	//! Buffer for the demangled name
	std::string namebuf;
public:
	object_name() noexcept
	{
		obj::demangle(typeid(object).name(), namebuf);
	}

	~object_name()
	{
	}

	//! Return the name of the class

	operator const char *() const noexcept
	{
		return namebuf.c_str();
	}
};

//! Default object %serialization method

//! \c serialize_object_bytes<T>::serialize(T &value, iterator_type &iter)
//! gets invoked to serialize or deserialize an object by repeatedly invoking
//! iter(member).
//!
//! The default implementation calls \c value's serialize() template.
//! serialize_object_bytes may be specialized to define a serialize() template
//! that's not a member of the given class.

template<typename value_type> class serialize_object_bytes {

public:
	//! Invoke the object's serialize() template

	template <typename iter_type>
	static void serialize(//! Object
			      value_type &value,

			      //! Iterator for %serialization or deserialization
			      iter_type &iter)
	{
		value.serialize(iter);
	}
};

#if 0
{
	{
#endif
	}
}
#endif
