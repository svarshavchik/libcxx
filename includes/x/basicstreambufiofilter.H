/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basicstreambufiofilter_H
#define x_basicstreambufiofilter_H

#include <x/basiciofilter.H>
#include <x/basicstreamobj.H>
#include <x/namespace.h>

#include <streambuf>
#include <vector>
#include <algorithm>
#include <cstdio>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Output std::basic_streambuf filter implementation

//! This template defines a subclass of \c std::basic_streambuf that implements
//! the output functionality of \c std::basic_streambuf by filtering the
//! output stream through an iofilter to another \c std::basic_ostream.
//!
//! This template virtually derives from iofilter, and the actual filtering
//! class must be defined by inheriting from this template and iofilter.
//!
//! The constructor takes a reference to some other \c std::basic_ostream.
//! The character stream written to this \c std::basic_streambuf gets
//! filtered to the \c std::basic_ostream.
//!
//! Template parameters:
//!
//! - \c buf_type - the character type written to this
//! \c std::basic_ostreambuf
//!
//! - \c stream_type - the output character type this stream buffer's output
//! is filtered into.
//!
//! The input type and the output type must match the input and the output
//! types of the iofilter. The input type matches this \c std::basic_streambuf
//! type, and the output type matches the type of the other
//! \c std::basic_ostream.
//!
//! The remaining template parameters, \c buf_traits, and
//! \c stream_traits are not normally specified, and a suitable default is
//! provided for each one. They specify the
//! character traits of the input and output type.
//!
//! \note
//! The subclass that inherits from this class and iofilter must invoke
//! sync() in its destructor, and trap all exceptions!
//!
//! \par Buffer positioning
//!
//! Generally, the actual stream position of this \c std::streambuf is
//! independent of the output stream's position. The seekoff() and seekpos()
//! methods flush and convert any remaining buffered character stream, then
//! invoke the output stream's seekp() method. This gives the most predictable
//! and meaningful semantics.

template<typename buf_type,
	 typename stream_type,
	 typename buf_traits=std::char_traits<buf_type>,
	 typename stream_traits=std::char_traits<stream_type> >
class basic_ostreambufiofilter
	: virtual public std::basic_streambuf<buf_type, buf_traits>,
	  virtual public iofilter<buf_type, stream_type>
{
protected:
	//! Output stream

	std::basic_ostream<stream_type, stream_traits> *output_stream;

public:
        //! Internally-allocated buffer

        std::vector<typename buf_traits::char_type> dynbuffer;

        //! Current input buffer
        typename buf_traits::char_type *buf_ptr;

        //! Input buffer's size
        std::streamsize buf_size;

	//! Output buffer

	std::vector<typename stream_traits::char_type> outputdynbuffer;

	//! Current output buffer

	typename stream_traits::char_type *out_buf_ptr;

	//! Output buffer's size

	std::streamsize out_buf_size;

public:
	//! Instantiate an output \c std::basic_streambuf subclass

	//! This constructor should not be used by a virtual subclass.
	//!
	basic_ostreambufiofilter(//! The output stream

				 //! Mapped character data gets written to this
				 //! stream. This object must remain in
				 //! existence as long as this object exists.
				 std::basic_ostream<stream_type,
				 stream_traits> &output_streamArg)
		noexcept : output_stream(&output_streamArg),

			  buf_ptr(0),
			  buf_size(0),
			  out_buf_ptr(0),
			  out_buf_size(0)
	{
	}

protected:

	//! Instantiate an output \c std::basic_streambuf subclass

	//! This constructor is used by virtual subclasses.
	//! The virtual subclass is
	//! responsible for manually initializing \c output_stream.

	basic_ostreambufiofilter() noexcept : output_stream(0),
		buf_ptr(0),
		buf_size(0),
		out_buf_ptr(0),
		out_buf_size(0)
	{
	}
public:
	//! Default destructor

	//! Any buffered output gets flushed.
	//!
	~basic_ostreambufiofilter() noexcept
	{
	}

	//! Implement the \c std::basic_streambuf::overflow() method

	//! The buffered output gets mapped and written the output stream.
	//!
	typename buf_traits::int_type overflow(typename buf_traits::int_type ch)
	{
		//! Flush out anything that's in the output buffer.

		if (this->pptr() && this->pbase() < this->pptr()
		    && internal_sync() < 0)
			return buf_traits::eof();

		//! Dynamically allocate the output buffer, if required

		if (!buf_ptr || buf_size == 0)
		{
			dynbuffer.resize(BUFSIZ);
			buf_ptr=&dynbuffer[0];
			buf_size=dynbuffer.size();
		}

		this->setp(buf_ptr, buf_ptr + buf_size);

		if (!buf_traits::eq_int_type(ch, buf_traits::eof()))
		{
			*this->pptr()=ch;
			this->pbump(1);
		}
		return 0;
	}

	//! Implement the \c std::basic_streambuf::sync() method

	//! The buffered output gets mapped and written the output stream.
	//!

	int sync()
	{
		int rc=1;

		//! Continue to flush the output buffer, as long as
		//! there's still something in it.
		while (this->pptr() && this->pbase() < this->pptr())
			if (internal_sync() < 0)
				return -1;

		//! Repeatedly try to flush, in order for any closing
		//! shift sequence to come out.
		do
		{
			if ((rc=internal_sync()) < 0)
				return -1;
		} while (rc > 0);

		//! Now, flush the output stream, as well.

		return output_stream->flush().good() ? 0:-1;
	}

	//! Implement the \c std::basic_streambuf::setbuf() method

	//! Any output that's already buffered gets flushed
	//!
	std::basic_streambuf<buf_type,
			     buf_traits> *setbuf(typename
						   buf_traits::char_type *s,
						   std::streamsize n)
	{
		if (overflow(buf_traits::eof()) == buf_traits::eof())
			return NULL;

		dynbuffer.clear();
		buf_ptr=s;
		buf_size=n;
		this->setp(buf_ptr, buf_ptr+buf_size);
		return this;
	}

	//! Set a custom buffer for mapped characters

	void setoutputbuf(typename stream_traits::char_type *s,
			  std::streamsize n)
	{
		out_buf_ptr=s;
		out_buf_size=n;
	}

	//! Implement the \c std::basic_streambuf::seekoff() method

        typename buf_traits::pos_type
	seekoff(typename buf_traits::off_type off,
			 std::ios_base::seekdir way,
                         std::ios_base::openmode which)
	{
		typedef typename buf_traits::pos_type pos_type;
		typedef typename buf_traits::off_type off_type;

		if (sync() < 0)
			return (typename buf_traits::pos_type)
				(typename buf_traits::off_type)-1;

		return pos_type((off_type)
				(output_stream->seekp(off, way).good() ? 0:-1));
	}

	//! Implement the \c std::basic_streambuf::seekpos() method

        typename buf_traits::pos_type
	seekpos(typename buf_traits::pos_type pos,
		std::ios_base::openmode which)
	{
		typedef typename buf_traits::pos_type pos_type;
		typedef typename buf_traits::off_type off_type;

		if (sync() < 0)
			return (typename buf_traits::pos_type)
				(typename buf_traits::off_type)-1;
		return pos_type((off_type)
				(output_stream->seekp(pos).good() ? 0:-1));
	}


private:
	//! Map buffered output.

	int internal_sync()
	{
		int rc=0;

		do
		{
			this->next_in=this->pbase();
			this->avail_in=this->pptr() - this->pbase();

			if (out_buf_ptr == 0)
			{
				outputdynbuffer.resize(BUFSIZ);

				out_buf_ptr=&outputdynbuffer[0];
				out_buf_size=outputdynbuffer.size();
			}

			this->next_out=out_buf_ptr;
			this->avail_out=out_buf_size;

			this->filter();

			if (this->avail_in)
				std::copy(this->next_in,
					  this->next_in+this->avail_in,
					  this->pbase());

			this->setp(this->pbase(), this->epptr());
			this->pbump(this->avail_in);

			if (this->next_out > out_buf_ptr)
			{
				output_stream->write(out_buf_ptr,
						  this->next_out - out_buf_ptr);

				if (output_stream->bad())
					return -1;
				rc=1;
			}
		} while ( this->pptr() && this->pptr() == this->epptr());
		return rc;
	}
};

//! Input std::basic_streambuf filter implementation

//! This template defines a subclass of \c std::basic_streambuf that implements
//! the input functionality of \c std::basic_streambuf by filtering another
//! input stream through an iofilter into this \c std::basic_streambuf
//!
//! This template virtually derives from iofilter, and the actual filtering
//! class must be defined by inheriting from this template and iofilter.
//!
//! The constructor takes a reference to some other \c std::basic_istream.
//! The character stream is written from that \c std::basic_istream, filtered
//! through the iofilter, and stored in this \c std::basic_streambuf.
//!
//! Template parameters:
//!
//! - \c stream_type - the character type read from another
//! \c std::basic_istream.
//!
//! - \c buf_type - the output character type of this stream buffer.
//!
//! The input type and the output type must match the input and the output
//! types of the iofilter. The input type matches \c std::basic_istream's
//! type, and the output type matches the type of this \c std::basic_streambuf.
//!
//! \par Buffer positioning
//!
//! Generally, the actual stream position of this \c std::streambuf is
//! independent of the input stream's position. The seekoff() and seekpos()
//! methods clear the input buffer, then invoke the other stream's seekg()
//! method.
//!
//! The seek direction may only be \c std::ios_base::beg or
//! \c std::ios_base::end. Specifying a seek direction of \c std::ios_base::cur
//! returns an error. Because characters are read from the other input stream
//! in blocks, and are buffered, the actual input position in the source
//! input stream is indeterminate, at any given time.

template<typename buf_type,
	 typename stream_type,
	 typename buf_traits=std::char_traits<buf_type>,
	 typename stream_traits=std::char_traits<stream_type> >
class basic_istreambufiofilter
	: virtual public std::basic_streambuf<buf_type, buf_traits>,
	  virtual public iofilter<stream_type, buf_type>
{
protected:
	//! Input stream

	std::basic_istream<stream_type, stream_traits> *input_stream;

private:
	//! Internally-allocated buffer

	std::vector<typename buf_traits::char_type> dynbuffer;

	//! Current output buffer

	typename buf_traits::char_type *buf_ptr;

	//! Output buffer's size

	std::streamsize buf_size;

	//! Input buffer

	std::vector<typename stream_traits::char_type> inputdynbuffer;

	//! Current input buffer

	typename stream_traits::char_type *input_buf_ptr;

	//! Input buffer's size

	std::streamsize input_buf_size;

public:

	//! Instantiate an input \c std::basic_streambuf subclass

	//! This constructor should not be used by a virtual subclass.
	//!
	basic_istreambufiofilter(//! The input stream

				 //! Mapped character data gets read from this
				 //! stream. This objet must remain in
				 //! existence as long as this object exists.

				 std::basic_istream<stream_type,
				 stream_traits> &input_streamArg) noexcept
		: input_stream(&input_streamArg),
		  buf_ptr(0),
		  buf_size(0),
		  input_buf_ptr(0),
		  input_buf_size(0)
	{
	}
protected:

	//! Instantiate an input \c std::basic_streambuf subclass

	//! This constructor is used by virtual subclasses.
	//! The virtual subclass is
	//! responsible for manually initializing \c input_stream.

	basic_istreambufiofilter() noexcept : input_stream(0),
		buf_ptr(0),
		buf_size(0),
		input_buf_ptr(0),
		input_buf_size(0)
	{
	}
public:

	//! Default destructor

	~basic_istreambufiofilter() noexcept
	{
	}

	//! Implement the \c std::basic_streambuf::showmanyc() method

	std::streamsize showmanyc()
	{
		typename buf_traits::char_type *p=this->gptr(),
			*e=this->egptr();

		if (p && e > p)
			return e-p;

		if (buf_traits::eq_int_type(underflow(),
					       buf_traits::eof()))
			return 0;

		return this->egptr() - this->gptr();
	}

	//! Implement the \c std::basic_streambuf::underflow() method

	typename buf_traits::int_type underflow()
	{
		if (!this->gptr() || this->eback() >= this->egptr())
		{
			dynbuffer.resize(BUFSIZ);
			buf_ptr=&dynbuffer[0];
			buf_size=dynbuffer.size();
			this->setg(buf_ptr, buf_ptr+buf_size, buf_ptr+buf_size);
		}

		typename buf_traits::char_type *b=this->eback(),
			*e=this->egptr();

		while (this->gptr() >= e)
		{
			if (!this->avail_in)
			{
				if (!input_buf_ptr)
				{
					inputdynbuffer.resize(input_buf_size=
							      BUFSIZ);
					input_buf_ptr=&inputdynbuffer[0];
				}

				this->next_in=input_buf_ptr;
				this->avail_in=input_stream->
					readsome(input_buf_ptr,
						 input_buf_size);
			}

			this->next_out=b;
			this->avail_out=e-b;

			bool more=false;

			if (this->avail_in)
				more=true;

			this->filter();

			typename buf_traits::char_type *ee=e;
			typename buf_traits::char_type *p=this->next_out;

			if (!more && p == b)
			{
				// There was no input, and no output produced

				return buf_traits::eof();
			}

			while (p > b)
				*--ee= *--p;

			this->setg(b, ee, e);
		}

		return buf_traits::to_int_type(*this->gptr());
	}


	//! Implement the \c std::basic_streambuf::setbuf() method

	//! Any output that's already buffered gets flushed
	//!
	std::basic_streambuf<buf_type,
			     buf_traits> *setbuf(typename
						 buf_traits::char_type *s,
						 std::streamsize n)
	{
		buf_ptr=s;
		buf_size=n;
		dynbuffer.clear();
		this->setg(s, s+n, s+n);
		filter_reset();
		return this;
	}

	//! Set a custom buffer for mapped characters

	void setinputbuf(typename stream_traits::char_type *s,
			 std::streamsize n)
	{
		input_buf_ptr=s;
		input_buf_size=n;
		inputdynbuffer.clear();
	}

	//! Implement the \c std::basic_streambuf::seekoff() method

        typename buf_traits::pos_type
	seekoff(typename buf_traits::off_type off,
			 std::ios_base::seekdir way,
                         std::ios_base::openmode which)
	{
		typedef typename buf_traits::pos_type pos_type;
		typedef typename buf_traits::off_type off_type;

		if (way == std::ios_base::cur)
			return pos_type((off_type)-1);

		if (this->gptr())
			this->setg(this->eback(), this->egptr(), this->egptr());
		filter_reset();

		return pos_type((off_type)
				(input_stream->seekg(off, way).good() ? 0:-1));
	}

	//! Implement the \c std::basic_streambuf::seekpos() method

        typename buf_traits::pos_type
	seekpos(typename buf_traits::pos_type pos,
		std::ios_base::openmode which)
	{
		typedef typename buf_traits::pos_type pos_type;
		typedef typename buf_traits::off_type off_type;

		if (this->gptr())
			this->setg(this->eback(), this->egptr(), this->egptr());

		filter_reset();
		return pos_type((off_type)
				(input_stream->seekg(pos).good() ? 0:-1));
	}

private:

	//! Flush partially-filtered data

	//! \internal
	//! When the input stream is about to be repositioned, flush out
	//! anything that's partially filtered.

	void filter_reset()
	{
		typename buf_traits::char_type *b=this->eback(),
			*e=this->egptr();

		do
		{
			this->next_in=0;
			this->avail_in=0;
			this->next_out=b;
			this->avail_out=e-b;

			this->filter();
		} while (this->next_out > b);
	}
};


//! Input/Output std::basic_streambuf filter implementation

//! This class inherits from basic_istreambufiofilter and
//! basic_ostreambufiofilter to provide input/output filtering facilities.
//!
//! \par Buffer positioning
//!
//! The semantics of the output half of the stream's buffer position are
//! generally well defined. See \c basic_ostreambufiofilter. The issues with
//! the semantics of the input half of the stream's buffer position are
//! documented by \c basic_istreambufiofilter.
//!
//! The semantics of the combined input/output stream are as follows.
//! The output buffer gets flushed, the input buffer gets cleared. seekoff()
//! and seekpos()
//! invokes the other stream's seekp() method. If the \c which parameter also
//! indicates the input
//! stream, the returned stream position is then passed to the other stream's
//! seekg(), setting both input and output stream buffer positions
//! to be the same.

template<typename buf_type,
	 typename stream_type,
	 typename buf_traits=std::char_traits<buf_type>,
	 typename stream_traits=std::char_traits<stream_type> >
class basic_iostreambufiofilter :
	virtual public basic_ostreambufiofilter<buf_type, stream_type,
						buf_traits, stream_traits>,
	virtual public basic_istreambufiofilter<buf_type, stream_type,
						buf_traits, stream_traits>
{
	//! Shortcut definition for the output stream filter superclass

	typedef basic_ostreambufiofilter<buf_type, stream_type,
					 buf_traits, stream_traits> ofilter;
	//! Shortcut definition for the input stream filter superclass

	typedef basic_istreambufiofilter<buf_type, stream_type,
					 buf_traits, stream_traits> ifilter;

	//! Save the original \c std::iostream

	std::basic_iostream<stream_type,
			    stream_traits> &ext_stream;

public:

	//! Instantiate an input/output \c std::basic_streambuf subclass

	basic_iostreambufiofilter(std::basic_iostream<stream_type,
				  stream_traits> &ext_streamArg) noexcept

		: ext_stream(ext_streamArg)
	{
		this->input_stream= &ext_stream;
		this->output_stream= &ext_stream;
	}

	//! Default destructor

	~basic_iostreambufiofilter() noexcept
	{
	}

	//! Implement the \c std::basic_streambuf::seekoff() method

        typename buf_traits::pos_type
	seekoff(typename buf_traits::off_type off,
		std::ios_base::seekdir way,
		std::ios_base::openmode which)
	{
		typedef typename buf_traits::pos_type pos_type;
		typedef typename buf_traits::off_type off_type;

		if (!reset_iobuf() || ext_stream.seekp(off, way).fail())
			return pos_type((off_type)-1);

		typename buf_traits::pos_type p=ext_stream.tellp();

		if ((which & std::ios_base::in) &&
		    (p != pos_type((off_type)-1)))
			ext_stream.seekg(p);
		return p;
	}

	//! Implement the \c std::basic_streambuf::seekpos() method

        typename buf_traits::pos_type
	seekpos(typename buf_traits::pos_type pos,
		std::ios_base::openmode which)
	{
		typedef typename buf_traits::pos_type pos_type;
		typedef typename buf_traits::off_type off_type;

		if (!reset_iobuf() || ext_stream.seekp(pos).fail())
			return pos_type((off_type)-1);

		typename buf_traits::pos_type p=ext_stream.tellp();

		if ((which & std::ios_base::in) &&
		    (p != pos_type((off_type)-1)))
			ext_stream.seekg(p);

		return p;
	}

	//! Implement the \c std::basic_streambuf::overflow() method

	//! Clear the input buffer, then invoke the real overflow() method.
	//!
	typename buf_traits::int_type overflow(typename buf_traits::int_type ch)
	{
		this->setg(this->eback(),
			   this->egptr(),
			   this->egptr());
		return ofilter::overflow(ch);
	}

	//! Implement the \c std::basic_streambuf::sync() method

	//! Clear the input buffer, then invoke the real sync() method.
	//!

	int sync()
	{
		this->setg(this->eback(),
			   this->egptr(),
			   this->egptr());

		return ofilter::sync();
	}

	//! Implement the \c std::basic_streambuf::underflow() method

	//! Clear the input buffer, then invoke the real underflow() method.
	//!
	typename buf_traits::int_type underflow()
	{
		if (!reset_obuf())
			return buf_traits::eof();

		return ifilter::underflow();
	}

	//! Forward setbuf() to both the ifilter and ofilter superclasses.

	std::basic_streambuf<buf_type,
			     buf_traits> *setbuf(typename
						 buf_traits::char_type *s,
						 std::streamsize n)
	{
		ifilter::setbuf(s, n);
		return ofilter::setbuf(s, n);
	}

private:
	//! Reset get area and put area pointers

	//! \internal
	//! If there's output buffered up, flush it.
	//! In all cases, flush the stacked stream.

	bool reset_iobuf()
	{
		this->setg(this->eback(),
			   this->egptr(),
			   this->egptr());

		return reset_obuf();
	}

	//! Reset the put area

	//! \internal
	//! If there's output buffered up, flush it.
	//! In all cases, reset the put area pointers.

	bool reset_obuf()
	{
		if (this->pptr() && this->pbase() < this->pptr())
		{
			if (this->pubsync() < 0)
				return false;
			return true;
		}
		
		if (ext_stream.flush().bad())
			return false;

		return true;
	}
};

extern template class basic_ostreambufiofilter<char, wchar_t>;
extern template class basic_ostreambufiofilter<wchar_t, char>;
extern template class basic_istreambufiofilter<char, wchar_t>;
extern template class basic_istreambufiofilter<wchar_t, char>;
extern template class basic_iostreambufiofilter<char, wchar_t>;
extern template class basic_iostreambufiofilter<wchar_t, char>;

#if 0
{
#endif
}
#endif
