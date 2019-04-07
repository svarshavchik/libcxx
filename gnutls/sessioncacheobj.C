/*
** Copyright 2013-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/sessioncache.H"
#include "x/gnutls/init.H"
#include "x/orderedcache.H"

#include <map>
#include <algorithm>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::gnutls::sessioncacheObj);

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

static gnutls::datum_t newticket()
{
	gnutls_datum_t datum;

	gnutls::chkerr(gnutls_session_ticket_key_generate(&datum),
		       "gnutls_session_ticket_key_generate");
	return gnutls::datumwrapper(datum);
}

gnutls::sessioncacheObj::sessioncacheObj()
	: ticket(newticket())
{
}

gnutls::sessioncacheObj::~sessioncacheObj()
{
}

static property::value<size_t> cache_size_property
(LIBCXX_NAMESPACE_STR "::gnutls::session_cache::size", 500);

class LIBCXX_HIDDEN gnutls::sessioncacheObj::basic_implObj
	: public sessioncacheObj {

public:

	class datum_comparator {

	public:
		bool operator()(const datum_t &a,
				const datum_t &b) const
		{
			return datum_t::base::container(*a)
				< datum_t::base::container(*b);
		}
	};

	ordered_cache<ordered_cache_traits<datum_t, datum_t, datum_comparator,
					   datum_comparator>, true> cache;

	basic_implObj() : cache(cache_size_property.get()) {}

	~basic_implObj() {}

	void store(const datum_t &key,
		   const datum_t &data) override
	{
		std::unique_lock<std::mutex> lock(objmutex);

		cache.add(key, data);
	}

	void remove(const gnutls::datum_t &key) override
	{
		std::unique_lock<std::mutex> lock(objmutex);

		cache.remove(key);
	}

	gnutls::datumptr_t retr(const gnutls::datum_t &key) override
	{
		std::unique_lock<std::mutex> lock(objmutex);

		auto p=cache.find(key);

		if (!p)
			return datumptr_t();

		return *p;
	}
};

gnutls::sessioncache gnutls::sessioncacheBase::create()
{
	return ptrref_base::objfactory<ref<sessioncacheObj::basic_implObj>>
		::create();
}

int gnutls::sessioncacheObj::store_func(void *ptr,
					gnutls_datum_t key, gnutls_datum_t data)
{
	sessioncacheObj *p=reinterpret_cast<sessioncacheObj *>(ptr);

	try {
		p->store(datum_t::create(key.data, key.data+key.size),
			 datum_t::create(data.data, data.data+data.size));
	} catch (const exception &e)
	{
		LOG_ERROR(e);
		LOG_TRACE(e->backtrace);
	}
	return 0;
}

int gnutls::sessioncacheObj::remove_func(void *ptr, gnutls_datum_t key)
{
	sessioncacheObj *p=reinterpret_cast<sessioncacheObj *>(ptr);

	try {
		p->remove(datum_t::create(key.data, key.data+key.size));
	} catch (const exception &e)
	{
		LOG_ERROR(e);
		LOG_TRACE(e->backtrace);
	}
	return 0;
}

gnutls_datum_t gnutls::sessioncacheObj::retr_func(void *ptr, gnutls_datum_t key)
{
	sessioncacheObj *p=reinterpret_cast<sessioncacheObj *>(ptr);

	gnutls::datumptr_t datum;

	try {
		datum=p->retr(datum_t::create(key.data, key.data+key.size));
	} catch (const exception &e)
	{
		LOG_ERROR(e);
		LOG_TRACE(e->backtrace);
	}

	gnutls_datum_t datum_ret;

	datum_ret.data=nullptr;
	datum_ret.size=0;

	if (!datum.null() && (datum_ret.data=
			      reinterpret_cast<decltype(datum_ret.data)>
			      (gnutls_malloc(datum->size()))))
		std::copy(datum->begin(),
			  datum->end(),
			  datum_ret.data);

	return datum_ret;
}

#if 0
{
#endif
};
