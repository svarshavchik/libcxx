/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_refiterator_H
#define x_refiterator_H

#include <vector>
#include <x/ref.H>
#include <x/obj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Internal helper class that determines if a class has a -> operator

//! \internal

template<typename T> struct has_arrow_operator
{
	//! Yes return value

	typedef char yes[1];

	//! No return value

	typedef char no[2];

	//! The class has an operator->

	template<typename S>
	static yes &check(decltype( ((S *)nullptr)->operator->()) *);

	//! The class does not have an operator->
	template<typename S>
	static no &check(...);

	//! Determine whether the class has an operator->

	static const bool value = sizeof(check<T>(0)) == sizeof(yes);
};

//! Invoke a class's operator-> if it has one.

template<bool has_arrow_operator> class invoke_arrow_operator;

//! Specialization: the class does not have an operator->

template<>
struct invoke_arrow_operator<false> {

	//! Undefined declaration

	//! This allows refobjiterator::operator-> to compile without an
	//! error, but an actual attempt to use it will fail to link.

	template<typename T> static int *invoke(T &);
};

//! Specialization: the class has an operator->

template<>
struct invoke_arrow_operator<true> {

	//! Invoke the class's operator->().

	template<typename T>
	static inline auto invoke(T &obj) -> decltype(obj.operator->())
	{
		return obj.operator->();
	}
};

//! Adapter for \ref ref "INSERT_LIBX_NAMESPACE::ref"s and \ref ptr "INSERT_LIBX_NAMESPACE::ptr"s that turns them into iterators

//! \c refobjiterator<T> creates a subclass of \c T, which must be a
//! \c INSERT_LIBX_NAMESPACE::ref 
//! \c INSERT_LIBX_NAMESPACE::ptr which implements iterator operators.
//! In general, the iterator operators get forwarded to the referenced class,
//! however there are some differences. See the documentation for the
//! individual iterators.
//!
//! \note
//! The \c * operator is redefined as an iterator operator. Use get() for
//! convenience to get a native reference to the underlying reference or a
//! pointer, to get the reference/pointer \c * operator. The -> operator
//! is also redefined in the same way.

template<typename ref_type>
class refobjiterator : public ref_type,
		       public ref_type::obj_type::iterator_traits {

	//! The object type

	typedef typename ref_type::obj_type obj_t;

public:

	//! Constructor. Forward arguments to the underlying ref/ptr
	template<typename ...Args>
	refobjiterator(Args && ...args)
		: ref_type(std::forward<Args>(args)...) {}

	//! Destructor
	~refobjiterator() noexcept {}

	//! Make sure that create() returns the correct type.

	template<typename ...Args>
	static refobjiterator<ref_type> create(Args && ...args)
	{
		return refobjiterator<ref_type>
			(ref_type::create(std::forward<Args>(args)...));
	}

	//! Iterator operator *

	//! This is forwarded to the underlying object's \c * operator

	decltype(static_cast<obj_t *>(nullptr)->operator*()) operator*() const
	{
		return *ref_type::operator*();
	}

	//! Access the underlying reference or a pointer.

	const ref_type &get() const
	{
		return *this;
	}

	//! Iterator operator ->

	//! This is forwarded to the underlying object's \c -> operator,
	//! if one exists.

	auto operator->() const
		-> decltype(invoke_arrow_operator< has_arrow_operator<obj_t>
			    ::value >
			    ::invoke(*(obj_t *)nullptr))
	{
		return invoke_arrow_operator< has_arrow_operator<obj_t>
					      ::value >::invoke(*get());
	}

	//! The increment operator invokes referenced object's increment().

	refobjiterator<ref_type> &operator++()
	{
		get()->operator++();
		return *this;
	}

	//! The increment operator invokes referenced object's decrement().

	refobjiterator<ref_type> &operator--()
	{
		get()->operator--();
		return *this;
	}

	//! Postfix increment.

	//! This invokes the referenced object's before_postoper(), and its
	//! returned value, whatever it is, gets returned. Then, operator++()
	//! gets invoked, to perform the actual increment.

	auto operator++(int) -> decltype((*(obj_t *)nullptr).before_postoper())
	{
		auto val= get()->before_postoper();
		operator++();
		return val;
	}

	//! Postfix decrement.

	//! This invokes the referenced object's before_postoper(), and its
	//! returned value, whatever it is, gets returned. Then, operator--()
	//! gets invoked, to perform the actual increment.

	auto operator--(int) -> decltype((*(obj_t *)nullptr).before_postoper())
	{
		auto val= get()->before_postoper();
		operator--();
		return val;
	}

	//! Iterator operator

	//! Invokes the referenced object's == operator.

	bool operator==(const refobjiterator<ref_type> &ref) const
	{
		return *get() == *ref.get();
	}

	//! Iterator operator

	//! Invokes the referenced object's != operator.

	bool operator!=(const refobjiterator<ref_type> &ref) const
	{
		return *get() != *ref.get();
	}
};

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrrefBase>
#ifdef DOXYGEN
class refiterator {}; // TODO
#else
using refiterator=refobjiterator<ref<obj_type, baseType>>;
#endif

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrrefBase>
#ifdef DOXYGEN
class ptriterator {}; // TODO
#else
using ptriterator=refobjiterator<ptr<obj_type, baseType>>;
#endif

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrrefBase>
#ifdef DOXYGEN
class const_refiterator {}; // TODO
#else
using const_refiterator=refobjiterator<const_ref<obj_type, baseType>>;
#endif

//! A referenced-counted pointer that implements iterator requirements.

//! \see refobjiterator

template<typename obj_type, typename baseType=ptrrefBase>
#ifdef DOXYGEN
class const_ptriterator {}; // TODO
#else
using const_ptriterator=refobjiterator<const_ptr<obj_type, baseType>>;
#endif

//! Construct an referenced-counted object that implements an iterator.

//! This is a convenience function that constructs a
//! \ref refobjiterator "refobjiterator".

template<typename ref_type>
refobjiterator<ref_type> make_refiterator(const ref_type &ref)
{
	return refobjiterator<ref_type>(ref);
}

//! Construct an referenced-counted object that implements an iterator.

//! This is a convenience function that constructs a
//! \ref refobjiterator "refobjiterator".

template<typename ref_type>
refobjiterator<ref_type> make_refiterator(ref_type &&ref)
{
	return refobjiterator<ref_type>(std::move(ref));
}

//! A convenient base class for reference-based output iterator.

//! When implementing an output iterator, subclass this, and implement
//! just the operator=().

template<typename value_type>
class outputrefiteratorObj : virtual public obj {

public:
	//! My iterator traits

	typedef std::iterator<std::output_iterator_tag, void,
			      void, void, void> iterator_traits;

	//! Constructor
	outputrefiteratorObj() {}

	//! Destructor
	~outputrefiteratorObj() noexcept {}

	//! Iterator operator

	outputrefiteratorObj &operator*() { return *this; }

	//! Iterator operator

	outputrefiteratorObj &operator++() { return *this; }

	//! Iterator operator

	outputrefiteratorObj *before_postoper()
	{
		return this;
	}

	//! Iterator operator

	//! Must be implemented in the subclass. The default superclass
	//! implementation discards output.
	virtual void operator=(value_type c) {}
};

//! A referenced-counted output iterator.

//! \see outputrefiteratorObj

template<typename value_type>
#ifdef DOXYGEN
class outputrefiterator {}; // TODO
#else
using outputrefiterator=refiterator<outputrefiteratorObj<value_type>>;
#endif

//! A referenced-counted output iterator.

//! \see outputrefiteratorObj

template<typename value_type>
#ifdef DOXYGEN
class outputrefiteratorptr {}; // TODO
#else
using outputrefiteratorptr=ptriterator<outputrefiteratorObj<value_type>>;
#endif

//! A referenced-counted output iterator.

//! \see outputrefiteratorObj

template<typename value_type>
#ifdef DOXYGEN
class const_outputrefiterator {}; // TODO
#else
using const_outputrefiterator=const_refiterator<outputrefiteratorObj<value_type>>;
#endif

//! A referenced-counted output iterator.

//! \see outputrefiteratorObj

template<typename value_type>
#ifdef DOXYGEN
class const_outputrefiteratorptr {}; // TODO
#else
using const_outputrefiteratorptr=const_ptriterator<outputrefiteratorObj<value_type>>;
#endif

//! A convenient base class for reference-based input iterator.

//! When implementing an input iterator, subclass this, and implement
//! just the fill() method.
//!
//! fill() should put the next input chunk into buffer, or leave buffer empty
//! if there's no more input.
//!
//! \note
//! fill() is constant, buffer and eof are mutable. This is done in order to
//! implement the postfix ++ operator, as well as the comparison operator.

template<typename value_type>
class inputrefiteratorObj : virtual public obj {

protected:

	//! Next input chunk
	mutable std::vector<value_type> buffer;

private:
	//! eof flag.
	mutable bool eof;

	//! Current position in the input chunk.
	mutable size_t buffer_pos;

	//! Must be implemented by the subclass, fill buffer with more input.

	//! The default no-op implementation results in getting the ending
	//! iterator value by instantiating this superclass directly.

	virtual void fill() const
	{
	}

public:

	//! My iterator traits

	typedef std::iterator<std::input_iterator_tag,
			      value_type> iterator_traits;

	//! Constructor
	inputrefiteratorObj() : eof(false), buffer_pos(0) {}

	//! Destructor
	~inputrefiteratorObj() noexcept {}

	//! Return the current value of the iterator.

	inline const value_type &operator*() const
	{
		set();
		if (eof)
			null_ptr_deref();
		return buffer[buffer_pos];
	}

	//! Iterator operator

	inline void operator++()
	{
		set();
		++buffer_pos;
	}

	//! Iterator operator

	inline const value_type *before_postoper()
	{
		set();
		if (eof)
			null_ptr_deref();
		return &buffer[buffer_pos];
	}

	//! Iterator comparison operator

	//! Returns true if this and the other iterator are ending iterators,
	//! true in all other instances.

	inline bool operator==(const inputrefiteratorObj<value_type> &o) const
	{
		set();
		o.set();

		return eof && o.eof;
	}

	//! Iterator comparison operator.

	inline bool operator!=(const inputrefiteratorObj<value_type> &o) const
	{
		return !operator==(o);
	}

private:

	//! Fill the input buffer, if necessary.

	inline void set() const
	{
		if (!eof && buffer_pos >= buffer.size())
		{
			buffer_pos=0;
			buffer.clear();
			fill();

			if (buffer.empty())
				eof=true;
		}
	}
};

//! A referenced-counted input iterator.

//! \see inputrefiteratorObj

template<typename value_type>
#ifdef DOXYGEN
class inputrefiterator {}; // TODO
#else
using inputrefiterator=refiterator<inputrefiteratorObj<value_type>>;
#endif

//! A referenced-counted input iterator.

//! \see inputrefiteratorObj

template<typename value_type>
#ifdef DOXYGEN
class inputrefiteratorptr {}; // TODO
#else
using inputrefiteratorptr=ptriterator<inputrefiteratorObj<value_type>>;
#endif

//! A referenced-counted input iterator.

//! \see inputrefiteratorObj

template<typename value_type>
#ifdef DOXYGEN
class const_inputrefiterator {}; // TODO
#else
using const_inputrefiterator=const_refiterator<inputrefiteratorObj<value_type>>;
#endif

//! A referenced-counted input iterator.

//! \see inputrefiteratorObj

template<typename value_type>
#ifdef DOXYGEN
class const_inputrefiteratorptr {}; // TODO
#else
using const_inputrefiteratorptr=const_ptriterator<inputrefiteratorObj<value_type>>;
#endif

extern template class outputrefiteratorObj<char>;
extern template class inputrefiteratorObj<char>;

#if 0
{
#endif
}
#endif

