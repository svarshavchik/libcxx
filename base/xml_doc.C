/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fditer.H"
#include "x/fd.H"
#include "x/tostring.H"
#include "x/uriimpl.H"
#include "x/messages.H"
#include "x/sysexception.H"
#include "x/weakptr.H"
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

std::string null_ok(xmlChar *p)
{
	std::string s(p ? reinterpret_cast<const char *>(p):"");
	xmlFree(p);
	return s;
}

std::string get_str(const xmlChar *p)
{
	return p ? reinterpret_cast<const char *>(p):"";
}

#include "xml_element_type.h"

static const size_t element_offsets_l=
	sizeof(element_offsets)/sizeof(element_offsets[0]);

impldocObj::impldocObj(xmlDocPtr pArg) : p(pArg), lock(rwlock::create())
{
}

impldocObj::~impldocObj()
{
	if (p)
		xmlFreeDoc(p);
}

docObj::docObj()
{
}

docObj::~docObj()
{
}

docObj::newElement::newElement(const std::string &nameArg)
	: newElement(nameArg, "", "")
{
	prefix_given=false;
}

docObj::newElement::newElement(const std::string &nameArg,
			       const std::string &uriArg)
	: newElement(nameArg, "", uriArg)
{
	prefix_given=false;
}

docObj::newElement::newElement(const std::string &nameArg,
			       const char *uriArg)
	: newElement(nameArg, "", uriArg)
{
	prefix_given=false;
}

docObj::newElement::newElement(const std::string &nameArg,
			       const uriimpl &uriArg)
	: newElement(nameArg, "", tostring(uriArg))
{
	prefix_given=false;
}

docObj::newElement::newElement(const std::string &nameArg,
			       const std::string &prefixArg,
			       const std::string &uriArg)
	: name(nameArg), prefix(prefixArg), uri(uriArg), prefix_given(true)
{
}

docObj::newElement::~newElement()
{
}

docObj::docAttribute::docAttribute(const std::string &attrnameArg,
				   const std::string &attrnamespaceArg)
	: attrname(attrnameArg), attrnamespace(attrnamespaceArg)
{
}

docObj::docAttribute::~docAttribute()
{
}

bool docObj::docAttribute::operator<(const docAttribute &o) const
{
	if (attrname < o.attrname)
		return true;

	if (o.attrname < attrname)
		return false;

	return attrnamespace < o.attrnamespace;
}

docObj::newAttribute::newAttribute(const std::string &attrnameArg,
				   const std::string &attrvalueArg)
	: newAttribute(attrnameArg, "", attrvalueArg)
{
}

docObj::newAttribute::newAttribute(const std::string &attrnameArg,
				   const std::string &attrnamespaceArg,
				   const std::string &attrvalueArg)
	: docAttribute(attrnameArg, attrnamespaceArg), attrvalue(attrvalueArg)
{
}

docObj::newAttribute::newAttribute(const std::string &attrnameArg,
				   const uriimpl &attrnamespaceArg,
				   const std::string &attrvalueArg)
	: newAttribute(attrnameArg, tostring(attrnamespaceArg), attrvalueArg)
{
}

docObj::newAttribute::newAttribute(const std::string &attrnameArg,
				   const char *attrnamespaceArg,
				   const std::string &attrvalueArg)
	: newAttribute(attrnameArg, std::string(attrnamespaceArg),
		       attrvalueArg)
{
}

docObj::newAttribute::~newAttribute()
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

docObj::readlockObj::~readlockObj()
{
}

docObj::writelockObj::writelockObj()
{
}

docObj::writelockObj::~writelockObj()
{
}

newdtd docObj::writelockObj::create_external_dtd(const std::string &external_id,
						 const char *system_id)
{
	return create_external_dtd(external_id, std::string(system_id));
}

newdtd docObj::writelockObj::create_external_dtd(const std::string &external_id,
						 const uriimpl &system_id)
{
	return create_external_dtd(external_id, tostring(system_id));
}

newdtd docObj::writelockObj::create_internal_dtd(const std::string &external_id,
						 const char *system_id)
{
	return create_internal_dtd(external_id, std::string(system_id));
}

newdtd docObj::writelockObj::create_internal_dtd(const std::string &external_id,
						 const uriimpl &system_id)
{
	return create_internal_dtd(external_id, tostring(system_id));
}

// Context pointer that's passed through via the libxml callback when
// saving an XML document.

class LIBCXX_HIDDEN impldocObj::save_impl {

 public:

	// We catch exceptions thrown while in a callback from libxml. The
	// caught exception is going to get propagated when libxml returns.

	exception e;

	bool caught_exception;
	save_to_callback &cb;

	save_impl(save_to_callback &cbArg)
		: caught_exception(false), cb(cbArg)
	{
	}

	~save_impl()
	{
	}
};

extern "C" {

	// Callback from xmlSaveFormatFileTo, delivering the next chunk of the
	// saved XML document. Pass the chunk through to the output iterator,
	// trap exceptions.

	static int save_file_write(void *context,
				   const char *buffer, int len)
	{
		auto impl=reinterpret_cast<impldocObj::save_impl *>(context);

		try {
			impl->cb.save(buffer, len);
		} catch (const exception &e)
		{
			impl->e=e;
			impl->caught_exception=true;
			return -1;
		} catch (...)
		{
			impl->e=EXCEPTION("Exception caught while saving XML document");
			impl->caught_exception=true;
		}
		return len;
	}

	static int close_file_dummy(void *context)
	{
		return 0;
	}
};

docObj::save_to_callback::save_to_callback()
{
}

docObj::save_to_callback::~save_to_callback()
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

	~readlockImplObj()
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
		if (n && n->parent && n->parent->type != XML_DOCUMENT_NODE)
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

	size_t get_child_element_count() const override
	{
		return n ? xmlChildElementCount(n):0;
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

	std::string name() const override
	{
		std::string name_str;

		if (n)
			name_str=n->name ?
				reinterpret_cast<const char *>(n->name):"";
		return name_str;
	}

	std::string prefix() const override
	{
		std::string prefix_str;

		if (n && n->type == XML_ELEMENT_NODE && n->ns &&
		    n->ns->prefix)
			prefix_str=reinterpret_cast<const char
						    *>(n->ns->prefix);

		return prefix_str;
	}

	std::string uri() const override
	{
		std::string uri_str;

		if (n && n->type == XML_ELEMENT_NODE && n->ns &&
		    n->ns->href)
			uri_str=reinterpret_cast<const char *>(n->ns->href);

		return uri_str;
	}

	std::string path() const override
	{
		std::string p;

		if (n)
			p=not_null(xmlGetNodePath(n), "getNodePath");
		return p;
	}

	std::string get_attribute(const std::string &attribute_name,
				  const uriimpl &attribute_namespace)
		const override
	{
		return get_attribute(attribute_name,
				     tostring(attribute_namespace));
	}

	std::string get_attribute(const std::string &attribute_name,
				  const std::string &attribute_namespace)
		const override
	{
		std::string s;

		if (n)
			s=null_ok(xmlGetNsProp(n,
					       reinterpret_cast<const xmlChar *>
					       (attribute_name.c_str()),
					       attribute_namespace.empty()
					       ? nullptr :
					       reinterpret_cast<const xmlChar *>
					       (attribute_namespace.c_str())));
		return s;
	}

	std::string get_attribute(const std::string &attribute_name)
		const override
	{
		std::string s;

		if (n)
			s=null_ok(xmlGetNoNsProp(n, reinterpret_cast
						 <const xmlChar *>
						 (attribute_name.c_str())));
		return s;
	}

	std::string get_any_attribute(const std::string &attribute_name) const
	{
		std::string s;

		if (n)
			s=null_ok(xmlGetProp(n,
					     reinterpret_cast<const xmlChar *>
					     (attribute_name.c_str())));
		return s;
	}

	void get_all_attributes(std::set<docAttribute> &attributes) const
	{
		if (!n || n->type != XML_ELEMENT_NODE)
			return;

		for (auto p=n->properties; p; p=p->next)
		{
			attributes.insert(docAttribute(get_str(p->name),
						       p->ns ?
						       get_str(p->ns->href):""
						       ));
		}
	}

	ref<xpathObj> get_xpath(const std::string &expr) override;

	bool is_blank() const
	{
		bool flag=true;

		if (n)
			flag=xmlIsBlankNode(n) ? true:false;
		return flag;
	}

	bool is_text() const
	{
		bool flag=false;

		if (n)
			flag=xmlNodeIsText(n) ? true:false;
		return flag;
	}

	std::string get_text() const
	{
		std::string s;

		if (n)
			s=null_ok(xmlNodeGetContent(n));
		return s;
	}

	std::string get_lang() const
	{
		std::string s;

		if (n)
			s=null_ok(xmlNodeGetLang(n));
		return s;
	}

	int get_space_preserve() const
	{
		int s=-1;

		if (n)
			s=xmlNodeGetSpacePreserve(n);
		return s;
	}

	std::string get_base() const
	{
		std::string s;

		if (n)
			s=null_ok(xmlNodeGetBase(impl->p, n));
		return s;
	}

	void save_file(const std::string &filename, bool format) const override
	{
		std::string tmpfile=filename + ".tmp";

		if (xmlSaveFormatFile(tmpfile.c_str(),
				      impl->p, format ? 1:0) < 0 ||
		    rename(tmpfile.c_str(), filename.c_str()) < 0)
		{
			auto e=SYSEXCEPTION(tmpfile);
			unlink(tmpfile.c_str());
			throw e;
		}
	}

	// Save to an output iterator. A template subclass of
	// save_to_callback will take care of delivering each chunk of the
	// XML document to the output iterator.

	void save_file(save_to_callback &cb, bool format) const override
	{
		save_impl impl(cb);

		// Interestingly enough, xmlSaveFormatFileTo is going to
		// take care of destroying the output buffer for us.

		auto io=xmlOutputBufferCreateIO(&save_file_write,
						&close_file_dummy,
						reinterpret_cast<void *>(&impl),
						xmlGetCharEncodingHandler(XML_CHAR_ENCODING_UTF8));

		if (!io)
			throw SYSEXCEPTION("xmlOutputBufferCreateIO");

		{
			error_handler::error trap_errors;

			auto result=xmlSaveFormatFileTo(io, this->impl->p,
							"UTF-8", format);

			if (impl.caught_exception)
				throw impl.e;

			trap_errors.check();

			if (result < 0)
				throw EXCEPTION(libmsg(_txt("Unable to save XML document")));
		}
	}

	ref<createnodeObj> create_child() override
	{
		throw EXCEPTION(libmsg(_txt("Somehow you ended up calling a virtual write method on a read object. Virtual objects are not for playing.")));
	}

	void remove() override
	{
		create_child();
	}

	virtual ref<obj> get_removal_mcguffin()
	{
		// We never remove anything here, so for the purposes of
		// the removal mcguffin, just return ourselves.

		return ref<obj>(this);
	}

	ref<createnodeObj> create_next_sibling() override
	{
		return create_child();
	}

	ref<createnodeObj> create_previous_sibling() override
	{
		return create_child();
	}

	void do_create_namespace(const std::string &uri,
				 const std::string &prefix) override
	{
		create_child();
	}

	void do_create_namespace(const std::string &uri,
				 const char *prefix) override
	{
		create_child();
	}

	void do_create_namespace(const std::string &uri,
				 const uriimpl &prefix) override
	{
		create_child();
	}

	void do_attribute(const newAttribute &attr) override
	{
		create_child();
	}

	void do_set_base(const std::string &uri) override
	{
		create_child();
	}

	void do_set_base(const char *uri) override
	{
		create_child();
	}

	void do_set_base(const uriimpl &uri) override
	{
		create_child();
	}

	void do_set_lang(const std::string &lang) override
	{
		create_child();
	}

	void do_set_space_preserve(bool) override
	{
		create_child();
	}

	dtd do_get_external_dtd_dtd() const override
	{
		return ref<dtdimplObj>::create(dtdimplObj::impl_external,
					       impl,
					       doc::base::
					       readlock(const_cast<
							readlockImplObj *>
							(this)));
	}

	dtd do_get_internal_dtd_dtd() const override
	{
		return ref<dtdimplObj>::create(dtdimplObj::impl_internal,
					       impl,
					       doc::base::
					       readlock(const_cast<
							readlockImplObj *>
							(this)));
	}

	newdtd do_get_external_dtd_newdtd() override
	{
		create_child();
		return newdtdptr();
	}

	newdtd do_get_internal_dtd_newdtd() override
	{
		create_child();
		return newdtdptr();
	}

	newdtd create_external_dtd(const std::string &external_id,
				      const std::string &system_id) override
	{
		create_child();
		return newdtdptr();
	}

	newdtd create_internal_dtd(const std::string &external_id,
				      const std::string &system_id) override
	{
		create_child();
		return newdtdptr();
	}

	void remove_external_dtd() override
	{
		create_child();
	}

	void remove_internal_dtd() override
	{
		create_child();
	}
};

class LIBCXX_HIDDEN impldocObj::createnodeImplObj : public createnodeObj {

 public:
	//! Superclass with the pointer to the document and current node

	readlockImplObj &lock;

	//! The constructor saves the pointer to the document and current node
	createnodeImplObj(readlockImplObj &lockArg) : lock(lockArg)
	{
	}

	~createnodeImplObj()
	{
	}

	void not_on_node() __attribute__((noreturn))
	{
		throw EXCEPTION(libmsg(_txt("Internal error: attempt to create a new XML node without a parent")));
	}

	// The methods here construct this guard object using the return
	// value from one of the XML methods. We expect a non-null xmlNodePtr.
	// A null NodePtr indicates an error, so we throw an exception.
	// This is then passed to create(), which is expected to add it to the
	// document, and reset it to null.
	//
	// If the destructor finds to be not null, it means that an exception
	// is being thrown, so we xmlFreeNode it().

	class guard {
	public:
		mutable xmlNodePtr n;

		guard(xmlNodePtr nArg, const std::string &method) : n(nArg)
		{
			if (!n)
				throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot create a new XML %1% element")), method));
		}

		~guard()
		{
			if (n)
				xmlFreeNode(n);
		}
	};

	virtual void create(const guard &n)=0;

	virtual xmlNsPtr do_search_ns(const xmlChar *prefix)=0;
	virtual xmlNsPtr do_search_ns_href(const xmlChar *uri)=0;

	xmlNsPtr search_ns(const std::string &prefix)
	{
		auto val=do_search_ns(prefix.size()
				      ? reinterpret_cast<const xmlChar *>
				      (prefix.c_str()):nullptr);

		if (!val)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Namespace prefix %1% not found")),
						   prefix));
		return val;
	}

	xmlNsPtr search_ns_href(const std::string &uri)
	{
		auto val=do_search_ns_href(uri.size()
					   ? reinterpret_cast<const xmlChar *>
					   (uri.c_str()):nullptr);

		if (!val)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Namespace URI %1% not found")),
						   uri));
		return val;
	}

	class created_element : public guard {

	public:
		created_element(xmlNsPtr ns, const std::string &name)
			: guard(xmlNewNode(ns,
					   reinterpret_cast<const xmlChar *>
					   (name.c_str())), "<" + name + ">")
		{
		}
		~created_element()
		{
		}
	};

	ref<createnodeObj> element(const newElement &e)
	{
		if (e.prefix_given)
		{
			// Create a new element in a new namespace

			created_element new_element=
				created_element(nullptr, e.name);

			// xmlNewNs makes the xmlNodePtr own the new namespace,
			// which is what we want. After it gets created, we just
			// manually stick it into ->ns.

			auto ns=xmlNewNs(new_element.n,
					 reinterpret_cast<const xmlChar *>
					 (e.uri.size() ? e.uri.c_str():nullptr),
					 reinterpret_cast<const xmlChar *>
					 (e.prefix.size() ? e.prefix.c_str()
					  :nullptr));

			if (!ns)
				throw EXCEPTION(libmsg(_txt("Invalid namespace")));

			new_element.n->ns=ns;
			create(new_element);
		}
		else
		{
			if (!e.uri.empty())
			{
				create(created_element(search_ns_href(e.uri),
						       e.name));
			}
			else
			{
				xmlNsPtr ns=nullptr;

				std::string n=e.name;

				size_t p=n.find(':');
				if (p != std::string::npos)
				{
					ns=search_ns(n.substr(0, p));
					n=n.substr(p+1);
				}

				create(created_element(ns, n));
			}
		}

		return ref<createnodeObj>(this);
	}

	ref<createnodeObj> cdata(const std::string &cdata) override
	{
		create(guard(xmlNewCDataBlock(lock.impl->p,
					      reinterpret_cast<const xmlChar *>
					      (cdata.c_str()), cdata.size()),
			     "cdata"));
		return ref<createnodeObj>(this);
	}

	ref<createnodeObj> text(const std::string &text) override
	{
		create(guard(xmlNewText(reinterpret_cast<const xmlChar *>
					(text.c_str())),
			     "text"));
		return ref<createnodeObj>(this);
	}

	ref<createnodeObj> entity(const std::string &text)
	{
		create(guard(xmlNewCharRef(lock.impl->p,
					   reinterpret_cast<const xmlChar *>
					   (text.c_str())),
			     "entity"));
		return ref<createnodeObj>(this);
	}

	void do_comment(const std::string &comment) override
	{
		create(guard(xmlNewComment(reinterpret_cast<const xmlChar *>
					   (comment.c_str())),
			     "comment"));
	}

	void do_processing_instruction(const std::string &name,
				       const std::string &content) override
	{
		create(guard(xmlNewPI(reinterpret_cast<const xmlChar *>
				      (name.c_str()),
				      reinterpret_cast<const xmlChar *>
				      (content.c_str())),
			     "processing_instruction"));
	}
};

// Factory that creates child nodes.

// create() implementation that creates a new child node.

class LIBCXX_HIDDEN impldocObj::createchildObj : public createnodeImplObj {

 public:

	createchildObj(readlockImplObj &rlockArg) : createnodeImplObj(rlockArg)
	{
	}

	~createchildObj()
	{
	}

	void create(const guard &n) override
	{
		if (!lock.n)
		{
			// Document root

			if (n.n->type != XML_ELEMENT_NODE)
				throw EXCEPTION(libmsg(_txt("Internal error: setting the document root to a non-element node")));

			if (xmlDocGetRootElement(lock.impl->p))
				throw EXCEPTION(libmsg(_txt("Write lock is not positioned on an existing node")));

			auto p=xmlDocSetRootElement(lock.impl->p, n.n);
			n.n=nullptr;
			if (p)
				xmlFreeNode(p);

			lock.n=xmlDocGetRootElement(lock.impl->p);
			// Position the lock on the root node, of course.
			return;
		}

		auto p=xmlAddChild(lock.n, n.n);

		if (!p)
			throw EXCEPTION(libmsg(_txt("Internal error: xmlAddChild()")));
		n.n=nullptr;
		lock.n=p;
	}

	xmlNsPtr do_search_ns(const xmlChar *prefix) override
	{
		return lock.n ? xmlSearchNs(lock.impl->p, lock.n, prefix)
			: nullptr;
	}

	xmlNsPtr do_search_ns_href(const xmlChar *uri) override
	{
		return lock.n ? xmlSearchNsByHref(lock.impl->p, lock.n, uri)
			: nullptr;
	}

	ref<createnodeObj> get_create_child()
	{
		return ref<createnodeObj>(this);
	}
};

// Factory that creates next sibling nodes.

// create() implementation that creates next sibling node.

class LIBCXX_HIDDEN impldocObj::createnextsiblingObj : public createnodeImplObj {

 public:
	createnextsiblingObj(readlockImplObj &rlockArg)
		: createnodeImplObj(rlockArg)
	{
	}

	~createnextsiblingObj()
	{
	}

	void create(const guard &n) override
	{
		if (!lock.n)
			not_on_node();

		auto p=xmlAddNextSibling(lock.n, n.n);

		if (!p)
			throw EXCEPTION(libmsg(_txt("Internal error: xmlAddNextSibling() failed")));
		n.n=nullptr;
		lock.n=p;
	}

	xmlNsPtr do_search_ns(const xmlChar *prefix) override
	{
		return lock.n && lock.n->parent
			? xmlSearchNs(lock.impl->p, lock.n->parent, prefix)
			: nullptr;
	}

	xmlNsPtr do_search_ns_href(const xmlChar *uri) override
	{
		return lock.n && lock.n->parent
			? xmlSearchNsByHref(lock.impl->p, lock.n->parent, uri)
			: nullptr;
	}

	ref<createnodeObj> get_create_next_sibling()
	{
		return ref<createnodeObj>(this);
	}
};

// Factory that creates previous sibling nodes.

// create() implementation that creates a previous sibling node.

class LIBCXX_HIDDEN impldocObj::createprevioussiblingObj : public createnodeImplObj {

 public:
	createprevioussiblingObj(readlockImplObj &rlockArg)
		: createnodeImplObj(rlockArg)
	{
	}

	~createprevioussiblingObj()
	{
	}

	void create(const guard &n) override
	{
		if (!lock.n)
			not_on_node();

		auto p=xmlAddPrevSibling(lock.n, n.n);

		if (!p)
			throw EXCEPTION(libmsg(_txt("Internal error: xmlAddPrevSibling() failed")));
		n.n=nullptr;
		lock.n=p;
	}

	xmlNsPtr do_search_ns(const xmlChar *prefix) override
	{
		return lock.n && lock.n->parent
			? xmlSearchNs(lock.impl->p, lock.n->parent, prefix)
			: nullptr;
	}

	xmlNsPtr do_search_ns_href(const xmlChar *uri) override
	{
		return lock.n && lock.n->parent
			? xmlSearchNsByHref(lock.impl->p, lock.n->parent, uri)
			: nullptr;
	}

	ref<createnodeObj> get_create_previous_sibling()
	{
		return ref<createnodeObj>(this);
	}
};

class LIBCXX_HIDDEN impldocObj::writelockImplObj
	: public readlockImplObj,
	  public createchildObj,
	  public createnextsiblingObj,
	  public createprevioussiblingObj {

 public:

	// Removal mcguffin

	// get_removal_mcguffin() returns this object, creating if it does
	// not exist. remove() releases this reference on the object.

	ptr<obj> removal_mcguffin;

	writelockImplObj(const ref<impldocObj> &implArg,
			 const ref<obj> &lockArg)
		: readlockImplObj(implArg, lockArg),
		createchildObj(static_cast<readlockImplObj &>(*this)),
		createnextsiblingObj(static_cast<readlockImplObj &>(*this)),
		createprevioussiblingObj(static_cast<readlockImplObj &>(*this))
	{
	}

	~writelockImplObj()
	{
	}

	ref<readlockObj> clone() const override
	{
		throw EXCEPTION(libmsg(_txt("Cannot clone a write lock")));
	}

	ref<createnodeObj> create_child() override
	{
		return get_create_child();
	}

	ref<createnodeObj> create_next_sibling() override
	{
		return get_create_next_sibling();
	}

	ref<createnodeObj> create_previous_sibling() override
	{
		return get_create_previous_sibling();
	}

	void do_create_namespace(const std::string &prefix,
				 const uriimpl &uri)
	{
		do_create_namespace(prefix, tostring(uri));
	}

	void do_create_namespace(const std::string &prefix,
				 const char *uri)
	{
		do_create_namespace(prefix, std::string(uri));
	}


	void do_create_namespace(const std::string &prefix,
				 const std::string &uri) override
	{
		if (n)
		{
			if (xmlNewNs(n,
				     reinterpret_cast<const xmlChar *>
				     (uri.size() ? uri.c_str():nullptr),
				     reinterpret_cast<const xmlChar *>
				     (prefix.size() ? prefix.c_str():nullptr)))
				return;
			throw EXCEPTION(gettextmsg
					(libmsg
					 (_txt
					  ("Namespace %1%:%2% cannot be added")
					  ), prefix, uri));
		}

		throw EXCEPTION(libmsg(_txt("create_namespace() called on a nonexistent node")));
	}

	void do_attribute(const newAttribute &attr) override
	{
		if (attr.attrnamespace.empty())
		{
			size_t p=attr.attrname.find(':');

			if (p == std::string::npos)
			{
				create_attribute(attr.attrname, nullptr,
						 attr.attrvalue);
				return;
			}

			xmlNsPtr ns;
			std::string prefix=attr.attrname.substr(0, p);

			if (n && (ns=xmlSearchNs(impl->p, n,
						 reinterpret_cast<const xmlChar
						 *>
						 (prefix.c_str()))) != 0)
			{
				create_attribute(attr.attrname.substr(p+1),
						 ns, attr.attrvalue);
				return;
			}

			throw EXCEPTION(gettextmsg
					(libmsg(_txt("Namespace prefix %1% not found")),
					 prefix));
		}

		if (!attr.attrnamespace.empty())
		{
			xmlNsPtr ns;

			if (n && (ns=xmlSearchNsByHref(impl->p, n,
						       reinterpret_cast<const
						       xmlChar *>
						       (attr.attrnamespace
							.c_str()))) != 0)
			{
				create_attribute(attr.attrname, ns,
						 attr.attrvalue);
				return;
			}
			throw EXCEPTION(gettextmsg(libmsg(_txt("Namespace URI %1% not found")),
						   attr.attrnamespace));
		}
		create_attribute(attr.attrname, nullptr, attr.attrvalue);
	}

	void create_attribute(const std::string &attrname,
			      const xmlNsPtr attrnamespace,
			      const std::string &attrvalue)
	{
		auto an=reinterpret_cast<const xmlChar *>(attrname.c_str());
		auto av=reinterpret_cast<const xmlChar *>(attrvalue.c_str());

		if (n && n->type == XML_ELEMENT_NODE)
		{
			if (!attrnamespace)
			{
				if (xmlNewProp(n, an, av))
					return;
			}
			else
			{
				if (xmlNewNsProp(n, attrnamespace, an, av))
					return;
			}
		}

		throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot set attribute %1%")),
					   attrname));
	}

	void remove() override
	{
		if (!n)
			return;

		auto parent=n->parent;
		if (!parent || parent->type == XML_DOCUMENT_NODE)
		{
			// Root node. Free DTDs, etc...

			auto p=impl->p->extSubset;

			if (p)
			{
				xmlUnlinkNode(reinterpret_cast<xmlNodePtr>(p));
				xmlFreeDtd(p);
			}

			if ((p=xmlGetIntSubset(impl->p)) != nullptr)
			{
				xmlUnlinkNode(reinterpret_cast<xmlNodePtr>(p));
				xmlFreeDtd(p);
			}
		}

		xmlUnlinkNode(n);
		xmlFreeNode(n);
		n=parent;
		removal_mcguffin=ptr<obj>();
	}

	ref<obj> get_removal_mcguffin() override
	{
		if (removal_mcguffin.null())
			removal_mcguffin=ref<obj>::create();
		return removal_mcguffin;
	}

	void do_set_base(const char *uri)
	{
		do_set_base(std::string(uri));
	}

	void do_set_base(const uriimpl &uri)
	{
		do_set_base(tostring(uri));
	}

	void do_set_base(const std::string &uri) override
	{
		if (!n || n->type != XML_ELEMENT_NODE)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot set the base URI"))));
		xmlNodeSetBase(n,
			       reinterpret_cast<const xmlChar *>(uri.c_str()));
	}

	void do_set_lang(const std::string &lang) override
	{
		if (!n || n->type != XML_ELEMENT_NODE)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot set the lang attribute"))));
		xmlNodeSetLang(n,
			       reinterpret_cast<const xmlChar *>(lang.c_str()));
	}

	void do_set_space_preserve(bool flag) override
	{
		if (!n || n->type != XML_ELEMENT_NODE)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot set the space preserve attribute"))));

		xmlNodeSetSpacePreserve(n, flag ? 1:0);
	}

	void do_parent() override
	{
		if (!get_parent())
			throw EXCEPTION(gettextmsg(libmsg(_txt("parent() called on the document's root node"))));
	}

	newdtd do_get_external_dtd_newdtd() override
	{
		return ref<newdtdimplObj>::create(dtdimplObj::impl_external,
						  impl,
						  doc::base::writelock(this));
	}

	newdtd do_get_internal_dtd_newdtd() override
	{
		return ref<newdtdimplObj>::create(dtdimplObj::impl_internal,
						  impl,
						  doc::base::writelock(this));
	}

	newdtd create_external_dtd(const std::string &external_id,
				      const std::string &system_id)
	{
		auto root=xmlDocGetRootElement(impl->p);

		if (!root)
			throw EXCEPTION(libmsg(_txt("Cannot install an external subset for an empty document")));

		xmlDtdPtr ptr;

		{
			error_handler::error trap_errors;
			const xmlChar *external_id_str=
				reinterpret_cast<const xmlChar *>
				(external_id.c_str()),
				*system_id_str=
				reinterpret_cast<const xmlChar *>
				(system_id.c_str());

			ptr=xmlNewDtd(impl->p,
				      root->name,
				      *external_id_str ?
				      external_id_str:nullptr,
				      system_id_str ? system_id_str:nullptr);

			trap_errors.check();
		}
		if (!ptr)
			throw EXCEPTION(libmsg(_txt("Cannot create external subset")));

		return ref<newdtdimplObj>::create(dtdimplObj::impl_external,
						  impl,
						  doc::base::writelock(this));
	}

	newdtd create_internal_dtd(const std::string &external_id,
				   const std::string &system_id)
	{
		auto root=xmlDocGetRootElement(impl->p);

		if (!root)
			throw EXCEPTION(libmsg(_txt("Cannot install an external subset for an empty document")));

		xmlDtdPtr ptr;

		{
			error_handler::error trap_errors;
			const xmlChar *external_id_str=
				reinterpret_cast<const xmlChar *>
				(external_id.c_str()),
				*system_id_str=
				reinterpret_cast<const xmlChar *>
				(system_id.c_str());

			ptr=xmlCreateIntSubset(impl->p,
					       root->name,
					       *external_id_str ?
					       external_id_str:nullptr,
					       system_id_str ? system_id_str
					       :nullptr);

			trap_errors.check();
		}
		if (!ptr)
			throw EXCEPTION(libmsg(_txt("Cannot create internal subset")));

		return ref<newdtdimplObj>::create(dtdimplObj::impl_internal,
						  impl,
						  doc::base::writelock(this));
	}

	void remove_external_dtd() override
	{
		auto p=impl->p->extSubset;

		if (p)
		{
			xmlUnlinkNode(reinterpret_cast<xmlNodePtr>(p));
			xmlFreeDtd(p);
		}
	}

	void remove_internal_dtd() override
	{
		auto p=xmlGetIntSubset(impl->p);

		if (p)
		{
			xmlUnlinkNode(reinterpret_cast<xmlNodePtr>(p));
			xmlFreeDtd(p);
		}
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

docObj::createnodeObj::createnodeObj()
{
}

docObj::createnodeObj::~createnodeObj()
{
}

ref<docObj::createnodeObj>
docObj::createnodeObj::attribute(const newAttribute &attr)
{
	do_attribute(attr);
	return ref<createnodeObj>(this);
}

ref<docObj::createnodeObj>
docObj::createnodeObj::create_namespace(const std::string &prefix,
					const std::string &uri)
{
	do_create_namespace(prefix, uri);
	return ref<createnodeObj>(this);
}

ref<docObj::createnodeObj>
docObj::createnodeObj::create_namespace(const std::string &prefix,
					const uriimpl &uri)
{
	do_create_namespace(prefix, uri);
	return ref<createnodeObj>(this);
}

ref<docObj::createnodeObj>
docObj::createnodeObj::create_namespace(const std::string &prefix,
					const char *uri)
{
	do_create_namespace(prefix, uri);

	return ref<createnodeObj>(this);
}

ref<docObj::createnodeObj>
docObj::createnodeObj::element(const newElement &e,
			       const std::vector<newAttribute> &a)
{
	element(e);

	for (const auto &aa:a)
	{
		do_attribute(aa);
	}
	return ref<createnodeObj>(this);
}

ref<docObj::createnodeObj>
docObj::createnodeObj::comment(const std::string &comment)
{
	do_comment(comment);
	return ref<createnodeObj>(this);
}

ref<docObj::createnodeObj>
docObj::createnodeObj::processing_instruction(const std::string &name,
					      const std::string &content)
{
	do_processing_instruction(name, content);
	return ref<createnodeObj>(this);
}

ref<docObj::createnodeObj>
docObj::createnodeObj::set_base(const std::string &uri)
{
	do_set_base(uri);
	return ref<createnodeObj>(this);
}

ref<docObj::createnodeObj> docObj::createnodeObj::set_base(const char *uri)
{
	do_set_base(uri);
	return ref<createnodeObj>(this);
}

ref<docObj::createnodeObj> docObj::createnodeObj::set_base(const uriimpl &uri)
{
	do_set_base(uri);
	return ref<createnodeObj>(this);
}


ref<docObj::createnodeObj> docObj::createnodeObj::set_lang(const std::string &lang)
{
	do_set_lang(lang);
	return ref<createnodeObj>(this);
}

ref<docObj::createnodeObj> docObj::createnodeObj::set_space_preserve(bool flag)
{
	do_set_space_preserve(flag);
	return ref<createnodeObj>(this);
}

ref<docObj::createnodeObj> docObj::createnodeObj::parent()
{
	do_parent();
	return ref<createnodeObj>(this);
}

////////////////////////////////////////////////////////////////////////////


class LIBCXX_HIDDEN impldocObj::xpathcontextObj : virtual public obj {

 public:
	ref<readlockImplObj> lock;

	xmlXPathContextPtr context;

	xpathcontextObj(ref<readlockImplObj> &&lockArg)
		: lock(std::move(lockArg)),
		context(xmlXPathNewContext(lock->impl->p))
		{
			if (!context)
				throw EXCEPTION(libmsg(_txt("xmlXpathNewContext failed")));
			if (!(context->node=lock->n))
				throw EXCEPTION(libmsg(_txt("Lock not positioned on a node")));
		}

	~xpathcontextObj()
	{
		if (context)
			xmlXPathFreeContext(context);
	}
};

docObj::xpathObj::xpathObj()
{
}


docObj::xpathObj::~xpathObj()
{
}

class LIBCXX_HIDDEN impldocObj::xpathImplObj : public xpathObj {

 public:
	ref<readlockImplObj> lock;
	std::string expression;
	xmlXPathObjectPtr objp;

	weakptr<ptr<obj>> mcguffin;

	xpathImplObj(const ref<xpathcontextObj> &context,
		     const std::string &expressionArg)
		: lock(context->lock), expression(expressionArg), objp(nullptr),
		mcguffin(lock->get_removal_mcguffin())
	{
		error_handler::error trap_errors;

		objp=xmlXPathEval(reinterpret_cast<const xmlChar *>
				 (expression.c_str()), context->context);

		trap_errors.check();

		if (!objp)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot parse xpath expression: %1%")),
						   expression));
	}

	~xpathImplObj()
	{
		if (objp)
			xmlXPathFreeObject(objp);
	}

	bool gone() const
	{
		return mcguffin.getptr().null();
	}

	size_t count() const override
	{
		return !gone() && objp && objp->nodesetval
			? objp->nodesetval->nodeNr:0;
	}

	void to_node() override
	{
		auto c=count();

		if (c < 1)
			throw EXCEPTION(gettextmsg(libmsg(_txt("%1% does not exist")),
						   expression));
		if (c > 1)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Found %1% %2% elements")),
						   c, expression));

		lock->n=objp->nodesetval->nodeTab[0];
	}

	void to_node(size_t n) override
	{
		if (n == 0 || n > count())
			throw EXCEPTION(gettextmsg(libmsg(_txt("%1% node #%2% does not exist")),
						   expression, n));

		lock->n=objp->nodesetval->nodeTab[n-1];
	}

	bool as_bool() const override
	{
		return !gone() && objp ? !!xmlXPathCastToBoolean(objp):false;
	}

	double as_number() const override
	{
		return !gone() && objp ? xmlXPathCastToNumber(objp):0;
	}

	std::string as_string() const override
	{
		return !gone() && objp ?
			not_null(xmlXPathCastToString(objp),
				 "xmlXPathCastToString"):"";
	}
};

ref<impldocObj::xpathObj>
impldocObj::readlockImplObj::get_xpath(const std::string &expr)
{
	return ref<xpathImplObj>::create(ref<xpathcontextObj>
					 ::create(ref<readlockImplObj>(this)),
					 expr);
}

#if 0
{
	{
#endif
	}
}
