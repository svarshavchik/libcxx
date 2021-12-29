/*
** Copyright 2013-2021 Double Precision, Inc.
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
#include "x/weaklist.H"
#include "x/functional.H"
#include "xml_internal.h"
#include "gettext_in.h"
#include <courier-unicode.h>
#include <libxml/xpathInternals.h>
#include <sstream>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
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

save_to_callback::save_to_callback()=default;

save_to_callback::~save_to_callback()=default;

typedef mpobj<xmlNodePtr, std::recursive_mutex> locked_xml_n_t;

class LIBCXX_HIDDEN impldocObj::readlockImplObj : public writelockObj,
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

	~readlockImplObj()=default;

	virtual void register_xpath(const ref<impldocObj::xpathImplObj> &)
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

	std::string get_attribute(const std::string_view &attribute_name,
				  const explicit_arg<uriimpl> &attribute_namespace)
		const override
	{
		return get_attribute(attribute_name,
				     to_string(attribute_namespace));
	}

	cloned_node get_cloned_node() const override
	{
		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (!xml_n)
			throw EXCEPTION("clone(): no current node");

		return { xmlCopyNode(xml_n, 1) };
	}
	std::string get_attribute(const std::string_view &attribute_name,
				  const std::string_view &attribute_namespace)
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

	std::string get_attribute(const std::string_view &attribute_name)
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

	std::string get_any_attribute(const std::string_view &attribute_name)
		const override
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

	get_all_attributes_t get_all_attributes() const override
	{
		get_all_attributes_t attributes;

		locked_xml_n_t::lock x_lock{locked_xml_n};

		auto &xml_n=*x_lock;

		if (!xml_n || xml_n->type != XML_ELEMENT_NODE)
			return attributes;

		for (auto p=xml_n->properties; p; p=p->next)
		{
			attributes.emplace(get_str(p->name, *this),
					   p->ns ? get_str(p->ns->href, *this)
					   :"");
		}

		return attributes;
	}

	std::unordered_map<std::string,
			   std::string> namespaces() const override
	{
		std::unordered_map<std::string, std::string> ret;

		auto l=get_global_locale();

		locked_xml_n_t::lock x_lock{locked_xml_n};

		extract_namespaces(
			x_lock,
			[&]
			(const xmlChar *prefix, const xmlChar *ns)
			{
				std::string ns_s{
					reinterpret_cast<const char *>(ns)
				};

				std::string pf_s{
					reinterpret_cast<const char *>(prefix)
				};

				ret.emplace(l->fromutf8(pf_s),
					    l->fromutf8(ns_s));
			});

		return ret;
	}

	template<typename F> void extract_namespaces(
		locked_xml_n_t::lock &x_lock,
		F &&f) const
	{
		do_extract_namespaces(
			x_lock,
			make_function<void (const xmlChar *, const xmlChar *)>
			(std::forward<F>(f)));
	}

	void do_extract_namespaces(locked_xml_n_t::lock &x_lock,
				   const function<void (const xmlChar *,
							const xmlChar *)> &f)
		const
	{

		for (auto xml_n=*x_lock; xml_n; xml_n=xml_n->parent)
			for (auto def=xml_n->nsDef; def; def=def->next)
				f(def->prefix, def->href);
	}

	xpath get_xpath(const std::string_view &expr) override;

	xpath get_xpath(const std::string_view &expr,
			const std::unordered_map<std::string,
			uriimpl> &namespaces) override;

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

	createnode create_child() override
	{
		throw EXCEPTION(libmsg(_txt("Somehow you ended up calling a virtual write method on a read object. Virtual objects are not for playing.")));
	}

	void remove() override
	{
		create_child();
	}

	createnode create_next_sibling() override
	{
		return create_child();
	}

	createnode create_previous_sibling() override
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

	void do_attribute(const new_attribute &attr) override
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
					       const_ref{this});
	}

	dtd do_get_internal_dtd_dtd() const override
	{
		return ref<dtdimplObj>::create(dtdimplObj::impl_internal,
					       impl,
					       const_ref{this});
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

	createnode element(const new_element &e) override
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

		return createnode{this};
	}

	createnode cdata(const std::string_view &cdata) override
	{
		to_xml_char cdata_xml{cdata, *this};

		create(guard(xmlNewCDataBlock(lock.impl->p,
					      cdata_xml, cdata_xml.size()),
			     "cdata"));
		return createnode{this};
	}

	createnode cdata(const std::u32string_view &cdata) override
	{
		to_xml_char cdata_xml{cdata};

		create(guard(xmlNewCDataBlock(lock.impl->p,
					      cdata_xml, cdata_xml.size()),
			     "cdata"));
		return createnode{this};
	}

	createnode text(const std::string_view &text) override
	{
		create(guard(xmlNewText(to_xml_char{text, *this}),
			     "text"));
		return createnode{this};
	}

	createnode text(const std::u32string_view &text) override
	{
		create(guard(xmlNewText(to_xml_char{text}),
			     "text"));
		return createnode{this};
	}

	createnode clone(const const_ref<readlockObj> &lock) override
	{
		create(guard(lock->get_cloned_node().node, "clone"));
		return createnode{this};
	}

	createnode entity(const std::string_view &text) override
	{
		create(guard(xmlNewCharRef(lock.impl->p,
					   to_xml_char{text, *this}),
			     "entity"));
		return createnode{this};
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

	createnode get_create_child()
	{
		return createnode{this};
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

	createnode get_create_next_sibling()
	{
		return createnode{this};
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

	createnode get_create_previous_sibling()
	{
		return createnode{this};
	}
};

//////////////////////////////////////////////////////////////////////////////

class LIBCXX_HIDDEN impldocObj::xpathImplObj : public xpathObj,
					       public get_localeObj {

 public:
	const ref<readlockImplObj> lock;

	// Return our locale object.
	const const_locale &get_global_locale() const final override;

	const std::string expression;

	// Access to nodes_under_lock requires a locked_xml_n_t. This is
	// all nodes in a nodeset, and this gets synchronized with remove()
	// and all other operation that access the write lock's XML node.

	std::vector<xmlNodePtr> nodes_under_lock;

	inline auto &nodes(locked_xml_n_t::lock &)
	{
		return nodes_under_lock;
	}

	inline const auto &nodes(locked_xml_n_t::lock &) const
	{
		return nodes_under_lock;
	}

	xpathImplObj(const ref<readlockImplObj> &lock,
		     const xpathcontext &context,
		     const std::string_view &expressionArg);

	~xpathImplObj();

	void about_to_remove(locked_xml_n_t::lock &lock);

	size_t count() const override;

	size_t count(locked_xml_n_t::lock &m_lock) const;

	void to_node() override;

	void to_node(size_t n) override;
};

class LIBCXX_HIDDEN impldocObj::writelockImplObj
	: public readlockImplObj,
	  public createchildObj,
	  public createnextsiblingObj,
	  public createprevioussiblingObj {

 public:

	typedef weaklist<impldocObj::xpathImplObj> current_xpaths_t;

	const current_xpaths_t current_xpaths;

	void register_xpath(const ref<impldocObj::xpathImplObj> &xp) override
	{
		current_xpaths->push_back(xp);
	}

	writelockImplObj(const ref<impldocObj> &implArg,
			 const ref<obj> &lockArg)
		: readlockImplObj(implArg, lockArg),
		createchildObj(static_cast<readlockImplObj &>(*this)),
		createnextsiblingObj(static_cast<readlockImplObj &>(*this)),
		createprevioussiblingObj(static_cast<readlockImplObj &>(*this)),
		current_xpaths{current_xpaths_t::create()}
	{
	}

	~writelockImplObj()=default;


	ref<readlockObj> clone() const override
	{
		throw EXCEPTION(libmsg(_txt("Cannot clone a write lock")));
	}

	createnode create_child() override
	{
		return get_create_child();
	}

	createnode create_next_sibling() override
	{
		return get_create_next_sibling();
	}

	createnode create_previous_sibling() override
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

	void do_attribute(const new_attribute &attr) override
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
			      const new_attribute::value_t &attrvalue)
	{
		locked_xml_n_t::lock lock{locked_xml_n};

		auto &xml_n=*lock;

		get_localeObj &SUPER=static_cast<readlockImplObj &>(*this);

		to_xml_char an{attrname, SUPER};

		auto av=std::visit([&]
				   (const auto &s)
		{
			return to_xml_char{s, SUPER};
		}, attrvalue);

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

		for (const auto &xp : *current_xpaths)
		{
			auto p=xp.getptr();

			if (!p)
				continue;

			p->about_to_remove(lock);
		}

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
						  ref<writelockObj>(this));
	}

	newdtd do_get_internal_dtd_newdtd() override
	{
		return ref<newdtdimplObj>::create(dtdimplObj::impl_internal,
						  impl,
						  ref<writelockObj>(this));
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
						  ref<writelockObj>(this));
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
						  ref<writelockObj>(this));
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

readlock impldocObj::readlock()
{
	return ref<readlockImplObj>::create(ref<impldocObj>(this),
					    lock->create_shared());
}

ref<writelockObj> impldocObj::writelock()
{
	return ref<writelockImplObj>::create(ref<impldocObj>(this),
					     lock->create_unique());
}

////////////////////////////////////////////////////////////////////////////

// Temporary object that gets constructed when creating a new xpath nodeset
//
// The first thing it does is acquire a node lock, then it calls
// xmlXPathNewContext to create the context for the xpath.

class LIBCXX_HIDDEN impldocObj::xpathcontext {

 public:
	locked_xml_n_t::lock x_lock;
	xmlXPathContextPtr context;

	xpathcontext(const ref<readlockImplObj> &lock) :
		x_lock{lock->locked_xml_n},
		context{xmlXPathNewContext(lock->impl->p)}
	{
		locked_xml_n_t::lock x_lock{lock->locked_xml_n};

		auto &xml_n=*x_lock;

		if (!context)
			throw EXCEPTION(libmsg(_txt("xmlXpathNewContext failed")
					));
		if (!(context->node=xml_n))
			throw EXCEPTION(libmsg(_txt("Lock not positioned on "
						    "a node")));
	}

	~xpathcontext()
	{
		if (context)
			xmlXPathFreeContext(context);
	}
};


const const_locale &impldocObj::xpathImplObj::get_global_locale() const
{
	return lock->get_global_locale();
}

// Helper object used when creating an xpath nodeset. Takes a context,
// then evaluates the xpath expression.
//
// nodes() retrieves the xmlNode pointers in the xpath nodeset and places
// them in a convenient vector.

struct LIBCXX_HIDDEN create_objp {

	xmlXPathObjectPtr created_objp=nullptr;

	create_objp(
		impldocObj::xpathImplObj &me,
		const impldocObj::xpathcontext &context)
	{
		error_handler::error trap_errors;

		auto objp=xmlXPathEval(to_xml_char{me.expression, me},
				       context.context);

		trap_errors.check();

		if (!objp)
			throw EXCEPTION(
				gettextmsg
				(libmsg
				 (_txt
				  ("Cannot parse xpath expression: %1%")),
				 me.expression));

		created_objp=objp;
	}

	std::vector<xmlNodePtr> nodes()
	{
		if (!created_objp || !created_objp->nodesetval)
			return {};

		return {created_objp->nodesetval->nodeTab,
			created_objp->nodesetval->nodeTab +
			created_objp->nodesetval->nodeNr};
	}

	~create_objp()
	{
		if (created_objp)
			xmlXPathFreeObject(created_objp);
	}
};

impldocObj::xpathImplObj::xpathImplObj(const ref<readlockImplObj> &lock,
				       const xpathcontext &context,
				       const std::string_view &expressionArg)
	: lock{lock}, expression{expressionArg},
	  nodes_under_lock{create_objp{*this, context}.nodes()}
{
}

impldocObj::xpathImplObj::~xpathImplObj()=default;

void impldocObj::xpathImplObj::about_to_remove(locked_xml_n_t::lock &lock)
{
	auto &nodes=this->nodes(lock);

	std::equal_to<xmlNodePtr> ptr_equal;

	// Iterate over the nodes in a nodeset, for each node: check if that
	// node or any of its parents is the one that's about to be removed.
	// If so invalidate the pointer in the nodeset by setting it to null.

	for (auto &ptr:nodes)
	{
		for (auto p=ptr; p; p=p->parent)
		{
			if (ptr_equal(p, *lock))
			{
				ptr=nullptr;
				break;
			}
		}
	}
}

size_t impldocObj::xpathImplObj::count() const
{
	locked_xml_n_t::lock m_lock{lock->locked_xml_n};

	return count(m_lock);
}

size_t impldocObj::xpathImplObj::count(locked_xml_n_t::lock &m_lock) const
{
	return nodes(m_lock).size();
}

void impldocObj::xpathImplObj::to_node()
{
	locked_xml_n_t::lock x_lock{lock->locked_xml_n};

	auto c=count(x_lock);

	auto &xml_n=*x_lock;

	if (c < 1)
		throw EXCEPTION(gettextmsg(libmsg(_txt("%1% does not exist")),
					   expression));
	if (c > 1)
		throw EXCEPTION(gettextmsg(libmsg(_txt("Found %1% %2% elements")
					   ), c, expression));

	auto &n=nodes(x_lock)[0];

	if (!n)
		throw EXCEPTION(gettextmsg(libmsg(_txt("%1% was removed")),
					   expression));

	xml_n=n;
}

void impldocObj::xpathImplObj::to_node(size_t n)
{
	locked_xml_n_t::lock x_lock{lock->locked_xml_n};

	auto &nodes=this->nodes(x_lock);

	auto &xml_n=*x_lock;

	if (n == 0 || n > count(x_lock))
		throw EXCEPTION(gettextmsg(libmsg(_txt("%1% node #%2% does not"
						       " exist")),
					   expression, n));

	if (!nodes[n-1])
	{
		throw EXCEPTION(gettextmsg(libmsg(_txt("%1% was removed")),
					   expression));
	}

	xml_n=nodes[n-1];
}

xpath
impldocObj::readlockImplObj::get_xpath(const std::string_view &expr)
{
	auto lock=ref{this};

	xpathcontext ctx{lock};

	extract_namespaces
		(ctx.x_lock,
		 [&]
		 (const xmlChar *prefix, const xmlChar *ns)
		{
			error_handler::error trap_errors;

			auto res=xmlXPathRegisterNs(
				ctx.context,
				prefix,
				ns
			);

			trap_errors.check();

			if (res)
			{
				throw EXCEPTION(libmsg(_txt("Namespace copy "
							    "failed")));
			}
		});

	auto new_xpath=ref<xpathImplObj>::create(lock, ctx, expr);

	lock->register_xpath(new_xpath);

	return new_xpath;
}

xpath
impldocObj::readlockImplObj::get_xpath(
	const std::string_view &expr,
	const std::unordered_map<std::string, uriimpl> &namespaces)
{
	auto lock=ref{this};

	xpathcontext ctx{lock};

	for (const auto &[prefix, ns] : namespaces)
	{
		error_handler::error trap_errors;

		auto res=xmlXPathRegisterNs(
			ctx.context,
			to_xml_char{prefix, *this},
			to_xml_char{to_string(ns), *this});

		trap_errors.check();

		if (res)
		{
			throw EXCEPTION(gettextmsg(libmsg(_txt("Prefix %1%")),
						   prefix));
		}
	}
	auto new_xpath=ref<xpathImplObj>::create(lock, ctx, expr);

	lock->register_xpath(new_xpath);

	return new_xpath;
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
#endif
}
