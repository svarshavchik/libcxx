/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fditer_H
#define x_fditer_H

#include <x/obj.H>
#include <x/ptr.H>
#include <x/fdbaseobj.H>
#include <x/property_value.H>
#include <vector>
#include <iterator>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class fdinputiter;
class fdoutputiter;

//! A bare-bones buffer for reading or writing to a file descriptor

//! This is a reference-counted object. Use
//! \ref fdBase::buffer "fd::buffer" as a reference to this object.
//!

class fdbufferObj : virtual public obj {

	//! The underlying file descriptor.

	//! When used for input iterators, a null reference defines the
	//! ending iterator value.

	fdbaseptr fd;

	//! Allocated buffer.
	std::vector<char> buffer;

	size_t buf_size,	//!< Buffer size value
		buf_ptr,	//!< Buffer pointer value
		requested_buf_size; //!< Requested buffer size

	//! Resize the buffer, non-destructively

	void resize(size_t requested_buf_sizeArg)
	{
		requested_buf_size=requested_buf_sizeArg;
	}
public:

	//! Constructor
	fdbufferObj(const fdbaseptr &fdArg,
		    size_t buffer_size) LIBCXX_INTERNAL;
	~fdbufferObj() LIBCXX_INTERNAL;

	friend class fdinputiter;
	friend class fdoutputiter;
};

//! A bare-bones input iterator that reads from an \ref fdbufferObj "fd::buffer".

//! This is a reference-counted object. Use
//! \ref fdBase::inputiter "fd::inputiter" as a reference to this
//! object.
//!

class fdinputiter {

	//! Buffer
	ref<fdbufferObj> buf;

public:

	//! Iterator trait
	typedef std::input_iterator_tag iterator_category;

	//! Iterator trait
	typedef char value_type;

	//! Iterator trait
	typedef std::ptrdiff_t difference_type;

	//! Iterator trait
	typedef value_type *pointer;

	//! Iterator trait
	typedef value_type &reference;


	//! Ending iterator constructor
	fdinputiter();

	//! Non-null iterator constructor
	//! A non-null fd reference constructs a beginning iterator.
	//! A null fd reference constructs an ending iterator.

	fdinputiter(const fdbase &fdArg,
		    size_t buffer_size=fdbaseObj::get_buffer_size());

	//! Destructor
	~fdinputiter();

	//! Update the file descriptor transport

	//! Any buffered input is discarded. This becomes an ending
	//! iterator.

	void update();

	//! Update the file descriptor transport

	void update(const fdbase &fdArg);

	//! Return the current file descriptor transport

	//! \return the file descriptor transport object that's connected
	//! to this iterator.

	fdbaseptr buffer();

	//! Resize the buffer, non-destructively

	void resize(size_t requested_buf_sizeArg)
	{
		buf->resize(requested_buf_sizeArg);
	}

	//! Iterator operator
	bool operator==(const fdinputiter &o) const;

	//! Iterator operator
	bool operator!=(const fdinputiter &o) const;

	//! Iterator operator
	char operator*() const;

	//! Iterator operator
	fdinputiter &operator++();

	//! Iterator operator
	const char *operator++(int);

protected:

	//! Invoke pubread() to read more data

	//! A subclass may override this to execute something value-added
	//! when the iterator wants to read more stuff from the file
	//! descriptor.

	virtual size_t pubread(const fdbase &fdref, char *ptr, size_t cnt)
		const;
};


//! A bare-bones output iterator that writes to a \ref fdbufferObj "fd::buffer".

//! This is a reference-counted object. Use
//! \ref fdBase::outputiter "fd::outputiter" as a reference to this
//! object.

class fdoutputiter {

	//! Buffer
	ref<fdbufferObj> buf;

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

	//! Default constructor
	fdoutputiter();

	//! Constructor
	fdoutputiter(const fdbase &fdArg,
		     size_t buffer_size=fdbaseObj::get_buffer_size());

	//! Destructor
	~fdoutputiter();

	//! Update the file descriptor transport

	//! Any buffered input is discarded.

	void update();

	//! Update the file descriptor transport

	void update(const fdbase &fdArg);

	//! Resize the buffer, non-destructively

	void resize(size_t requested_buf_sizeArg)
	{
		buf->resize(requested_buf_sizeArg);
	}

	//! Iterator operator
	fdoutputiter &operator++()
	{
		return *this;
	}

	//! Iterator operator
	fdoutputiter &operator++(int)
	{
		return *this;
	}

	//! Iterator operator
	fdoutputiter &operator*()
	{
		return *this;
	}

	//! Iterator operator
	fdoutputiter &operator=(char c);

	//! Iterator operator

	virtual void flush();
};


#if 0
{
#endif
}
#endif
