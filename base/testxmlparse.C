/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/parser.H"
#include "x/xml/doc.H"
#include "x/exception.H"
#include "x/fd.H"
#include "x/uriimpl.H"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <unistd.h>
	
static LIBCXX_NAMESPACE::xml::doc parse(const std::string &str)
{
	return std::copy(str.begin(), str.end(),
			 LIBCXX_NAMESPACE::xml::parser::create("STRING"))
		.get()->done();
}

void test1(size_t bufsize)
{
	std::string filler(bufsize, 'X');

	{
		auto d=parse("<root><child name='value'>"
			     + filler +
			     "</child></root>");
	}

	bool flag=false;
	try {
		parse("<root><child />" + filler);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Caught expected exception:" << std::endl
			  << e;
		flag=true;
	}

	if (!flag)
		throw EXCEPTION("Did not catch the expected exception");

	{
		std::ofstream o("testxml.doc");

		o << "<root><child name='value'>" + filler + "</child></root>";

		o.close();

		auto d=LIBCXX_NAMESPACE::xml::doc::create("testxml.doc",
							  "nonet");
		unlink("testxml.doc");
	}
}

void test2()
{
	auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

	auto lock=empty_document->readlock();

	if (lock->get_root())
		throw EXCEPTION("An empty document should not have a root node");
	if (lock->type() != "")
		throw EXCEPTION("Why do I have a type, here?");

	auto document=({
			std::string dummy=
				"<root attribute='value' xmlns:x='http://www.example.com'>"
				"<child xml:lang='GB' xml:space='preserve'>Text<i>element</i></child>"
				"<child2>Text2<i x:attribute='value'>element2<empty> </empty></i></child2>"
				"</root>";

			LIBCXX_NAMESPACE::xml::doc::create(dummy.begin(),
							   dummy.end(),
							   "string");
		});

	lock=document->readlock();
	if (!lock->get_root() || lock->type() != "element_node")
		throw EXCEPTION("Root node is not an element node");

	if (lock->name() != "root" || lock->prefix() != "" || lock->uri() != "")
		throw EXCEPTION("Root node is not <root>");

	if (lock->get_child_element_count() != 2)
		throw EXCEPTION("Root node does not have two children");

	std::set<LIBCXX_NAMESPACE::xml::doc::base::attribute> attributes;

	lock->get_all_attributes(attributes);

	if (attributes.size() != 1)
		throw EXCEPTION("Root node does not have one attribute");

	{
		const auto &a=*attributes.begin();

		if (a.attrname != "attribute" || a.attrnamespace != "")
			throw EXCEPTION("Root node does not have attribute");

		if (lock->get_attribute("attribute") != "value")
			throw EXCEPTION("get_attribute() failed");
		if (lock->get_attribute(a) != "value")
			throw EXCEPTION("get_attribute(doc::base::attribute) failed");
		if (lock->get_attribute("attribute", "") != "value")
			throw EXCEPTION("get_attribute(empty namespace str) failed");
	}

	if (lock->get_any_attribute("attribute") != "value")
		throw EXCEPTION("get_any_attribute() failed");

	if (lock->path() != "/root")
		throw EXCEPTION("Root node is not /root");
	if (!lock->get_first_child() || lock->path() != "/root/child" ||
	    !lock->get_parent())
		throw EXCEPTION("get_first_child/parent failed");

	if (!lock->get_last_child() || lock->path() != "/root/child2" ||
	    !lock->get_previous_element_sibling() || lock->path() != "/root/child")
		throw EXCEPTION("get_last_child/get_prev_sibling_element failed");
	auto lock2=lock->clone();

	if (!lock2->get_next_element_sibling() || lock2->get_next_element_sibling() ||
	    lock->get_previous_element_sibling() ||
	    lock->path() != "/root/child" || lock2->path() != "/root/child2")
		throw EXCEPTION("clone()/get_next_element_sibling() failed");

	if (!lock2->get_first_element_child() ||
	    lock2->path() != "/root/child2/i" ||
	    !lock2->get_previous_sibling() ||
	    lock2->type() != "text_node")
		throw EXCEPTION("get_first_element_child/get_previous_sibling failed");
	if (!lock2->get_next_sibling() ||
	    lock2->path() != "/root/child2/i")
		throw EXCEPTION("get_next_sibling failed");
	if (lock2->get_attribute("attribute") != "")
		throw EXCEPTION("get_attribute should not return namespace attribute values");
	if (lock2->get_any_attribute("attribute") != "value")
		throw EXCEPTION("get_any_attribute did not return a namespaced attribute");
	if (lock2->get_attribute("attribute", "http://www.example.com")
	    != "value")
		throw EXCEPTION("namespaces get_attribute() did not work");

	if (lock2->get_attribute("attribute", LIBCXX_NAMESPACE::uriimpl("http://www.example.com"))
	    != "value")
		throw EXCEPTION("namespaces get_attribute(uriimpl) did not work");

	attributes.clear();

	lock2->get_all_attributes(attributes);

	if (attributes.size() != 1)
		throw EXCEPTION("<i> node does not have one attribute");

	{
		const auto &a=*attributes.begin();

		if (a.attrname != "attribute" ||
		    a.attrnamespace != "http://www.example.com")
			throw EXCEPTION("<i> node does not have attribute");
	}

	if (lock2->is_text())
		throw EXCEPTION("<i> is a text node");

	if (lock2->is_blank())
		throw EXCEPTION("<i> is blank");

	if (!lock2->get_first_element_child() ||
	    !lock2->get_first_child())
		throw EXCEPTION("Could not position to empty's text node");

	if (!lock2->is_text())
		throw EXCEPTION("is not a text node");

	if (!lock2->is_blank())
		throw EXCEPTION("is not a blank node");

	lock->get_root();
	lock->get_first_element_child();

	if (lock->get_text() != "Textelement")
		throw EXCEPTION("child text mismatch");
	lock->get_first_child();

	if (lock->get_lang() != "GB")
		throw EXCEPTION("Did not get language");

	if (!lock->get_parent() || lock->get_space_preserve() <= 0)
		throw EXCEPTION("Did not get space preserve setting");
}

class test_3_throw {

public:

	test_3_throw &operator*()
	{
		return *this;
	}

	test_3_throw &operator=(char c)
	{
		throw EXCEPTION("Abort");
	}

	test_3_throw &operator++()
	{
		return *this;
	}
};

void test3()
{
	auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

	{
		auto lock=empty_document->writelock();

		lock->create_child()->element({"root1"});

		if (!lock->get_root() || lock->name() != "root1")
			throw EXCEPTION("Did not create <root1> somehow");
	}

	{
		auto lock=empty_document->writelock();

		lock->create_child()->element({"root2"});

		if (!lock->get_root() || lock->name() != "root2")
			throw EXCEPTION("Did not create <root2> somehow");

		lock->create_child()->text("Text")->text("<>Node");

		{
			LIBCXX_NAMESPACE::xml::doc::base::createnode
				cn=lock->create_next_sibling();
			cn->cdata("Foo<bar>");
		}
		lock->get_root();
		if (lock->get_text() != "Text<>NodeFoo<bar>")
			throw EXCEPTION("Unexpected text() and cdata() results");
		lock->save_file("testxmlparse.tmp");
	}
	empty_document=LIBCXX_NAMESPACE::xml::doc::create("testxmlparse.tmp",
							  "nonet");
	unlink("testxmlparse.tmp");

	{
		auto lock=empty_document->readlock();

		lock->get_root();

		if (lock->get_text() != "Text<>NodeFoo<bar>")
			throw EXCEPTION("Something wrong with save_file()");
	}

	std::string big(32768, 'X');

	empty_document=LIBCXX_NAMESPACE::xml::doc::create();

	{
		auto lock=empty_document->writelock();

		lock->create_child()->element({"root"});
		lock->create_child()->text(big);

		std::string cpy;

		lock->save_to(std::back_insert_iterator<std::string>(cpy));

		bool caught=false;
		try {
			lock->save_to(test_3_throw());
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			caught=true;
			std::cout << "Caught expected exception: " << e
				  << std::endl;
		}

		if (!caught)
			throw EXCEPTION("Did not catch expected exception");
	}

	{
		std::string doc="<root><child1>text</child1><child2>text</child2></root>";

		empty_document=LIBCXX_NAMESPACE::xml::doc::create(doc.begin(), doc.end(), "STRING");
		doc.clear();

		empty_document->readlock()
			->save_to(std::back_insert_iterator<std::string>(doc),
				  true);

		std::cout << doc << std::endl;
	}
}

void test4()
{
	auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

	auto lock=empty_document->writelock();

	lock->create_child()->element({"item", "prefix",
				"http://www.example.com"})
		->element({"prefix:subitem"});
	lock->create_next_sibling()->element({"subitem2", LIBCXX_NAMESPACE::uriimpl("http://www.example.com")});

	{
		std::string doc;

		lock->save_to(std::back_insert_iterator<std::string>(doc));

		std::cout << doc << std::endl;
	}
	lock->get_root();

	if (lock->name() != "item" || lock->prefix() != "prefix"
	    || lock->uri() != "http://www.example.com")
		throw EXCEPTION("Top level <prefix:item> element not found");

	lock->get_last_element_child();

	if (lock->name() != "subitem2" || lock->prefix() != "prefix"
	    || lock->uri() != "http://www.example.com")
		throw EXCEPTION("<prefix:subitem2> element not found");
}

void test5()
{
	auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

	auto lock=empty_document->writelock();

	bool caught=false;

	try {
		lock->create_child()->element({
				"head", "xml",
					"http://www.w3.org/XML/1998/namespace"
					});
	} catch (...)
	{
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("Did not catch expected create_child() exception");
}

void test6()
{
	auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

	auto lock=empty_document->writelock();

	lock->create_child()->element({"head"});
	lock->create_namespace("ns1", "http://www.example.com");

	bool caught=false;
	try {
		lock->create_namespace("ns1", "http://www.example.com");
	} catch (...)
	{
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("Did not catch expected create_namespace() exception");
	lock->create_child()
		->create_namespace("ns2", std::string("http://www.example.com"))
		->create_namespace("ns3", LIBCXX_NAMESPACE::uriimpl("http://www.example.com/x"))
		->element({"ns2:body"});
	lock->create_next_sibling()->element({"body", "http://www.example.com/x"})->attribute({"attr1", "value1"});

	if (lock->prefix() != "ns3" ||
	    lock->uri() != "http://www.example.com/x")
		throw EXCEPTION("element() specifying a URI failed");
	if (lock->get_attribute("attr1") != "value1")
		throw EXCEPTION("Namespaceless attribute() failed");
	lock->create_child()->attribute({"ns2:attr2", "value2"})
		->attribute({"attr3", LIBCXX_NAMESPACE::uriimpl
					("http://www.example.com/x"),
					"value3"});
	if (lock->get_attribute("attr2", "http://www.example.com") != "value2")
		throw EXCEPTION("prefix:name attribute() failed");
	
	if (lock->get_attribute("attr3", "http://www.example.com/x") != "value3")
		throw EXCEPTION("prefix, namespace attribute() failed");

	lock->get_previous_sibling();
	if (lock->prefix() != "ns2" ||
	    lock->uri() != "http://www.example.com")
		throw EXCEPTION("element() specifying a prefix");
	{
		std::string doc;

		lock->save_to(std::back_insert_iterator<std::string>(doc));

		std::cout << doc << std::endl;
	}
}

void test7()
{
	auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

	auto lock=empty_document->writelock();

	lock->create_child()->element({"head", "html", "http://www.example.com"},
				      {
					      {"html:attr1", "value1"},
					      {"attr2", "http://www.example.com",
							      "value2"}
				      });

	if (lock->prefix() != "html" || lock->uri() != "http://www.example.com")
		throw EXCEPTION("test7: prefix/uri not right");

	{
		std::set<LIBCXX_NAMESPACE::xml::doc::base::attribute> s;

		lock->get_all_attributes(s);

		if (s.size() != 2)
			throw EXCEPTION("Not two attributes");

		auto first=*s.begin();
		auto second=*--s.end();

		if (first.attrname != "attr1" ||
		    first.attrnamespace != "http://www.example.com" ||
		    lock->get_attribute(first) != "value1")
			throw EXCEPTION("first attribute is wrong");

		if (second.attrname != "attr2" ||
		    second.attrnamespace != "http://www.example.com" ||
		    lock->get_attribute(second) != "value2")
			throw EXCEPTION("second attribute is wrong");
	}
}

void test8()
{
	auto document=({
			std::string dummy=
				"<root attribute='value' xmlns:x='http://www.example.com'>"
				"<child xml:lang='GB' xml:space='preserve'>Text<i>element</i></child>"
				"<child2>Text2<i x:attribute='value'>element2<empty> </empty></i></child2>"
				"</root>";

			LIBCXX_NAMESPACE::xml::doc::create(dummy.begin(),
							   dummy.end(),
							   "string");
		});

	auto lock=document->writelock();

	lock->get_root();
	lock->get_first_child();

	if (lock->path() != "/root/child")
		throw EXCEPTION("Where are we in test8?");

	lock->remove();
	if (lock->path() != "/root")
		throw EXCEPTION("remove() did not reposition writer lock");
	lock->remove();
}

int main(int argc, char **argv)
{
	try {
		test1(100);
		test1(LIBCXX_NAMESPACE::fdbaseObj::get_buffer_size());
		test2();
		test3();
		test4();
		test5();
		test6();
		test7();
		test8();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

