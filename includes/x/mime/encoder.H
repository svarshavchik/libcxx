/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_encoder_H
#define x_mime_encoder_H

#include <type_traits>
#include <x/mime/encoderbase.H>
#include <x/mime/structured_content_header.H>
#include <x/joiniterator.H>
#include <x/headersimpl.H>
#include <x/vector.H>
#include <x/fditer.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	//! Classes and templates for parsing MIME-formatted messages.

	namespace mime {
#if 0
	}
}
#endif

class encoderObj;
class encoderBase;

//! A MIME encoder

//! This is a reference to a reference-counted object that encodes a MIME
//! section. The referenced object implements a begin() and end() methods
//! that return
//! \ref encoderBase "INSERT_LIBX_NAMESPACE::encoder::base::iterator"s
//! that define an input sequence containing a formatted MIME section.

typedef ref<encoderObj, encoderBase> encoder;

//! A reference to a constant \ref encoder "MIME encoder".

typedef const_ref<encoderObj, encoderBase> const_encoder;

template<typename header_type>
encoder make_message_rfc822_section(const headersimpl<header_type> &,
				    const_encoder &&);

//! Generate input iterators for a sequence containing an encoding MIME object.

//! An encoded MIME object recursively contains other objects that produce
//! their own beginning/ending iterators. Eventually, there's an object
//! that subclasses encoderbaseObj and produces iterators to some real content.

class encoderObj: public encoderbaseObj {

	//! A component of this encoded MIME object.

	//! \internal
	struct section : public const_encoderbase {

	public:
		//! Constructor forwards its argument to the underlying ref

		template<typename Arg>
		section(Arg &&arg) : const_encoderbase(std::forward<Arg>(arg))
		{
		}

		//! Destructor
		~section();

		using const_encoderbase::operator*;
		using const_encoderbase::operator->;

		//! Return the beginning iterator for the referenced subsection

		auto begin() const
			-> decltype( (*(const_encoderbase *)nullptr)->begin());

		//! Return the ending iterator for the referenced subsection
		auto end() const
			-> decltype( (*(const_encoderbase *)nullptr)->end());
	};

	//! Components of this MIME section

	//! \internal

	vector<section> sections;

	//! Underlying ending iterator for the join iterator

	//! \internal
	//!
	//! Override the default constructor that joiniterator uses. refs
	//! don't have a default constructor. For our purposes, constructing
	//! a non-subclassed inputrefiterator will be sufficient to produce
	//! something that carries an ending iterator value.

	struct ending_iterator_factory {

		//! Construct an ending iterator value

		static inputrefiterator<char> ending_iterator();
	};

	//! A join iterator that combines all the components of this object.

	//! \internal
	typedef joiniterator<std::vector<section>::iterator,
			     ending_iterator_factory> joiniterator_t;

	class combinedObj;

	//! Return the beginning join iterator

	//! \internal
	joiniterator_t beg_iter() const LIBCXX_HIDDEN;

public:
	//! Constructor
	encoderObj();

	//! Destructor
	~encoderObj();

	//! Return the beginning iterator for the encoded MIME object.
	inputrefiterator<char> begin() const override;

	//! Return the ending iterator for the encoded MIME object.
	inputrefiterator<char> end() const override;

	//! Add the header portion section of this MIME object.

	//! \internal
	template<typename header_type>
	void add_headers(const headersimpl<header_type> &headers,
			 const headersimpl<header_type> &extra_header);

private:

	//! Boundary marker type
	//! \internal
	enum class boundary_type { initial, intermediate, final };

	//! Add a boundary marker section to this MIME object

	//! \internal
	void add_boundary(//! Type of a boundary
			  boundary_type type,

			  //! Boundary delimiter string
			  const std::string &boundary,

			  //! Newline sequence to use.
			  const char *newline_seq);

	//! Helper class used to allocate a section vector of sufficient size.

	//! \internal
	template<typename iterator_category>
	struct reserve_unless_input_iterator {

		//! Reserve the size of the section vector.
		template<typename iter_type>
		static void reserve(std::vector<section> &vec,
				    iter_type b,
				    iter_type e)
		{
			vec.reserve(2+std::distance(b, e)*2);
		}
	};

public:

	//! Assemble a non-multipart MIME section

	//! \internal
	template<typename header_type>
	static encoder assemble_section(const headersimpl<header_type> &header,
					const encoderbase &section,
					const char *extra_header_name,
					const char *extra_header_value);

	//! Assemble a multipart Content-Type: section

	template<typename header_type>
	static void
	assemble_multipart_content_type(headersimpl<header_type> &headers,
					const std::string &subtype,
					const std::string &boundary,
					size_t maxwidth);

	//! Assemble a multipart MIME section

	//! \internal
	//! The contents are defined by an input sequence defined by a
	//! beginnning and an ending iterator.

	template<typename header_type,
		 typename add_header_functor_type,
		 typename iter_type>
	static encoder
	assemble_multipart_section(//! Functor that adds any header section
				   add_header_functor_type &&add_headers,

				   //! Maximum width of Content-Type header
				   size_t maxwidth,

				   //! Beginning iterator value

				   //! Input sequence of encoded MIME objects
				   //! that make up the multipart MIME object.

				   iter_type beg_iter,

				   //! Ending iterator value

				   //! Input sequence of encoded MIME objects
				   //! that make up the multipart MIME object.
				   iter_type end_iter,

				   //! Use this boundary delimiter string.
				   const std::string &boundary)
	{
		auto e=encoder::create();

		reserve_unless_input_iterator<typename
					      std::iterator_traits<iter_type>
					      ::iterator_category>
			::reserve(*e->sections, beg_iter, end_iter);

		boundary_type b=boundary_type::initial;

		add_headers(e);

		while (beg_iter != end_iter)
		{
			e->add_boundary(b, boundary, header_type::eol_str);
			b=boundary_type::intermediate;
			e->sections->push_back(*beg_iter);
			++beg_iter;
		}
		e->add_boundary(boundary_type::final, boundary,
				header_type::eol_str);

		return e;
	}

	//! Create a new boundary string

	//! \internal
	static std::string new_boundary();

	//! Create a message/rfc822 section

	template<typename header_type>
	encoder make_message_rfc822_section(const headersimpl<header_type> &,
					    const_encoder &&);

};

//! Base class for \ref encoder "MIME encoders"

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::mime::encoder::base.
//! This class defines the iterator type returned by
//! encoder->begin() and encoder->end().

class encoderBase : public ptrrefBase {

public:

	//! The iterator type returned by begin() and end()

	typedef decltype(ptr<encoderObj>()->begin()) iterator;

};

//! Specialization for an input multipart sequence

//! \internal
//! When the sequence of MIME objects that make up this multipart sequence
//! is an input sequence, we can't reserve the size of the vector where the
//! sequence gets copied to, since that would iterate over the sequence.

template<>
struct encoderObj::reserve_unless_input_iterator<std::input_iterator_tag> {

	//! Do not calculate the distance between b and e

	template<typename iter_type>
	static void reserve(std::vector<section> &vec,
			    iter_type b,
			    iter_type e)
	{
	}
};

//! Create a MIME section, encoding its contents, as necessary.

//! The encoding method is selected heuristically.
//!
template<typename header_type, typename container_type>
encoder make_section(//! This section's headers

		     //! make_section() adds the appropriate
		     //! Content-Transfer-Encoding: header (which is why
		     //! this is not a const), then copies the headers and
		     //! the container.

		     const headersimpl<header_type> &headers,

		     //! A container that defines the contents of the MIME sectin

		     //! The container must be copy-constructible, and
		     //! implement begin() and end() methods that
		     //! define the input sequence containing the contents
		     //! of the MIME object.
		     //!
		     //! The input sequence gets read to determine the best
		     //! encoding method, according to the remaining parameters.
		     container_type &&container,

		     //! Whether 8bit transfer encoding is allowed

		     //! If set to true, and the container has 8bit data,
		     //! the encoding method is set to quoted-printable or
		     //! base64.
		     bool no8bit=false,

		     //! Maximum size of each line in the encoded MIME object.

		     //! If the container has lines that exceed this size,
		     //! the encoding method gets set to quoted-printable or
		     //! base64, even if it's not required, in order to produce
		     //! shorter lines.
		     //!
		     //! The newline sequence is obtained from the type of
		     //! the headers parameter.

		     size_t maxlinesize=76)
{
	auto e=create_encoder(std::forward<container_type>(container),
			      no8bit,
			      header_type::usecrlf,
			      maxlinesize);

	return encoderObj::assemble_section(headers, e.first,
					    nullptr, e.second);
}

//! Create a base64-encoded MIME section

//! This is a version of make_section() that dispenses with the heuristics,
//! and uses the base64 content transfer encoding method.

template<typename header_type, typename container_type>
encoder make_base64_section(const headersimpl<header_type> &headers,
			    container_type &&container,
			    size_t maxlinesize=76)
{
	return encoderObj::assemble_section
		(headers,
		 create_base64_encoder(std::forward<container_type>(container),
				       header_type::usecrlf,
				       maxlinesize), nullptr, "base64");
}

//! Create a quoted-printable-encoded MIME section

//! This is a version of make_section() that dispenses with the heuristics,
//! and uses the base64 content transfer encoding method.

template<typename header_type, typename container_type>
encoder make_qp_section(const headersimpl<header_type> &headers,
			container_type &&container,
			size_t maxlinesize=76)
{
	return encoderObj::assemble_section
		(headers,
		 create_qp_encoder(std::forward<container_type>(container),
				   header_type::usecrlf,
				   maxlinesize), nullptr, "quoted-printable");
}

//! Create an unencoded section.

//! This is a version of make_section() that inserts the contents of the
//! container as the MIME object, as is, without adding the
//! Content-Transfer-Encoding header (the headers parameter should have the
//! appropriate header already).

template<typename header_type, typename container_type>
encoder make_plain_section(const headersimpl<header_type> &headers,
			   container_type &&container)
{
	return encoderObj::assemble_section
		(headers,
		 create_plain_encoder(std::forward<container_type>(container)),
		 nullptr, nullptr);
}

//! Create a message/rfc822 section

//! Takes a previously-created encoder of an entire message, and constructs
//! a message/rfc822 entity from it. Adds "Content-Type: message/rfc822"
//! header.

template<typename header_type>
encoder make_message_rfc822_section(const headersimpl<header_type> &headers,
				    const_encoder && encoder)
{
	return encoderObj::assemble_section
		(headers,
		 create_plain_encoder(encoderObj::section(encoder)),
		 structured_content_header::content_type,
		 structured_content_header::message_rfc822);
}

//! Create a message/rfc822 section

//! \overload

template<typename header_type>
encoder make_message_rfc822_section(const headersimpl<header_type> &headers,
				    const const_encoder &encoder)
{
	return make_message_rfc822_section(headers, (const_encoder)encoder);
}

//! Create a multipart MIME section.

//! Takes a sequence of encoded MIME sections, defined by a beginning and an
//! ending iterator, and creates an object that encodes a multipart MIME
//! section.
//!
//! The beginning and ending iterator define a sequence containing objects
//! that were returned by previous calls to make_section() or
//! make_multipart_section().

template<typename header_type, typename iter_type>
encoder make_multipart_section(//! Other multipart section headers
			       const headersimpl<header_type> &headers,

			       //! Subtype ("mixed", "alternative", etc...)

			       const std::string &subtype,

			       //! Beginning iterator value
			       iter_type beg_iter,

			       //! Ending iterator value
			       iter_type end_iter,

			       //! Maximum width of the Content-Type: header

			       //! ... before it wraps. Set to 0 to disable,
			       //! so it always gets emitted as 1 line
			       size_t maxwidth=76)
{
	headersimpl<header_type> content_type_header;
	std::string boundary=encoderObj::new_boundary();

	encoderObj::assemble_multipart_content_type(content_type_header,
						    subtype,
						    boundary,
						    maxwidth);

	return encoderObj::assemble_multipart_section<header_type>
		([&content_type_header, &headers]
		 (const encoder &e)
		 {
			 e->add_headers(headers, content_type_header);
		 },
		 maxwidth,
		 beg_iter,
		 end_iter,
		 boundary
		 );
}

//! Contents of a MIME section come from a file

//! Suitable to be passed as a container parameter to make_section(), or
//! its variations.
//!
//! The file gets opened by the iterator that gets returned by
//! the \c INSERT_LIBX_NAMESPACE::mime::encoder->begin(), so its
//! expected that the file's contents will always be the same, otherwise lots
//! of stuff is going to break.

class from_file_container {

	//! Saved filename
	const std::string filename;

public:
	//! Constructor
	from_file_container(const std::string &filenameArg);

	//! Destructor
	~from_file_container();

	//! Open the file
	fdinputiter begin() const;

	//! Return the ending iterator value
	static fdinputiter end();
};

//! Contents of a MIME section come from a file descriptor

//! Suitable to be passed as a container parameter to make_section(), or
//! its variations.
//!
//! If the file is not a regular file, its contents get copied into a temporary
//! file.

class from_fd_container {

	//! Saved filename
	const fd filedesc;

	//! Copy a non-regular file descriptor to a temporary file.

	//! \internal

	static fd check_filedesc(const fd &filedescArg) LIBCXX_HIDDEN;
public:
	//! Constructor
	from_fd_container(const fd &filedescArg);

	//! Destructor
	~from_fd_container();

	//! Open the file
	fdinputiter begin() const;

	//! Return the ending iterator value
	static fdinputiter end();
};

//! Contents of a MIME section come from a file

//! Pass from_file(filename) to make_section(), or alternatives.
//! \see from_file_container.

inline from_file_container from_file(const std::string &filename)
{
	return from_file_container(filename);
}

//! Contents of a MIME section come from a file descriptor

//! Pass from_file(filename) to make_section(), or alternatives.
//! \see from_file_container.

inline from_fd_container from_file(const fd &filedesc)
{
	return from_fd_container(filedesc);
}

//! Determine the MIME type of a file.

//! Searches for the file extension in /etc/mime.types.
//! If the type cannot be determined, application/octet-stream gets returned.

std::string filetype(//! Filename
		     const std::string &filename,

		     //! If set to true, and extension not found in /etc/mime.types, use libmagic to figure it out.
		     bool check_contents=false);

//! Determine the MIME type of a file.

//! Uses libmagic

std::string filetype(//! File handle
		     const fd &filedesc);

//! Determine the character set of a file.

std::string filecharset(const std::string &filename);

//! Determine the character set of a file.

std::string filecharset(const fd &filedesc);

#if 0
{
	{
#endif
	}
}
#endif
