/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sigset_H
#define x_sigset_H

#include <x/namespace.h>
#include <x/exception.H>
#include <x/threads/run.H>
#include <type_traits>
#include <string_view>
#include <signal.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class thread_attr;
class signalfdObj;

//! A light C++ wrapper for sigset_t

class sigset {

	//! The signal mask that's managed by this \c sigset.

	sigset_t mask;

public:
	friend class signalfdObj;

	//! Constructor -- empty set (sigemptyset or sigfillset)
	sigset(bool fill=false);

	//! Destructor
	~sigset();

	//! Obtain current thread signal mask

	static sigset current();

	//! Add these signals to the current thread's signal mask

	void block() const;

	//! Remove these signals to the current thread's signal mask

	void unblock() const;

	//! Set the current thread's signal mask to these signals

	void setmask() const;

	//! Convert signal name to number

	//! \return The number of the signal specified by name ("SIGHUP"
	//! returns SIGHUP), or a negative value for an unknown signal.

	static int name2sig(const std::string_view &signame) noexcept;

	//! Convert signal name to number

	//! \return The number of the signal specified by name ("SIGHUP"
	//! returns SIGHUP). An %exception gets thrown if the signal name is
	//! not recognized.

	static int name2sig_orthrow(const std::string_view &signame);

	//! Convert signal number to name

	//! \return The name of the signal, or NULL if the name is not defined

	static const char *sig2name(int) noexcept;

	//! Add a signal to the set (sigaddset)

	sigset &operator+=(int signum);

	//! Add a signal to the set (sigaddset)

	//! \overload

	sigset &operator+=(const std::string_view &signame);

	//! Remove a signal from the set (sigdelset)

	sigset &operator-=(int signum);

	//! Remove a signal from the set (sigdelset)

	//! \overload

	sigset &operator-=(const std::string_view &signame);

	//! Convenience function

	sigset operator+(int signum) const
	{
		sigset s=*this;

		s += signum;
		return s;
	}

	//! Convenience function

	//! \overload

	sigset operator+(const std::string_view &n) const
	{
		return operator+(name2sig_orthrow(n));
	}

	//! Convenience function

	sigset operator-(int signum) const
	{
		sigset s=*this;

		s -= signum;
		return s;
	}

	//! Convenience function

	//! \overload

	sigset operator-(const std::string_view &n) const
	{
		return operator-(name2sig_orthrow(n));
	}

	//! Check if the set contains a signal (sigismember)

	bool operator&(int signum) const;

	class block_all;
};


//! Temporary block all signals except for \c SIGABRT.

//! This object is meant to be instantiated on the stack, to block
//! all signals for the duration of its scope. The constructor blocks
//! all signals except SIGABRT. The destructor restores the original
//! signal mask.

class sigset::block_all {

	//! Original thread signal mask.

	sigset orig;

public:
	//! Constructor
	block_all() noexcept;

	//! Destructor
	~block_all();

	//! A convenience method

	//! Invokes \ref run()
	//! This convenience method allows for a
	//! "block_all().run(thread, arguments)" convenient shortcut.
	template<typename ...Args>
	auto run(Args && ...args) ->
		decltype(LIBCXX_NAMESPACE::run(std::forward<Args>(args)...))
	{
		return LIBCXX_NAMESPACE::run(std::forward<Args>(args)...);
	}
};


#if 0
{
#endif
}
#endif
