/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basicstreamobj_H
#define x_basicstreamobj_H

#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/basicstreambufobj.H>
#include <x/interface.H>
#include <streambuf>
#include <iostream>
#include <sstream>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<class streamType> class basic_streamObj;


//! A reference-counted stream

//! This template defines a reference-counted stream.
//! The template parameter is a stream object defined in the standard C++
//! library: a \c std::basic_istream, \c std::basic_ostream,
//! or a \c std::basic_iostream type.
//!
//! The template defines a subclass of the stream object and obj, implementing
//! a reference-counted stream.
//!
//! The constructor takes a stream buffer reference, a reference
//! to a basic_streambufObj, saves it internally, and uses it to instantiate
//! the stream buffer superclass.
//!
//! This template redefines rdbuf() to get or set a stream buffer reference,
//! accordingly.
//!
//! \see istream, ostream, iostream, wistream, wostream, wiostream

template<class streamType>
class basic_streamObj : public streamType, virtual public obj {

public:
	//! Type of our stream buffer

	typedef ref<basic_streambufObj> streambufref_t;
private:

	//! Our stream buffer

	streambufref_t streambufref;

public:
	//! Define the reference type for this reference-counted object.

	typedef ref<basic_streamObj< streamType > > ref_t;

	//! Bind this new stream object to a stream buffer

	//! The constructor initializes the C++ stream superclass using a
	//! pointer to the stream buffer passed as an argument.

	basic_streamObj(//! A reference to a stream buffer
			const streambufref_t &streambufrefArg)
		: streamType(interface<std::streambuf>
			     ::getRef(streambufrefArg)),
		  streambufref(streambufrefArg)
	{
		this->exceptions(std::ios::badbit);
	}

	//! The default destructor

	~basic_streamObj()
	{
	}

	//! Return this stream's buffer.

	//! \return the reference to the stream buffer used by this
	//! stream object.
	streambufref_t rdbuf() const
	{
		return streambufref;
	}

	//! Redefine this stream's buffer

	//! This stream gets rebound to a new buffer.

	void rdbuf(//! New buffer object
		   const streambufref_t &streambufrefArg)
	{
		streamType::rdbuf(interface<std::streambuf>
				  ::getRef(streambufrefArg));
		streambufref=streambufrefArg;
	}
};
//! A reference-counted \c std::istream

//! \code
//! streambuf buf(streambuf::create());
//!
//! istream is(istream::create(buf));
//! \endcode
//!
//! \see ptr

typedef ref<basic_streamObj< std::istream > > istream;

//! A reference-counted \c std::ostream

//! \code
//! streambuf buf(streambuf::create());
//!
//! ostream os(ostream::create(buf));
//! \endcode
//!
//! \see ptr

typedef ref<basic_streamObj< std::ostream > > ostream;

//! A reference-counted \c std::iostream

//! \code
//! streambuf buf(streambuf::create());
//!
//! iostream ios(iostream::create(buf));
//! \endcode
//!
//! \see ptr

typedef ref<basic_streamObj< std::iostream > > iostream;

//! A reference-counted \c std::istream

//! \see istream

typedef ptr<basic_streamObj< std::istream > > istreamptr;

//! A reference-counted \c std::ostream

//! \see ostream

typedef ptr<basic_streamObj< std::ostream > > ostreamptr;

//! A pointer to a reference-counted \c std::iostream

//! \see iostream

typedef ptr<basic_streamObj< std::iostream > > iostreamptr;

#if 0
{
#endif
}
#endif
