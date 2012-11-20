/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_contentheadercollector_H
#define x_mime_contentheadercollector_H

#include <x/namespace.h>
#include <x/ref.H>
#include <x/refiterator.H>
#include <x/obj.H>
#include <x/chrcasecmp.H>
#include <x/mime/headercollector.H>
#include <string>
#include <iterator>
#include <map>

#include <type_traits>

namespace LIBCXX_NAMESPACE {

	//! Classes and templates for parsing MIME-formatted messages.

	namespace mime {
#if 0
	}
}
#endif

//! A subclass of header_collectorObj that collects Content- headers.

//! \see header_collector

class contentheader_collectorObj : public header_collectorObj {

	//! Whether the Mime-Version: 1.0 header has been seen, if required.

	bool mime_version_seen;

public:

	//! The container used for content_headers

	typedef std::multimap<std::string, std::string,
			      chrcasecmp::str_less> content_headers_t;

	//! The Content- headers

	//! This is valid only after processing a \c body_start

	content_headers_t content_headers;

	//! Constructor
	contentheader_collectorObj(//! Whether Mime-Version: 1.0 header required
				   bool mime_10_required);

	//! Destructor
	~contentheader_collectorObj() noexcept;

	using header_collectorObj::operator*;
	using header_collectorObj::operator++;

	//! Override operator=

	void operator=(int c) override;

private:
	void header() override LIBCXX_HIDDEN;
};

//! An implementation of \ref header_collector "header_collector" that collector MIME Content-* headers.

//! Implements \ref header_collector "header_collector" that collects MIME
//! Content-* headers.

typedef refiterator<contentheader_collectorObj> contentheader_collector;

//! A nullable pointer reference to a \ref contentheader_collectorObj "contentheader_collectorObj".

//! \see contentheader_collector

typedef ptriterator<contentheader_collectorObj> contentheader_collectorptr;

#if 0
{
	{
#endif
	}
}
#endif
