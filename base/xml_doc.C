/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fditer.H"
#include "x/fd.H"
#include "xml_internal.h"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

impldocObj::impldocObj(xmlDocPtr pArg) : p(pArg), lock(rwlock::create())
{
}

impldocObj::~impldocObj() noexcept
{
	if (p)
		xmlFreeDoc(p);
}

docObj::docObj()
{
}

docObj::~docObj() noexcept
{
}

doc docBase::create()
{
	return ref<impldocObj>::create(xmlNewDoc((const xmlChar *)"1.0"));
}

doc docBase::create(const std::string &filename)
{
	return create(filename, "");
}

doc docBase::create(const std::string &filename,
		    const std::string &options)
{
	auto file=fd::base::open(filename, O_RDONLY);

	return create(fdinputiter(file), fdinputiter(), filename, options);
}

/////////////////////////////////////////////////////////////////////////////
//
// readlockObj is the publicly exposed read lock. writelockObj is a publicly
// exposed write lock, and a subclass of readlockObj.
//
// readlockimplObj is a subclass of writelockObj, that implements the read
// functionality, and writelockimplObj is a subclass of readlockimplObj
// that implements the write functionality.
//
// In this manner, readlockimplObj implements read functionality for both
// readlockObj and writelockObj, throw an exception for write functionality,
// and implements clone() that copies the read lock on the document that it
// holds internally. writelockimplObj inherits the read functionality from
// readlockimplObj, implements the write functionality, and overrides clone()
// to throw an exception, can't clone() a write lock.

docObj::readlockObj::readlockObj()
{
}

docObj::readlockObj::~readlockObj() noexcept
{
}

docObj::writelockObj::writelockObj()
{
}

docObj::writelockObj::~writelockObj() noexcept
{
}

class LIBCXX_HIDDEN impldocObj::readlockImplObj : public writelockObj {

 public:

	ref<impldocObj> impl;
	ref<obj> lock;

	xmlNodePtr n;

	readlockImplObj(const ref<impldocObj> &implArg,
			const ref<obj> &lockArg)
		: impl(implArg), lock(lockArg), n(nullptr)
	{
	}

	~readlockImplObj() noexcept
	{
	}

	// Read functionality
	ref<readlockObj> clone() const override
	{
		auto p=ref<readlockImplObj>::create(impl, lock);
		p->n=n;
		return p;
	}

	bool get_root() override
	{
		n=xmlDocGetRootElement(impl->p);

		return !!n;
	}
};

class LIBCXX_HIDDEN impldocObj::writelockImplObj : public readlockImplObj {

 public:

	writelockImplObj(const ref<impldocObj> &implArg,
			 const ref<obj> &lockArg)
		: readlockImplObj(implArg, lockArg)
	{
	}

	~writelockImplObj() noexcept
	{
	}

	ref<readlockObj> clone() override
	{
		throw EXCEPTION(libmsg(_txt("Cannot clone a write lock")));
	}
};


ref<docObj::readlockObj> impldocObj::readlock()
{
	return ref<readlockImplObj>::create(ref<impldocObj>(this),
					    lock->readlock());
}

ref<docObj::writelockObj> impldocObj::writelock()
{
	return ref<writelockImplObj>::create(ref<impldocObj>(this),
					     lock->writelock());
}

#if 0
{
	{
#endif
	}
}
