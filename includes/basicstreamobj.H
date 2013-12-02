/*
** Copyright 2012-2013 Double Precision, Inc.
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

	typedef ref<basic_streambufObj<typename streamType::char_type,
				       typename streamType::traits_type>
		    > streambufref_t;
private:

	//! Our stream buffer

	streambufref_t streambufref;

	//! Stream buffer type used by this stream

	typedef std::basic_streambuf<typename streamType::char_type,
				     typename streamType::traits_type
				     > streambuf_type;

public:
	//! Define the reference type for this reference-counted object.

	typedef ref<basic_streamObj< streamType > > ref_t;


	//! Bind this new stream object to a stream buffer

	//! The constructor initializes the C++ stream superclass using a
	//! pointer to the stream buffer passed as an argument.

	basic_streamObj(//! A reference to a stream buffer
			const streambufref_t &streambufrefArg)
		: streamType(interface<streambuf_type>
			     ::getRef(streambufrefArg)),
		  streambufref(streambufrefArg)
	{
		this->exceptions(std::ios::badbit);
	}

	//! The default destructor

	~basic_streamObj() noexcept
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
		streamType::rdbuf(interface<streambuf_type>
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

//! A reference-counted \c std::wistream

//! \code
//! wstreambuf buf(wstreambuf::create());
//!
//! wistream wis(wistream::create(buf));
//! \endcode
//!
//! \see ptr

typedef ref<basic_streamObj< std::wistream > > wistream;

//! A reference-counted \c std::wostream

//! \code
//! wstreambuf buf(wstreambuf::create());
//!
//! wostream wos(wostream::create(buf));
//! \endcode
//!
//! \see ptr

typedef ref<basic_streamObj< std::wostream > > wostream;

//! A reference-counted \c std::wiostream

//! \code
//! wstreambuf buf(wstreambuf::create());
//!
//! wiostream wios(wiostream::create(buf));
//! \endcode
//!
//! \see ptr

typedef ref<basic_streamObj< std::wiostream > > wiostream;

//! A reference-counted \c std::istream

//! \see istream

typedef ptr<basic_streamObj< std::istream > > istreamptr;

//! A reference-counted \c std::ostream

//! \see ostream

typedef ptr<basic_streamObj< std::ostream > > ostreamptr;

//! A pointer to a reference-counted \c std::iostream

//! \see iostream

typedef ptr<basic_streamObj< std::iostream > > iostreamptr;

//! A reference-counted \c std::wistream

//! \see wistream

typedef ptr<basic_streamObj< std::wistream > > wistreamptr;

//! A reference-counted \c std::wostream

//! \see wostream

typedef ptr<basic_streamObj< std::wostream > > wostreamptr;

//! A reference-counted \c std::wiostream

//! \see wiostream

typedef ptr<basic_streamObj< std::wiostream > > wiostreamptr;

#if 0
{
#endif
}
#endif
