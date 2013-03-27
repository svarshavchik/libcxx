/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fditer.H"
#include "x/fd.H"
#include "xml_internal.h"
#include "gettext_in.h"

#include <sstream>

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

void throw_last_error(const char *context)
{
	auto p=xmlGetLastError();

	throw EXCEPTION(std::string(context) + ": " +
			(p ? p->message:"error"));
}

std::string not_null(xmlChar *p, const char *context)
{
	if (!p)
		throw_last_error(context);

	std::string s(reinterpret_cast<const char *>(p));
	xmlFree(p);
	return s;
}

#include "xml_element_type.h"

static const size_t element_offsets_l=
	sizeof(element_offsets)/sizeof(element_offsets[0]);

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

	bool get_parent() override
	{
		if (n && n->parent)
		{
			n=n->parent;
			return true;
		}
		return false;
	}

	bool get_first_child() override
	{
		if (n && xmlGetLastChild(n) && n->children)
		{
			n=n->children;
			return true;
		}
		return false;
	}

	bool get_last_child() override
	{
		if (n)
		{
			xmlNodePtr p;

			if ((p=xmlGetLastChild(n)) != nullptr)
			{
				n=p;
				return true;
			}
		}
		return false;
	}

	bool get_next_sibling() override
	{
		if (n && n->next)
		{
			n=n->next;
			return true;
		}
		return false;
	}

	bool get_previous_sibling() override
	{
		if (n && n->prev)
		{
			n=n->prev;
			return true;
		}

		return false;
	}

	bool get_first_element_child() override
	{
		if (n)
		{
			xmlNodePtr p;

			if ((p=xmlFirstElementChild(n)) != nullptr)
			{
				n=p;
				return true;
			}
		}
		return false;
	}

	bool get_last_element_child() override
	{
		if (n)
		{
			xmlNodePtr p;

			if ((p=xmlLastElementChild(n)) != nullptr)
			{
				n=p;
				return true;
			}
		}
		return false;
	}

	bool get_next_element_sibling() override
	{
		if (n)
		{
			xmlNodePtr p;

			if ((p=xmlNextElementSibling(n)) != nullptr)
			{
				n=p;
				return true;
			}
		}
		return false;
	}

	bool get_previous_element_sibling() override
	{
		if (n)
		{
			xmlNodePtr p;

			if ((p=xmlPreviousElementSibling(n)) != nullptr)
			{
				n=p;
				return true;
			}
		}
		return false;
	}

	std::string type() const override
	{
		std::string type_str;

		if (n)
		{
			size_t i=(size_t)n->type;

			if (i < element_offsets_l &&
			    (i+1 == element_offsets_l ||
			     element_offsets[i] != element_offsets[i+1]))
			{
				type_str=element_names + element_offsets[i];
			}
			else
			{
				std::ostringstream o;

				o << "unknown_node_type_" << i;
				type_str=o.str();
			}
		}
		return type_str;
	}

	std::string path() const override
	{
		std::string p;

		if (n)
			p=not_null(xmlGetNodePath(n), "getNodePath");
		return p;
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

	ref<readlockObj> clone() const override
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
