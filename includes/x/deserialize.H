/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_deserialize_H
#define x_deserialize_H

#include <stdint.h>

#include <string>
#include <cstring>
#include <cwchar>
#include <sstream>
#include <locale>
#include <vector>
#include <type_traits>
#include <unordered_map>
#include <functional>
#include <charconv>

#include <x/exception.H>
#include <x/serialization.H>
#include <x/namespace.h>

//! Object deserializing

//! This namespace defines the
//! \ref deserialize::iterator "iterator" member class, a template for
//! an %iterator that %deserializes objects from any STL input
//! %iterator, and a pair of object() convenience functions.
//!
//! Other classes in this namespace are internal classes.

namespace LIBCXX_NAMESPACE::deserialize {
#if 0
};
#endif

using namespace serialization;

template<typename value_type> class deserialize_value_nonconst;

//! %Deserialize a value

//! Instantiate \c deserialize<T> to create an instance of an object that
//! handles deserialization of T.
//!
//! First, call the \c verify_type(iter) method. It returns \c true if the
//! deserialized signature matches the expected signature for T.
//!
//! Second, call the \c deserialize_type(value, iter) method to place the
//! %deserialized value into \c value.
//!
//! If \c verify_iter() returned \c false, invoke \c output_type(o,flag) to
//! write either the expected type, when \c flag is false, or the partially
//! deserialized type, when \c flag is true, the mismatched type, onto the
//! \c ostream o.
//!
//! Each specialized instance of deserialize_value also defines a class type,
//! \c result_type, which is basically the deserialized type without any
//! const qualifier. \c std::map and \c std::multimap contain \c std::pair
//! values whose \c first member is a const value.
//!
//! In order for the first's parser to be able to actually deserialize something
//! and place the deserialized value into the object, it needs to have its
//! const qualifier stripped.

template<typename value_type>
using deserialize_value=
	deserialize_value_nonconst<std::remove_cv_t<value_type>>;

//! %Deserialized sequence cannot be converted to a floating point value

//! Throw an %exception.
//!

void floatmismatch(const std::string &actual)
	__attribute__((noreturn));

//! %Deserialize type does not match the expected type

//! Throw an %exception.
//!
void mismatch(const std::string &expected,
	      const std::string &actual)
	__attribute__((noreturn));

//! End of input sequence reached before deserialization completed

//! Throw an %exception.
//!
void serialization_truncated()
	__attribute__((noreturn));

//! Deserialization value could not be inserted into its container

//! Throw an %exception.
//!
void deserialization_failure(const std::string &type)
	__attribute__((noreturn));

//! Deserialized a null object reference.

//! Throw an %exception.
//!
void deserialization_nullobj(const std::string &type)
	__attribute__((noreturn));

//! %Deserialize a native integer value

//! Invoke
//! <tt>%deserialize_bytestream<iter,INTTYPE,sizeof(INTTYPE)>::%deserialize(i)</tt>
//! to %deserialize a native integer value, in network byte order.

template<typename iter_type, typename value_type, int byte_cnt>
class deserialize_bytestream {

public:
	//! Default template - deserialize the high order bytes, first

	static value_type deserialize(//! The deserialization iterator
				      iter_type &i)
	{
		return (deserialize_bytestream<iter_type, value_type,
			byte_cnt-1> ::deserialize(i) << 8) | i.get();
	}
};

//! Specialization for %deserialize the first, most significant byte

template<typename iter_type, typename value_type>
class deserialize_bytestream<iter_type, value_type, 1> {

public:
	//! Deserialize the most significant byte

	static value_type deserialize(//! The deserialization iterator
				      iter_type &i)
	{
		return i.get();
	}
};

//! Deserialize something that's not a container, perhaps it's an object

template<typename object_type>
class deserialize_value_nonconst {

public:

	//! Succesful %deserialization of the signature
	bool err_flag;

	//! %Deserialization failed, deserialized a different type
	datatype err_value;

	//! Validate the expected object signature, the object's name

	template<typename iter_type>
	bool verify_typename(//! Expected type name
			     const char *cp,

			     //! The deserialization iterator
			     iter_type &i)
	{
		if ( (err_value=(datatype)i.get()) != serialization::tobject )
		{
			err_flag=true;
			return false;
		}

		uint16_t len(deserialize_bytestream<iter_type, uint16_t,
			     sizeof(uint16_t)>::deserialize(i));

		if (len != strlen(cp))
		{
			err_flag=true;
			return false;
		}

		while (*cp)
		{
			if (*cp++ != (char)i.get())
			{
				err_flag=true;
				return false;
			}
		}

		return true;
	}

public:
	//! The deserialized object type

	typedef object_type result_type;

	deserialize_value_nonconst() noexcept : err_flag(false),
		err_value(serialization::tobject)
	{
	}

	~deserialize_value_nonconst()=default;

	//! Verify the type signature

	template<typename iter_type>
	bool verify_type(//! The deserialization iterator
			 iter_type &i)
	{
		return verify_typename(serialization::object_name<object_type>(), i);
	}

	//! %Deserialize the object

	template<typename iter_type>
	void deserialize_type(//! Place %deserialized value here
			      result_type &value,

			      //! The deserialization iterator
			      iter_type &i)
	{
		serialization::serialize_object_bytes<result_type>::serialize(value, i);
	}

	//! Diagnostic function

	bool output_type(std::ostream &o, bool err_type) noexcept
	{
		if (err_type && err_flag)
		{
			o << tname(err_value);
			return true;
		}

		o << "OBJECT<" << serialization::object_name<object_type>()
		  << ">";

		return false;
	}
};

//! Deserialize a std::pair

template<typename first, typename second>
class deserialize_value_nonconst< std::pair<first, second> > {

	//! Succesful %deserialization of the signature
	bool err_flag;

	//! %Deserialization failed, deserialized a different type
	datatype err_value;

	//! Deserializer for the pair's first value
	deserialize_value<first> first_value;

	//! Deserializer for the pair's second value
	deserialize_value<second> second_value;

public:
	//! The deserialized object type
	typedef std::pair<std::remove_const_t<first>,
			  std::remove_const_t<second>> result_type;

	deserialize_value_nonconst() noexcept : err_flag(false),
		err_value(serialization::tpair)
	{
	}

	~deserialize_value_nonconst()=default;

	//! Verify the type signature

	template<typename iter_type>
	bool verify_type(//! The deserialization iterator
			 iter_type &i)
	{
		if ( (err_value=(datatype)i.get()) != serialization::tpair)
		{
			err_flag=true;
			return false;
		}

		if (!first_value.verify_type(i))
			return false;

		if (!second_value.verify_type(i))
			return false;

		return true;
	}

	//! %Deserialize the object

	template<typename iter_type>
	void deserialize_type(//! Place %deserialized value here
			      result_type &value,

			      //! The deserialization iterator
			      iter_type &i)
	{
		first_value.deserialize_type(value.first, i);
		second_value.deserialize_type(value.second, i);
	}

	//! Diagnostic function

	bool output_type(std::ostream &o, bool err_type) noexcept
	{
		if (err_type && err_flag)
		{
			o << tname(err_value);
			return true;
		}

		o << "PAIR<";

		if (first_value.output_type(o, err_type) && err_type)
			return true;
		o << ",";

		if (second_value.output_type(o, err_type) && err_type)
			return true;
		o << ">";

		return false;
	}
};

//! Deserialize a tuple

template<typename ...Types>
class deserialize_value_nonconst< std::tuple<Types...> > {

	//! Succesful %deserialization of the signature
	bool err_flag;

	//! %Deserialization failed, deserialized a different type
	datatype err_value;

	uint32_t c=0;

	//! Deserializer for the pair's first value
	std::tuple<deserialize_value<Types>...> values;

public:
	//! The deserialized object type
	typedef std::tuple<std::remove_const_t<Types>...> result_type;

	deserialize_value_nonconst() noexcept : err_flag(false),
		err_value(serialization::ttuple)
	{
	}

	~deserialize_value_nonconst()=default;

	//! Verify the type signature

	template<typename iter_type>
	bool verify_type(//! The deserialization iterator
			 iter_type &i)
	{
		if ( (err_value=(datatype)i.get()) != serialization::ttuple)
		{
			err_flag=true;
			return false;
		}

		c=deserialize_bytestream<iter_type, uint32_t,
					 sizeof(uint32_t)>::deserialize(i);

		if (c != sizeof...(Types))
		{
			err_flag=true;
			return false;
		}

		if (!std::apply([&]
				(deserialize_value<Types> & ...type)
				{
					return (type.verify_type(i)
						&& ...);
				}, values))
		{
			err_flag=true;
			return false;
		}

		return true;
	}

	//! %Deserialize the object

	template<typename iter_type>
	void deserialize_type(//! Place %deserialized value here
			      result_type &value,

			      //! The deserialization iterator
			      iter_type &i)
	{
		return do_deserialize_type(value, i,
					   std::make_index_sequence
					   <sizeof...(Types)>{});
	}

	template<typename iter_type, size_t ...i>
	void do_deserialize_type(result_type &value,
				 iter_type &itr,
				 std::index_sequence<i...>)
	{
		(std::get<i>(values).deserialize_type(std::get<i>(value), itr),
		 ...);
	}

	//! Diagnostic function

	bool output_type(std::ostream &o, bool err_type) noexcept
	{
		return do_output_type(o, err_type,
				      std::make_index_sequence
				      <sizeof...(Types)>{});
	}

	template<size_t ...i>
	bool do_output_type(std::ostream &o, bool err_type,
			    std::index_sequence<i...>)
	{
		if (err_type && err_flag)
		{
			o << tname(err_value);
			return true;
		}

		o << "TUPLE<";

		if constexpr (sizeof...(Types))
		{
			const char *sep="";

			if (( ((o << sep), (sep=","),
			       (std::get<i>(values).output_type(o, err_type)
				&& err_type)) || ...))
				return true;
		}
		o << ">";

		return false;
	}
};

//! Deserialize a reference_wrapper

template<typename T>
class deserialize_value_nonconst< std::reference_wrapper<T> >
	: public deserialize_value<T> {
};

//! %Deserialize a native integer type

template<serializable_integer value_type>
class deserialize_value_nonconst<value_type> {

	typedef typename serialization::serialize_integer_type<value_type>
	::int_t native_value_type;

	//! Succesful %deserialization of the signature
	bool err_flag;

	//! %Deserialization failed, deserialized a different type
	datatype err_value;

public:
	//! The type code for this native integer type
	static constexpr datatype
	typecode=serialization::basic_type<native_value_type>::type;

	//! The result type of the deserialization

	typedef value_type result_type;

	deserialize_value_nonconst() noexcept
	: err_flag(false), err_value(typecode)
	{
	}

	~deserialize_value_nonconst()=default;

	//! Verify the type signature

	template<typename iter_type>
	bool verify_type(//! The deserialization iterator
			 iter_type &i)
	{
		if ( (err_value=(datatype)i.get()) == typecode )
			return true;

		err_flag=true;
		return false;
	}

	//! %Deserialize the object

	template<typename iter_type>
	void deserialize_type(//! Place %deserialized value here
			      value_type &v,

			      //! The deserialization iterator
			      iter_type &i)
	{
		v=(value_type)
			deserialize_bytestream<iter_type, native_value_type,
					       sizeof(v)>
			::deserialize(i);
	}

	//! Diagnostic function

	bool output_type(std::ostream &o, bool err_type) noexcept
	{
		if (err_type && err_flag)
		{
			o << tname(err_value);
			return true;
		}

		o << tname(typecode);

		return false;
	}
};

//! Deserialize something that's a container

template<serializable_container container_type>
class deserialize_value_nonconst<container_type> {

	//! What's in the container
	typedef typename container_type::value_type value_type;

	//! Measure of the container's size
	typedef typename container_type::size_type size_type;

	//! A %deserializer for the iterator's value
	deserialize_value<value_type> container_value;

	//! Succesful %deserialization of the signature
	bool err_flag;

	//! %Deserialization failed, deserialized a different type
	datatype err_value;

	//! Maximum allowed sequence size

	uint32_t maxseqsizevalue;

public:

	//! The deserialized object type

	typedef container_type result_type;

	deserialize_value_nonconst() noexcept : err_flag(false),
		err_value(serialization::tsequence),
		maxseqsizevalue(std::numeric_limits<uint32_t>::max())
	{
	}

	~deserialize_value_nonconst()=default;

	//! Set maximum container size

	void max(uint32_t maxValue) noexcept
	{
		maxseqsizevalue=maxValue;
	}

	//! Verify the type signature

	template<typename iter_type>
	bool verify_type(//! The deserialization iterator
			 iter_type &i)
	{
		if ( (err_value=(datatype)i.get()) != serialization::tsequence )
		{
			err_flag=true;
			return false;
		}

		return container_value.verify_type(i);
	}

	//! %Deserialize the object

	template<typename iter_type>
	void deserialize_type(//! Place %deserialized value here
			      result_type &value,

			      //! The deserialization iterator
			      iter_type &i)
	{
		value.clear();

		uint32_t seqsize(deserialize_bytestream<iter_type, uint32_t,
				 sizeof(uint32_t)>
				 ::deserialize(i));

		size_type seqsizet(seqsize);

		if (seqsizet != seqsize || seqsize > maxseqsizevalue)
			serialization::container_toolong();

		container_traits<container_type>::reserve(value, seqsizet);

		while (seqsizet)
		{
			typename deserialize_value<value_type>::result_type
				result_type;

			container_value.deserialize_type(result_type, i);

			if (!container_traits<container_type>
			    ::insert_chk(value, result_type))
			{
				std::ostringstream o;

				output_type(o, false);
				deserialization_failure(o.str());
			}
			--seqsizet;
		}
	}

	//! Diagnostic function

	bool output_type(std::ostream &o, bool err_type) noexcept
	{
		if (err_type && err_flag)
		{
			o << tname(err_value);
			return true;
		}

		o << "SEQUENCE[";

		if (container_value.output_type(o, err_type) && err_type)
			return true;

		o << "]";
		return false;
	}
};

//! Deserialize a native float value

template<std::floating_point value_type>
class deserialize_value_nonconst<value_type> {

	//! The actual deserialized type is a \c std::string

	deserialize_value<std::string> impl;

public:
	//! The deserialized object type

	typedef value_type result_type;

	//! Constructor
	deserialize_value_nonconst() noexcept=default;

	//! Destructor
	~deserialize_value_nonconst()=default;

	//! Verify the type signature

	template<typename iter_type>
	bool verify_type(//! The deserialization iterator
			 iter_type &i)
	{
		return impl.verify_type(i);
	}

	//! %Deserialize the object

	template<typename iter_type>
	void deserialize_type(//! Place %deserialized value here
			      result_type &value,

			      //! The deserialization iterator
			      iter_type &i)
	{
		std::string strvalue;

		impl.deserialize_type(strvalue, i);

		auto p=strvalue.c_str();

		auto ret=std::from_chars(p, p+strvalue.size(), value);

		if (ret.ec != std::errc{} ||
		    ret.ptr != p+strvalue.size())
			floatmismatch(strvalue);
	}

	//! Diagnostic function

	bool output_type(std::ostream &o, bool err_type) noexcept
	{
		return impl.output_type(o, err_type);
	}
};

//! Deserialize an object pointer

template<typename obj_type, typename base_type>
class deserialize_value_nonconst<ptr<obj_type, base_type> > {

	//! Succesful %deserialization of the signature
	bool err_flag;

	//! %Deserialization failed, deserialized a different type
	datatype err_value;

	//! Deserializer for the referenced object

	deserialize_value<obj_type> reference_value;

public:
	//! The deserialized object type

	typedef std::remove_const_t<ptr<obj_type, base_type> > result_type;

	deserialize_value_nonconst() noexcept : err_flag(false),
		err_value(serialization::treference)
	{
	}

	~deserialize_value_nonconst()=default;

	//! Verify the type signature

	template <typename iter_type>
	bool verify_type(//! The deserialization iterator
			 iter_type &i)
	{
		if ( (err_value=(datatype)i.get()) == serialization::treference)
			return true;

		err_flag=true;
		return false;
	}

	//! %Deserialize the object

	template<typename iter_type>
	void deserialize_type(//! Place %deserialized value here
			      result_type &value,

			      //! The deserialization iterator
			      iter_type &i)

	{
		if ( i.get() == 0)
		{
			value=result_type();
			return;
		}

		iter_type::traits_type_t::classcreate(value);
		i(*value);
	}

	//! Diagnostic function

	bool output_type(std::ostream &o, bool err_type) noexcept
	{
		if (err_type && err_flag)
		{
			o << tname(err_value);
			return true;
		}

		o << "REFERENCE<";

		bool flag=reference_value.output_type(o, err_type);
		o << ">";
		return flag;
	}

};

//! Deserialize an object reference, similar to a pointer, except for a call to classcreate().

template<typename obj_type, typename base_type>
class deserialize_value_nonconst<ref<obj_type, base_type> > {

	//! Succesful %deserialization of the signature
	bool err_flag;

	//! %Deserialization failed, deserialized a different type
	datatype err_value;

	//! Deserializer for the referenced object

	deserialize_value<obj_type> reference_value;

public:
	//! The deserialized object type

	typedef typename std::remove_const_t<ref<obj_type, base_type>
					     > result_type;

	deserialize_value_nonconst() noexcept : err_flag(false),
		err_value(serialization::treference)
	{
	}

	~deserialize_value_nonconst()=default;

	//! Verify the type signature

	template <typename iter_type>
	bool verify_type(//! The deserialization iterator
			 iter_type &i)
	{
		if ( (err_value=(datatype)i.get()) == serialization::treference)
			return true;

		err_flag=true;
		return false;
	}

	//! %Deserialize the object

	template<typename iter_type>
	void deserialize_type(//! Place %deserialized value here
			      result_type &value,

			      //! The deserialization iterator
			      iter_type &i)

	{
		if ( i.get() == 0)
		{
			std::ostringstream o;

			output_type(o, false);
			deserialization_nullobj(o.str());
		}

		i(*value);
	}

	//! Diagnostic function

	bool output_type(std::ostream &o, bool err_type) noexcept
	{
		if (err_type && err_flag)
		{
			o << tname(err_value);
			return true;
		}

		o << "REFERENCE<";

		bool flag=reference_value.output_type(o, err_type);
		o << ">";
		return flag;
	}

};

//! Deserialize the name of an object

template<typename iter_type>
void deserialize_object_name(//! Input iterator
			     iter_type &i,

			     //! Object name placed here
			     std::string &name)
{
	uint16_t len(deserialize_bytestream<iter_type, uint16_t,
		     sizeof(uint16_t)>::deserialize(i));

	name.reserve(len);
	for (uint16_t n=0; n<len; ++n)
		name.push_back(i.get());
}

template<typename iter_type, typename traits_type,
	 typename object_iter_type>
class iterator_deserialize_any;

//! %Deserialize objects from an input %iterator

//! The first template parameter is any STL input iterator which iterates over
//! chars.
//! The constructor takes a reference to an instance of
//! input %iterator and the ending input %iterator.
//! Invoking the () operator %deserializes an object.
//! An %exception gets thrown if %deserialiation continues after
//! the input %iterator reaches the ending %iterator value
//!
//! The optional second template parameter defines a traits class. The traits
//! class's \ref serialization::default_traits::classcreate() "classcreate()"
//! template gets invoked to construct a reference to an object before the
//! referenced object gets deserialized. A suitable default traits
//! implementation is provided.

template<typename iter_type,
	 typename traits_type=serialization::default_traits>
class iterator {

	//! The deserialization input iterator
	iter_type &i;

	//! The ending input iterator

	iter_type &end;

public:
	//! The traits type

	typedef traits_type traits_type_t;

	//! This is a %deserializing iterator
	static constexpr bool serializing=false;

	//! Constructor

	iterator(//! The input iterator
		 iter_type &iterArg,

		 //! The ending input iterator
		 iter_type &endArg
		 ) noexcept : i(iterArg), end(endArg)
	{
	}

	//! Destructor

	~iterator()
	{
	}

	//! Retrieve the next byte to %deserialize

	inline uint8_t get()
	{
		if (i == end)
			serialization_truncated();

		return *i++;
	}

	//! %Deserialize an object

	//! The object is %deserialized by deserializing its type
	//! followed by its value value.
	//!
	//! \return a reference to \c *this, for a shorthand:
	//!
	//! \c iter(obj1)(obj2)(obj3)...

	template<typename value_type>
	iterator<iter_type> &operator()(value_type &value)

	{
		deserialize_value<value_type> parser;

		return deserialize(parser, value);
	}

	//! %Deserialize a sequence, enforce maximum sequence size

	template<typename value_type>
	iterator<iter_type> &operator()(value_type &value,
					uint32_t maxseqsize)

	{
		deserialize_value<value_type> parser;

		parser.max(maxseqsize);
		return deserialize(parser, value);
	}

private:

	//! Deserialization implementation

	template<typename value_type> iterator<iter_type>
	&deserialize(deserialize_value<value_type> &parser,
		     value_type &value)

	{
		if (!parser.verify_type(*this))
		{
			std::ostringstream expected_type, actual_type;

			parser.output_type(expected_type, false);
			parser.output_type(actual_type, true);

			mismatch(expected_type.str(),
				 actual_type.str());
		}

		parser.deserialize_type(value, *this);
		return *this;
	}

public:

	//! Alias.

	//! See https://bugzilla.redhat.com/show_bug.cgi?id=1632063

	template<typename object_iter_type>
	using any=iterator_deserialize_any<iter_type, traits_type,
					   object_iter_type>;
};

//! Deserialize one of several objects.

//! Instantiate an object that uses a
//! \ref iterator "deserialization iterator" to
//! deserialize one of several possible objects. Use this when
//! the next object to be deserialized may be one of several objects.
//! Construct this object, then invoke operator() to deserialize
//! whatever object comes next.
//!
//! The template parameter, \c object_iter_type, specifies
//! a class that implements:
//!
//! - An overloaded deserialized() method for each class
//! that may be deserialized.
//!
//! - A classlist() template function.
//!
//! The classlist() function should take the following form:
//!
//! \code
//! template<typename ser_type> void classlist(ser_type &arg)
//! {
//!     arg.template serialize<class1>();
//!     arg.template serialize<class2>();
//!     // ...
//! }
//! \endcode
//!
//! \c class1 and \c class2 are classes that may be deserialized.
//! The classlist() function enumerates all classes that
//! can be deserialized in this manner. The class list should
//! be the same list of classes used by the overloaded
//! deserialized() method.
//!
//! Each class enumerated by classlist should have
//! a constructor that takes a reference to an \c object_iter_type
//! as a parameter. The instance of the deserialized class
//! gets constructed with the \c object_iter_type
//! instance passed to it
//! as a parameter. The class should probably have a default
//! constructor as well, since the same class is presumably
//! used on the serialization side, without the deserialization
//! class.
//!
//! classlist's call to template serialize may also specify
//! an optional second template parameter:
//!
//! \code
//! template<typename ser_type> void classlist(ser_type &arg)
//! {
//!     arg.template serialize<class1, class1inst>();
//!     // ...
//! }
//! \endcode
//!
//! When the object to be deserialized is \c class1,
//! \c class1inst would get constructed on the stack instead,
//! and it's deserialize() method invoked, followed by
//! \c object_iter_type's deserialized(). Use this to deserialize an
//! object into a slightly different class that has the same
//! serialize() implementation.

template<typename iter_type, typename traits_type, typename object_iter_type>
class iterator_deserialize_any {

	//! Alias for the bug 1632063 workaround.

	typedef iterator<iter_type, traits_type> iterator_t;

	//! Base class for the deserialization implementer

	class deserBaseObj : virtual public obj {

	public:
		//! Constructor
		deserBaseObj()=default;

		//! Destructor
		~deserBaseObj()=default;

		//! Deserialize

		virtual void deserialize(object_iter_type &objRef,
					 iterator_t &i)=0;
	};

	//! Deserialization implemented for a given class

	template<typename obj_type> class deserObj :
		public deserBaseObj {

	public:
		//! Constructor
		deserObj()=default;

		//! Destructor
		~deserObj()=default;

		//! Implement deserialization for this class

		void deserialize(//! Handler for the deserialized object
				 object_iter_type &objRef,

				 //! Iterator over the serialized bytestream
				 iterator_t &i) override
		{
			obj_type objInstance(objRef);

			objInstance.serialize(&objInstance, i);
			objRef.deserialized(objInstance);
		}
	};

	//! Lookup table of class names to implementers

	std::unordered_map<std::string, ref<deserBaseObj> > objmap;

	//! The deserialization iterator
	iterator_t &iter;

	//! The deserialization handler
	object_iter_type &objRef;
public:
	//! Constructor

	iterator_deserialize_any(//! The deserialization iterator
				 iterator_t &iterArg,

				 //! The deserialization handler
				 object_iter_type &objRefArg)
		: iter(iterArg),
		  objRef(objRefArg)
	{
		objRef.classlist(*this);
	}

	//! Destructor
	~iterator_deserialize_any()=default;

	//! Callback, initialize the object map

	template<typename obj_type>
	void serialize()
	{
		objmap.emplace(std::string(serialization::
					   object_name<obj_type>()),
			       ref<deserObj<obj_type> >
			       ::create());
	}

	//! Callback, initialize the object map

	template<typename obj_type, typename obj_instance_type>
	void serialize()
	{
		objmap.emplace(std::string(serialization::
					   object_name<obj_type>()),
			       ref<deserObj<obj_instance_type> >
			       ::create());
	}

	//! Deserialize the next generic object

	void operator()() const
	{
		datatype t((datatype)iter.get());

		if (t != serialization::tobject)
			mismatch(tname(serialization::tobject),
				 tname(t));

		std::string name;

		deserialize_object_name(iter, name);

		auto p=objmap.find(name);

		if (p == objmap.end())
			serialization::unknown_class(name);

		p->second->deserialize(objRef, iter);
	}
};

//! Convenience function for %deserializing a single object

//! This function takes a reference to an object, and a beginning and an
//! ending input %iterator, constructs an
//! iterator object, then %deserializes the referenced
//! object.

template<typename iter_type, typename value_type>
static void object(//! Object to %deserialize
		   value_type &value,

		   //! Beginning input %iterator
		   iter_type &begin,

		   //! Ending input %iterator
		   iter_type &end)
{
	iterator<iter_type> deser(begin, end);

	deser(value);
}

//! Convenience function for %deserializing a single object

//! This function takes a reference to an object, and a reference to
//! a container from which a beginning and an ending %iterator is
//! obtained, from which the object gets %deserialized.

template<typename value_type, typename container_type>
static void object(//! Object to %deserialize
		   value_type &value,

		   //! Container
		   const container_type &container)
{
	typename container_type::const_iterator b(container.begin()),
		e(container.end());

	return object(value, b, e);
}

#if 0
{
#endif
}
#endif
