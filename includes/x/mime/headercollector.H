/*
** Copyright 2012-2020 Double Precision, Inc.
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

namespace LIBCXX_NAMESPACE::mime {
#if 0
}
#endif

//! Implement a reference-counted output iterator that collects headers.

//! \see header_collector
//!

class header_collectorObj : public outputrefiteratorObj<int> {

	//! Current state, current element handler.
	
	typedef void (header_collectorObj::*state_func)(int);

	//! The underlying iterator.
	state_func state;

	//! Helper metadata
	size_t spacecount;

public:

	//! Constructor

	header_collectorObj();

	void operator=(int c) override;

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

class header_collectorBase;

//! Something that \ref header_iter "header_iter" can write to.

//! Presents an output iterator for
//! \ref header_iter "header_iter". Collects headers into
//! something more useful.
//!
//! A custom subclass can be defined that imlpements header(). Alternatively,
//! create() takes a functor or a lamda for each parsed header.
//!
//! - the \c std::string header name.
//!
//! - the \c std::string header name converted to lowercase.
//!
//! - the \c std::string header value, leading and trailing whitespace trimmed
//! off, and each folded line replaced by a single space.

typedef refiterator<header_collectorObj, header_collectorBase> header_collector;

//! A nullable pointer reference to a \ref header_collectorObj "header_collectorObj".

//! \see header_collector

typedef ptriterator<header_collectorObj,
		    header_collectorBase> header_collectorptr;

//! Base class for \ref header_collector "header_collector"

//! The base class provides a custom create() method;

class header_collectorBase {

public:

	
	//! Subclass of header_collector that uses a functor/lambda

	template<typename func_type>
	class implObj : public header_collectorObj {

		//! Callback
		func_type func;

	public:

		//! Constructor
		implObj(const func_type &funcArg)
			: func(funcArg) {}

		//! Move constructor
		implObj(func_type &&funcArg)
			: func(std::move(funcArg)) {}

		//! Callback

		void header() override
		{
			func(name, name_lc, contents);
		}
	};

	//! A factory that provides a custom create() implementation

	//! Create takes a functor/lambda, and instantiates a
	//! implObj;

	template<typename ref_type>
	class objfactory {

	public:

		//! Custom create() implementation

		template<typename func_type>
		static ref_type create(func_type &&functor)
		{
			return ref<implObj<typename std::decay<func_type>
					   ::type> >
				::create(std::forward<func_type>(functor));
		}
	};

};

#if 0
{
#endif
}
#endif
