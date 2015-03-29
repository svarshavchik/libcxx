/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/semaphore.H"
#include "x/weakptr.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::semaphoreObj);

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

// A pending request for a semaphore

class semaphoreObj::pendingObj : virtual public obj {

 public:
	weakptr<ptr<ownerObj> > requester;
	size_t request_count;

	pendingObj(const ptr<ownerObj> &requesterArg,
		   size_t request_countArg) LIBCXX_INTERNAL 
		: requester(requesterArg), request_count(request_countArg)
	{
	}

	~pendingObj() noexcept LIBCXX_INTERNAL
	{
	}
};

semaphoreObj::ownerObj::ownerObj() {}
semaphoreObj::ownerObj::~ownerObj() noexcept {}

// Acquired semaphore. The mcguffin's destructor invokes release()

class LIBCXX_HIDDEN semaphoreObj::acquired_mcguffin : virtual public obj {

 public:
	size_t acquired;
	semaphore sem;

	acquired_mcguffin(const semaphore &semArg)
		: acquired(0), sem(semArg)
	{
	}

	~acquired_mcguffin() noexcept
	{
		if (acquired)
			sem->release(acquired);
	}
};

semaphoreObj::semaphoreObj(property::value<size_t> &semaphore_sizeArg)
	: semaphore_size(semaphore_sizeArg), count(0)
{
}

semaphoreObj::~semaphoreObj() noexcept
{
}

void semaphoreObj::request(const ref<ownerObj> &req,
			      size_t howmany)
{
	if (howmany == 0)
		howmany=1; // Joker

	process(&req, howmany, 0);
}

void semaphoreObj::release(size_t cnt) noexcept
{
	process(nullptr, 0, cnt);
}

// Semaphore processing logic. Fill requests, until we run out or the
// semaphore is full.

void semaphoreObj::process(const ref<ownerObj> *push,
			   size_t howmany, size_t pop) noexcept
{
	while (1)
	{
		// Presume that someone will be ready

		auto mcguffin=ref<acquired_mcguffin>::create(semaphore(this));

		// Presume it'll be this one

		ptr<pendingObj> pen;

		{
			std::lock_guard<std::mutex> acquired_lock(objmutex);

			if (push)
			{
				pending_list.push(ref<pendingObj>
						  ::create(*push, howmany));
				push=nullptr;
			}

			count -= pop;
			pop=0;

			if (!pending_list.empty())
			{
				pen=pending_list.front();

				size_t size=semaphore_size.getValue();

				// If there's nothing acquired, always take the
				// first request, even if it's larger than the
				// entire semaphore. This makes sure that the
				// requests continue to get processed if the
				// property value is bad.

				if (count == 0 ||
				    (count < size &&
				     pen->request_count <= (size - count)))
				{
					mcguffin->acquired = pen->request_count;
					count += pen->request_count;
					pending_list.pop();
				}
				else
				{
					pen=ptr<pendingObj>();
				}
			}

			if (pen.null())
				break;
		}

		try {
			ptr<ownerObj> req(pen->requester.getptr());

			if (!req.null())
				req->process(mcguffin);
		} catch (const exception &e)
		{
			LOG_ERROR(e);
			LOG_TRACE(e->backtrace);
		}
	}
}

#if 0
{
#endif
}
