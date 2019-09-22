/*
** Copyright 2013-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fditer.H"
#include "x/fd.H"
#include "x/to_string.H"
#include "x/uriimpl.H"
#include "x/messages.H"
#include "x/sysexception.H"
#include "x/weakptr.H"
#include "xml_internal.h"
#include "gettext_in.h"
#include <courier-unicode.h>
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

std::string not_null(xmlChar *p, const char *context,
		     const get_localeObj &l)
{
	if (!p)
		throw_last_error(context);

	std::string s(reinterpret_cast<const char *>(p));
	xmlFree(p);
	return l.get_global_locale()->fromutf8(s);
}

std::string null_ok(xmlChar *p,
		    const get_localeObj &l)
{
	std::string s(p ? reinterpret_cast<const char *>(p):"");
	xmlFree(p);
	return l.get_global_locale()->fromutf8(s);
}

std::string get_str(const xmlChar *p,
		    const get_localeObj &l)
{
	return l.get_global_locale()->fromutf8
		(p ? reinterpret_cast<const char *>(p):"");
}

#include "xml_element_type.h"

static const size_t element_offsets_l=
	sizeof(element_offsets)/sizeof(element_offsets[0]);

impldocObj::impldocObj(xmlDocPtr p, const const_locale &global_locale)
	: p{p}, global_locale{global_locale}, lock{sharedlock::create()}
{
}

impldocObj::~impldocObj()
{
	if (p)
		xmlFreeDoc(p);
}

docObj::docObj()=default;

docObj::~docObj()=default;

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
	: newElement(nameArg, "", to_string(uriArg))
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
	: newAttribute(attrnameArg, to_string(attrnamespaceArg), attrvalueArg)
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
	return ref<impldocObj>::create(xmlNewDoc((const xmlChar *)"1.0"),
				       locale::base::global());
}

doc docBase::create(const std::string_view &filename)
{
	return create(filename, "");
}

doc docBase::create(const std::string_view &filename,
		    const std::string_view &options)
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

docObj::readlockObj::readlockObj()=default;

docObj::readlockObj::~readlockObj()=default;

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
	return create_external_dtd(external_id, to_string(system_id));
}

newdtd docObj::writelockObj::create_internal_dtd(const std::string &external_id,
						 const char *system_id)
{
	return create_internal_dtd(external_id, std::string(system_id));
}

newdtd docObj::writelockObj::create_internal_dtd(const std::string &external_id,
						 const uriimpl &system_id)
{
	return create_internal_dtd(external_id, to_string(system_id));
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

typedef mpobj<xmlNodePtr, std::recursive_mutex> locked_xml_n_t;

class LIBCXX_HIDDEN removal_mcguffinObj;

struct removal_mcguffinObj : virtual public obj {


public:

	mpobj<bool> removed{false};
};

typedef ref<removal_mcguffinObj> removal_mcguffin;
typedef ptr<removal_mcguffinObj> removal_mcguffinptr;

class LIBCXX_HIDDEN impldocObj::readlockImplObj : public writelockObj,
						  public removal_mcguffinObj,
						  public get_localeObj {

 public:

	const ref<impldocObj> impl;
	const ref<obj> lock;

	// Return our locale object.
	const const_locale &get_global_locale() const final override
	{
		return impl->global_locale;
	}

	mutable locked_xml_n_t locked_xml_n;

	readlockImplObj(const ref<impldocObj> &implArg,
			const ref<obj> &lockArg)
		: impl(implArg), lock(lockArg), locked_xml_n(nullptr)
	{
	}

	~readlockImplObj()
	{
	}

	// Read functionality
	ref<readlockObj> clone() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		auto p=ref<readlockImplObj>::create(impl, lock);
		p->locked_xml_n=xml_n;
		return p;
	}

	doc clone_document(bool recursive) const override
	{
		auto xml_n=xmlCopyDoc(impl->p, recursive ? 1:0);

		if (!xml_n)
			throw_last_error("clone");

		try {
			return ref<impldocObj>::create(xml_n,
						       impl->global_locale);
		} catch (...)
		{
			xmlFreeDoc(xml_n);
			throw;
		}
	}

	bool get_root() override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		xml_n=xmlDocGetRootElement(impl->p);

		return !!xml_n;
	}

	bool get_parent() override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (xml_n && xml_n->parent && xml_n->parent->type != XML_DOCUMENT_NODE)
		{
			xml_n=xml_n->parent;
			return true;
		}
		return false;
	}

	bool get_first_child() override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (xml_n && xmlGetLastChild(xml_n) && xml_n->children)
		{
			xml_n=xml_n->children;
			return true;
		}
		return false;
	}

	bool get_last_child() override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (xml_n)
		{
			xmlNodePtr p;

			if ((p=xmlGetLastChild(xml_n)) != nullptr)
			{
				xml_n=p;
				return true;
			}
		}
		return false;
	}

	bool get_next_sibling() override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (xml_n && xml_n->next)
		{
			xml_n=xml_n->next;
			return true;
		}
		return false;
	}

	bool get_previous_sibling() override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (xml_n && xml_n->prev)
		{
			xml_n=xml_n->prev;
			return true;
		}

		return false;
	}

	bool get_first_element_child() override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (xml_n)
		{
			xmlNodePtr p;

			if ((p=xmlFirstElementChild(xml_n)) != nullptr)
			{
				xml_n=p;
				return true;
			}
		}
		return false;
	}

	bool get_last_element_child() override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (xml_n)
		{
			xmlNodePtr p;

			if ((p=xmlLastElementChild(xml_n)) != nullptr)
			{
				xml_n=p;
				return true;
			}
		}
		return false;
	}

	bool get_next_element_sibling() override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (xml_n)
		{
			xmlNodePtr p;

			if ((p=xmlNextElementSibling(xml_n)) != nullptr)
			{
				xml_n=p;
				return true;
			}
		}
		return false;
	}

	bool get_previous_element_sibling() override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (xml_n)
		{
			xmlNodePtr p;

			if ((p=xmlPreviousElementSibling(xml_n)) != nullptr)
			{
				xml_n=p;
				return true;
			}
		}
		return false;
	}

	size_t get_child_element_count() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		return xml_n ? xmlChildElementCount(xml_n):0;
	}

	std::string type() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		std::string type_str;

		if (xml_n)
		{
			size_t i=(size_t)xml_n->type;

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
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		std::string name_str;

		if (xml_n)
			name_str=get_str(xml_n->name, *this);
		return name_str;
	}

	std::string prefix() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		std::string prefix_str;

		if (xml_n && xml_n->type == XML_ELEMENT_NODE && xml_n->ns)
			prefix_str=get_str(xml_n->ns->prefix, *this);

		return prefix_str;
	}

	std::string uri() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		std::string uri_str;

		if (xml_n && xml_n->type == XML_ELEMENT_NODE && xml_n->ns)
			uri_str=get_str(xml_n->ns->href, *this);

		return uri_str;
	}

	std::string path() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		std::string p;

		if (xml_n)
			p=not_null(xmlGetNodePath(xml_n), "getNodePath",
			   *this);
		return p;
	}

	std::string get_attribute(const std::string &attribute_name,
				  const uriimpl &attribute_namespace)
		const override
	{
		return get_attribute(attribute_name,
				     to_string(attribute_namespace));
	}

	std::string get_attribute(const std::string &attribute_name,
				  const std::string &attribute_namespace)
		const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		std::string s;

		if (xml_n)
			s=null_ok(xmlGetNsProp(xml_n,
					       to_xml_char
					       {attribute_name, *this},
					       to_xml_char_or_null
					       {attribute_namespace, *this}),
				  *this);
		return s;
	}

	std::string get_attribute(const std::string &attribute_name)
		const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		std::string s;

		if (xml_n)
			s=null_ok(xmlGetNoNsProp(xml_n,
						 to_xml_char
						 {attribute_name, *this}),
				  *this);
		return s;
	}

	std::string get_any_attribute(const std::string &attribute_name) const
		override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		std::string s;

		if (xml_n)
			s=null_ok(xmlGetProp(xml_n,
					     to_xml_char{attribute_name,
								 *this}),
				  *this);
		return s;
	}

	void get_all_attributes(std::set<docAttribute> &attributes) const
		override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (!xml_n || xml_n->type != XML_ELEMENT_NODE)
			return;

		for (auto p=xml_n->properties; p; p=p->next)
		{
			attributes.insert(docAttribute(get_str(p->name, *this),
						       p->ns ?
						       get_str(p->ns->href,
							       *this):""
						       ));
		}
	}

	ref<xpathObj> get_xpath(const std::string &expr) override;

	bool is_blank() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		bool flag=true;

		if (xml_n)
			flag=xmlIsBlankNode(xml_n) ? true:false;
		return flag;
	}

	bool is_text() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		bool flag=false;

		if (xml_n)
			flag=xmlNodeIsText(xml_n) ? true:false;
		return flag;
	}

	std::string get_text() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		std::string s;

		if (xml_n)
			s=null_ok(xmlNodeGetContent(xml_n), *this);
		return s;
	}

	std::string get_lang() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		std::string s;

		if (xml_n)
			s=null_ok(xmlNodeGetLang(xml_n), *this);
		return s;
	}

	int get_space_preserve() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		int s=-1;

		if (xml_n)
			s=xmlNodeGetSpacePreserve(xml_n);
		return s;
	}

	std::string get_base() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		std::string s;

		if (xml_n)
			s=null_ok(xmlNodeGetBase(impl->p, xml_n), *this);
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

	virtual removal_mcguffin get_removal_mcguffin()
	{
		// We never remove anything here, so for the purposes of
		// the removal mcguffin, just return ourselves.

		return removal_mcguffin{this};
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

class LIBCXX_HIDDEN impldocObj::createnodeImplObj : public createnodeObj,
						    public get_localeObj {

 public:
	//! Superclass with the pointer to the document and current node

	readlockImplObj &lock;

	// Return our locale object.
	const const_locale &get_global_locale() const final override
	{
		return lock.get_global_locale();
	}

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
		mutable xmlNodePtr new_xml_n;

		guard(xmlNodePtr nArg, const char *method) : new_xml_n{nArg}
		{
			if (!new_xml_n)
				throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot create a new XML %1% element")), method));
		}

		~guard()
		{
			if (new_xml_n)
				xmlFreeNode(new_xml_n);
		}
	};

	virtual void create(const guard &xml_n)=0;

	virtual xmlNsPtr do_search_ns(const xmlChar *prefix)=0;
	virtual xmlNsPtr do_search_ns_href(const xmlChar *uri)=0;

	xmlNsPtr search_ns(const std::string &prefix)
	{
		auto val=do_search_ns(to_xml_char_or_null{prefix, *this});

		if (!val)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Namespace prefix %1% not found")),
						   prefix));
		return val;
	}

	xmlNsPtr search_ns_href(const std::string &uri)
	{
		auto val=do_search_ns_href(to_xml_char_or_null{uri, *this});

		if (!val)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Namespace URI %1% not found")),
						   uri));
		return val;
	}

	class created_element : public guard {

	public:
		created_element(xmlNsPtr ns, const std::string &name,
				const get_localeObj &gl)
			: created_element{ns, name, "<" + name + ">", gl}
		{
		}

		created_element(xmlNsPtr ns, const std::string &name,
				const std::string &tag,
				const get_localeObj &gl)
			: guard{xmlNewNode(ns, to_xml_char{name, gl}),
					   tag.c_str()}
		{
		}
		~created_element()
		{
		}
	};

	ref<createnodeObj> element(const newElement &e) override
	{
		if (e.prefix_given)
		{
			// Create a new element in a new namespace

			created_element new_element{nullptr, e.name, *this};

			// xmlNewNs makes the xmlNodePtr own the new namespace,
			// which is what we want. After it gets created, we just
			// manually stick it into ->ns.

			auto ns=xmlNewNs(new_element.new_xml_n,
					 to_xml_char_or_null{e.uri, *this},
					 to_xml_char_or_null{e.prefix, *this});

			if (!ns)
				throw EXCEPTION(libmsg(_txt("Invalid namespace")));

			new_element.new_xml_n->ns=ns;
			create(new_element);
		}
		else
		{
			if (!e.uri.empty())
			{
				create(created_element{search_ns_href(e.uri),
						       e.name, *this});
			}
			else
			{
				xmlNsPtr ns=nullptr;

				std::string xml_n=e.name;

				size_t p=xml_n.find(':');
				if (p != std::string::npos)
				{
					ns=search_ns(xml_n.substr(0, p));
					xml_n=xml_n.substr(p+1);
				}

				create(created_element{ns, xml_n, *this});
			}
		}

		return ref<createnodeObj>(this);
	}

	ref<createnodeObj> cdata(const std::string &cdata) override
	{
		to_xml_char cdata_xml{cdata, *this};

		create(guard(xmlNewCDataBlock(lock.impl->p,
					      cdata_xml, cdata_xml.size()),
			     "cdata"));
		return ref<createnodeObj>(this);
	}

	ref<createnodeObj> text(const std::string &text) override
	{
		create(guard(xmlNewText(to_xml_char{text, *this}),
			     "text"));
		return ref<createnodeObj>(this);
	}

	ref<createnodeObj> entity(const std::string &text) override
	{
		create(guard(xmlNewCharRef(lock.impl->p,
					   to_xml_char{text, *this}),
			     "entity"));
		return ref<createnodeObj>(this);
	}

	void do_comment(const std::string &comment) override
	{
		create(guard(xmlNewComment(to_xml_char{comment, *this}),
			     "comment"));
	}

	void do_processing_instruction(const std::string &name,
				       const std::string &content) override
	{
		create(guard(xmlNewPI(to_xml_char{name, *this},
				      to_xml_char{content, *this}),
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

	void create(const guard &new_xml_n) override
	{
		locked_xml_n_t::lock x_lock{lock.locked_xml_n};

		auto &xml_n=*x_lock;

		if (!xml_n)
		{
			// Document root

			if (new_xml_n.new_xml_n->type != XML_ELEMENT_NODE)
				throw EXCEPTION(libmsg(_txt("Internal error: setting the document root to a non-element node")));

			if (xmlDocGetRootElement(lock.impl->p))
				throw EXCEPTION(libmsg(_txt("Write lock is not positioned on an existing node")));

			auto p=xmlDocSetRootElement(lock.impl->p,
						    new_xml_n.new_xml_n);
			new_xml_n.new_xml_n=nullptr;
			if (p)
				xmlFreeNode(p);

			xml_n=xmlDocGetRootElement(lock.impl->p);
			// Position the lock on the root node, of course.
			return;
		}

		auto p=xmlAddChild(xml_n, new_xml_n.new_xml_n);

		if (!p)
			throw EXCEPTION(libmsg(_txt("Internal error: xmlAddChild()")));
		new_xml_n.new_xml_n=nullptr;
		xml_n=p;
	}

	xmlNsPtr do_search_ns(const xmlChar *prefix) override
	{
		locked_xml_n_t::lock x_lock{lock.locked_xml_n};

		auto &xml_n=*x_lock;

		return xml_n ? xmlSearchNs(lock.impl->p, xml_n, prefix)
			: nullptr;
	}

	xmlNsPtr do_search_ns_href(const xmlChar *uri) override
	{
		locked_xml_n_t::lock x_lock{lock.locked_xml_n};

		auto &xml_n=*x_lock;

		return xml_n ? xmlSearchNsByHref(lock.impl->p, xml_n, uri)
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

	void create(const guard &new_xml_n) override
	{
		locked_xml_n_t::lock x_lock{lock.locked_xml_n};

		auto &xml_n=*x_lock;

		if (!xml_n)
			not_on_node();

		auto p=xmlAddNextSibling(xml_n, new_xml_n.new_xml_n);

		if (!p)
			throw EXCEPTION(libmsg(_txt("Internal error: xmlAddNextSibling() failed")));
		new_xml_n.new_xml_n=nullptr;
		xml_n=p;
	}

	xmlNsPtr do_search_ns(const xmlChar *prefix) override
	{
		locked_xml_n_t::lock x_lock{lock.locked_xml_n};

		auto &xml_n=*x_lock;

		return xml_n && xml_n->parent
			? xmlSearchNs(lock.impl->p, xml_n->parent, prefix)
			: nullptr;
	}

	xmlNsPtr do_search_ns_href(const xmlChar *uri) override
	{
		locked_xml_n_t::lock x_lock{lock.locked_xml_n};

		auto &xml_n=*x_lock;

		return xml_n && xml_n->parent
			? xmlSearchNsByHref(lock.impl->p, xml_n->parent, uri)
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

	void create(const guard &new_xml_n) override
	{
		locked_xml_n_t::lock x_lock{lock.locked_xml_n};

		auto &xml_n=*x_lock;

		if (!xml_n)
			not_on_node();

		auto p=xmlAddPrevSibling(xml_n, new_xml_n.new_xml_n);

		if (!p)
			throw EXCEPTION(libmsg(_txt("Internal error: xmlAddPrevSibling() failed")));
		new_xml_n.new_xml_n=nullptr;
		xml_n=p;
	}

	xmlNsPtr do_search_ns(const xmlChar *prefix) override
	{
		locked_xml_n_t::lock x_lock{lock.locked_xml_n};

		auto &xml_n=*x_lock;

		return xml_n && xml_n->parent
			? xmlSearchNs(lock.impl->p, xml_n->parent, prefix)
			: nullptr;
	}

	xmlNsPtr do_search_ns_href(const xmlChar *uri) override
	{
		locked_xml_n_t::lock x_lock{lock.locked_xml_n};

		auto &xml_n=*x_lock;

		return xml_n && xml_n->parent
			? xmlSearchNsByHref(lock.impl->p, xml_n->parent, uri)
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

	mpobj<removal_mcguffinptr> current_removal_mcguffin;

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
				 const uriimpl &uri) override
	{
		do_create_namespace(prefix, to_string(uri));
	}

	void do_create_namespace(const std::string &prefix,
				 const char *uri) override
	{
		do_create_namespace(prefix, std::string(uri));
	}


	void do_create_namespace(const std::string &prefix,
				 const std::string &uri) override
	{
		locked_xml_n_t::lock lock{locked_xml_n};

		auto &xml_n=*lock;

		if (xml_n)
		{
			get_localeObj &SUPER=static_cast<readlockImplObj &>
				(*this);

			if (xmlNewNs(xml_n,
				     to_xml_char_or_null{uri, SUPER},
				     to_xml_char_or_null{prefix, SUPER}))
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
		locked_xml_n_t::lock lock{locked_xml_n};

		auto &xml_n=*lock;
		get_localeObj &SUPER=static_cast<readlockImplObj &>(*this);

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

			if (xml_n && (ns=xmlSearchNs(impl->p, xml_n,
						     to_xml_char{prefix, SUPER}
						     )) != 0)
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

			if (xml_n && (ns=xmlSearchNsByHref(impl->p, xml_n,
							   to_xml_char
							   {attr.attrnamespace,
								    SUPER}))
			    != 0)
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
		locked_xml_n_t::lock lock{locked_xml_n};

		auto &xml_n=*lock;

		get_localeObj &SUPER=static_cast<readlockImplObj &>(*this);

		to_xml_char an{attrname, SUPER};
		to_xml_char av{attrvalue, SUPER};

		if (xml_n && xml_n->type == XML_ELEMENT_NODE)
		{
			if (!attrnamespace)
			{
				if (xmlNewProp(xml_n, an, av))
					return;
			}
			else
			{
				if (xmlNewNsProp(xml_n, attrnamespace, an, av))
					return;
			}
		}

		throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot set attribute %1%")),
					   attrname));
	}

	void remove() override
	{
		locked_xml_n_t::lock lock{locked_xml_n};

		auto &xml_n=*lock;

		if (!xml_n)
			return;

		mpobj<removal_mcguffinptr>::lock
			mcguffin_lock{current_removal_mcguffin};

		if (*mcguffin_lock)
			(*mcguffin_lock)->removed=true;

		auto parent=xml_n->parent;
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

		xmlUnlinkNode(xml_n);
		xmlFreeNode(xml_n);
		xml_n=parent;

		*mcguffin_lock=removal_mcguffinptr{};
	}

	removal_mcguffin get_removal_mcguffin() override
	{
		locked_xml_n_t::lock lock{locked_xml_n};
		mpobj<removal_mcguffinptr>::lock
			mcguffin_lock{current_removal_mcguffin};

		if (!*mcguffin_lock)
			*mcguffin_lock=removal_mcguffin::create();

		removal_mcguffin m=*mcguffin_lock;

		return m;
	}

	void do_set_base(const char *uri) override
	{
		do_set_base(std::string(uri));
	}

	void do_set_base(const uriimpl &uri) override
	{
		do_set_base(to_string(uri));
	}

	void do_set_base(const std::string &uri) override
	{
		locked_xml_n_t::lock lock{locked_xml_n};

		auto &xml_n=*lock;

		if (!xml_n || xml_n->type != XML_ELEMENT_NODE)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot set the base URI"))));
		get_localeObj &SUPER=static_cast<readlockImplObj &>(*this);
		xmlNodeSetBase(xml_n, to_xml_char{uri, SUPER});
	}

	void do_set_lang(const std::string &lang) override
	{
		locked_xml_n_t::lock lock{locked_xml_n};

		auto &xml_n=*lock;

		if (!xml_n || xml_n->type != XML_ELEMENT_NODE)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot set the lang attribute"))));
		get_localeObj &SUPER=static_cast<readlockImplObj &>(*this);
		xmlNodeSetLang(xml_n, to_xml_char{lang, SUPER});
	}

	void do_set_space_preserve(bool flag) override
	{
		locked_xml_n_t::lock lock{locked_xml_n};

		auto &xml_n=*lock;

		if (!xml_n || xml_n->type != XML_ELEMENT_NODE)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot set the space preserve attribute"))));

		xmlNodeSetSpacePreserve(xml_n, flag ? 1:0);
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
				   const std::string &system_id) override
	{
		auto root=xmlDocGetRootElement(impl->p);

		if (!root)
			throw EXCEPTION(libmsg(_txt("Cannot install an external subset for an empty document")));

		xmlDtdPtr ptr;

		get_localeObj &SUPER=static_cast<readlockImplObj &>(*this);
		{
			error_handler::error trap_errors;

			ptr=xmlNewDtd(impl->p,
				      root->name,

				      to_xml_char_or_null{external_id, SUPER},
				      to_xml_char_or_null{system_id, SUPER});

			trap_errors.check();
		}
		if (!ptr)
			throw EXCEPTION(libmsg(_txt("Cannot create external subset")));

		return ref<newdtdimplObj>::create(dtdimplObj::impl_external,
						  impl,
						  doc::base::writelock(this));
	}

	newdtd create_internal_dtd(const std::string &external_id,
				   const std::string &system_id) override
	{
		auto root=xmlDocGetRootElement(impl->p);

		if (!root)
			throw EXCEPTION(libmsg(_txt("Cannot install an external subset for an empty document")));

		xmlDtdPtr ptr;

		get_localeObj &SUPER=static_cast<readlockImplObj &>(*this);
		{
			error_handler::error trap_errors;

			ptr=xmlCreateIntSubset
				(impl->p,
				 root->name,
				 to_xml_char_or_null{external_id, SUPER},
				 to_xml_char_or_null{system_id, SUPER});

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
					    lock->create_shared());
}

ref<docObj::writelockObj> impldocObj::writelock()
{
	return ref<writelockImplObj>::create(ref<impldocObj>(this),
					     lock->create_unique());
}

docObj::createnodeObj::createnodeObj()=default;

docObj::createnodeObj::~createnodeObj()=default;


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
			locked_xml_n_t::lock x_lock{lock->locked_xml_n};

			auto &xml_n=*x_lock;

			if (!context)
				throw EXCEPTION(libmsg(_txt("xmlXpathNewContext failed")));
			if (!(context->node=xml_n))
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

class LIBCXX_HIDDEN impldocObj::xpathImplObj : public xpathObj,
					       public get_localeObj {

 public:
	const ref<readlockImplObj> lock;

	// Return our locale object.
	const const_locale &get_global_locale() const final override
	{
		return lock->get_global_locale();
	}

	const std::string expression;
	const removal_mcguffin mcguffin;
	xmlXPathObjectPtr objp;

	struct mcguffin_lock : locked_xml_n_t::lock {

	private:
		mpobj<bool>::lock removal_lock;

	public:
		mcguffin_lock(const xpathImplObj &me)
			: locked_xml_n_t::lock{me.lock->locked_xml_n},
			  removal_lock{me.mcguffin->removed}
		{
		}

		inline bool gone() const
		{
			return *removal_lock;
		}

		using locked_xml_n_t::lock::operator*;
	};

	static auto create_objp(xpathImplObj &me,
				const ref<xpathcontextObj> &context)
	{
		mcguffin_lock lock{me};

		error_handler::error trap_errors;

		auto objp=xmlXPathEval(to_xml_char{me.expression, me},
				       context->context);

		trap_errors.check();

		if (!objp)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Cannot parse xpath expression: %1%")),
						   me.expression));

		return objp;
	}

	xpathImplObj(const ref<xpathcontextObj> &context,
		     const std::string &expressionArg)
		: lock(context->lock), expression(expressionArg),
		mcguffin(lock->get_removal_mcguffin()),
		objp{create_objp(*this, context)}
	{
	}

	~xpathImplObj()
	{
		if (objp)
			xmlXPathFreeObject(objp);
	}

	size_t count() const override
	{
		mcguffin_lock m_lock{*this};

		return count(m_lock);
	}

	size_t count(mcguffin_lock &m_lock) const
	{
		return !m_lock.gone() && objp && objp->nodesetval
			? objp->nodesetval->nodeNr:0;
	}

	void to_node() override
	{
		mcguffin_lock x_lock{*this};

		auto c=count(x_lock);

		auto &xml_n=*x_lock;

		if (c < 1)
			throw EXCEPTION(gettextmsg(libmsg(_txt("%1% does not exist")),
						   expression));
		if (c > 1)
			throw EXCEPTION(gettextmsg(libmsg(_txt("Found %1% %2% elements")),
						   c, expression));

		xml_n=objp->nodesetval->nodeTab[0];
	}

	void to_node(size_t n) override
	{
		mcguffin_lock x_lock{*this};

		auto &xml_n=*x_lock;

		if (n == 0 || n > count(x_lock))
			throw EXCEPTION(gettextmsg(libmsg(_txt("%1% node #%2% does not exist")),
						   expression, xml_n));

		xml_n=objp->nodesetval->nodeTab[n-1];
	}

	bool as_bool() const override
	{
		mcguffin_lock x_lock{*this};

		return !x_lock.gone() && objp ?
			!!xmlXPathCastToBoolean(objp):false;
	}

	double as_number() const override
	{
		mcguffin_lock x_lock{*this};

		return !x_lock.gone() && objp ? xmlXPathCastToNumber(objp):0;
	}

	std::string as_string() const override
	{
		mcguffin_lock x_lock{*this};


		return !x_lock.gone() && objp ?
			not_null(xmlXPathCastToString(objp),
				 "xmlXPathCastToString", *this):"";
	}
};

ref<impldocObj::xpathObj>
impldocObj::readlockImplObj::get_xpath(const std::string &expr)
{
	return ref<xpathImplObj>::create(ref<xpathcontextObj>
					 ::create(ref<readlockImplObj>(this)),
					 expr);
}

std::string quote_string_literal(const std::string_view &str, char quote)
{
	std::string p;

	size_t n=str.size();

	p.reserve(n + n/20 + 2); // Opening bid: add 5%

	p.push_back(quote);

	char double_quote[]={quote, quote, 0};

	for (const auto &c:str)
	{
		if (c == quote)
		{
			p += double_quote;
		}
		else if (c == '<')
		{
			p += "&lt;";
		}
		else if (c == '>')
		{
			p += "&gt;";
		}
		else if (c == '&')
		{
			p += "&amp;";
		}
		else if (c == '\0')
		{
			p += "&#0;"; // Script kiddy?
		}
		else
			p.push_back(c);
	}

	p.push_back(quote);
	return p;
}

#if 0
{
	{
#endif
	}
}
