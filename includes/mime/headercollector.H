/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_headercollector_H
#define x_mime_headercollector_H

#include <x/namespace.h>
#include <x/ref.H>
#include <x/refiterator.H>
#include <x/obj.H>
#include <string>
#include <iterator>

#include <type_traits>

namespace LIBCXX_NAMESPACE {

	//! Classes and templates for parsing MIME-formatted messages.

	namespace mime {
#if 0
	}
}
#endif

//! Implement a reference-counted output iterator that collects headers.

//! \see header_collector
//!

class header_collectorObj : virtual public obj {

	//! Current state, current element handler.
	
	typedef void (header_collectorObj::*state_func)(int);

	//! The underlying iterator.
	state_func state;

	//! Helper metadata
	size_t spacecount;

public:

	//! My iterator traits

	typedef std::iterator<std::output_iterator_tag, void,
			      void, void, void> iterator_traits;

	//! Constructor

	header_collectorObj();

	//! Iterator operator

	header_collectorObj &operator*() { return *this; }

	//! Iterator operator

	header_collectorObj &operator++() { return *this; }

	//! Iterator operator

	header_collectorObj *before_postoper() { return this; }

	//! Iterator operator

	virtual void operator=(int c);

	//! The subclass implements this callback.

	//! The subclass simply reads name, name_lc, contents.

	virtual void header()=0;

protected:

	//! The header name.
	std::string name;

	//! The header name, in lowercase.
	std::string name_lc;

	//! The contents of the header.

	//! Leading/trailing whitespace is trimmed off. All embedded whitespace
	//! characters converted to SP.

	std::string contents;

private:
	//! Waiting for the next header
	void wait_header_start(int c) LIBCXX_HIDDEN;

	//! Waiting for the end of header
	void wait_header_end(int c) LIBCXX_HIDDEN;

	//! Waiting for header contents
	void wait_contents_start(int c) LIBCXX_HIDDEN;

	//! Waiting for the end of header contents
	void wait_contents_end(int c) LIBCXX_HIDDEN;

	//! Waiting for end of embedded newline or folded header indicator
	void wait_folded_end(int c) LIBCXX_HIDDEN;
};

//! Something that \ref header_iter "header_iter" can write to.

//! Presents an output iterator for
//! \ref header_iter "header_iter". Collects headers into
//! something more useful.
//!
//! What happens with the headers is determined by its subclass, which
//! implements the header() method.

typedef refiterator<header_collectorObj> header_collector;

//! A nullable pointer reference to a \ref header_collectorObj "header_collectorObj".

//! \see header_collector

typedef ptriterator<header_collectorObj> header_collectorptr;

//! Subclass of header_collector that uses a functor/lambda

template<typename func_type>
class header_collector_implObj : public header_collectorObj {

	//! Callback
	func_type func;

public:

	//! Constructor
	header_collector_implObj(const func_type &funcArg) : func(funcArg) {}

	//! Move constructor
	header_collector_implObj(func_type &&funcArg)
		: func(std::move(funcArg)) {}

	//! Callback

	void header() override
	{
		func(name, name_lc, contents);
	}
};

//! Convenience function for lambdas/functors.

//! Construct a reference-counted \ref header_collector "header_collector"
//! that invokes a functor or a lambda, for each parsed header.
//!
//! The functor/lambda receives three parameters:
//!
//! - the \c std::string header name.
//!
//! - the \c std::string header name converted to lowercase.
//!
//! - the \c std::string header value, leading and trailing whitespace trimmed
//! off, and each folded line replaced by a single space.

template<typename func_type>
header_collector make_header_collector(func_type &&functor)
{
	return ref<header_collector_implObj<typename
					    std::decay<func_type>::type> >
		::create(std::forward<func_type>(functor));
}

#if 0
{
	{
#endif
	}
}
#endif
