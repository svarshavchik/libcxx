/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_sectionprocessor_H
#define x_mime_sectionprocessor_H

#include <x/namespace.h>
#include <x/mime/tokens.H>
#include <x/mime/sectioniter.H>
#include <x/mime/sectioninfo.H>
#include <iterator>
#include <type_traits>

namespace LIBCXX_NAMESPACE {

	//! Classes and templates for parsing MIME-formatted messages.

	namespace mime {
#if 0
	}
}
#endif

//! An output iterator that forwards the header and the body portion.

//! \see make_section_processor()
//!
//! The output iterator constructed by make_section_processor() expects to
//! iterate over an output sequence from
//! \ref bodystart_iter "bodystart_iter" and
//! \ref header_iter "header_iter".

template<typename header_iterator_functor,
	 typename start_body_functor>
class section_processorObj : public outputrefiteratorObj<int> {

public:
	//! Iterator over the header
	header_iterator_functor header_iter;

	//! Invoked when start_body is received
	start_body_functor start_body;

	//! Where we keep track of how much we chewed through
	sectioninfo this_section;

private:
	//! Previous character
	int prev_token;

	//! What start_body() returns

	typedef decltype( (*(start_body_functor *)nullptr)() ) body_iter_t;

	//! Container for the iterator returned by start_body().

	class body_iterObj : virtual public obj {

	public:
		//! The body iterator

		body_iter_t iter;

		//! Constructor

		body_iterObj(body_iter_t &&iterArg)
			: iter(std::move(iterArg))
		{
		}

		~body_iterObj() noexcept
		{
		}
	};

	//! The iterator returned by start_body()

	ptr<body_iterObj> body_iter;

public:

	//! After start_body() gets invoked, the iterator it returns

	//! The iterator goes out of scope and gets destroyed together with
	//! this object.

	body_iter_t *body_iterptr;

private:
	//! This template instance type

	typedef section_processorObj<header_iterator_functor,
				     start_body_functor> this_t;

	//! Current handler

	void (this_t::*handler)(int);

	//! Forwarding output sequence to the body iterator
	void in_body(int token)
	{
		if (token == newline_end)
			++this_section->body_line_cnt;
		if (nontoken(token))
			++this_section->body_char_cnt;

		*(*body_iterptr)=token;
		++*body_iterptr;
	}

	//! Forwarding output sequence to the header iterator
	void in_headers(int token)
	{
		if (token == newline_end)
			++this_section->header_line_cnt;
		if (nontoken(token))
			++this_section->header_char_cnt;

		if (token == eof) // EOF in headers?
		{
			in_headers(body_start);
			if (prev_token != newline_end)
			{
				operator=(newline_start);
				operator=(newline_end);
			}
			operator=(eof);
			return;
		}

		if (token == body_start)
		{
			*header_iter=eof;
			++header_iter;

			auto body_iterval=
				ref<body_iterObj>::create(start_body());

			body_iter=body_iterval;
			body_iterptr= &body_iterval->iter;
			handler= &this_t::in_body;
			return;
		}
		*header_iter=token;
		++header_iter;
	}
public:

	//! Constructor
	template<typename hType, typename sType>
	section_processorObj(hType && header_iterArg,
			     sType && start_bodyArg,
			     const sectioninfo &this_sectionArg)
		: header_iter(std::forward<hType>(header_iterArg)),
		  start_body(std::forward<sType>(start_bodyArg)),
		  this_section(this_sectionArg),
		  prev_token(0),
		  body_iterptr(nullptr),
		  handler(&this_t::in_headers)
	{
	}

	//! Destructor

	~section_processorObj() noexcept
	{
	}

	//! Iterator operator

	void operator=(int c) override final
	{
		if (c == newline_end)
		{
			if (prev_token == newline_start)
				this_section->no_trailing_newline=true;
		}

		(this->*handler)(c);
		prev_token=c;
	}
};

//! Create an output iterator over a MIME section

//! make_section_processor() takes two parameters and returns a reference-
//! counted object that's suitable to be adopted by
//! refiterator<int>.
//!
//! See make_entity_parser() for the description of the parameters to this
//! function.

template<typename header_iterator_functor,
	 typename start_body_functor>
auto make_section_processor(header_iterator_functor &&header_iterator,
			    start_body_functor &&start_body,
			    const sectioninfo &info)
	-> refobjiterator<ref<
	section_processorObj<
		typename std::decay<header_iterator_functor>::type,
		typename std::decay<start_body_functor>::type>>>
{
	return refobjiterator<ref<
		section_processorObj<
			typename std::decay<header_iterator_functor>::type,
			typename std::decay<start_body_functor>::type>>>
		::create(std::forward<header_iterator_functor>
			 (header_iterator),
			 std::forward<start_body_functor>(start_body),
			 info);
}

//! Iterator object over a multipart MIME section

//! \see make_multipart_processor()

template<typename functor_type,
	 typename discard_functor_type>
class multipart_section_processorObj : public outputrefiteratorObj<int> {

	//! The section functor

	functor_type functor;

	//! The discard functor

	discard_functor_type discard_functor;

	//! Last token

	int last_token;

	//! What the section functor returns

	typedef decltype( (*(functor_type *)nullptr)() ) iter_t;

	//! A container for the current iterator

	class containerObj : virtual public obj {

	public:
		//! The current iterator
		iter_t iter;

		//! Constructor
		containerObj(iter_t &&iterArg) : iter(std::move(iterArg))
		{
		}

		//! Destructor
		~containerObj() noexcept
		{
		}
	};

	//! Current section's iterator

	ptr<containerObj> container;

public:
	//! A native pointer to the current container

	//! This saves a null() check in operator=()

	iter_t *iterptr;

	//! Constructor
	template<typename functorArg,
		 typename discardArg>
	multipart_section_processorObj(functorArg &&functor_arg,
				       discardArg &&discard_functor_arg)
		: functor(std::forward<functorArg>(functor_arg)),
		  discard_functor(std::forward<discardArg>(discard_functor_arg)
				  ), last_token(0),
		  iterptr(nullptr)
	{
	}

	//! Destructor
	~multipart_section_processorObj() noexcept
	{
	}

	//! Iterator operator

	void operator=(int c) override final
	{
		if (c == section_start)
		{
			auto new_section=
				ref<containerObj>
				::create(functor());
			container=new_section;
			iterptr= &new_section->iter;
			last_token=0;
			return;
		}

		if (c == section_end)
		{
			// If a section did not end in a newline, emit a
			// fake newline sequence.

			if (last_token != newline_end)
			{
				operator=(newline_start);
				operator=(newline_end);
			}

			auto save_container=container;

			container=ptr<containerObj>();
			iterptr=nullptr;

			if (!save_container.null()) // Should always be the case
			{
				auto &ref= *save_container;

				*ref.iter=eof;
				++ref.iter;
				discard_functor(ref.iter);
			}
			return;
		}

		if (iterptr)
		{
			**iterptr=c;
			++*iterptr;
			last_token=c;
		}
	}
};

//! Helper class for make_multipart_processor()

//! Reduce the insanity in make_multipart_processor()'s decltype.

template<typename functor_type,
	 typename discard_functor_type>
class make_multipart_processor_types {

public:

	//! The type of the functor/lambda that returns a new MIME subsection iterator.

	typedef typename std::decay<functor_type>::type functor_type_t;

	//! The type of the discard iterator functor/lambda
	typedef typename std::decay<discard_functor_type
				    >::type discard_functor_type_t;

	//! The type of the multipart_section_processorObj template instance.
	typedef multipart_section_processorObj<functor_type_t,
					       discard_functor_type_t> iter_t;

	//! The type of the reference iterator to the section processor.
	typedef refobjiterator<ref<iter_t>> refiter_t;

	//! What make_multipart_processor() returns.

	typedef decltype(section_iter<refiter_t>::create
			 (*(refiter_t *)nullptr, "")) type;

};

//! Construct an output iterator over a multipart MIME section body

//! make_multipart_processor() returns an iterator over the contents of a
//! multipart MIME section. The iterator expects to iterate over the output
//! sequence that make_entity_parser() (or equivalent) already
//! iterates (int values that include \c newline_start and \c newline_end,
//! at a minimum), followed by an \c eof value.
//!
//! The first parameter is the multipart boundary delimiter string.
//! make_multipart_processor() constructs a \ref section_iter "section_iter"
//! to insert \c section_start and \c section_end values into the output
//! sequence, then invokes a functor/lambda that's passed as the 2nd
//! argument to make_multipart_processor(). The 2nd argument is a functor/lambda
//! that returns an output iterator that gets iterated over each
//! part of this multipart MIME section, followed by an eof value.
//! If the part did not end in a newline_sequence, the output iterator will
//! iterate over a trailing newline_start/newline_end token sequence, without
//! an actual newline octet(s).
//!
//! It's expected that the functor/lambda will invoke
//! make_entity_parser(), which returns an iterator that's suitable
//! for this exact purpose.
//!
//! Each part of a multipart MIME section results in the functor/lambda
//! getting invoked. After the iterator it returns iterates over an \c eof
//! value, the functor/lambda specified by the 3rd argument, the
//! discard functor. The discard functor takes one parameter, the final value
//! of the iterator that iterated over the MIME subsection.
//!
//! In all cases, make_multipart_processor() releases the reference on the
//! iterator, that gets returned by the 2nd functor/lambda, after it iterates
//! over an eof value. If it's not in any other scope, it goes out of scope
//! and gets destroyed. Since exceptions should not be thrown from destructors,
//! the discard functor gets used to perform any cleanup-type activities that
//! might throw exceptions, rather than doing it in the destructor. This is the
//! only purpose for the discard functor.

template<typename functor_type,
	 typename discard_functor_type> typename
make_multipart_processor_types<functor_type, discard_functor_type>::type
make_multipart_processor(const std::string &boundary,
			 functor_type &&functor,
			 discard_functor_type &&discard_functor)
{
	typedef typename make_multipart_processor_types<functor_type,
							discard_functor_type>
		::refiter_t refiter_t;

	typedef typename make_multipart_processor_types<functor_type,
							discard_functor_type>
		::type type;

	return type::create(refiter_t::create
			    (std::forward<functor_type>(functor),
			     std::forward<discard_functor_type>
			     (discard_functor)), boundary);
}

//! A do-nothing discard functor for make_multipart_processor()

template<typename functor_type>
class make_multipart_default_discarder {

public:

	//! Do nothing

	void operator()(const decltype((*(typename std::decay<functor_type>
					  ::type *)nullptr)()) &dummy)
	{
	}
};

//! Construct an output iterator over a multipart MIME section body

//! \overload
//! Supplies a dummy discard functor. The two-argument version of
//! make_multipart_processor() gets used when an explicit discard functor
//! is not needed.

template<typename functor_type>
typename make_multipart_processor_types<functor_type,
					make_multipart_default_discarder
					<functor_type> >::type
make_multipart_processor(const std::string &boundary,
			 functor_type &&functor)
{
	return make_multipart_processor
		(boundary,
		 std::forward<functor_type>(functor),
		 make_multipart_default_discarder<functor_type>());
}


#if 0
{
	{
#endif
	}
}
#endif
