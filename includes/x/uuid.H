/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_uuidfactory_H
#define x_uuidfactory_H

#include <x/namespace.h>
#include <x/logger.H>
#include <x/container_traits.H>
#include <x/singleton.H>
#include <x/base64.H>
#include <sys/time.h>
#include <stdint.h>

#include <string>
#include <thread>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A fast unique id generator

//! This is a quick unique id generator class. Instantiate this class to
//! produce a unique identifier.
//!
//! The unique id is formed by combining the thread starting time, the
//! thread id, and a 64 bit counter. The counter gets incremented to produce
//! each successive unique id. Every 65536 unique IDs, the time component
//! of the unique ID generator gets updated with the current time.
//!
//! The unique identifier also includes the machine's network MAC address.
//! This should make these identifiers globally unique.

class uuid {

	//! Internal logger object

	LOG_CLASS_SCOPE;

public:
	//! A singleton object class for holding the MAC address

	//! This MAC address is used for seeding the UUIDs
	//!

	class macObj : virtual public obj {

	public:
		//! Constructor

		macObj();

		//! Destructor
		~macObj();

		//! The MAC address

		std::vector<unsigned char> mac;
	};

public:
	//! A reference to a singleton object that holds the UUID MAC address

	typedef const_ptr<macObj> refmac;

private:
	//! The singleton object that holds the UUID MAC address

	static singleton<macObj> mac;

public:

	//! Retrieve the reference to the MAC address used for UUID seeding

	static refmac getmac()
	{
		return mac.get();
	}

private:
	//! Synchronization flag for uuid generator initialization.

	static __thread bool uuid_init LIBCXX_INTERNAL;

	//! The unique identifier structure
	struct uuid_parts {

		//! The randomly initialized counter

		uint64_t uuid_random __attribute__((packed));

		//! The thread ID

		tid_t    uuid_tid __attribute__((packed));

		//! Time value - seconds

		uint64_t uuid_tv_sec __attribute__((packed));

		//! Time value - microseconds

		uint32_t uuid_tv_usec __attribute__((packed));

		//! The machine's MAC address.
		unsigned char uuid_hwaddr[6];
	};

	//! Current unique id value.

	static __thread struct uuid_parts parts LIBCXX_INTERNAL;

public:

	//! How big the unique ID is

	static const size_t uuid_size=
		(sizeof(struct uuid_parts)+sizeof(uint16_t)-1)/
		sizeof(uint16_t);

	//! Type of an instantiated uuid

	typedef uint16_t val_t[uuid_size];

	//! Instantiated unique ID

	val_t val;

	//! Default constructor

	uuid();

	//! Default destructor
	~uuid();

	//! Ordering operator.

	bool operator<(const uuid &o) const noexcept
	{
		size_t i;

		for (i=0; i<sizeof(val)/sizeof(val[0]); i++)
		{
			if (val[i] < o.val[i])
				return true;

			if (val[i] > o.val[i])
				return false;
		}
		return false;
	}

	//! Ordering operator.

	bool operator==(const uuid &o) const noexcept
	{
		size_t i;

		for (i=0; i<sizeof(val)/sizeof(val[0]); i++)
			if (val[i] != o.val[i])
				return false;

		return true;
	}

	//! Ordering operator.

	bool operator!=(const uuid &o) const noexcept
	{
		return !operator==(o);
	}


	//! The buffer big enough to hold text representation of unique IDs.

	typedef char charbuf[(sizeof(val_t)+2)/3*4+1];

	//! Convert unique ID to a text string.
	void asString(charbuf cb) const noexcept;

	//! Alternate string conversion operator.
	template<typename OutputIterator>
	OutputIterator to_string(//! Output iterator
				OutputIterator iter,

				//! Locale for the conversion
				const const_locale &localeRef=locale::base::global())
		const
	{
		charbuf cb;
		asString(cb);

		for (const char *p=cb; *p; p++)
			*iter++= *p;

		return iter;
	}

	//! The base64 encoder used by to_string()
	typedef base64<0, base64alphabet<',', '_'> > base64_t;

	//! Construct from a previously-stringized uuid

	uuid(const char *str);

	//! Construct from a previously-stringized uuid

	uuid(const std::string &str);

	//! A helper object for serializing/deserializing the uuid object

	//! This helper object is made-up to look like a container, good enough
	//! to be handled by container-oriented serialization templates.

	class serialize_helper {

	public:

		//! The contents of this pseudo-container

		typedef uint16_t value_type;

		//! The size of this pseudo-container

		typedef size_t size_type;

		//! Deserialization pointer

		uint16_t *uptr;

		//! Constructor

		serialize_helper(uint16_t *ptrArg) noexcept : uptr(ptrArg)
		{
		}

		//! Destructor

		~serialize_helper()
		{
		}

		//! A no-op clear() method of this pseudo-container

		void clear() noexcept
		{
		}

		//! The size() function of this pseudo-container

		size_t size() const noexcept
		{
			return uuid_size;
		}

		//! The iterator for this pseudo-container

		typedef const uint16_t *const_iterator;

		//! This pseudo-container's beginning iterator

		const_iterator begin() const noexcept
		{
			return uptr;
		}

		//! This pseudo-container's ending iterator

		const_iterator end() const noexcept
		{
			return begin() + size();
		}
	};

	//! The serialization function

	template<typename iter_type> void serialize(iter_type &)
;

private:

	//! Decode from the stringized form

	void decode(const char *str) LIBCXX_INTERNAL;

	//! Thread initialization function

	static void init() LIBCXX_INTERNAL;

};

//! Implementation of traits for the uuid::serialize_helper pseudo-container

template<>
class container_traits_common<uuid::serialize_helper>
	: public container_traits_sequence {

public:

	//! The reserve() implementation

	static void reserve(uuid::serialize_helper &obj, size_t n)

	{
		if (n != uuid::uuid_size)
			throw EXCEPTION("Invalid container size");
	}

	//! The insert_chk() implementation

	static bool insert_chk(uuid::serialize_helper &c,
			       const uint16_t &value) noexcept
	{
		*c.uptr++=value;
		return true;
	}
};

template<typename iter_type> void uuid::serialize(iter_type &iter)

{
	serialize_helper helper(val);

	iter(helper);
}

extern template class base64alphabet<',', '_'>;
extern template class base64<0, base64alphabet<',', '_'> >;

#if 0
{
#endif
}
#endif
