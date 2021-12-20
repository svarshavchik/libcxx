/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_readlockobj_H
#define x_xml_readlockobj_H

#include <x/uriimplfwd.H>
#include <x/explicit_arg.H>
#include <x/obj.H>
#include <x/xml/dtdfwd.H>
#include <x/xml/newdtdfwd.H>
#include <x/xml/docobj.H>
#include <x/xml/docfwd.H>
#include <x/xml/xpathfwd.H>
#include <x/xml/readlockfwd.H>
#include <x/xml/save_to_callback.H>
#include <x/xml/attribute.H>
#include <type_traits>
#include <charconv>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

//! Read lock implementation object.

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

	//! Clone the entire document.
	virtual doc clone_document(bool recursive=true) const=0;

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
					  const std::string_view &attribute_name,
					  //! Attribute's namespace
					  const explicit_arg<uriimpl> &attribute_namespace) const=0;

	//! Return the value of an attribute of the current element node.

	//! \overload

	virtual std::string get_attribute(//! Attribute's name
					  const std::string_view &attribute_name,
					  //! Attribute's_view namespace
					  const std::string_view &attribute_namespace) const=0;


	//! Return the value of an attribute of the current element node.

	//! \overload
	std::string get_attribute(const attribute &attr) const;

	//! Return the value of an attribute of the current element node.

	//! Returns the value of an attribute in no namespace.
	//! An empty string gets returned if this attribute does not
	//! exist, or the current node is not an element node.

	virtual std::string get_any_attribute(//! Attribute's name

					      //! This attribute name is not
					      //! in any namespace.
					      const std::string_view
					      &attribute_name) const=0;

	//! Return the value of an attribute of the current element node.

	//! Returns the value of an attribute, in any namespace.
	//! An empty string gets returned if this attribute does not
	//! exist, or the current node is not an element node.

	virtual std::string get_attribute(//! Attribute's name

					  //! This attribute name is not
					  //! in any namespace.
					  const std::string_view &attribute_name
					  ) const=0;

	//! Enumerate all attributes of this element node

	virtual get_all_attributes_t get_all_attributes() const=0;

	//! Whether the current node is a text node.

	virtual bool is_text() const=0;

	//! Whether the current text node is blank.

	virtual bool is_blank() const=0;

	//! Return the text representation of the current node
	virtual std::string get_text() const=0;

	//! Return the current node converted to a numeric value.

	template<typename T,
		 typename=std::enable_if_t< std::is_arithmetic_v<T>>>
	T get_text() const
	{
		auto s=get_text();

		T v;

		auto cp=s.c_str();

		auto ret=std::from_chars(cp, cp+s.size(), v);

		if (ret.ec != std::errc{} || *ret.ptr)
			value_conversion_error(s);

		return v;
	}

	//! Return an attribute converted to a numeric value.
	template<typename T,
		 typename ...Args,
		 typename=std::enable_if_t< std::is_arithmetic_v<T>>,
		 typename=std::void_t<
			 decltype(
				 std::declval<readlockObj &>().get_attribute(
					 std::declval<Args &&>()...
				 ))>>
	T get_attribute(Args && ...args) const
	{
		auto s=get_attribute(std::forward<Args>(args)...);

		T v;

		auto cp=s.c_str();

		auto ret=std::from_chars(cp, cp+s.size(), v);

		if (ret.ec != std::errc{} || *ret.ptr)
			value_conversion_error(s);

		return v;
	}

	//! Return an attribute converted to a numeric value.

	template<typename T,
		typename=std::enable_if_t< std::is_arithmetic_v<T>>>
	T get_any_attribute(const std::string_view &value) const
	{
		auto s=get_any_attribute(value);

		T v;

		auto cp=s.c_str();

		auto ret=std::from_chars(cp, cp+s.size(), v);

		if (ret.ec != std::errc{} || *ret.ptr)
			value_conversion_error(s);

		return v;
	}

	static void value_conversion_error(const std::string &s)
		__attribute__((noreturn));

	//! Return the text representation of the current node as a u32string
	std::u32string get_u32text() const;

	//! Return the attribute as a u32string.
	std::u32string get_u32any_attribute(//! Attribute's name

					    //! This attribute name is not
					    //! in any namespace.
					    const std::string_view
					    &attribute_name) const;

	//! Return the attribute as a u32string.
	std::u32string get_u32attribute(//! Attribute's name
					const std::string_view &attribute_name,
					//! Attribute's namespace
					const explicit_arg<uriimpl> &attribute_namespace) const;

	//! Return the attribute as a u32string.

	std::u32string get_u32attribute(//! Attribute's name
					const std::string_view &attribute_name,
					//! Attribute's_view namespace
					const std::string_view &attribute_namespace) const;

	//! Return the attribute as a u32string.
	std::u32string get_u32attribute(const attribute &attr) const;

	//! Return the attribute as a u32string.
	std::u32string get_u32attribute(//! Attribute's name

				     //! This attribute name is not
				     //! in any namespace.
				     const std::string_view &attribute_name
				     ) const;

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
	//! \return \ref xpath "INSERT_LIBX_NAMESPACE::xml::xpath".
	virtual xpath get_xpath(const std::string &expr)=0;

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
		~save_to_callback_iter()=default;

		//! Save XML fragment into an output iterator
		void save(const char *c, int n) override
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

	dtd get_external_dtd() const;

	//! Return the document's internal dtd

	dtd get_internal_dtd() const;

private:

	//! Implement get_external_dtd
	virtual dtd do_get_external_dtd_dtd() const=0;

	//! Implement get_internal_dtd
	virtual dtd do_get_internal_dtd_dtd() const=0;
};

#if 0
{
#endif
}
#endif
