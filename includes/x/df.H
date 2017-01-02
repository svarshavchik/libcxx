/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_df_H
#define x_df_H

#include <x/ref.H>
#include <x/obj.H>
#include <x/fdfwd.H>
#include <sys/types.h>
#include <stdint.h>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! An object that keeps track of remaining disk space

//! All documented methods are thread-safe.
//!
class dfObj : virtual public obj {

protected:
	//! Reserved blocks
	refcnt res_blks;

	//! Reserved inodes
	refcnt res_inodes;

public:
	class dfvfsObj;
	class dfvfspathObj;
	class dfvfsfdObj;

	class reservationObj;

	friend class reservationObj;

	//! Constructor

	dfObj();

	//! Destructor
	~dfObj();

	//! Return filesystem allocation unit size
	virtual size_t allocSize()=0;

	//! Return number of allocation units left in the filesystem

	//! The count of reserved allocation units is not included in the
	//! returned value. 0 gets returned if reserved allocation units
	//! exceed the remaining number of remaining allocation units.

	virtual uint64_t allocFree()=0;

	//! Return number of inodes left in the filesystem

	//! The count of reserved inodes is not included in the returned value.
	//! 0 gets returned if reservd inode count exceed the number of
	//! remaining inodes.

	virtual uint64_t inodeFree()=0;

	//! Refresh filesystem free space

	virtual void refresh()=0;

	//! Reserve some amount of free disk space and/or inodes

	//! The requested space/inode count gets subtracted, and won't be
	//! included in allocFree() and inodeFree() until the reservation
	//! handle goes out of scope.
	//!
	//! \return a reservation handle. When the last reference to the
	//! reservation goes out of scope, the reserved space gets added back.
	ref<reservationObj> reserve(//! Number of allocation units to reserve
				    long alloc,

				    //! Number of inodes to reserve
				    long inodes);

private:

	//! Invoked by reservationObj::commit()

	virtual void commit(long, long)=0;
};

class dfBase;

//! Track free disk space

//! \code
//! df space(df::create("/home"));
//!
//! std::cout << "Free disk space: " << space->allocFree() << " of "
//!           << space->allocSize() << "-sized allocation units" << std::endl;
//! std::cout << "Inodes free: " << space->inodeFree() << std::endl;
//! \endcode

typedef ref<dfObj, dfBase> df;

//! A reference-counted pointer to a free space tracker

//! \see df

typedef ptr<dfObj, dfBase> dfptr;

//! Refer to this class as \c df::base or \c dfptr::base

class dfBase {

public:

	//! The same create() gets implemented for \c df and \c dfptr

	class newobj {

	public:
		//! The create() method - status of partition specified by filename

		static df create(//! Pathname
				 const std::string &pathname);

		//! The create() method - status of partition specified by file descriptor

		static df create(//! File descriptor
				 const fd &filedesc);
	};

	//! Always return a df() for new objects or references, for simplicity.

	template<typename dummy> class objfactory : public newobj {
	};

	//! What reserve() returns

	typedef ref<dfObj::reservationObj> reservation;

	//! A nullable pointer to what reserve() returns

	typedef ptr<dfObj::reservationObj> reservationptr;
};

//! A reservation for a given number of blocks and inodes

class dfObj::reservationObj : virtual public obj {

	//! Reserved number of blocks
	long res_blks;

	//! Reserved number of inodes
	long res_inodes;

	//! Where this is reserved from
	df space;

public:
	//! Constructor
	reservationObj(long res_blksArg,
		       long res_inodesArg,
		       const df &spaceArg);

	//! Destructor
	~reservationObj();

	//! Commit the reservation

	//! Do not credit back the reserved blocks/inodes when this reservation
	//! goes out of scope (until the next refresh, which resets it).

	void commit() noexcept
	{
		space->commit(res_blks, res_inodes);

		res_blks=0;
		res_inodes=0;
	}
};


#if 0
{
#endif
}

#include <x/dirobj.H>
#endif
