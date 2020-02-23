/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sigset.H"
#include <map>
#include <pthread.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#include "mksigtab.h"

const char *signame(size_t n) noexcept LIBCXX_INTERNAL;

const char *signame(size_t n) noexcept
{
	int o;

	return (n < nsigs && (o=sigoff[n]) >= 0) ? sigtab + o:"";
}

int sigset::name2sig(const std::string &n) noexcept
{
	for (size_t i=0; i<nsigs; ++i)
		if (n == signame(i))
			return i;

	return -1;
       
}

int sigset::name2sig_orthrow(const std::string &n)

{
	int v=name2sig(n);

	if (v < 0)
		throw EXCEPTION(n + ": no such signal");
	return v;
}

const char *sigset::sig2name(int i) noexcept
{
	const char *p=signame(i);

	return *p ? p:NULL;
}

	//! Constructor -- empty set
sigset::sigset(bool fill)
{
	if ((fill ? sigfillset(&mask):sigemptyset(&mask)) < 0)
		throw EXCEPTION("sigset");
}

sigset::~sigset()
{
}


sigset sigset::current()
{
	sigset ss, ss2;

	if (pthread_sigmask(SIG_BLOCK, &ss.mask, &ss2.mask) < 0)
		throw EXCEPTION("pthread_sigmask");

	return ss2;
}

void sigset::block() const
{
	if (pthread_sigmask(SIG_BLOCK, &mask, NULL) < 0)
		throw EXCEPTION("pthread_sigmask(SIG_BLOCK)");
}


void sigset::unblock() const
{
	if (pthread_sigmask(SIG_UNBLOCK, &mask, NULL) < 0)
		throw EXCEPTION("pthread_sigmask(SIG_UNBLOCK)");
}

void sigset::setmask() const
{
	if (pthread_sigmask(SIG_SETMASK, &mask, NULL) < 0)
		throw EXCEPTION("pthread_sigmask(SIG_SETMASK)");
}


sigset &sigset::operator+=(int signum)
{
	if (sigaddset(&mask, signum) < 0)
		throw EXCEPTION("sigaddset");

	return *this;
}

sigset &sigset::operator+=(const std::string &n)
{
	return operator+=(name2sig_orthrow(n));
}

sigset &sigset::operator-=(int signum)
{
	if (sigdelset(&mask, signum) < 0)
		throw EXCEPTION("sigaddset");

	return *this;
}

sigset &sigset::operator-=(const std::string &n)
{
	return operator-=(name2sig_orthrow(n));
}

bool sigset::operator&(int signum) const
{
	int rc=sigismember(&mask, signum);

	if (rc < 0)
		throw EXCEPTION("sigismember");

	return !!rc;
}

sigset::block_all::block_all() noexcept
{
	orig=sigset::current();

	sigset ss(true);

	ss -= SIGABRT;

	ss.setmask();
}

sigset::block_all::~block_all()
{
	orig.setmask();
}
		
#if 0
{
#endif
}
