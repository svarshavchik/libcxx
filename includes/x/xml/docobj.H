/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_docobj_H
#define x_xml_docobj_H

#include <x/obj.H>
#include <x/xml/dtd.H>
#include <x/xml/newdtd.H>
#include <set>
#include <vector>
namespace LIBCXX_NAMESPACE {

	class uriimpl;
	namespace xml {
#if 0
	};
};
#endif

//! A parsed XML document.

class docObj : virtual public obj {

protected:
	//! Base class for an internal callback from libxml

	//! \internal
	class save_to_callback {
	public:
		//! Constructor
		save_to_callback();

		//! Destructor
		~save_to_callback();

		//! Save a chunk of formatted XML
		virtual void save(const char *, int)=0;
	};
public:
	//! Constructor
	docObj();

	//! Destructor
	~docObj();

	//! Helper class for creating a new element

	//! Refer to this class as
	//! \c INSERT_LIBX_NAMESPACE::xml::doc::base::newelement.
	//! See \ref docBase::writelock "INSERT_LIBX_NAMESPACE::doc::base::writelock"
	//! for more information.

	class newElement {

	public:
		//! New element's name
		std::string name;
		//! New element's prefix
		std::string prefix;

		//! New element's URI
		std::string uri;

		//! Whether this helper class was constructed with a given prefix.
		bool prefix_given;

		//! Create a new element in an unnamed namespace.
		newElement(//! New element's name
			   const std::string &nameArg);

		//! Create a new element in a given namespace

		newElement(//! New element's name
			   const std::string &namearg,

			   //! New element's URI. This URI must be opened in
			   //! some parent node of the new element.
			   const std::string &uriArg);

		//! Create a new element in a given namespace

		newElement(//! New element's name
			   const std::string &nameArg,

			   //! New element's URI. This URI must be opened in
			   //! some parent node of the new element.
			   const char *uriArg);

		//! Create a new element in a given namespace

		newElement(//! New element's name
			   const std::string &nameArg,

			   //! New element's URI. This URI must be opened in
			   //! some parent node of the new element.
			   const uriimpl &uriArg);

		//! Create a new element in a new namespace
		newElement(//! New element's name
			   const std::string &nameArg,

			   //! The new namespace is opened in this element,
			   //! this is its prefix.
			   const std::string &prefixArg,

			   //! The new namespace's URI
			   const std::string &uriArg);

		//! Destructor

		~newElement();
	};


	//! An attribute of an XML element node.

	//! Refer to this class as INSERT_LIBX_NAMESPACE::xml::doc::base::attribute

	class docAttribute {
	public:
		//! Attribute name
		std::string attrname;

		//! Attribute namespace. You should be able to convert this to
		//! a \ref uriimpl "INSERT_LIBX_NAMESPACE::uriimpl".

		std::string attrnamespace;

		//! Constructor
		docAttribute(const std::string &attrnameArg,
			     const std::string &attrnamespaceArg);

		//! Destructor
		~docAttribute();

		//! Comparison operator for associative containers.

		bool operator<(const docAttribute &) const;
	};

	//! A new attribute of an XML document

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::xml::doc::base::newattribute.
	//! See \ref docBase::writelock "INSERT_LIBX_NAMESPACE::doc::base::writelock"
	//! for more information.

	class newAttribute : public docAttribute {
	public:
		//! Attribute value
		std::string attrvalue;

		//! Create a new attribute

		newAttribute(//! Attribute name

			     //! It can be specified as 'prefix:name', creating
			     //! the attribute in the given namespace. A
			     //! namespace with the given prefix must be opened
			     //! by this element or one of its parent elements.
			     //! Otherwise the attribute gets created in the
			     //! unknown namespace.
			     const std::string &attrnameArg,

			     //! Attribute's value
			     const std::string &attrvalueArg);

		//! Create a new attribute in a given namespace

		newAttribute(//! Attribute name
			     const std::string &attrnameArg,

			     //! Attribute's namespace URI.

			     //! A namespace with the given URI must be opened
			     //! by this element or one of its parent elements.

			     const std::string &attrnamespaceArg,

			     //! Attribute's value
			     const std::string &attrvalueArg);

		//! Create a new attribute in a given namespace

		newAttribute(//! Attribute name
			     const std::string &attrnameArg,

			     //! Attribute's namespace URI.

			     //! A namespace with the given URI must be opened
			     //! by this element or one of its parent elements.

			     const uriimpl &attrnamespaceArg,

			     //! Attribute's value
			     const std::string &attrvalueArg);

		//! Create a new attribute in a given namespace

		newAttribute(//! Attribute name
			     const std::string &attrnameArg,

			     //! Attribute's namespace URI.

			     //! A namespace with the given URI must be opened
			     //! by this element or one of its parent elements.

			     const char *attrnamespaceArg,

			     //! Attribute's value
			     const std::string &attrvalueArg);

		//! Destructor
		~newAttribute();

		//! Comparison operator for associative containers.

		using docAttribute::operator<;
	};

	class readlockObj;

	//! The result of get_xpath().

	//! get_xpath() returns a \ref docBase::xpath "INSERT_LIBX_NAMESPACE::doc::xml::base::xpath",
	//! which is a reference to this object.

	class xpathObj : virtual public obj {

	public:
		xpathObj();
		~xpathObj();

		//! How many nodes in the resulting nodeset.
		virtual size_t count() const=0;

		//! Position the lock to the only node in the nodeset.

		virtual void to_node()=0;

		//! Position the lock to one of the nodes in the nodeset.

		virtual void to_node(//! This is 1-based, not 0-based index.
				     size_t n)=0;


		//! Evaluate the xpath expression as a bool.
		virtual bool as_bool() const=0;

		//! Evaluate the xpath expression as a number.
		virtual double as_number() const=0;

		//! Evaluate the xpath expression as a string.
		virtual std::string as_string() const=0;
	};

	//! A reader lock on an XML document.

	//! A \ref ref "reference" to this object gets returned by readlock().
	//!
	//! \code
	//! INSERT_LIBX_NAMESPACE::xml::doc::base::readlock lock=doc->readlock();
	//!
	//! \endcode

	class readlockObj : virtual public obj {

	public:
		//! Constructor
		readlockObj();

		//! Destructor
		~readlockObj();

		//! Create another read lock

		//! The cloned lock points to the same node that this lock
		//! points to. An exception gets thrown if this is a write
		//! lock, write locks cannot be cloned.

		virtual ref<readlockObj> clone() const=0;

		//! Go to the document's root node.

		//! Returns \c false if the document is empty, and has no
		//! root node.
		virtual bool get_root()=0;

		//! Go to the document's parent node.

		//! Returns \c true if the current node is not the document's
		//! root node, and positions the current node to the parent
		//! node, and \c false in all other cases.

		virtual bool get_parent()=0;

		//! Go to the document's first child node

		//! Returns \c true if the current node has child nodes,
		//! and positions the current node to the first child node,
		//! and \c false in all other cases.

		virtual bool get_first_child()=0;

		//! Go to the document's last child node

		//! Returns \c true if the current node has child nodes,
		//! and positions the current node to the last child node,
		//! and \c false in all other cases.

		virtual bool get_last_child()=0;

		//! Go to the next sibling node.

		//! Returns \c true and moves the current node to its next
		//! sibling node if the current node has a next sibling node,
		//! returns \c false in all other cases.

		virtual bool get_next_sibling()=0;

		//! Go to the previous sibling node.

		//! Returns \c true and moves the current node to its previous
		//! sibling node if the current node has a previous sibling
		//! node, returns \c false in all other cases.

		virtual bool get_previous_sibling()=0;

		//! Go to the document's first child node that's an element node

		//! Returns \c true if the current node has child nodes that
		//! are element nodes,
		//! and positions the current node to the first child element
		//! node and \c false in all other cases.

		virtual bool get_first_element_child()=0;

		//! Go to the document's last child node that's an element node

		//! Returns \c true if the current node has child nodes that
		//! are element nodes,
		//! and positions the current node to the last child element
		//! node and \c false in all other cases.

		virtual bool get_last_element_child()=0;

		//! Go to the next sibling node that's an element node.

		//! If the current node has a next sibling node that's an
		//! element node, returns \c true and moves the current node to
		//! its next sibling node; returns \c false in all other
		//! cases.

		virtual bool get_next_element_sibling()=0;

		//! Go to the previous sibling node that's an element node.

		//! If the current node has a previous sibling node that's an
		//! element node, returns \c true and moves the current node to
		//! its previous sibling node; returns \c false in all other
		//! cases.

		virtual bool get_previous_element_sibling()=0;

		//! Return the current node's type.

		//! Returns an empty string if the lock is not positioned
		//! on a node, this is a new lock.

		virtual std::string type() const=0;

		//! Returns the current node's name

		//! An empty name gets returned if the lock is not positioned
		//! on a node, this is a new lock.
		virtual std::string name() const=0;

		//! Returns the current node's namespace prefix

		//! An empty string gets returned if the lock is not positioned
		//! on an element node or if the element node is in an unnamed
		//! namespace.
		virtual std::string prefix() const=0;

		//! Returns the current node's namespace URI

		//! An empty string gets returned if the lock is not positioned
		//! on an element node or if the element node is in an unnamed
		//! namespace.
		virtual std::string uri() const=0;


		//! Return the current node's path.

		//! Returns an empty string if the lock is not positioned
		//! on a node, this is a new lock.

		virtual std::string path() const=0;

		//! Return the number of children elements

		virtual size_t get_child_element_count() const=0;

		//! Return the value of an attribute of the current element node.

		//! Returns the value of a attribute in a specific namespace.
		//! An empty string gets returned if this attribute does not
		//! exist, or the current node is not an element node.

		virtual std::string get_attribute(//! Attribute's name
						 const std::string &attribute_name,
						 //! Attribute's namespace
						 const uriimpl &attribute_namespace) const=0;

		//! Return the value of an attribute of the current element node.

		//! \overload

		virtual std::string get_attribute(//! Attribute's name
						 const std::string &attribute_name,
						 //! Attribute's namespace
						 const std::string &attribute_namespace) const=0;

		//! Return the value of an attribute of the current element node.

		//! \overload

		std::string get_attribute(//! Attribute's name
					  const std::string &attribute_name,
					  //! Attribute's namespace
					  const char *attribute_namespace) const
		{
			return get_attribute(attribute_name,
					     std::string(attribute_namespace));
		}

		//! Return the value of an attribute of the current element node.

		//! \overload
		std::string get_attribute(const docAttribute &attribute) const
		{
			return get_attribute(attribute.attrname,
					     attribute.attrnamespace);
		}

		//! Return the value of an attribute of the current element node.

		//! Returns the value of an attribute in no namespace.
		//! An empty string gets returned if this attribute does not
		//! exist, or the current node is not an element node.

		virtual std::string get_any_attribute(//! Attribute's name

						      //! This attribute name is not
						      //! in any namespace.
						      const std::string &attribute_name) const=0;

		//! Return the value of an attribute of the current element node.

		//! Returns the value of an attribute, in any namespace.
		//! An empty string gets returned if this attribute does not
		//! exist, or the current node is not an element node.

		virtual std::string get_attribute(//! Attribute's name

						  //! This attribute name is not
						  //! in any namespace.
						  const std::string &attribute_name) const=0;

		//! Enumerate all attributes of this element node

		//! The names of all attributes of this element node are
		//! added to the \c attributeSet.

		virtual void get_all_attributes(//! All attributes of this node are placed here

						//! \note
						//! The existing contents of
						//! this set are not removed.
						std::set<docAttribute> &attributeSet) const=0;

		//! Whether the current node is a text node.

		virtual bool is_text() const=0;

		//! Whether the current text node is blank.

		virtual bool is_blank() const=0;

		//! Return the text representation of the current node
		virtual std::string get_text() const=0;

		//! Return the current node's language
		virtual std::string get_lang() const=0;

		//! Return the current node's space preserving behavior.

		//! Return 0 for the default behavior, 1 for "preserve"
		//! behavior, -1 if xml:space was not inherited.
		virtual int get_space_preserve() const=0;

		//! Returns the node's base URL as defined in RFC 2396 section 5.1.1

		virtual std::string get_base() const=0;

		//! Save the XML document to a file

		virtual void save_file(//! Filename
				       const std::string &filename,

				       //! Set to true to indent formatted XML
				       bool format=false) const=0;

		//! Evaluate an XPATH expression against the current node

		//! An exception gets thrown if \c expr is not a valid XPath
		//! expression.
		//!
		//! \return \ref docBase::xpath "INSERT_LIBX_NAMESPACE::xml::doc::base::xpath".
		virtual ref<xpathObj> get_xpath(const std::string &expr)=0;

	protected:
		//! Callback shim to save formatted XML into an output iterator

		template<typename iter_type>
		class save_to_callback_iter : public save_to_callback {

		public:
			//! Output iterator
			iter_type iter;

			//! Constructor
			save_to_callback_iter(iter_type iter_arg)
				: iter(iter_arg)
			{
			}

			//! Destructor
			~save_to_callback_iter()
			{
			}

			//! Save XML fragment into an output iterator
			void save(const char *c, int n)
			{
				while (n)
				{
					*iter= *c;
					++c;
					++iter;
					--n;
				}
			}
		};

		//! Save file to a callback output function

		//! \internal
		virtual void save_file(save_to_callback &cb,
				       bool format) const=0;

	public:

		//! Save the XML document to an output iterator

		//! The XML document gets written to an output iterator that's
		//! passed as the first parameter. save_to() returns the value
		//! of the output iterator after the XML document is written
		//! to it.

		template<typename iter_type>
		iter_type save_to(//! Output iterator
				  iter_type iter,

				  //! Set to true to indent formatted XML
				  bool format=false)
		{
			save_to_callback_iter<iter_type> cb
				=save_to_callback_iter<iter_type>(iter);

			save_file(cb, format);
			return cb.iter;
		}

		//! Return the document's external dtd

		dtd get_external_dtd() const
		{
			return do_get_external_dtd_dtd();
		}

		//! Return the document's internal dtd

		dtd get_internal_dtd() const
		{
			return do_get_internal_dtd_dtd();
		}

	private:

		//! Implement get_external_dtd
		virtual dtd do_get_external_dtd_dtd() const=0;

		//! Implement get_internal_dtd
		virtual dtd do_get_internal_dtd_dtd() const=0;
	};

	//! Create a new element in the document

	//! A reference to this object gets returned by
	//! \ref writelockObj "write lock"'s create_child(),
	//! create_next_sibling() and create_previous_sibling(), which return a
	//! \c INSERT_LIBX_NAMESPACE::xml::doc::base::createnode, which is
	//! \ref ref "reference" to this object.
	//!
	//! Invoking one
	//! of the methods here actually creates the new node in the XML
	//! document.

	class createnodeObj : virtual public obj {

	public:
		//! Constructor
		createnodeObj();

		//! Destructor
		~createnodeObj();

		//! Create a new cdata block
		virtual ref<createnodeObj> cdata(//! Content of the new CDATA
						 const std::string &cdata)=0;

		//! Create a new text node
		virtual ref<createnodeObj> text(//! Content of the new text node
						const std::string &text)=0;

		//! Create a new entity reference.
		virtual ref<createnodeObj> entity(const std::string &text)=0;

		//! Create a new element
		virtual ref<createnodeObj> element(const newElement &)=0;

		//! Create a new attribute in the current element node

		//! This is the same as calling attribute() in the
		//! \ref writelock "writer lock".

		ref<createnodeObj> attribute(const newAttribute &);

		//! Create a new namespace in the current element

		//! This is for convenience, this method simply invokes
		//! the writer lock's create_namespace() method, and returns
		//! another reference to this creator node.
		ref<createnodeObj> create_namespace(const std::string &prefix,
						    const std::string &uri);

		//! Create a new namespace in the current element

		//! \overload
		ref<createnodeObj> create_namespace(const std::string &prefix,
						    const uriimpl &uri);

		//! Create a new namespace in the current element

		//! \overload
		ref<createnodeObj> create_namespace(const std::string &prefix,
						    const char *uri);

		//! Create a new element with attributes

		ref<createnodeObj> element(//! New element
					   const newElement &e,

					   //! Attributes
					   const std::vector<newAttribute> &a);
		//! Create a new comment

		ref<createnodeObj>
		comment(const std::string &comment);

		//! Create a new processing instruction
		ref<createnodeObj>
		processing_instruction(const std::string &name,
				       const std::string &content);

		//! Set the xml:base attribute
		ref<createnodeObj> set_base(const std::string &uri);

		//! Set the xml:base attribute
		ref<createnodeObj> set_base(const char *uri);

		//! Set the xml:base attribute
		ref<createnodeObj> set_base(const uriimpl &uri);

		//! Set the xml:lang attribute
		ref<createnodeObj> set_lang(const std::string &lang);

		//! Set the xml::space attribute
		ref<createnodeObj> set_space_preserve(bool flag);

		//! Switch to a different creator
		virtual ref<createnodeObj> create_child()=0;

		//! Switch to a different creator
		virtual ref<createnodeObj> create_next_sibling()=0;

		//! Switch to a different creator
		virtual ref<createnodeObj> create_previous_sibling()=0;

		//! Move the writer lock to the parent node.
		ref<createnodeObj> parent();

	private:
		//! Implement attribute().

		virtual void do_attribute(const newAttribute &)=0;

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

		//! Implement comment() virtually

		//! \internal
		virtual void do_comment(const std::string &comment)=0;

		//! Implement processing_instruction() virtually

		//! \internal
		virtual void
		do_processing_instruction(const std::string &name,
					  const std::string &content)=0;

		//! Implement set_base() virtually
		virtual void do_set_base(const std::string &uri)=0;

		//! Implement set_base() virtually
		virtual void do_set_base(const char *uri)=0;

		//! Implement set_base() virtually
		virtual void do_set_base(const uriimpl &uri)=0;

		//! Implement set_lang() virtually
		virtual void do_set_lang(const std::string &lang)=0;

		//! Implement set_space_preserve() virtually
		virtual void do_set_space_preserve(bool flag)=0;

		//! Implement parent()
		virtual void do_parent()=0;
	};

	//! A \ref ref "reference" to this object gets returned by writelock()

	class writelockObj : public readlockObj {
	public:
		//! Constructor
		writelockObj();

		//! Destructor
		~writelockObj();

		//! Create a new child element

		//! This call does not actually add a new child element.
		//! It returns a \ref createnodeObj "INSERT_LIBX_NAMESPACE::doc::base::createnode"
		//! reference. Invoke one of its methods to actually add a
		//! new child element. The new child element is appended to
		//! this node's child list. The write lock remains positioned
		//! on this element, so invoking
		//! another method from the returned
		//! \ref createnodeObj "INSERT_LIBX_NAMESPACE::doc::base::createnode"
		//! adds another child element to the same element.
		//!
		//! As a special case, invoking create_child() after obtaining
		//! a write lock on the document, but before invoking
		//! get_root(), installs a new document root element.
		//!
		//! \note
		//! This method returns a \ref docBase::createnode "INSERT_LIBX_NAMESPACE::doc::base::create"
		//! which holds a reference on this writelock, until it goes
		//! out of scope, and gets destroyed.

		virtual ref<createnodeObj> create_child()=0;

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
		//! \ref createnodeObj "INSERT_LIBX_NAMESPACE::doc::base::createnode"
		//! adds another next sibling element, after the first one.
		//!
		//! \note
		//! This method returns a \ref docBase::createnode "INSERT_LIBX_NAMESPACE::doc::base::create"
		//! which holds a reference on this writelock, until it goes
		//! out of scope, and gets destroyed.

		virtual ref<createnodeObj> create_next_sibling()=0;

		//! Create a new previous sibling element

		//! Similar to newchild(), but the new element becomes the
		//! current element's previous
		//! sibling node, and the write lock also gets
		//! repositioned to the newly added element, so invoking
		//! another method from the returned
		//! \ref createnodeObj "INSERT_LIBX_NAMESPACE::doc::base::createnode"
		//! adds another previous sibling element, before the one
		//! that was just added.
		//!
		//! \note
		//! This method returns a \ref docBase::createnode "INSERT_LIBX_NAMESPACE::doc::base::create"
		//! which holds a reference on this writelock, until it goes
		//! out of scope, and gets destroyed.

		virtual ref<createnodeObj> create_previous_sibling()=0;

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
			       const newAttribute &attr)
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

		newdtd get_external_dtd()
		{
			return do_get_external_dtd_newdtd();
		}

		//! Return the document's internal dtd

		newdtd get_internal_dtd()
		{
			return do_get_internal_dtd_newdtd();
		}

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
					  const newAttribute &attr)=0;

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

	//! Create a read lock on the document.

	//! Blocks if a write lock exists.
	virtual ref<readlockObj> readlock()=0;

	//! Create a write lock on the document.

	//! Blocks if a read or a write lock exists.
	virtual ref<writelockObj> writelock()=0;
};

#if 0
{
	{
#endif
	}
}
#endif
