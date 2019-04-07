/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_httportmapfwd_H
#define x_httportmapfwd_H

#include <x/ptr.H>
#include <x/fd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class httportmapObj;
class httportmapBase;

//! An HTTP-based portmapper

//! This is a portmapper service that uses HTTP as the underlying service
//! location protocol.
//!
//! This portmapper service allows different applications to find each other
//! and connect. An application that wants to receive connections from other
//! applications registers itself with the portmapper daemon service:
//!
//! \code
//!
//! fd socket;
//!
//! // ...
//!
//! httportmap portmapper(httportmap::create());
//!
//! if (!portmapper->reg("backup@region.example.com", socket,
//!      httportmap::base::pm_exclusive))
//!     exit(1);
//!
//! \endcode
//!
//! \c socket is a socket that listens on some arbitrary port.
//! reg() registers an application with an associated application name. Other
//! applications will be able to connect to this application by searching for
//! the application's name.
//!
//! Applications should be named as valid hostnames or email addresses, and
//! applications should use domains that they own. This way, the global
//! application name space should be unique, and individual applications will
//! avoid clashing names.
//!
//! This implementation reserves the "libx" domain name suffix for internal
//! use.
//!
//! In the above example, the second parameter is the socket that the 
//! application already bound to some port and is listening on. The second
//! parameter can also be an \c int, a \c std::string, or several other
//! possibilities. The portmapper
//! service generally ignores the port names, except that they may not
//! contain control characters. By convention, though, port names should either
//! be numeric, or absolute pathnames, referring to \c AF_UNIX domain sockets.
//! Several convenience portmapper functions assume that the port names follow
//! this convention, and automatically connect to the specified port.
//!
//! The third parameter is a set of ORed flags:
//!
//! - httportmap::base::pm_exclusive: this flag specifies that this service is
//! exclusive. The portmapper prevents more than one application from
//! registering the same service. Without this flag multiple
//! registrations of the same service, by the same user, are permitted.
//!
//! - httportmap::base::pm_public: this flag specifies that the portmapper is
//! allowed to publish the path to this application's executable filename.
//!
//! reg() returns \c true if the service was registered, or \c false if an
//! exclusive service is already registered. Any other error condition
//! results in a thrown %exception.
//!
//! reg() and dereg() are thread-safe.
//!
//! \note
//! Service exclusivity is on a per-userid basis. Different userids may
//! register the same exclusive service. An exclusive service may be registered
//! at most once for the same userid.
//!
//! Overloaded versions of reg() allow "atomic" registration of multiple
//! services, at once, and there's also the dereg() method that deregisters
//! a service.
//! See httportmapObj methods for more information.
//!
//! In all cases, the default portmapper configuration limits each process to
//! ten registered services. This is an aggregate limit for all instances
//! of the \c httportmap class originating from the same process ID.
//!
//! \par Connecting to a service.
//!
//! \code
//! fd socket=httportmap::create()->connect("addressbook.example.com", getuid());
//! \endcode
//!
//! This example creates a socket and connects to the "addressbook.example.com"
//! service running under the same userid, on the same server.
//!
//! \note
//! A registered application is presumed to be listening
//! on a wildcard address. Alternatively the application may bind a separate
//! socket on the same port for each network interface's IP address.
//!
//! \par Authenticating by pathname
//!
//! On Linux, reading /proc/$pid/exe fails if $pid runs under a different
//! system userid. INSERT_LIBX_NAMESPACE::pidinfo() throws an
//! %exception in that case.
//!
//! However, sometimes it's desirable to authenticate another process, by its
//! pathname, even if the process is being executed by a different userid.
//!
//! \code
//! portmapper->regpid2exe();
//! \endcode
//!
//! Invoking regpid2exe() announces what executable pathname this process
//! is running from.
//!
//! \code
//!
//! pid_t p;
//!
//! std::string portmapper->pid2exe(p);
//! \endcode
//!
//! pid2exe() returns the given process's pathname, if the process has
//! announced itself via regpid2exe().
//!
//! There's also a deregpid2exe(), which stops announcing this process's
//! pathname. After deregpid2exe(),
//! other processes will no longer be able to see which pathname this process
//! is running. But, as the saying goes, what has been seen cannot be
//! un-seen.
//!
//! Note that regpid2exe() counts towards the limit on the maximum number of
//! registered services, with the portmapper, from the same process.
//!
//! \note
//! Long-running daemons should invoke regpid2exe() when they start, and
//! keep the portmapper connection running. A prelink(1) may result
//! in the process /proc/self/exe becoming undefined, when prelink(1)
//! replaces the binary, so this must be done during startup.
//!
//! \par INSERT_LIBX_NAMESPACE::httportmap::base:regpid2exe()
//!
//! \code
//! INSERT_LIBX_NAMESPACE::httportmap portmap=INSERT_LIBX_NAMESPACE::httportmap::base:regpid2exe();
//! \endcode
//!
//! This returns a singleton portmapper instance that already invoked
//! regpid2exe(). Use this to register with the portmapper in a library. The
//! singleton avoids multiple registrations.

typedef ref<httportmapObj, httportmapBase> httportmap;

//! A pointer to a portmapper object

//! \see portmap
typedef ptr<httportmapObj, httportmapBase> httportmapptr;

#if 0
{
#endif
}
#endif
