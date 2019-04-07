/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_bodycallbackobj_H
#define x_http_bodycallbackobj_H

#include <x/http/fdclientimpl.H>
#include <x/http/useragentobj.H>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! Callback object for processing the content of a response to a request

//! Subclass this, and implement the body() method in the subclass.

class useragentObj::bodycallbackObj : virtual public obj {

public:

	//! Content iterator received by body()
	typedef fdclientimpl::iterator iterator;

	//! Default constructor
	bodycallbackObj();

	//! Default destructor
	~bodycallbackObj();

	//! Process the body of a response

	//! The subclass receives a beginning and an ending iterator
	//! that defines an input sequence of the response received
	//! to a request.

	virtual void body(//! Beginning iterator
			  iterator beg_iter,
				  
			  //! Ending iterator
			  iterator end_iter)
;
};

#if 0
{
	{
#endif
	}
}
#endif
