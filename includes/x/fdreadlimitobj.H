/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdreadlimitobj_H
#define x_fdreadlimitobj_H

#include <x/obj.H>
#include <x/ptr.H>
#include <x/fdbaseobj.H>
#include <x/exception.H>
#include <iostream>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Implement read limits on a file descriptor.

//! This reference-counted class
//! takes a fdbaseObj, and implements another fdbaseObj that return 0
//! after the number of bytes read from the original fdbaseObj
//! exceeds a preset maximum. The read limit can be turned on and off.
//! When the read limit is off, reading from this fdbaseObj carries no
//! other effects.
//!
//! \code
//!
//! fd socket;
//!
//! // ...
//!
//! fdreadlimit socket_limited(fdreadlimit::create(socket));
//!
//! socket_limited->set_read_limit(8192);
//! \endcode
//!
//! Sets the limit on the number of bytes read. When the given number of bytes
//! gets read, read() throws an %exception.
//!
//! Only one read limit objects may be attached to a socket.
//!
//! \par Attaching both a read limit and a timeout object to a socket
//!
//! \code
//! fd socket;
//!
//! // ...
//!
//! fdreadlimit socket_limited(fdreadlimit::create(socket));
//!
//! fdtimeout socket_timeout(fdtimeout::create(socket_limited));
//!
//! // ...
//!
//! istream i(socket_timeout->getistream());
//!
//! std::getline(line, *i);
//!
//! \endcode
//!
//! The read limit object and the the timeout object can be daisy-chained
//! together. This can be done in either order, but whichever one's at the
//! end of the daisy-chain is the one whose file descriptor transport
//! methods (pubread(), pubwrite(), and others),  must be used.
//!
//! Exceeding the read limit results in a thrown \c EOVERFLOW
//! \ref sysexception "INSERT_LIBX_NAMESPACE::sysexception".

class fdreadlimitObj : public fdbaseObj::adapterObj {

	//! Read limit currently in force
	size_t read_limit;

	//! What to %throw in case of a read limit.
	std::string exceptionmsg;

	//! Flag: the read limit has been reached

	bool read_limit_reached;

public:
	//! Constructor

	fdreadlimitObj(//! The real file descriptor
		       const fdbase &ptr);

	//! Destructor
	~fdreadlimitObj();

	//! Activate a read limit.

	void set_read_limit(//! The read limit, must be greater than zero
			    size_t read_limitArg,

			    //! Throw this error message
			    const std::string &exceptionmsgArg);

	//! Turn off the read limit.
	void cancel_read_limit()
	{
		read_limit=0;
		read_limit_reached=false;
	}

	//! Implement the read limit.

	size_t pubread(//! Buffer

		       char *buffer,

		       //! Element count

		       size_t cnt)
		override LIBCXX_HIDDEN;
};

#if 0
{
#endif
}
#endif
