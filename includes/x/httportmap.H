/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_httportmap_H
#define x_httportmap_H

#include <x/httportmapfwd.H>
#include <x/http/useragentfwd.H>
#include <x/httportmapobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Refer to this class as \c httpportmap::base or \c httportmapptr::base

//! \see httportmap
//!

class httportmapBase : public ptrref_base {

public:
	//! Flag: exclusive service registration
	static constexpr int pm_exclusive=1;

	//! Flag: make my path public

	//! \internal
	//! Internal flag used by regpid2exe().
	//!
	static constexpr int pm_public=2;

	//! Service registration information

	typedef httportmapObj::reginfo reginfo;

	//! Application service information

	typedef httportmapObj::service service;

	//! Internal portmapper service name

	//! \internal
	//!

	static const char portmap_service[];

	//! Internal portmapper pid2exe service name

	//! \internal
	static const char pid2exe_service[];

	//! Return default port

	static int getDefaultPort()
	{
		return httportmapObj::getDefaultPort();
	}

	//! A portmapper instance that invoked %regpid2exe().

	//! This returns a single portmapper instance that invoked
	//! regpid2exe().

	static httportmap regpid2exe();

	class regpid2exeObj;

	//! Handshake with the portmap server on the internal socket.

	//! \internal
	static void connect_service(fd &newfd);
};

#if 0
{
#endif
}
#endif
