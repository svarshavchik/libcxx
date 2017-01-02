/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sentry_H
#define x_sentry_H

#include <type_traits>

#include <x/exception.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for \ref make_sentry sentries.

class sentry {

public:

	//! Constructor
	inline sentry() {}

	//! Destructor
	inline ~sentry() {}

	//! Begin guarding
	virtual void guard()=0;

	//! Finalize, disable the rollback.

	virtual void unguard()=0;
};

//! \ref make_sentry "sentry" implementing subclass

template<typename rollback_type>
class sentry_impl : public sentry {

	//! Lambda
	rollback_type rollback;

	//! Whether unguard() has been called.

	bool guarding=false;
public:

	//! Constructor
	inline sentry_impl(rollback_type &&arg)
		: rollback(std::move(arg))
	{
	}

	//! Constructor
	inline sentry_impl(const rollback_type &arg)
		: rollback(arg)
	{
	}

	//! Destructor
	~sentry_impl()
	{
		try {
			if (guarding)
				rollback();
		} catch (const exception &e)
		{
			e->caught();
		}
	}

	//! The sentry has been guarding
	inline void guard() override
	{
		guarding=true;
	}

	//! The sentry has been guarding
	inline void unguard() override
	{
		guarding=false;
	}
};

//! Create a sentry

//! \code
//! dbh->begin();
//!
//! auto s=INSERT_LIBX_NAMESPACE::make_sentry([]
//!                       {
//!                           dbh->rollback();
//!                       });
//! s.guard();
//!
//! // ...
//! dbh->commit();
//! s.unguard();
//! \endcode
//!
//! make_sentry() constructs a sentry object on the stack. If the sentry
//! object goes out of scope between the guard() and unguard() methods,
//! the sentry's lambda gets invoked.
//!
//! \note
//! The sentry's lambda should not throw exceptions.
//!
//! \note
//! The sentry object itself should not be assigned to, or copied.

template<typename builder_type>
auto make_sentry(builder_type &&builder)
{
	return sentry_impl<typename std::decay<builder_type>::type>
		(std::forward<builder_type>(builder));
}

#if 0
{
#endif
}

#endif
