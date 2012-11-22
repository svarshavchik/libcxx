/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_sectiondecoder_H
#define x_mime_sectiondecoder_H

#include <x/namespace.h>
#include <x/mime/newlineiter.H>
#include <x/qp.H>
#include <x/base64.H>
#include <vector>

namespace LIBCXX_NAMESPACE {

	//! Classes and templates for parsing MIME-formatted messages.

	namespace mime {
#if 0
	}
}
#endif

//! Implement a reference-counted MIME content transfer encoding decoder.

//! This is a base class that implements a no-op decoder.
//!
//! \see section_decoder

class section_decoderObj : virtual public obj {

public:
	//! My iterator traits

	typedef std::iterator<std::output_iterator_tag, void,
			      void, void, void> iterator_traits;

	//! Constructor
	section_decoderObj();

	//! Destructor
	~section_decoderObj() noexcept;

	//! Iterator operator

	section_decoderObj &operator*() { return *this; }

	//! Iterator operator

	section_decoderObj &operator++() { return *this; }

	//! Iterator operator

	section_decoderObj *before_postoper()
	{
		return this;
	}

	//! Iterator operator

	virtual void operator=(int c)=0;
};

//! Subclass that implements the appropriate decoding.

template<typename output_iter_type>
class section_decoderImplObj : public section_decoderObj {

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
	
//! The base class implements a create() that provides the appropriate decoding.

class section_decoderBase {

public:

	//! \internal

	static bool is_quoted_printable(const std::string &te);

	//! \internal
	static bool is_base64(const std::string &te);

	//! The factory class with the create() method.

	template<typename ref_type> class objfactory {

	public:
		//! The custom create() function

		template<typename iter_type>
		static
		ref<section_decoderObj> create(//! Content-Transfer-Encoding
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
		

//! A reference-counter output iterator that decodes a transfer encoding.

//! This is an output iterator suitable to be used after
//! \ref bodystart_iter "bodystart_iter" emits a \c body_start token, until
//! \c eof.
//!
//! \ref sectionBase::create "create()" constructs a
//! \ref refiterator "refiterator" based on the transfer-encoding value,
//! and writes the transfer-decoded stream to the output iterator that's
//! passed to create().
//!
//! This is a template alias for a
//! \ref refiterator "refiterator" to a
//! \ref section_decoderObj "section_decoderObj".

template<typename output_iter>
#ifdef DOXYGEN
class section_decoder {}; // TODO
#else
using section_decoder=refiterator<section_decoderObj, section_decoderBase>;
#endif

//! A nullable pointer reference to a \ref section_decoderObj "section_decoderObj".

//! \see section_decoder

template<typename output_iter>
#ifdef DOXYGEN
class section_decoderptr {}; // TODO
#else
using section_decoderptr=ptriterator<section_decoderObj, section_decoderBase>;
#endif

#if 0
{
	{
#endif
	}
}
#endif
