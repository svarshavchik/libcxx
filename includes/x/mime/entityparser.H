/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_mime_entityparser_H
#define x_mime_entityparser_H

#include <x/namespace.h>
#include <x/mime/sectionprocessor.H>
#include <x/mime/newlineiter.H>
#include <x/mime/headeriter.H>
#include <x/mime/structured_content_header.H>

namespace LIBCXX_NAMESPACE {

	//! Classes and templates for parsing MIME-formatted messages.

	namespace mime {
#if 0
	}
}
#endif


//! Create an output iterator that parses a MIME entity

//! Returns an output iterator that parses a MIME entity.
//! make_entity_parser() constructs a new
//! \ref bodystart_iter "bodystart_iter" and
//! \ref header_iter "header_iter", then uses make_section_processor() to
//! construct an output iterator that parses a MIME entity.
//!
//! The first parameter is an output iterator, the header iterator.
//! The second parameter is a lambda/functor.
//!
//! make_entity_parser() returns an output iterator that iterates over a
//! sequence of \c int values that, for a top-level MIME entity, are iterated
//! over by \ref newline_iter "newline_iter" iterator, followed by an
//! \c eof value.
//!
//! The header iterator iterates over the header portion of the MIME entity,
//! followed by an \c eof value. After the header iterator iterates over the
//! \c eof value, the lambda/functor that's passed as a second parameter gets
//! invoked. The lambda/functor is expected to return an output iterator,
//! which iterates over the body portion of the MIME entity, followed by an
//! \c eof value.
//!
//! The parameter is a reference to a \ref sectioninfo "section info object".
//! The iterator returned by make_entity_parser() automatically updates
//! the section object, as it parser the MIME entity. The values in the
//! section object get finalized only after the iterator returned by
//! make_entity_parser() iterates over an eof value.

template<typename header_iterator_functor,
	 typename start_body_functor,
	 typename sectioninfo_type>
auto make_entity_parser(header_iterator_functor &&header_iterator,
			start_body_functor &&start_body,
			const sectioninfo_type &info)
	-> decltype( bodystart_iter<header_iter<
		     decltype(make_section_processor
			      (std::forward<header_iterator_functor>
			       (header_iterator),
			       std::forward<start_body_functor>
			       (start_body), info)) >>
		     ::create
		     (header_iter<
		      decltype(make_section_processor
			       (std::forward<header_iterator_functor>
				(header_iterator),
				std::forward<start_body_functor>
				(start_body), info)) >
		      ::create(make_section_processor
			       (std::forward<header_iterator_functor>
				(header_iterator),
				std::forward<start_body_functor>
				(start_body), info))))
{
	return bodystart_iter<header_iter<
				      decltype(make_section_processor
				       (std::forward
						<header_iterator_functor>
						(header_iterator),
						std::forward<start_body_functor>
						(start_body), info)) >>
		::create
		(header_iter<
		 decltype(make_section_processor
			  (std::forward<header_iterator_functor>
			   (header_iterator),
			   std::forward<start_body_functor>
			   (start_body), info)) >
		 ::create(make_section_processor
			  (std::forward<header_iterator_functor>
			   (header_iterator),
			   std::forward<start_body_functor>
			   (start_body), info)));
}

//! Construct an output iterator for a MIME document

//! Returns an output iterator that iterates over a char sequence comprising
//! a MIME document. After iterating over the entire char sequence, invoke
//! get()->eof() on the returned iterator to finish the iteration.
//!
//! The first parameter is a "section processor factory", which is a functor or
//! a lambda that returns an iterator as described below. The second optional
//! parameter specifies the MIME document's newline sequence. Setting it to
//! \c true recognizes CRLF sequence as a newline sequence. The default
//! value of \c false recognizes LF as a newline sequence.
//!
//! The third parameter defaults to true, signifying that this MIME document
//! is a \c message/rfc822 entity. This allows sharing the functor/lambda,
//! that's passed as a first parameter, with make_multipart_parser().
//!
//! The section processor factory functor/lambda gets called. It is expected to
//! return an output iterator that parses an output sequence of \c int values,
//! with marked newline sequences, as produced by
//! \ref newline_iter "newline_iter".
//! A suitable output iterator can be constructed by invoking
//! make_entity_parser(). The functor/lambda is generally expected to make
//! use it, and it takes two parameters.
//!
//! The first parameter is a \ref sectioninfo "sectioninfo" object constructed
//! for the top level \c MIME entity of the iterated document, and should be
//! passed to make_entity_parser(), as is. The second parameter to the
//! functor/lambda is the \c message/rfc822 entity flag that's passed as the
//! third parameter make_document_entity_parser().
//!
//! The default value of \c true for this flag should indicate to the
//! functor/lambda that a "Mime-Version: 1.0" header must be present in order
//! to recognize a \c MIME document. Otherwise, a \c text/plain document
//! should be presumed, and the body iterator factor parameter to
//! make_document_entity_parser() should return a suitable parameter,
//! accordingly.
//!
//! The flag can be \c false when this is called to parse a non-top level
//! \c MIME entity part, in a document that's been previously parsed, with the
//! assistance of a previously created \ref sectioninfo "sectioninfo".

template<typename section_processor_type,
	 typename sectioninfo_type=sectioninfo>
auto make_document_entity_parser(section_processor_type &&processor,
				 bool crlfArg=false,
				 bool is_message_rfc822=true,
				 const sectioninfo_type &info=
				 sectioninfo_type::create())
	-> decltype(newline_iter
		    <decltype(processor(info, is_message_rfc822))>
		    ::create(processor(info, is_message_rfc822), crlfArg))
{
	return newline_iter
		<decltype(processor(info, is_message_rfc822))>
		::create(processor(info, is_message_rfc822), crlfArg);
}

//! Construct an output iterator for a \c message/rfc822 MIME section.

//! This is typically called to construct a body iterator for a
//! \c message/rfc822 MIME section, by the body iterator factory parameter
//! that's passed to make_entity_parser(). It returns an output iterator
//! over \c int values that's convertible to a
//! \ref outputrefiterator "outputrefiterator<int>".
//!
//! The first parameter is a section processor factory lambda or a functor,
//! that's described by make_document_entity_parser()'s documentation.
//!
//! The second parameter is a \ref sectioninfo "sectioninfo" object that
//! make_entity_parser() passed to the parent MIME entity's
//! section processor factory that's responsible for this call
//! to make_message_rfc822_parser().
//!
//! The primary differences between make_message_rfc822_parser() and
//! make_document_entity_parser().
//!
//! - make_message_rfc822_parser() returns an output iterator of int values,
//! instead of \c char values, this is expected to be the return value
//! from the parent entity's section processor factory's call.
//!
//! - The section processor factory lambda/functor parameter to this call
//! gets invoked with a \ref sectioninfo "sectioninfo" for the
//! \c message/rfc822 MIME subpart, and the \c message/rfc822 parameter flag
//! of \c true.

template<typename section_processor_type,
	 typename sectioninfo_type>
auto make_message_rfc822_parser(section_processor_type && section_processor,
				const sectioninfo_type &info)
	-> decltype(section_processor(info->create_subsection(), true))
{
	auto child=info->create_subsection();

	auto new_processor=section_processor(child, true);
	info->children.push_back(child);
	return new_processor;
}

//! Helper class for make_multipart_parser()

//! \internal
//! This is, essentially, an internal lambda functor used by
//! make_multipart_parser(). An explicit template class makes it possible
//! to decltype make_multipart_parser()'s return value.

template<typename section_processor_type,
	 typename sectioninfo_type>
class make_multipart_parser_struct {

	//! The captured section processor factory functor/lambda

	section_processor_type processor;

	//! The captured multipart part's sectioninfo

	sectioninfo_type parent;

public:
	//! Constructor

	template<typename processorArgType>
	make_multipart_parser_struct(processorArgType &&processorArg,
				     const sectioninfo_type &parentArg)
		: processor(std::forward<processorArgType>(processorArg)),
		  parent(parentArg)
	{
	}

	//! Destructor
	~make_multipart_parser_struct()
	{
	}

	//! Factory for the next MIME subpart's iterator

	decltype(processor(parent, false)) operator()() const
	{
		auto child=parent->create_subsection();

		auto new_processor=processor(child, false);

		parent->children.push_back(child);

		return new_processor;
	}
};

//! Helper class for inferring the type of make_multipart_parser() return value

//! \internal

template<typename functor_type,
	 typename sectioninfo_type>
class make_multipart_parser_ret_type {

public:

	//! Decayed functor type
	typedef typename std::decay<functor_type>::type decayed_type;

	//! Output iterator factory for make_multipart_processor()
	typedef make_multipart_parser_struct<decayed_type,
					     sectioninfo_type> struct_type;

	//! Supply the document argument to make_multipart_processor()
	static functor_type &&dummy_parameter()
		__attribute__((unused));

	//! Supply the sectioninfo argument to make_multipart_processor()

	static sectioninfo_type &dummy_sectioninfo()
		__attribute__((unused));

	//! Return type from make_multipart_processor.

	typedef decltype
	(make_multipart_processor("",
				  struct_type(std::forward<functor_type>
					      (dummy_parameter()),
					      dummy_sectioninfo()))
	 ) ret_type_t;
};

//! Construct an output iterator for a \c multipart MIME section.

//! This is typically called to construct a body iterator for a
//! \c multipart MIME section, by the body iterator factory parameter
//! that's passed to make_entity_parser(). It returns an output iterator
//! over \c int values that's convertible to a
//! \ref outputrefiterator "outputrefiterator<int>".
//!
//! The first parameter is the boundary parameter value from the
//! \c Content-Type: header.
//!
//! The second parameter is a section processor factory lambda or a functor.
//! The output iterator returned by make_multipart_processor() iterates over
//! the entire \c multipart MIME section, and invokes the factory lambda
//! functor once for each MIME subpart of the \c multipart section.
//!
//! The section processor factory lambda is described by
//! make_document_entity_parser().
//!
//! The third parameter is a \ref sectioninfo "sectioninfo" object that
//! make_entity_parser() passed to the parent \c multipart MIME entity's
//! section processor factory that's responsible for this call
//! to make_multipart_parser().
//!
//! The primary differences between make_multipart_parser() and
//! make_document_entity_parser().
//!
//! - make_multipart_parser() returns an output iterator of int values,
//! instead of \c char values, this is expected to be the return value
//! from the parent entity's section processor factory's call.
//!
//! - The section processor factory lambda/functor parameter to this call
//! gets invoked for each \c MIME subpart, and
//! gets invoked with a \ref sectioninfo "sectioninfo" for each subpart.
//! The \c message/rfc822 parameter flag is \c false.

template<typename section_processor_type,
	 typename sectioninfo_type>
auto make_multipart_parser(const std::string &boundary,
			   section_processor_type &&processor,
			   const sectioninfo_type &parent)
	-> typename make_multipart_parser_ret_type<section_processor_type,
						   sectioninfo_type>
	::ret_type_t
{
	return make_multipart_processor(boundary,
					make_multipart_parser_struct
					<typename std::decay
					<section_processor_type>::type,
					sectioninfo_type>
					(std::forward
					 <section_processor_type>
					 (processor), parent));
}

//! Construct an output iterator for a MIME section.

//! This is typically called to construct a body iterator for a MIME
//! section, by the body iterator factory parameter
//! that's passed to make_entity_parser(). It returns an output iterator
//! over \c int values that's convertible to a
//! \ref outputrefiterator "outputrefiterator<int>".
//!
//! The first parameter is the parsed \c Content-Type: header.
//! make_parser() examines the header. If the \c Content-Type: is
//! \c message/rfc822, or \c multipart, make_parser() invokes
//! make_message_rfc822_parser(), if it is \c multipart make_parser()
//! invokes make_multipart_parser().
//!
//! The second parameter is a \ref sectioninfo "sectioninfo" object that
//! make_entity_parser() passed to section processor factory for
//! the MIME entity that's calling make_parser().
//!
//! The third parameter is a section processor factory lambda or a functor.
//!
//! The second and third parameters get passed to
//! make_message_rfc822_parser() or make_multipart_parser().
//!
//! The fourth parameter is a body processor factory lambda or a functor.
//! If the MIME entity is not
//! \c message/rfc822, or \c multipart, the body processor factory gets
//! invoked. It is expected to return an output iterator that's convertible
//! to \ref outputrefiterator "outputrefiterator<int>".
//! The \ref sectioninfo "sectioninfo" parameter gets forwarded to the
//! body processor factory.

template<typename section_processor_type,
	 typename body_processor_type,
	 typename sectioninfo_type>
outputrefiterator<int> make_parser(const structured_content_header &hdr,
				   const sectioninfo_type &info,
				   section_processor_type && section_processor,
				   body_processor_type && body_processor)
{
	if (hdr.is_message())
		return make_message_rfc822_parser
			(std::forward<section_processor_type>
			 (section_processor), info);

	if (hdr.is_multipart())
		return make_multipart_parser
			(hdr.boundary(),
			 std::forward<section_processor_type>
			 (section_processor), info);

	return body_processor(info);
}

#if 0
{
	{
#endif
	}
}
#endif
