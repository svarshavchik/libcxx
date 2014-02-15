/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ondestroy_H
#define x_ondestroy_H

#include <x/ref.H>
#include <x/ondestroyfwd.H>
#include <x/ondestroyobj.H>
#include <x/mpobj.H>
#include <x/namespace.h>

#include <list>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif


//! Proxy destructor callback used by onAnyDestroyed().

class destroyCallbackObj::onAnyDestroyCallbackObj : public destroyCallbackObj {


public:

	//! Cancellable destructor callbacks that were set up.

	class cbListObj : public std::list<ondestroy>, virtual public obj {

	public:
		cbListObj();
		~cbListObj() noexcept;
	};

	//! Constructor
	onAnyDestroyCallbackObj(const ref<cbListObj> &cb);

	//! Destructor
	~onAnyDestroyCallbackObj() noexcept;

private:

	//! One of the mcguffins went out of scope. We're done, cancel the whole show.
	void destroyed() noexcept;

	//! Cancellable destructor callbacks that were set up

	//! They get cancelled by the virtue of
	mpobj<ptr<cbListObj> > callbacks;
};

template<typename iter_type>
void destroyCallbackObj::onAnyDestroyed(iter_type b, iter_type e)
{
	auto cblist=ref<onAnyDestroyCallbackObj::cbListObj>::create();

	auto mcguffin=ref<onAnyDestroyCallbackObj>::create(cblist);

	while (b != e)
	{
		cblist->push_back(ondestroy::create(mcguffin, *b, true));
		++b;
	}

	mcguffin->addOnDestroy(ref<destroyCallbackObj>(this));
}

#if 0
{
#endif
}

#endif
