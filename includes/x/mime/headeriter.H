/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_mime_headeriter_H
#define x_mime_headeriter_H

#include <x/namespace.h>
#include <x/mime/newlineiter.H>
#include <x/mime/bodystartiter.H>

namespace LIBCXX_NAMESPACE::mime {
#if 0
}
#endif


//! Base class of an output iterator that extracts individual headers

class header_iterbase {

	//! Current state, current element handler.

	typedef void (header_iterbase::*state_func)(int);

	//! The underlying iterator.

	state_func state;

	//! In header contents from the previous header. Helper state metadata.
	bool in_header;

protected:

	//! Constructor

	header_iterbase();

	//! Process the next element

	void operator=(int c)
	{
		(this->*state)(c);
	}

	//! Emit the next element, defined in the subclass

	virtual void emit(int)=0;

public:
	//! This is considered to be whitespace:

	static inline bool space(int c)
	{
		return ((c) == ' ' || (c) == '\t' || (c) == '\r');
	}

private:

	//! Start of headers
	void start_header(int c) LIBCXX_HIDDEN;

	//! In message body
	void in_body(int c) LIBCXX_HIDDEN;

	//! In end of line sequence
	void in_endofline(int c) LIBCXX_HIDDEN;

	//! Folding a header
	void in_headerfold(int c) LIBCXX_HIDDEN;

	//! The header name
	void in_headername(int c) LIBCXX_HIDDEN;

	//! Starting header contents
	void in_starting_contents(int c) LIBCXX_HIDDEN;

	//! Starting header contents
	void in_starting_contents_newline(int c) LIBCXX_HIDDEN;

	//! The header contents
	void in_contents(int c) LIBCXX_HIDDEN;

};

//! Implement a reference-counted iterator that marks headers

//! \see header_iter
//!

template<typename output_iter_type>
class header_iterObj : public header_iterbase, virtual public obj {

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

	//! Underlying output iterator over ints.

	output_iter_type iter;

public:

	//! Constructor
	header_iterObj(const output_iter_type &iterArg)
		: iter(iterArg)
	{
	}

	//! Move constructor
	header_iterObj(output_iter_type && iterArg)
		: iter(std::move(iterArg))
	{
	}

	//! Iterator operator

	header_iterObj<output_iter_type> &operator*() { return *this; }

	//! Iterator operator

	header_iterObj<output_iter_type> &operator++() { return *this; }

	//! Iterator operator

	header_iterObj<output_iter_type> *before_postoper() { return this; }

#ifndef DOXYGEN
		// TODO - doxygen fix
	//! Iterator operator
	using header_iterbase::operator=;

#endif

private:
	//! Write to the output iterator

	void emit(int c) override
	{
		*iter++=c;
	}
};

//! Mark headers in an output sequence.

//! This iterator iterates over the output of
//! \ref bodystart_iter "bodystart_iter", and inserts
//! the following additional elements into the int output:
//!
//! Each header has the header name marked with \c header_name_start and
//! \c header_name_end. The contents of the header is marked with
//! \c header_contents_start and \c header_contents_end.
//!
//! Once \c body_start appears in the output sequence, no further header
//! elements appear past that point.
//!
//! Between \c header_contents_start and \c header_contents_end:
//!
//! - the sequence from \c newline_start to \c newline_end should be ignored.
//!
//! - the sequence from \c header_fold_start and \c header_fold_end
//! should be logically replaced with a space.
//!
//! The template parameter is an output
//! iterator that takes ints. The constructor takes the instance of the
//! template parameter.
//!
//! The ints that this iterator receives get passed through to the underlying
//! int operator, with the int stream modified as described above.
//!
//! \note
//! The logical contents of header_name_start/end and header_contents/start_end
//! will include any leading and trailing spaces.

template<typename output_iter>
using header_iter=refiterator<header_iterObj<output_iter> >;

//! A nullable pointer reference to a \ref header_iterObj "header_iterObj".

//! \see header_iter

template<typename output_iter>
using header_iterptr=ptriterator<header_iterObj<output_iter> >;

#if 0
{
#endif
}
#endif
