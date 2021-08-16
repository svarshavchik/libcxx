/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef httportmap_server_impl_H
#define httportmap_server_impl_H

#include "x/http/fdserverobj.H"
#include "x/http/accept_header.H"

#include <vector>

class httportmap_server;

class httpserverimpl : public LIBCXX_NAMESPACE::http::fdserverimpl {

	bool isinternal;
	bool allowstop;
	httportmap_server &portmap;

	typedef std::vector<LIBCXX_NAMESPACE::http::accept_header
			    ::media_type_t> media_type_list_t;

	media_type_list_t media_type_list;

	class svclistiter;

public:
	httpserverimpl(bool isinternalArg,
		       bool allowstopArg,
		       httportmap_server &portmapArg);
	~httpserverimpl();

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag) override;
};

class httpserver : virtual public LIBCXX_NAMESPACE::obj {

	bool isinternal;
	bool allowstop;
	httportmap_server &portmap;

public:
	httpserver(bool isinternalArg,
		   bool allowstopArg,
		   httportmap_server &portmapArg);
	~httpserver();

	void run(const LIBCXX_NAMESPACE::fd &socket,
		 const LIBCXX_NAMESPACE::fd &termpipe);
};


#endif
