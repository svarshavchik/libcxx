/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_encoderbase_H
#define x_mime_encoderbase_H

#include <iterator>
#include <x/obj.H>
#include <x/refiterator.H>
#include <x/qp.H>
#include <x/base64.H>
#include <x/fdbaseobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	//! Classes and templates for parsing MIME-formatted messages.

	namespace mime {
#if 0
	}
}
#endif

//! Reference-counted object referenced by \ref encoderbase "encoderbase"

class encoderbaseObj : virtual public obj {

public:
	//! Constructor
	encoderbaseObj();

	//! Destructor
	~encoderbaseObj();

	//! Return a beginning iterator for a MIME-encoded section

	//! begin() is implemented in a subclass.
	//!

	virtual inputrefiterator<char> begin() const=0;

	//! Return an ending interator value for a MIME-encoded section
 
	virtual inputrefiterator<char> end() const;
};

//! An encoder for a MIME section

//! \code
//! INSERT_LIBX_NAMESPACE::mime::encoderbase encoder= // ...
//!
//! std::copy(encoder->begin(), encoder->end(),
//!           std::ostreambuf_iterator<char>(std::cout));
//! \endcode
//! This is a reference to a reference-counted object that returns a beginning
//! and an ending iterator for a MIME-encoded entity.

typedef ref<encoderbaseObj> encoderbase;

//! A nullable pointer reference to a \ref encoderbase "MIME section".

typedef ptr<encoderbaseObj> encoderbaseptr;

//! A reference to a constant \ref encoderbase "MIME section".

typedef const_ref<encoderbaseObj> const_encoderbase;

//! A nullable pointer reference to a constant \ref encoderbase "MIME section".

typedef const_ptr<encoderbaseObj> const_encoderbaseptr;

//! base64 and quoted-printable encoders need an eof() call to wrap things up.

//! \internal

template<typename encoder_type>
inline void signal_encoder_eof(encoder_type &encoder)
{
	encoder.eof();
}

//! 7bit and 8bit encoders dump directly into the output buffer, no eof().

//! \internal
//!
template<>
inline void signal_encoder_eof(std::back_insert_iterator<std::vector<char> > &)
{
}

//! Implement an input iterator for MIME-encoded sections.

//! \internal
//!
//! Subclasses inputrefiteratorObj. Constructed by encoderImplObj. The
//! constructor takes a beginning and an ending iterator for an input sequence
//! that's MIME-encoded, and becomes the beginning iterator value of the
//! MIME-encoded sequence.

template<typename iterator_type,
	 typename encoder_type>
class encoderbaseImplObj : public inputrefiteratorObj<char> {

	//! Current iterator value.
	mutable iterator_type cur_iter;

	//! Ending iterator value
	iterator_type end_iter;

	//! The encoder class.

	//! An output iterator that writes to the buffer in the superclass
	//!
	mutable encoder_type encoder;

	//! The object that creates this input iterator

	//! Keep a reference to this object, as long as this input iterator
	//! is in scope. This makes sure that cur_iter and end_iter remain
	//! valid.
	const_ref<obj> section;

public:
	//! Constructor

	template<typename encoderInitArg>
	encoderbaseImplObj(iterator_type &&beg_iterArg,
			   iterator_type &&end_iterArg,
			   const encoderInitArg &encoderInit,
			   const const_ref<obj> &sectionArg)
		: cur_iter(std::move(beg_iterArg)),
		  end_iter(std::move(end_iterArg)),
		  encoder(encoderInit(this->buffer)),
		  section(sectionArg)
	{
		this->buffer.reserve(fdbaseObj::get_buffer_size());
	}

	//! Destructor
	~encoderbaseImplObj() {}

	//! Fill the input buffer with the next MIME-encoded chunk.

	void fill() const
	{
		auto n=fdbaseObj::get_buffer_size();

		while (cur_iter != end_iter)
		{
			*encoder++=*cur_iter;

			if (++cur_iter == end_iter)
				signal_encoder_eof(encoder);

			auto s=this->buffer.size();

			if (s && s+10 >= n)
				break;
		}
	}
};

//! Subclass of encoderbaseObj that implements begin()

//! \internal
//!
//! The constructor takes a container, and an encoder initializer
//! functor/lambda.
//!
//! begin() obtains the container's beginning and ending iterator, and uses it
//! together with the encoder to construct the beginning iterator for the
//! MIME-encoded input sequence.

template<typename container_type,
	 typename encoder_init_type>
class encoderImplObj : public encoderbaseObj {

	//! The container for the MIME blob to be encoded.
	container_type container;

	//! Mime encoder initializer.
	encoder_init_type encoder_init;

public:

	//! Constructor
	encoderImplObj(const container_type &containerArg,
		       encoder_init_type &&encoder_initArg)
		: container(containerArg),
		  encoder_init(std::move(encoder_initArg))
	{
	}

	//! Constructor
	encoderImplObj(container_type &&containerArg,
		       encoder_init_type &&encoder_initArg)
		: container(std::move(containerArg)),
		  encoder_init(std::move(encoder_initArg))
	{
	}

	//! Destructor
	~encoderImplObj() {}

	//! Construct the beginning iterator.
	inputrefiterator<char> begin() const override
	{
		// Figure out what iterators that container produces.

		typedef decltype(std::begin(container)) iterator_t;

		// Figure out whate encoder the initializer returns.
		extern std::vector<char> &unused() __attribute__((unused));
		typedef decltype(encoder_init(unused())) encoder_t;

		// Then, construct the iterator.
		return ref<encoderbaseImplObj<iterator_t, encoder_t> >
			::create(std::begin(container),
				 std::end(container),
				 encoder_init,
				 const_ref<obj>(this));
	}
};

//! Helper function for constructing a MIME encoder object.

//! \internal
//!
template<typename container_type,
	 typename encoder_init_type>
encoderbase make_encoder(//! Contents of the MIME section
			 container_type &&container,

			 //! Encoder initializer
			 encoder_init_type &&encoder_init)
{
	return ref<encoderImplObj<typename std::remove_reference<container_type>
				  ::type, encoder_init_type> >
		::create(std::forward<container_type>(container),
			 std::move(encoder_init));
}

//! Create a base64 or a quoted-printable MIME encoder object.

//! \internal
//!
template<typename encoder_t, typename container_type>
encoderbase create_mime_encoded_section(//! Container
					container_type &&container,

					//! Whether to use CRLF newline sequence
					bool usercrlf,

					//! Maximum line size
					size_t maxlinesize)
{
	typedef std::back_insert_iterator< std::vector<char> > insert_iter_t;

	return make_encoder
		(std::forward<container_type>(container),
		 [usercrlf, maxlinesize]
		 (std::vector<char> &buffer)
		 {
			 return encoder_t(insert_iter_t(buffer),
					  maxlinesize, usercrlf);
		 });
}

//! Create a base64-encoded MIME section

template<typename container_type>
inline encoderbase create_base64_encoder(container_type &&container,
					 bool usecrlf=false,
					 size_t maxlinesize=76)
{
	typedef std::back_insert_iterator< std::vector<char> > insert_iter_t;

	return create_mime_encoded_section< base64<>::encoder<insert_iter_t> >
		(std::forward<container_type>(container),
		 usecrlf, maxlinesize);
}

//! Create a quoted-pritnable MIME section

template<typename container_type>
inline encoderbase create_qp_encoder(container_type &&container,
				     bool usecrlf=false,
				     size_t maxlinesize=76)
{
	typedef std::back_insert_iterator< std::vector<char> > insert_iter_t;

	return create_mime_encoded_section< qp_encoder<insert_iter_t> >
		(std::forward<container_type>(container),
		 usecrlf, maxlinesize);
}

//! Create a 7bit or an 8bit MIME section.

template<typename container_type>
encoderbase create_plain_encoder(container_type &&container)
{
	typedef std::back_insert_iterator< std::vector<char> > insert_iter_t;

	return make_encoder
		(std::forward<container_type>(container),
		 []
		 (std::vector<char> &buffer)
		 {
			 return insert_iter_t(buffer);
		 });
}

//! Create a MIME section, choosing the most appropriate encoding type.

//! Returns a pair of the created MIME section, and a string naming the
//! encoding type.

template<typename container_type>
std::pair<encoderbase, const char *>
create_encoder(container_type && container,
	       bool no8bit=false,
	       bool usecrlf=false,
	       size_t maxlinesize=76)
{
	std::streamsize all_chars=0;
	std::streamsize bit8_chars=0;

	size_t maxwidth=0;
	size_t linewidth=0;
	char prevchar=0;

	for (unsigned char c: std::forward<container_type>(container))
	{
		++all_chars;

		if ((c < ' ' && (!usecrlf || c != '\r')
		     && c != '\n' && c != '\t') || c > 0x7F)
			++bit8_chars;

		prevchar=c;

		if ((!usecrlf || prevchar == '\r') && c == '\n')
			linewidth=0;
		else
			if (++linewidth > maxwidth)
				maxwidth=linewidth;
	}

	// quoted-printable or base64-encoding is required when
	//
	// * there are 8 bit characters, and we don't want any
	//
	// * or if there is a line that exceeds the maximum given line size.

	if ((bit8_chars && no8bit) || (maxwidth > maxlinesize))
		// Estimated size of base64-encoded content: bytes*4/3
		// Estimated size of quoted-printable content: bytes + #bit8*2
		//
		// Choose base64 when bytes*4/3 < bytes + #bit8 * 2
		//
		// bytes*4/3 - bytes < #bit8 * 2
		// bytes / 3 < #bit8 * 2
		// bytes / 6 < #bit8

		return all_chars / 6 + 1 < bit8_chars ?
			std::make_pair(create_base64_encoder
				       (std::forward<container_type>(container),
					usecrlf, maxlinesize),
				       "base64") :
			std::make_pair(create_qp_encoder
				       (std::forward<container_type>(container),
					usecrlf, maxlinesize),
				       "quoted-printable");

	return std::make_pair(create_plain_encoder
			      (std::forward<container_type>(container)),
			      bit8_chars ? "8bit":"7bit");

}

#if 0
{
	{
#endif
	}
}
#endif
