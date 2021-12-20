/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_createnodeobj_H
#define x_xml_createnodeobj_H

#include <x/obj.H>
#include <x/uriimplfwd.H>
#include <x/xml/createnodefwd.H>
#include <x/xml/new_element.H>
#include <x/xml/new_attribute.H>

#include <string>
#include <charconv>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

//! Return value from createnode().

template<typename T, typename=void>
struct createnode_ret_t;

//! Create a new element in the document

//! A reference to this object gets returned by
//! \ref writelock "write lock"'s create_child(),
//! create_next_sibling() and create_previous_sibling(), which return a
//! \c INSERT_LIBX_NAMESPACE::xml::createnode, which is
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
	virtual createnode cdata(//! Content of the new CDATA
				 const std::string_view &cdata)=0;

	//! Create a new text node
	virtual createnode text(//! Content of the new text node
				const std::string_view &text)=0;

	//! Create a new cdata block
	virtual createnode cdata(//! Content of the new CDATA
				 const std::u32string_view &cdata)=0;

	//! Create a new text node
	virtual createnode text(//! Content of the new text node
				const std::u32string_view &text)=0;

	//! Create a new text node

	template<typename T>
	typename createnode_ret_t<T>::type_t text(T t);

	//! Create a new entity reference.
	virtual createnode entity(const std::string_view &text)=0;

	//! Create a new element
	virtual createnode element(const new_element &)=0;

	//! Create a new attribute in the current element node

	//! This is the same as calling attribute() in the
	//! \ref writelock "writer lock".

	createnode attribute(const new_attribute &);

	//! Create a new namespace in the current element

	//! This is for convenience, this method simply invokes
	//! the writer lock's create_namespace() method, and returns
	//! another reference to this creator node.
	createnode create_namespace(const std::string &prefix,
				    const std::string &uri);

	//! Create a new namespace in the current element

	//! \overload
	createnode create_namespace(const std::string &prefix,
				    const uriimpl &uri);

	//! Create a new namespace in the current element

	//! \overload
	createnode create_namespace(const std::string &prefix,
				    const char *uri);

	//! Create a new element with attributes

	createnode element(//! New element
			   const new_element &e,

			   //! Attributes
			   const std::vector<new_attribute> &a);
	//! Create a new comment

	createnode comment(const std::string &comment);

	//! Create a new processing instruction
	createnode processing_instruction(const std::string &name,
					  const std::string &content);

	//! Set the xml:base attribute
	createnode set_base(const std::string &uri);

	//! Set the xml:base attribute
	createnode set_base(const char *uri);

	//! Set the xml:base attribute
	createnode set_base(const uriimpl &uri);

	//! Set the xml:lang attribute
	createnode set_lang(const std::string &lang);

	//! Set the xml::space attribute
	createnode set_space_preserve(bool flag);

	//! Switch to a different creator
	virtual createnode create_child()=0;

	//! Switch to a different creator
	virtual createnode create_next_sibling()=0;

	//! Switch to a different creator
	virtual createnode create_previous_sibling()=0;

	//! Move the writer lock to the parent node.
	createnode parent();

private:
	//! Implement attribute().

	virtual void do_attribute(const new_attribute &)=0;

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

template<typename T>
typename createnode_ret_t<T>::type_t createnodeObj::text(T t)
{
	return createnode_ret_t<T>::text(*this, t);
}

//! Specialization for \ref writelock "writelock->text()".

//! Implements writelock->text(n) for all numeric types except chars.
//! TODO: Drop the decltype SFINAE when gcc implements
//! to_chars for floating point values.

template<typename T>
struct createnode_ret_t<T, std::enable_if_t
			<std::is_integral_v<T> ||
			 std::is_floating_point_v<T>>> {

	//! Return type for SFINAE
	typedef createnode type_t;

	//! Generate a text node from a numeric value.
	static inline createnode text(createnodeObj &me, T t)
	{
		char buffer[std::numeric_limits<T>::digits10+10];

		auto r=std::to_chars(buffer, buffer+sizeof(buffer)-1, t);

		if (r.ec != std::errc{})
			return ref{&me}; // Shouldn't happen

		*r.ptr=0;
		return me.text(buffer);
	}
};

#define LIBCXX_TEMPLATE_DECL extern
#include <x/xml/createnode_to.H>
#undef LIBCXX_TEMPLATE_DECL

#include <x/xml/createnode_number_to.H>

#if 0
{
#endif
}
#endif
