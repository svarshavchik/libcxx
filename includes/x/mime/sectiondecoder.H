/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_sectiondecoder_H
#define x_mime_sectiondecoder_H

#include <x/namespace.h>
#include <x/mime/newlineiter.H>
#include <x/mime/structured_content_header.H>
#include <x/qp.H>
#include <x/base64.H>
#include <x/iconviofilter.H>
#include <x/fdbaseobj.H>
#include <x/refiterator.H>
#include <vector>

namespace LIBCXX_NAMESPACE {

	//! Classes and templates for parsing MIME-formatted messages.

	namespace mime {
#if 0
	}
}
#endif

//! Subclass that implements the appropriate decoding.

template<typename output_iter_type>
class section_decoderImplObj : public outputrefiteratorObj<int> {

public:

	//! The output iterator instance
	output_iter_type iter;

	//! Constructor
	section_decoderImplObj(const output_iter_type &iterArg)
		: iter(iterArg)
	{
	}

	//! Move constructor
	section_decoderImplObj(output_iter_type && iterArg)
		: iter(std::move(iterArg))
	{
	}

	//! Implement the decoding.
	void operator=(int c) override
	{
		if (nontoken(c))
			*iter++=(char)c;
	}
};

//! Subclass that transcodes decoded MIME section to the requested character set.

template<typename output_iter_type>
class section_transcoderObj : virtual public obj {

	//! Input buffer
	std::vector<char> inbuf;

	//! Output buffer
	std::vector<char> outbuf;

	//! iconv transcoder
	iconviofilter filter;

public:

	//! Iterator traits.
	typedef std::iterator<std::output_iterator_tag, void,
			      void, void, void> iterator_traits;

	//! The output iterator instance
	output_iter_type iter;

	//! Constructor
	section_transcoderObj(const std::string &from,
			      const std::string &to,
			      const output_iter_type &iterArg)
		: filter(from, to), iter(iterArg)
	{
		inbuf.resize(fdbaseObj::get_buffer_size());
		outbuf.resize(fdbaseObj::get_buffer_size());
		filter.avail_in=0;
	}

	//! Move constructor
	section_transcoderObj(const std::string &from,
			      const std::string &to,
			      output_iter_type && iterArg)
		: filter(from, to), iter(std::move(iterArg))
	{
		inbuf.resize(fdbaseObj::get_buffer_size());
		outbuf.resize(fdbaseObj::get_buffer_size());
		filter.avail_in=0;
	}

	//! Iterator operator

	section_transcoderObj<output_iter_type> &operator*() { return *this; }

	//! Iterator operator

	section_transcoderObj<output_iter_type> &operator++() { return *this; }

	//! Iterator operator

	section_transcoderObj<output_iter_type> *before_postoper()
	{
		return this;
	}

	//! Implement the decoding.
	void operator=(char c)
	{
		while (filter.avail_in == inbuf.size())
			flush();

		inbuf[filter.avail_in++]=c;
	}

	//! Reached and of input, flush anything still buffered.

	void eof()
	{
		// Flush anything on the input size
		while (filter.avail_in)
			flush();

		// Keep flushing until no more output shows up.
		do
		{
			flush();
		} while (filter.next_out > &outbuf[0]);
	}
private:

	//! Call the iconv filter

	void flush()
	{
		filter.next_in=&inbuf[0];
		filter.next_out=&outbuf[0];
		filter.avail_out=outbuf.size();
		try {
			filter.filter();
		} catch (const exception &e)
		{
			std::ostringstream o;

			o << "[" << e << "]";
			std::string s=o.str();

			for (char c:s)
				*iter++=c;
			return;
		}

		std::copy(filter.next_in, filter.next_in+filter.avail_in,
			  &inbuf[0]);

		for (auto c=&outbuf[0]; c<filter.next_out; ++c)
			*iter++=*c;
	}
};

class section_decoderBase;

//! A reference-counter output iterator that decodes a transfer encoding.

//! This is an output iterator suitable to be used after
//! \ref bodystart_iter "bodystart_iter" emits a \c body_start token, until
//! \c eof.
//!
//! \ref section_decoderBase::objfactory::create "create()" constructs a
//! \ref refiterator "refiterator" based on the transfer-encoding value,
//! and writes the transfer-decoded stream to the output iterator that's
//! passed to create().
//!
//! Optionally, create() constructs a decoder that automatically transcodes
//! the decoded section to a requested character set.

typedef refiterator<outputrefiteratorObj<int>, section_decoderBase> section_decoder;

//! A nullable pointer reference to a \ref outputrefiteratorObj "outputrefiteratorObj<int>".

//! \see section_decoder

typedef ptriterator<outputrefiteratorObj<int>, section_decoderBase> section_decoderptr;

template<typename transcoder_iter_type>
class section_decodertranscoderObj;


//! The base class implements a create() that provides the appropriate decoding.

class section_decoderBase {

public:

	//! \internal

	static bool is_quoted_printable(const std::string &te);

	//! \internal
	static bool is_base64(const std::string &te);

	//! The factory class with custom create() methods.

	template<typename ref_type> class objfactory {

	public:

		//! Create a non-transcoding decoder

		template<typename iter_type>
		static ref_type create(//! Content-Transfer-Encoding
				       const std::string &te,

				       //! Output iterator over chars.
				       iter_type &&iter)
		{
			return int_create(te, std::forward<iter_type>(iter));
		}

		//! Create a transcoding decoder.

		template<typename iter_type>
		static ref_type create(//! Content-Transfer-Encoding
				       const std::string &te,

				       //! Output iterator over chars.
				       iter_type &&iter,

				       //! From character set
				       const std::string &from,

				       //! To character set
				       const std::string &to)
		{
			typedef typename std::decay<iter_type>::type iter_t;

			// First, create the iterator that does the transcoding

			auto transcoder=make_refiterator
				(ref<section_transcoderObj<iter_t>>
				 ::create(from, to,
					  std::forward<iter_type>(iter)));

			// Now, create the decoder that feeds the transcoder,
			// then create the wrapper that forwards eof to the
			// transcoder, so that it can flush its output.

			return ref<section_decodertranscoderObj
				   <decltype(transcoder)> >
				::create(int_create(te, transcoder),
					 transcoder);
		}

		//! Create a non-transcoding decoder, get transfer encoding from headers
		template<typename iter_type>
		static ref_type create(//! Headers
				       const headersbase &headers,

				       //! Output iterator over chars.
				       iter_type &&iter)
		{
			return create(structured_content_header
				      (headers,
				       structured_content_header
				       ::content_transfer_encoding).value,
				      std::forward<iter_type>(iter));
		}
		
		//! Create a transcoding decoder, get transfer encoding and from charset from headers
		template<typename iter_type>
		static ref_type create(//! Headers
				       const headersbase &headers,

				       //! Default from character set
				       const std::string &from,

				       //! to character set
				       const std::string &to,

				       //! Output iterator over chars.
				       iter_type &&iter)
		{
			return create(structured_content_header
				      (headers,
				       structured_content_header
				       ::content_transfer_encoding).value,
				      std::forward<iter_type>(iter),

				      structured_content_header
				      (headers,
				       structured_content_header
				       ::content_type).charset(from),
				      to);
		}

	private:
		//! Create a non-transcoding decoder.

		template<typename iter_type>
		static outputrefiterator<int>
		int_create(//! Content-Transfer-Encoding
			   const std::string &te,

			   //! Output iterator over chars.
			   iter_type &&iter)
		{
			typedef typename std::decay<iter_type>::type iter_t;

			if (is_quoted_printable(te))
				return ref<section_decoderImplObj<qp_decoder
								  <iter_t> >
					   >::create(std::forward<iter_type>
						     (iter));

			if (is_base64(te))
				return ref<section_decoderImplObj<base64<>
								  ::decoder
								  <iter_t> >
					   >::create(std::forward<iter_type>
						     (iter));

			return ref<section_decoderImplObj<iter_t> >
				::create(std::forward<iter_type>(iter));
		}
	};
};

//! Glue a decoder together with a transcoder

//! Output iterator writes to the transfer decoder. Intercepts eof, then
//! flush the iconv transcoder's input buffer.
//! \internal

template<typename transcoder_iter_type>
class section_decodertranscoderObj : public outputrefiteratorObj<int> {

public:
	//! The decoder
	ref<outputrefiteratorObj<int>> decoder;

	//! The transcoder
	transcoder_iter_type transcoder;

	//! Constructor
	section_decodertranscoderObj(ref<outputrefiteratorObj<int>> &&decoderArg,
				     const transcoder_iter_type &transcoderArg)
		: decoder(std::move(decoderArg)),
		  transcoder(std::move(transcoderArg))
	{
	}

	~section_decodertranscoderObj()
	{
        }

	//! Iterator operator

	void operator=(int c)
	{
		*decoder=c;
		if (c == eof)
			transcoder.get()->eof();
        }
};

#if 0
{
	{
#endif
	}
}
#endif
