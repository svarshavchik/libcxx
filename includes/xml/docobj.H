/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_docobj_H
#define x_xml_docobj_H

#include <x/obj.H>
#include <set>
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
		~save_to_callback() noexcept;

		//! Save a chunk of formatted XML
		virtual void save(const char *, int)=0;
	};
public:
	//! Constructor
	docObj();

	//! Destructor
	~docObj() noexcept;

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
		~docAttribute() noexcept;

		//! Comparison operator for associative containers.

		bool operator<(const docAttribute &) const;
	};

	//! A \ref ref "reference" to this object gets returned by readlock()

	class readlockObj : virtual public obj {

	public:
		//! Constructor
		readlockObj();

		//! Destructor
		~readlockObj() noexcept;

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

		//! Return the current node's path.

		//! Returns an empty string if the lock is not positioned
		//! on a node, this is a new lock.

		virtual std::string path() const=0;

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
			~save_to_callback_iter() noexcept
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
		~createnodeObj() noexcept;

		//! Create a new cdata block
		virtual ref<createnodeObj> cdata(//! Content of the new CDATA
						 const std::string &cdata)=0;

		//! Create a new text node
		virtual ref<createnodeObj> text(//! Content of the new text node
						const std::string &text)=0;

		//! Create a new element
		virtual ref<createnodeObj> element(//! Element name
						   const std::string &name)=0;
	};

	//! A \ref ref "reference" to this object gets returned by writelock()

	class writelockObj : public readlockObj {
	public:
		//! Constructor
		writelockObj();

		//! Destructor
		~writelockObj() noexcept;

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
