/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ondestroy.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

obj::destroyCallbackObj::destroyCallbackObj() noexcept
{
}

obj::destroyCallbackObj::~destroyCallbackObj() noexcept
{
}

//////////////////////////////////////////////////////////////////////////////
//
// Proxy destructor callback used by on_any_destroyed().

class LIBCXX_HIDDEN onAnyDestroyCallbackObj : virtual public obj {

public:

	//! Cancellable destructor callbacks that were set up.

	class cbListObj : public std::list<LIBCXX_NAMESPACE::ondestroy>,
			  virtual public obj {

	public:
		cbListObj();
		~cbListObj() noexcept;
	};

	//! Constructor
	onAnyDestroyCallbackObj(const ref<cbListObj> &cb);

	//! Destructor
	~onAnyDestroyCallbackObj() noexcept;

	//! One of the mcguffins went out of scope. We're done, cancel the whole show.
	void destroyed();

private:

	//! Cancellable destructor callbacks that were set up

	//! They get cancelled by the virtue of
	mpobj<ptr<cbListObj> > callbacks;
};

onAnyDestroyCallbackObj::cbListObj::cbListObj()
{
}

onAnyDestroyCallbackObj::cbListObj::~cbListObj() noexcept
{
}

onAnyDestroyCallbackObj::onAnyDestroyCallbackObj(const ref<cbListObj> &cb)
{
	*mpobj<ptr<cbListObj> >::lock(callbacks)=cb;
}

onAnyDestroyCallbackObj::~onAnyDestroyCallbackObj() noexcept
{
}

void onAnyDestroyCallbackObj::destroyed()
{
	ptr<cbListObj> save;

	{
		mpobj<ptr<cbListObj> >::lock lock(callbacks);

		save= *lock;
		*lock=ptr<cbListObj>();
	}
}

ref<obj> on_any_destroyed_mcguffin(onAnyDestroyBase &base)
{
	auto cblist=ref<onAnyDestroyCallbackObj::cbListObj>::create();
	auto mcguffin=ref<onAnyDestroyCallbackObj>::create(cblist);

	while (base.more())
	{
		cblist->push_back(ondestroy::create([mcguffin]
						    {
							    mcguffin->destroyed();
						    }, base.next(), true));
	}

	return mcguffin;
}

#if 0
{
#endif
}
