/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basicstreamcodecvtobj_H
#define x_basicstreamcodecvtobj_H

#include <x/basicstreamcodecvttype.H>
#include <x/basicstreambufiofilter.H>
#include <x/basicstreambufobj.H>
#include <x/ptr.H>
#include <x/ref.H>
#include <x/interface.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! \c std::basic_streambuf subclass for character conversion

//! This template defines a subclass of \c std::basic_streambuf that implements
//! the output functionality of \c std::basic_streambuf by mapping the
//! written characters to another character class, and writing the converted
//! stream to another \c std::basic_ostream (the destination stream).
//!
//! The constructor takes a reference to some other \c std::basic_ostream.
//! The character stream written to this \c std::basic_streambuf gets
//! mapped and written to the \c std::basic_ostream.
//!
//! It is expected that a \c std::basic_ostream gets instantiated using
//! this \c std::basic_streambuf (the source stream). Therefore, characters
//! written to the source stream get mapped and written to the destination
//! stream.
//!
//! Template parameters:
//!
//! \li \c buf_type - the character type that's written to this
//! \c std::basic_ostreambuf, either \c char or \c wchar_t.
//!
//! \li \c stream_type - the output character type this stream buffer's output
//! is mapped to, also \c char or \c wchar_t.
//!
//! The remaining template parameters, \c state_type, \c buf_traits, and
//! \c stream_traits are not normally specified, and a suitable default is
//! provided for each one. They specify the conversion state object type
//! (which the C++ standard defines that \c std::mbstate_t as the only
//! documented type, anything else is implementation defined), and the
//! character traits of the input and output type.
//!
//! There are two implementation of this template:
//! \link ::wtoc_ostreambuf wtoc_ostreambuf \endlink
//! and \link ::ctow_ostreambuf ctow_ostreambuf\endlink.
//!
//! The semantics of the stream buffer's position are described by
//! basic_ostreambufiofilter.

template<typename buf_type,
	 typename stream_type,
	 typename state_type=std::mbstate_t,
	 typename buf_traits=std::char_traits<buf_type>,
	 typename stream_traits=std::char_traits<stream_type> >
class basic_ostreambufcodecvt : 
	private basic_streamcodecvt_type<buf_type,
					 stream_type,
					 state_type>::basicstreamcodecvt_t,
	virtual public basic_ostreambufiofilter<buf_type, stream_type,
						buf_traits, stream_traits>
{

public:

	//! \c std::basic_streambuf character type

	typedef buf_type buf_t;

	//! \c std::basic_streambuf character trait type

	typedef buf_traits buf_traits_t;

	//! The stream type that this streambuf should be used for

	typedef std::basic_ostream<buf_type, buf_traits> buf_stream_t;

	//! Output stream type

	typedef std::basic_ostream<stream_type, stream_traits> stream_t;

	//! Default constructor

	basic_ostreambufcodecvt(//! Output stream

				//! Mapped character data gets written to this
				//! stream. This object must remain in existence
				//! as long as this object exist.
				stream_t &output_bufArg,

				//! Conversion locale
				const const_locale &localeArg)
		noexcept : basic_streamcodecvt_type<buf_type,
						   stream_type,
						   state_type>
			  ::basicstreamcodecvt_t (localeArg)
	{
		this->output_stream= &output_bufArg;
	}

	//! Default destructor

	~basic_ostreambufcodecvt() noexcept
	{
		try {
			this->sync();
		} catch (...)
		{
		}
	}
};

//! Character to wide-character \c std::streambuf subclass

//! This is a subclass of \c std::streambuf that handles the output written
//! to this \c std::streambuf by converting it to wide characters and writing
//! the converted character stream to some other \c std::wostream.
//!
//! \code
//! std::wostringstream os;
//!
//! ctow_ostreambuf wco(os, locale("en_US.utf8"));
//! \endcode
//!
//! The constructor takes a reference to a \c std::wostream subclass, and a
//! \c locale object. The \c std::wostream subclass must exist as long as this
//! \c ctow_ostreambuf object exists. Output written to this \c std::streambuf
//! subclass gets translated to wide characters, in the given locale, and
//! written to the \c std::wostream object.
//!
//! \code
//! std::ostream outw(&wco);
//!
//! outw << "Hello world" << std::endl;
//! \endcode
//!
//! Because this is a subclass of \c std::streambuf, this
//! \c ctow_ostreambuf object can instantiate a \c std::ostream after which
//! anything that's written to the \c std::ostream gets translated to wide
//! characters and written to the "stacked" \c std::wostream object.

typedef basic_ostreambufcodecvt<char, wchar_t> ctow_ostreambuf;

//! Wide-character to character \c std::wstreambuf subclass

//! This is a subclass of \c std::wstreambuf that handles the
//! wide-character output written
//! to this \c std::wstreambuf by converting it to characters and writing
//! the converted character stream to some other \c std::ostream.
//!
//! \code
//! std::ostringstream os;
//!
//! wtoc_ostreambuf wco(os, locale("en_US.utf8"));
//! \endcode
//!
//! The constructor takes a reference to a \c std::ostream subclass, and a
//! \c locale object. The \c std::ostream subclass must exist as long as this
//! \c ctow_ostreambuf object exists. Wide-character output written to this
//! \c std::wstreambuf subclass gets translated to characters,
//! in the given locale, and written to the \c std::ostream object.
//!
//! \code
//! std::wostream outw(&wco);
//!
//! outw << L"Hello world" << std::endl;
//! \endcode
//!
//! Because this is a subclass of \c std::wstreambuf, this
//! \c wtoc_ostreambuf object can instantiate a \c std::wostream after which
//! anything that's written to the \c std::wostream gets translated to
//! characters and written to the "stacked" \c std::ostream object.

typedef basic_ostreambufcodecvt<wchar_t, char> wtoc_ostreambuf;

extern template class basic_ostreambufcodecvt<char, wchar_t>;
extern template class basic_ostreambufcodecvt<wchar_t, char>;

//! \c std::basic_streambuf subclass for character conversion

//! This template defines a subclass of \c std::basic_streambuf that implements
//! the input functionality of \c std::basic_streambuf by converting a
//! different character stream that's read from another \c std::basic_istream
//! (the destination stream) to another \c std::basic_istream (the source
//! stream.
//!
//! The constructor takes a reference to some other \c std::basic_istream
//! (the destinations tream).
//! It is expected that a \c std::basic_istream gets instantiated using
//! this \c std::basic_streambuf (the source stream).
//! Reading from this \c std::basic_istream returns the stream read from
//! the other \c std::basic_istream and mapped to this character type.
//! This is why this \c std::basic_streambuf is called the source stream.
//! The other \c std::basic_istream is refered to as the destination stream
//! in order to use the consistent naming with \c basic_ostreambufcodecvt.
//!
//! Template parameters:
//!
//! \li \c buf_type - the character type of this \c std::basic_streambuf
//! subclass, either \c char or \c wchar_t.
//!
//! \li \c stream_type - the character type of the other \c std::basic_istream,
//! also either \c char or \c wchar_t.
//!
//! The remaining template parameters, \c state_type, \c buf_traits, and
//! \c stream_traits are not normally specified, and a suitable default is
//! provided for each one. They specify the conversion state object type
//! (which the C++ standard defines that \c std::mbstate_t as the only
//! documented type, anything else is implementation defined), and the
//! character traits of the buffer type and the external stream type
//!
//! There are two implementation of this template:
//! \link ::wtoc_istreambuf wtoc_istreambuf \endlink
//! and \link ::ctow_istreambuf ctow_istreambuf\endlink.
//!
//! The semantics of the stream buffer's position are described by
//! basic_istreambufiofilter.

template<typename buf_type,
	 typename stream_type,
	 typename state_type=std::mbstate_t,
	 typename buf_traits=std::char_traits<buf_type>,
	 typename stream_traits=std::char_traits<stream_type> >
class basic_istreambufcodecvt : 
	private basic_streamcodecvt_type<stream_type,
					 buf_type,
					 state_type>::basicstreamcodecvt_t,
	virtual public basic_istreambufiofilter<buf_type, stream_type,
						buf_traits, stream_traits>
{
public:

	//! \c std::basic_streambuf character type

	typedef buf_type buf_t;

	//! \c std::basic_streambuf character trait type

	typedef buf_traits buf_traits_t;

	//! The stream type that this streambuf should be used for

	typedef std::basic_istream<buf_type, buf_traits> buf_stream_t;

	//! Output stream type

	typedef std::basic_istream<stream_type, stream_traits> stream_t;

	//! Default constructor

	basic_istreambufcodecvt(//! Inputt stream

				//! The data is read from this external stream,
				//! filtered, and loaded into the
				//! \c streambuf.
				//! This object must remain in existence
				//! as long as this object exist.
				stream_t &input_bufArg,

				//! Conversion locale
				const const_locale &localeArg)
		noexcept : basic_streamcodecvt_type<stream_type,
						   buf_type,
						   state_type>
			  ::basicstreamcodecvt_t (localeArg)
	{
		this->input_stream= &input_bufArg;
	}

	//! Default destructor
	~basic_istreambufcodecvt() noexcept
	{
	}
};

//! Character from wide-character \c std::streambuf subclass

//! This is a subclass of \c std::streambuf that implements the input
//! functionality of this \c std::streambuf by converting a wide
//! character stream from another \c std::wistream, and mapping into a
//! character stream.
//!
//! \code
//! std::wistringstream is;
//!
//! wtoc_istreambuf wco(is, locale("en_US.utf8"));
//! \endcode
//!
//! The constructor takes a reference to a \c std::wistream subclass, and a
//! \c locale object. The \c std::wistream subclass must exist as long as this
//! \c wtoc_istreambuf object exists. This \c std::streambuf obtains input
//! by reading from the \c std::wistream and converting the wide character
//! stream into a narrow character stream, in the given locale.
//!
//! \code
//! std::istream inw(&wco);
//!
//! std::string l;
//!
//! std::getline(l, inw);
//! \endcode
//!
//! Because this is a subclass of \c std::streambuf, this
//! \c wtoc_istreambuf object can instantiate a \c std::istream.

typedef basic_istreambufcodecvt<char, wchar_t> wtoc_istreambuf;

//! Wide-character from character \c std::wstreambuf subclass

//! This is a subclass of \c std::wstreambuf that implements the input
//! functionality of this \c std::wstreambuf by converting a wide
//! character stream from another \c std::istream, and mapping into a
//! wide character stream.
//!
//! \code
//! std::istringstream is;
//!
//! ctow_istreambuf wco(is, locale("en_US.utf8"));
//! \endcode
//!
//! The constructor takes a reference to a \c std::istream subclass, and a
//! \c locale object. The \c std::istream subclass must exist as long as this
//! \c ctow_istreambuf object exists. This \c std::wstreambuf obtains input
//! by reading from the \c std::istream and converting the character stream
//! into a wide character stream, in the given locale.
//!
//! \code
//! std::wistream inw(&wco);
//!
//! std::wstring l;
//!
//! std::getline(l, inw);
//! \endcode
//!
//! Because this is a subclass of \c std::wstreambuf, this
//! \c ctow_istreambuf object can instantiate a \c std::wistream.

typedef basic_istreambufcodecvt<wchar_t, char> ctow_istreambuf;

extern template class basic_istreambufcodecvt<char, wchar_t>;
extern template class basic_istreambufcodecvt<wchar_t, char>;

//! \c std::basic_streambuf subclass for character conversion

//! This class derives from
//! \c basic_istreambufcodecvt and \c basic_ostreambufcodecvt, and
//! implements both input and output streambuf interfaces.
//!
//! The semantics of the stream buffer's position are described by
//! basic_iostreambufiofilter.

template<typename buf_type,
	 typename stream_type,
	 typename state_type=std::mbstate_t,
	 typename buf_traits=std::char_traits<buf_type>,
	 typename stream_traits=std::char_traits<stream_type> >
class basic_iostreambufcodecvt : 
	public basic_istreambufcodecvt<buf_type, stream_type, state_type,
				       buf_traits, stream_traits>,
	public basic_ostreambufcodecvt<buf_type, stream_type, state_type,
				       buf_traits, stream_traits>,
	public basic_iostreambufiofilter<buf_type, stream_type,
					 buf_traits, stream_traits>
{
public:

	//! \c std::basic_streambuf character type

	typedef buf_type buf_t;

	//! \c std::basic_streambuf character trait type

	typedef buf_traits buf_traits_t;

	//! The stream type that this streambuf should be used for

	typedef std::basic_iostream<buf_type, buf_traits> buf_stream_t;

	//! Input/output stream type

	typedef std::basic_iostream<stream_type, stream_traits> stream_t;

	//! Default constructor

	basic_iostreambufcodecvt(//! Output stream

				 //! Mapped character data gets written to this
				 //! stream. This object must remain in existence
				 //! as long as this object exist.
				 stream_t &stream_arg,

				 //! Conversion locale
				 const const_locale &localeArg)
		noexcept :
		basic_istreambufcodecvt<buf_type, stream_type,
					state_type, buf_traits,
					stream_traits>(stream_arg,
						       localeArg),
		basic_ostreambufcodecvt<buf_type, stream_type,
					state_type, buf_traits,
					stream_traits>(stream_arg,
						       localeArg),
		basic_iostreambufiofilter<buf_type, stream_type,
					  buf_traits,
					  stream_traits>(stream_arg)
	{
	}

	//! Default destructor
	~basic_iostreambufcodecvt() noexcept
	{
		internal_cleanup();
	}

private:

	//! Internal cleanup function

	void internal_cleanup() noexcept
	{
		try {
			this->sync();
		} catch (...)
		{
		}
	}
};

//! Implementation of \c std::streambuf sourcing the stream from a std::wiostream

//! This is an implementation of the basic_iostreambufcodecvt class template
//! that implements a \c std::streambuf which reads or writes from a stacked
//! \c std::wiostream, converting the wide character stream to/from narrow
//! characters.

typedef basic_iostreambufcodecvt<char, wchar_t> ctow_iostreambuf;

//! Implementation of \c std::wstreambuf sourcing the stream from a std::iostream

//! This is an implementation of the basic_iostreambufcodecvt class template
//! that implements a \c std::wstreambuf which reads or write from a stacked
//! \c std::iostream, converting the character stream to/from wide
//! characters.

typedef basic_iostreambufcodecvt<wchar_t, char> wtoc_iostreambuf;

extern template class basic_iostreambufcodecvt<char, wchar_t>;
extern template class basic_iostreambufcodecvt<wchar_t, char>;

//! Reference-counted \c basic_(i|o|io)streambufcodecvt

//! This template defines a reference-counted object subclass of either
//! \c basic_istreambufcodecvt, \c basic_ostreambufcodecvt, or
//! \c basic_iostreambufcodecvt template classes. The template parameter
//! defines the class instance type of one of these three templates.
//! The template class inherits from the specified instance type, and
//! a matching \c basic_streambufObj template class.

template<typename basic_codecvtobj> class basic_streambufcodecvtObj :
	virtual public basic_streambufObj<typename basic_codecvtobj::buf_t,
					  typename basic_codecvtobj::buf_traits_t>,
	public basic_codecvtobj {

public:

	//! C++ standard library stream type

	//! Which stream type class this stream buffer object converts to/from.
	//!

	typedef typename basic_codecvtobj::stream_t stream_t;

private:
	//! Connected stream object reference

	//! This is a reference to the stream this code conversion object
	//! uses. The object that this reference refers to must be type-
	//! convertable to stream_t.

	ptr<obj> streamref;

 public:
	//! Constructor - C++ input/output library object

	//! Read or write from a native C++ input/output library object.

	basic_streambufcodecvtObj(//! Native C++ input/output library object
				  stream_t &streamArg,

				  //! Locale for conversion
				  const const_locale &localeArg)
		: basic_codecvtobj(streamArg, localeArg)
	{
	}

	//! Constructor - use a reference-counted stream subclass.

	//! Read or write from a a reference-counted subclass of a native
	//! C++ input/output library object.

	basic_streambufcodecvtObj(//! Destination stream

				  //! This is a \c basic_streamObj subclass
				  ref<obj> streamrefArg,

				  //! Locale for conversion
				  const const_locale &localeArg)
		: basic_codecvtobj(*interface<stream_t>
				   ::getRef(streamrefArg), localeArg),
		  streamref(streamrefArg)
	{
	}

	//! Default destructor

	~basic_streambufcodecvtObj() noexcept
	{
	}
};

//! Convert between characters and wide characters

//! This template defines stream objects that convert between characters
//! and wide characters. This template can implement one of several conversions:
//!
//! * A \c std::ostream. Writing to it converts characters to wide characters
//! which are then written to a connected \c std::wostream.
//!
//! * A \c std::wostream. Writing to it converts wide characters to characters
//! which are then written to a connected \c std::ostream.
//!
//! * A \c std::istream. Reading from it reads wide characters from a connected
//! \c std::wistream and converts them to characters.
//!
//! * A \c std::wistream. Reading from it reads characters from a connected
//! \c std::istream and converts them to wide characters.
//!
//! * A \c std::iostream. Reading or writing from it reads or writes wide
//! characters from a connected \c std::wiostream.
//!
//! * A \c std::wiostream. Reading or writing from it reads or writes wide
//! characters from a connected \c std::iostream.
//!
//! All of these objects are reference-counted subclasses of the corresponding
//! C++ input/output library stream objects, instantiated using
//! \c basic_streamObj. This template class defines a static create()
//! function that takes a reference to a reference-counted stream object
//! (the destination object), and
//! a locale object, then returns a reference to a strem object, the source
//! object, that's connected to the destination object.
//!
//! A second version of the create() function is also defined, which takes
//! a reference to a native C++ input/output library stream object (but
//! still returns a reference to a \c basic_streamObj). Care must be taken
//! that the destination native C++ input/output library stream object remains
//! in scope and existance, as long as the source stream object reference
//! is used.
//!
//! \see ctow_ostream, wtoc_ostream, ctow_istream, wtoc_istream,
//! ctow_iostream, wtoc_iostream.
//!
//! The semantics of the stream's buffer position are described by
//! basic_istreambufiofilter, basic_ostreambufiofilter, and
//! basic_iostreambufiofilter.

template<typename conv_streambuf> class basic_convstream {

public:

	//! Reference to the source stream buffer.

	typedef ref< basic_streambufcodecvtObj<conv_streambuf> > streambufref_t;

	//! Pointer to the source stream buffer.

	typedef ptr< basic_streambufcodecvtObj<conv_streambuf> > streambufptr_t;

	//! Source C++ input/output stream

	//! This is the C++ library's stream object, \c std::basic_istream,
	//! \c std::basic_ostream, or \c std::basic_iostream which is connected
	//! to the character conversion filter and is directly read or written
	//! from/to.

	typedef typename conv_streambuf::buf_stream_t stream_t;

	//! Input/output stream reference

	//! This is a reference to the reference-counted \c stream_t,
	//! a \c basic_streamObj template class.

	typedef ref<basic_streamObj<stream_t> > streamref_t;

	//! Input/output stream reference

	//! This is a pointer to the reference-counted \c stream_t,
	//! a \c basic_streamObj template class.

	typedef ptr<basic_streamObj<stream_t> > streamptr_t;

	//! Destination C++ input/output stream

	//! This is the C++ library's stream object, \c std::basic_istream,
	//! \c std::basic_ostream, or \c std::basic_iostream which is connected
	//! to the other end of the character conversion filter, and receives
	//! or provides that characters that get processed by the character
	//! conversion filter. This is generally the same stream object as
	//! \c stream_t, but for the other character type (if \c stream_t is
	//! the narrow character type, this is the wide character type, and
	//! vice versa).

	typedef typename conv_streambuf::stream_t conv_stream_t;

	//! Create a source stream, given the destination stream.

	//! Given the destination stream, create a source stream, and connect
	//! it to the destination stream.
	//!
	//! \return A reference to the source stream.

	static streamref_t
	create(//! Stream being converted to/from, the destination stream.

	       //! This is a reference to a reference-counted subclass
	       //! of one of the standard C++ library input/output streams,
	       //! such as \c std::istream, that matches this
	       //! \c basic_streambufcodecvtObj type
	       const ref<obj> &streamArg,

	       //! Locale for character conversion
	       const const_locale &localeArg)
	{
		return streamref_t::create(streambufref_t
					   ::create(streamArg, localeArg));
	}

	//! Create a source stream, given the destination stream.

	//! \overload
	//! This version of create() takes a native reference to a native
	//! C++ input/output library stream object.
	//!
	//! \return A reference to the source stream.

	static streamref_t
	create(//! Stream being converted to/from

	       //! One of the standard C++ library input/output streams,
	       //! such as \c std::istream, that matches this
	       //! \c basic_streambufcodecvtObj type
	       conv_stream_t &streamArg,

	       //! Locale for character conversion
	       const locale &localeArg)
	{
		return streamref_t::create(streambufref_t
					   ::create(streamArg, localeArg));
	}
};

extern template class basic_streambufcodecvtObj<ctow_ostreambuf>;
extern template class basic_streambufcodecvtObj<wtoc_ostreambuf>;
extern template class basic_streambufcodecvtObj<ctow_istreambuf>;
extern template class basic_streambufcodecvtObj<wtoc_istreambuf>;

extern template class basic_convstream<ctow_ostreambuf>;
extern template class basic_convstream<wtoc_ostreambuf>;
extern template class basic_convstream<ctow_istreambuf>;
extern template class basic_convstream<wtoc_istreambuf>;

//! Output stream for converting narrow characters to wide characters

//! This is a reference to a reference-counted object that subclasses
//! \c std::ostream. The constructor takes a reference to a wide character
//! output stream. Narrow character data written to this stream gets converted
//! to wide characters and written to the stacked output stream.
//!
//! \code
//! std::wostringstream oback;
//!
//! locale l(locale::create("en_US.utf8");
//!
//! ctow_ostream::streamref_t o=ctow_ostream::create(oback, l);
//!
//! (*o) << "narrow character data" << std::flush;
//!
//! std::wstring s=oback.str();
//! \endcode
//!
//! The \c streamref_t typedef is defined for convenience, this is the
//! \c ::ostream reference type. The first argument to the create() function
//! is a native C++ reference to a \c std::wostream.
//! The resulting \c streamref_t
//! is a reference to a reference-counted object. \c (*o) is a
//! \c std::ostream subclass. The second argument is a locale reference.
//!
//! After all formatted data is written to the
//! \c std::ostream, it must be flushed in order to finish converting any
//! buffered data.
//!
//! Instead of a native C++ reference, the second argument to create() may
//! be a reference to a suitable reference-counted subclass of
//! \c std::ostream:
//!
//! \code
//! wostringstream oback(wostringstream::create());
//!
//! locale l(locale::create("en_US.utf8");
//!
//! ctow_ostream::streamref_t o=ctow_ostream::create(oback, l);
//!
//! (*o) << "narrow character data" << std::flush;
//!
//! std::wstring s=oback->str();
//! \endcode

typedef basic_convstream< ctow_ostreambuf > ctow_ostream;

//! Output stream for converting wide characters to characters

//! This is a reference to a reference-counted object that subclasses
//! \c std::wostream. The constructor takes a reference to a narrow character
//! output stream. Wide character data written to this stream gets converted
//! to narrow characters and written to the stacked output stream.
//!
//! \code
//! std::ostringstream oback;
//!
//! locale l(locale::create("en_US.utf8");
//!
//! wtoc_ostream::streamref_t o=wtoc_ostream::create(oback, l);
//!
//! (*o) << L"wide character data" << std::flush;
//!
//! std::string s=oback.str();
//! \endcode
//!
//! The \c streamref_t typedef is defined for convenience, this is the
//! \c ::wostream reference type. The first argument to the create() function
//! is a native C++ reference to a \c std::ostream. The resulting \c streamref_t
//! is a reference to a reference-counted object. \c (*o) is a
//! \c std::wostream subclass. The second argument is a locale reference.
//!
//! After all formatted data is written to the
//! \c std::wostream, it must be flushed in order to finish converting any
//! buffered data.
//!
//! Instead of a native C++ reference, the second argument to create() may
//! be a reference to a suitable reference-counted subclass of
//! \c std::wostream:
//!
//! \code
//! ostringstream oback(ostringstream::create());
//!
//! locale l(locale::create("en_US.utf8");
//!
//! wtoc_ostream::streamref_t o=wtoc_ostream::create(oback, l);
//!
//! (*o) << L"wide character data" << std::flush;
//!
//! std::string s=oback->str();
//! \endcode

typedef basic_convstream< wtoc_ostreambuf > wtoc_ostream;

//! Input stream for converting narrow characters to wide characters

//! This is a reference to a reference-counted object that subclasses
//! \c std::wistream. The constructor takes a reference to a narrow character
//! input stream. Reading from this \c std::wistream subclass reads from the
//! narrow character input stream, converting the character stream data to
//! wide character stream data, in the given locale:
//!
//! \code
//! std::istringstream isrc("Narrow character data");
//!
//! locale l(locale::create("en_US.utf8");
//!
//! ctow_istream::streamref_t i=ctow_istream::create(isrc, l);
//!
//! std::wstring w;
//!
//! std::getline( *i, w);
//! \endcode
//!
//! The \c streamref_t typedef is defined for convenience, this is the
//! \c ::wistream reference type. The first argument to the create() function
//! is a native C++ reference to a \c std::istream. The resulting \c streamref_t
//! is a reference to a reference-counted object. \c (*i) is a
//! \c std::wistream subclass. The second argument is a locale reference.
//!
//! Instead of a native C++ reference, the second argument to create() may
//! be a reference to a suitable reference-counted subclass of
//! \c std::wistream:
//!
//! \code
//! istringstream isrc(istringstream::create("Narrow character data"));
//!
//! locale l(locale::create("en_US.utf8");
//!
//! ctow_istream::streamref_t o=ctow_istream::create(isrc, l);
//!
//! std::wstring w;
//!
//! std::getline( *i, w);
//! \endcode
//!
//! \note
//! This stream is seekable, but seeking from \c std::ios_base::cur always
//! fails. This stream buffers narrow characters read from the input stream,
//! in arbitrary chunks. The current position in this stream has no relationship
//! with the current input position in the narrow character stream. As such,
//! seeking from the current position always returns an error. Seeking to
//! absolute positions (from the beginning or the end markers), works fine
//! as long as the absolute position in the narrow character stream is outside
//! of any beginning/ending shift sequence range in the locale's character set.

typedef basic_convstream< ctow_istreambuf > ctow_istream;

//! Input stream for converting wide characters to narrow characters

//! This is a reference to a reference-counted object that subclasses
//! \c std::istream. The constructor takes a reference to a wide character
//! input stream. Reading from this \c std::istream subclass reads from the
//! wide character input stream, converting the character stream data from
//! wide character stream data, in the given locale:
//!
//! \code
//! std::wistringstream isrc(L"Wide character data");
//!
//! locale l(locale::create("en_US.utf8");
//!
//! wtoc_istream::streamref_t i=wtoc_istream::create(isrc, l);
//!
//! std::string w;
//!
//! std::getline( *i, w);
//! \endcode
//!
//! The \c streamref_t typedef is defined for convenience, this is the
//! \c ::istream reference type. The first argument to the create() function
//! is a native C++ reference to a \c std::wistream.
//! The resulting \c streamref_t
//! is a reference to a reference-counted object. \c (*i) is a
//! \c std::istream subclass. The second argument is a locale reference.
//!
//! Instead of a native C++ reference, the second argument to create() may
//! be a reference to a suitable reference-counted subclass of
//! \c std::istream:
//!
//! \code
//! wistringstream isrc(wistringstream::create("Wide character data"));
//!
//! locale l(locale::create("en_US.utf8");
//!
//! wtoc_istream::streamref_t o=wtoc_istream::create(isrc, l);
//!
//! std::string w;
//!
//! std::getline( *i, w);
//! \endcode
//!
//! \note
//! This stream is seekable, but seeking from \c std::ios_base::cur always
//! fails. This stream buffers wide characters read from the input stream,
//! in arbitrary chunks. The current position in this stream has no relationship
//! with the current input position in the wide character stream. As such,
//! seeking from the current position always returns an error. Seeking to
//! absolute positions (from the beginning or the end markers), works fine.

typedef basic_convstream< wtoc_istreambuf > wtoc_istream;

//! Convert from/to wide characters

//! This class combines the functionality of ::ctow_ostream and ::wtoc_istream
//! by creating a reference to a reference-counted subclass of
//! \c std::iostream. Reading or writing from this class reads or writes from
//! an associated \c std::wiostream.
//!
//! \code
//! std::wstringstream oback;
//!
//! locale l(locale::create("en_US.utf8");
//!
//! ctow_iostream::streamref_t o=ctow_iostream::create(oback, l);
//!
//! (*o) << "Narrow character string";
//!
//! o->seekg(0);
//!
//! std::string c;
//!
//! std::getline(*o, c);
//!
//! std::wstring w=oback.str();
//!
//! // w is now the wide-character version of c
//! \endcode
//!
//! The above example converts a narrow character stream to a wide character
//! stream, storing the result in a \c std::wstringstream, then converts it
//! back from wide characters to narrow characters.
//!
//! Instead of a native C++ reference, the second argument to create() may
//! be a reference to a suitable reference-counted subclass of
//! \c std::wiostream:
//!
//! \code
//! wstringstream oback(wstringstream::create());
//!
//! locale l(locale::create("en_US.utf8");
//!
//! ctow_iostream::streamref_t o=ctow_iostream::create(oback, l);
//!
//! (*o) << "Narrow character string";
//!
//! o->seekg(0);
//!
//! std::string c;
//!
//! std::getline(*o, c);
//!
//! std::wstring w=oback.str();
//!
//! // w is now the wide-character version of c
//! \endcode
//!
//! \note
//! This class shares the same restrictions on the get position's seeking
//! as ::wtoc_istream. The semantics of seekp() are well-defined, and are
//! passed through and executed on the associated stream's object.
//! seekg() fails for positions relative to \c std::ios_base::cur. Absolute
//! positions for seekg() are allowed, and seekg() is passed through to the
//! associated stream's object.

typedef basic_convstream< ctow_iostreambuf > ctow_iostream;

//! Convert from/to narrow characters

//! This class combines the functionality of ::wtoc_ostream and ::ctow_istream
//! by creating a reference to a reference-counted subclass of
//! \c std::wiostream. Reading or writing from this class reads or writes from
//! an associated \c std::iostream.
//!
//! \code
//! std::stringstream oback;
//!
//! locale l(locale::create("en_US.utf8");
//!
//! wtoc_iostream::streamref_t o=wtoc_iostream::create(oback, l);
//!
//! (*o) << L"Narrow character string";
//!
//! o->seekg(0);
//!
//! std::wstring w;
//!
//! std::getline(*o, w);
//!
//! std::string c=oback.str();
//!
//! // c is now the narrow-character version of w
//! \endcode
//!
//! The above example converts a wide character stream to a narrow character
//! stream, storing the result in a \c std::stringstream, then converts it
//! back from narrow characters to wide characters.
//!
//! Instead of a native C++ reference, the second argument to create() may
//! be a reference to a suitable reference-counted subclass of
//! \c std::iostream:
//!
//! \code
//! stringstream oback(stringstream::create());
//!
//! locale l(locale::create("en_US.utf8");
//!
//! wtoc_iostream::streamref_t o=wtoc_iostream::create(oback, l);
//!
//! (*o) << L"Narrow character string";
//!
//! o->seekg(0);
//!
//! std::wstring w;
//!
//! std::getline(*o, w);
//!
//! std::string c=oback.str();
//!
//! // c is now the narrow-character version of w
//! \endcode
//!
//! \note
//! This class shares the same restrictions on the get position's seeking
//! as ::ctow_istream. The semantics of seekp() are well-defined, and are
//! passed through and executed on the associated stream's object.
//! seekg() fails for positions relative to \c std::ios_base::cur. Absolute
//! positions for seekg() are allowed, provided that they do not specify
//! a position within a beginning/ending shift sequence in the locale's
//! character set. seekg() is passed through to the
//! associated stream's object.

typedef basic_convstream< wtoc_iostreambuf > wtoc_iostream;

#if 0
{
#endif
}
#endif
