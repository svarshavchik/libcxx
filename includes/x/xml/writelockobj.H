/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_writelockobj_H
#define x_xml_writelockobj_H

#include <x/uriimplfwd.H>
#include <x/obj.H>
#include <x/xml/dtdfwd.H>
#include <x/xml/newdtdfwd.H>
#include <x/xml/createnodefwd.H>
#include <x/xml/docobj.H>
#include <x/xml/readlockobj.H>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

//! A \ref ref "reference" to this object gets returned by writelock()

class writelockObj : public readlockObj {
public:
	//! Constructor
	writelockObj();

	//! Destructor
	~writelockObj();

	//! Create a new child element

	//! This call does not actually add a new child element.
	//! It returns a \ref createnode "INSERT_LIBX_NAMESPACE::xml::createnode"
	//! reference. Invoke one of its methods to actually add a
	//! new child element. The new child element is appended to
	//! this node's child list. The write lock remains positioned
	//! on this element, so invoking
	//! another method from the returned
	//! \ref createnode "INSERT_LIBX_NAMESPACE::xml::createnode"
	//! adds another child element to the same element.
	//!
	//! As a special case, invoking create_child() after obtaining
	//! a write lock on the document, but before invoking
	//! get_root(), installs a new document root element.
	//!
	//! \note
	//! This method returns a \ref createnode "INSERT_LIBX_NAMESPACE::xml::createnode"
	//! which holds a reference on this writelock, until it goes
	//! out of scope, and gets destroyed.

	virtual createnode create_child()=0;

	//! Remove element

	//! Remove the current element node that the writer lock
	//! is positioned on. The writer lock gets repositioned
	//! to the element's parent node. If the current node is the
	//! the document root node, the writer lock is no longer
	//! positioned on any node.
	//!
	//! No effect if the writer lock is not currently positioned
	//! on any node.
	virtual void remove()=0;

	//! Create a new next sibling element

	//! Similar to newchild(), but the new element becomes the
	//! current
	//! element's next sibling node, and the write lock also gets
	//! repositioned to the newly added element, so invoking
	//! another method from the returned
	//! \ref createnode "INSERT_LIBX_NAMESPACE::xml::createnode"
	//! adds another next sibling element, after the first one.
	//!
	//! \note
	//! This method returns a \ref createnode "INSERT_LIBX_NAMESPACE::xml::createnode"
	//! which holds a reference on this writelock, until it goes
	//! out of scope, and gets destroyed.

	virtual createnode create_next_sibling()=0;

	//! Create a new previous sibling element

	//! Similar to newchild(), but the new element becomes the
	//! current element's previous
	//! sibling node, and the write lock also gets
	//! repositioned to the newly added element, so invoking
	//! another method from the returned
	//! \ref createnode "INSERT_LIBX_NAMESPACE::xml::createnode"
	//! adds another previous sibling element, before the one
	//! that was just added.
	//!
	//! \note
	//! This method returns a \ref createnode "INSERT_LIBX_NAMESPACE::xml::createnode"
	//! which holds a reference on this writelock, until it goes
	//! out of scope, and gets destroyed.

	virtual createnode create_previous_sibling()=0;

	//! Add a new namespace to the current element node.

	void create_namespace(//! New namespace prefix
			      const std::string &prefix,

			      //! New namespace URI
			      const std::string &uri)
	{
		do_create_namespace(prefix, uri);
	}

	//! Add a new namespace to the current element node.

	//! \overload
	void create_namespace(//! New namespace prefix
			      const std::string &prefix,
			      //! New namespace URI
			      const uriimpl &uri)
	{
		do_create_namespace(prefix, uri);
	}

	//! Add a new namespace to the current element node.

	//! \overload

	void create_namespace(//! New namespace prefix
			      const std::string &prefix,
			      //! New namespace URI
			      const char *uri)
	{
		do_create_namespace(prefix, uri);
	}

	//! Create a new attribute in the current element node

	void attribute(//! Attribute
		       const new_attribute &attr)
	{
		do_attribute(attr);
	}

	//! Set the xml:base attribute
	void set_base(const std::string &uri)
	{
		do_set_base(uri);
	}

	//! Set the xml:base attribute
	void set_base(const char *uri)
	{
		do_set_base(uri);
	}

	//! Set the xml:base attribute
	void set_base(const uriimpl &uri)
	{
		do_set_base(uri);
	}

	//! Set the xml:lang attribute
	void set_lang(const std::string &lang)
	{
		do_set_lang(lang);
	}

	//! Set the xml::space attribute
	void set_space_preserve(bool flag)
	{
		do_set_space_preserve(flag);
	}

	//! Return the document's external dtd

	newdtd get_external_dtd();

	//! Return the document's internal dtd

	newdtd get_internal_dtd();

	//! Create the document's external dtd
	virtual newdtd create_external_dtd(const std::string &external_id,
					   const std::string &system_id)=0;

	//! Create the document's external dtd

	//! \overload
	//!
	newdtd create_external_dtd(const std::string &external_id,
				   const char *system_id);

	//! Create the document's external dtd

	//! \overload
	//!
	newdtd create_external_dtd(const std::string &external_id,
				   const uriimpl &system_id);

	//! Create the document's internal dtd
	virtual newdtd create_internal_dtd(const std::string &external_id,
					   const std::string &system_id)=0;

	//! Create the document's internal dtd

	//! \overload
	//!
	newdtd create_internal_dtd(const std::string &external_id,
				   const char *system_id);

	//! Create the document's internal dtd

	//! \overload
	//!
	newdtd create_internal_dtd(const std::string &external_id,
				   const uriimpl &system_id);

	//! Remove any existing external dtd
	virtual void remove_external_dtd()=0;

	//! Remove any existing internal dtd
	virtual void remove_internal_dtd()=0;

private:
	//! Implement create_namespace().
	virtual void do_create_namespace(//! New namespace prefix
					 const std::string &prefix,

					 //! New namespace URI
					 const std::string &uri)=0;

	//! Implement create_namespace().

	//! \overload
	virtual void do_create_namespace(//! New namespace prefix
					 const std::string &prefix,
					 //! New namespace URI
					 const uriimpl &uri)=0;

	//! Implement create_namespace().

	//! \overload

	virtual void do_create_namespace(//! New namespace prefix
					 const std::string &prefix,
					 //! New namespace URI
					 const char *uri)=0;

	//! Implement attribute().
	virtual void do_attribute(//! Attribute
				  const new_attribute &attr)=0;

	//! Implement set_base().
	virtual void do_set_base(const std::string &uri)=0;

	//! Implement set_base().
	virtual void do_set_base(const char *uri)=0;

	//! Implement set_base().
	virtual void do_set_base(const uriimpl &uri)=0;

	//! Implement set_lang().
	virtual void do_set_lang(const std::string &lang)=0;

	//! Implement set_space_preserve().
	virtual void do_set_space_preserve(bool)=0;

	//! Implement get_external_dtd
	virtual newdtd do_get_external_dtd_newdtd()=0;

	//! Implement get_internal_dtd
	virtual newdtd do_get_internal_dtd_newdtd()=0;
};
#if 0
{
#endif
}
#endif
