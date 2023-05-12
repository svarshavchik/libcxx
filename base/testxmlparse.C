/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/parser.H"
#include "x/xml/doc.H"
#include "x/xml/newdtd.H"
#include "x/xml/dtd.H"
#include "x/xml/readlock.H"
#include "x/xml/writelock.H"
#include "x/xml/attribute.H"
#include "x/xml/xpath.H"
#include "x/number.H"
#include "x/exception.H"
#include "x/fd.H"
#include "x/uriimpl.H"
#include "x/locale.H"
#include "x/netaddr.H"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <unistd.h>
#include <poll.h>

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

	auto attributes=lock->get_all_attributes();

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
		if (lock->get_u32attribute(a) != U"value")
			throw EXCEPTION("get_attribute(doc::base::attribute) failed");
		if (lock->get_attribute("attribute", "") != "value")
			throw EXCEPTION("get_attribute(empty namespace str) failed");
	}

	if (lock->get_any_attribute("attribute") != "value")
		throw EXCEPTION("get_any_attribute() failed");
	if (lock->get_u32any_attribute("attribute") != U"value")
		throw EXCEPTION("get_u32any_attribute() failed");

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
	if (lock2->get_u32attribute("attribute", "http://www.example.com")
	    != U"value")
		throw EXCEPTION("namespaces get_attribute() did not work");

	if (lock2->get_attribute("attribute", LIBCXX_NAMESPACE::uriimpl("http://www.example.com"))
	    != "value")
		throw EXCEPTION("namespaces get_attribute(uriimpl) did not work");

	if (lock2->get_u32attribute("attribute", LIBCXX_NAMESPACE::uriimpl("http://www.example.com"))
	    != U"value")
		throw EXCEPTION("namespaces get_u32attribute(uriimpl) did not work");

	attributes=lock2->get_all_attributes();

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
		lock->remove();
	}

	{
		auto lock=empty_document->writelock();

		lock->create_child()->element({"root2"});

		if (!lock->get_root() || lock->name() != "root2")
			throw EXCEPTION("Did not create <root2> somehow");

		lock->create_child()->text("Text")->text("<>Node");

		{
			LIBCXX_NAMESPACE::xml::createnode
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
	lock->create_next_sibling()->element({"body", "http://www.example.com/x"})->attribute({"attr1", U"value1"});

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
		auto ss=lock->get_all_attributes();

		if (ss.size() != 2)
			throw EXCEPTION("Not two attributes");

		std::set<LIBCXX_NAMESPACE::xml::attribute>
			s{ss.begin(), ss.end()};

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

void test9()
{
	auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

	auto lock=empty_document->writelock();

	lock->create_child()->element({"root"});

	lock->set_base("http://www.example.com");
	lock->set_lang("EN");
	lock->set_space_preserve(true);

	lock->create_child()->comment("A comment");
	lock->create_next_sibling()->processing_instruction("html","visible");

	lock->get_root();

	if (lock->get_base() != "http://www.example.com")
		throw EXCEPTION("get_base() in test9 did not work");

	if (lock->get_lang() != "EN")
		throw EXCEPTION("get_lang() in test9 did not work");

	if (lock->get_space_preserve() <= 0)
		throw EXCEPTION("get_space_preserve() in test9 did not work");

	lock->get_first_child();

	if (lock->type() != "comment_node" ||
	    lock->get_text() != "A comment")
		throw EXCEPTION("test9 failed to verify the comment node");

	lock->get_next_sibling();

	if (lock->type() != "pi_node" ||
	    lock->name() != "html" ||
	    lock->get_text() != "visible")
		throw EXCEPTION("test9 failed to verify the processing instruction node");
}

void test10()
{
	auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

	auto lock=empty_document->writelock();

	lock->create_child()
		->element({"root"})
		->set_base(LIBCXX_NAMESPACE::uriimpl("http://www.example.com"))
		->set_lang("EN")
		->set_space_preserve(true)
		->comment("A comment")
		->create_next_sibling()
		->processing_instruction("html","visible")
		->parent();

	if (lock->get_base() != "http://www.example.com")
		throw EXCEPTION("get_base() in test10 did not work");

	if (lock->get_lang() != "EN")
		throw EXCEPTION("get_lang() in test10 did not work");

	if (lock->get_space_preserve() <= 0)
		throw EXCEPTION("get_space_preserve() in test10 did not work");

	lock->get_first_child();

	if (lock->type() != "comment_node" ||
	    lock->get_text() != "A comment")
		throw EXCEPTION("test10 failed to verify the comment node");

	lock->get_next_sibling();

	if (lock->type() != "pi_node" ||
	    lock->name() != "html" ||
	    lock->get_text() != "visible")
		throw EXCEPTION("test10 failed to verify the processing instruction node");
}

void test20()
{
	{
		std::ofstream teeny("teeny.dtd");

		teeny <<
			"<!ELEMENT html (body?)>\n"
			"<!ELEMENT body (p?)>\n"
			"<!ELEMENT p (#PCDATA)>\n";
	}

	auto document=({
			std::string dummy=
				"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
				"<!DOCTYPE html SYSTEM \"teeny.dtd\">"
				"<html>"
				"<body><p>Hello world</p></body></html>";

			LIBCXX_NAMESPACE::xml::doc::create(dummy.begin(),
							   dummy.end(),
							   "test20 string",
							   "nonet dtdload");
		});

	{
		auto lock=document->readlock();

		lock->get_root();

		if (lock->name() != "html")
			throw EXCEPTION("test20: root node is not HTML");

		LIBCXX_NAMESPACE::xml::dtd dd=
			lock->get_external_dtd();

		if (dd->name() != "html" ||
		    dd->system_id() != "teeny.dtd")
			throw EXCEPTION("test20: read lock external subset fail");

		dd=lock->get_internal_dtd();

		if (dd->name() != "html" ||
		    dd->system_id() != "teeny.dtd")
			throw EXCEPTION("test20: read lock external subset fail");
	}

	{
		auto lock=document->writelock();

		lock->get_root();

		if (lock->name() != "html")
			throw EXCEPTION("test20: root node is not HTML");

		LIBCXX_NAMESPACE::xml::newdtd dd=
			lock->get_external_dtd();

		if (dd->name() != "html" ||
		    dd->system_id() != "teeny.dtd")
			throw EXCEPTION("test20: read lock external subset fail");

		dd=lock->get_internal_dtd();

		if (dd->name() != "html" ||
		    dd->system_id() != "teeny.dtd")
			throw EXCEPTION("test20: read lock external subset fail");

	}

	unlink("teeny.dtd");
}

void test21()
{
	auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

	auto lock=empty_document->writelock();

	lock->create_child()->element({"html"})
		->element({"body"})
		->element({"p"})
		->text("Hello world");

	lock->create_external_dtd("-//W3C//DTD XHTML 1.0 Strict//EN",
				  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd");
	lock->remove_external_dtd();

	lock->create_external_dtd("-//W3C//DTD XHTML 1.0 Strict//EN",
				  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd");

	LIBCXX_NAMESPACE::xml::newdtd newdtd=
		lock->create_internal_dtd("-//W3C//DTD XHTML 1.0 Strict//EN",
					  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd");

	// One mo time.
	lock->remove_internal_dtd();

	newdtd=lock->create_internal_dtd("-//W3C//DTD XHTML 1.0 Strict//EN",
					 "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd");

	if (newdtd->external_id() != "-//W3C//DTD XHTML 1.0 Strict//EN")
		throw EXCEPTION("test21: external_id mismatch");

	if (newdtd->system_id() != "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd")
		throw EXCEPTION("test21: external_id mismatch");

	std::string buffer;

	lock->save_to(std::back_insert_iterator<std::string>(buffer), true);
	std::cout << buffer;

	empty_document=LIBCXX_NAMESPACE::xml::doc::create(buffer.begin(),
							  buffer.end(),
							  "test21-document");

	auto rlock=empty_document->readlock();

	if (rlock->get_external_dtd()->exists())
		throw EXCEPTION("test21: external subset is not null");

	auto rdtd=rlock->get_internal_dtd();

	if (!rdtd->exists())
		throw EXCEPTION("test21: internal subset does not exist?");

	if (rdtd->name() != "html")
		throw EXCEPTION("test21: name mismatch(3)");

	if (rdtd->external_id() != "-//W3C//DTD XHTML 1.0 Strict//EN")
		throw EXCEPTION("test21: external_id mismatch (3)");

	if (rdtd->system_id() != "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd")
		throw EXCEPTION("test21: external_id mismatch (3)");
}

void test22()
{
	auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

	auto lock=empty_document->writelock();

	lock->create_child()->element({"html"})
		->element({"body"})
		->element({"p"})
		->text("Hello world");

	auto extdtd=lock->create_external_dtd("-//W3C//DTD XHTML 1.0 Strict//EN",
					      LIBCXX_NAMESPACE::uriimpl("http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"));

	auto intdtd=lock->create_internal_dtd("-//W3C//DTD XHTML 1.0 Strict//EN",
					      LIBCXX_NAMESPACE::uriimpl("http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"));

	lock->get_root();
	lock->remove();

	if (extdtd->exists() || intdtd->exists())
		throw EXCEPTION("test22 has no idea what happened");
}

void test30()
{
	std::string docstr=({
			auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

			empty_document->writelock()
				->create_child()->element({"html"})
				->element({"body"})
				->element({"p"})
				->text("Hello ")
				->create_next_sibling()
				->entity("XML")
				->text(" world");

			{
				auto intdtd=empty_document->writelock()
					->create_internal_dtd("-//W3C//DTD XHTML 1.0 Strict//EN",
							      LIBCXX_NAMESPACE::uriimpl("http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"));

				intdtd->create_general_entity("XML", "<i>XML</i>");
			}

			std::ostringstream o;

			empty_document->readlock()
				->save_to(std::ostreambuf_iterator<char>(o),
					  false);
			o.str();
		});

	auto doc=LIBCXX_NAMESPACE::xml::doc::create(docstr.begin(),
						    docstr.end(), "test30");

	auto lock=doc->readlock();
	lock->get_root();
	lock->get_first_child();
	lock->get_first_child();

	std::cout << docstr;

	if (lock->get_text() != "Hello XML world")
		throw EXCEPTION("Generic entity expansion fail");
}

void test31()
{
	std::list<LIBCXX_NAMESPACE::fd> fdlist;

	LIBCXX_NAMESPACE::netaddr::create("localhost", "")->bind(fdlist, true);

	for (auto sock:fdlist)
	{
		sock->listen();
	}
	int portnum=fdlist.front()->getsockname()->port();

	auto [r, w] = LIBCXX_NAMESPACE::fd::base::pipe();

	LIBCXX_NAMESPACE::run_lambda
		([r, sock=fdlist.front()]
		{
			struct pollfd pfd[2];

			while (1)
			{
				do
				{
					pfd[0].fd=r->get_fd();
					pfd[1].fd=sock->get_fd();

					pfd[0].events=pfd[1].events=
						(POLLIN|POLLHUP);
				} while (poll(pfd, 2, -1) <= 0);

				if (pfd[0].revents & (POLLIN|POLLHUP))
					break;

				if (!(pfd[1].revents & POLLIN))
					continue;

				auto c=sock->accept();

				if (!c)
					continue;

				auto io=c->getiostream();

				std::string s;

				while (std::getline(*io, s))
				{
					if (s == "\r")
						break;
				}

				(*io) << "HTTP/1.0 200 OK\r\n\r\n";

				(*io) << std::flush;
			}
		});

	alarm(30);

	std::ostringstream o;

	o << "http://localhost:" << portnum << "/";

	std::string docstr=({
			auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

			empty_document->writelock()
				->create_child()->element({"html"})
				->element({"body"})
				->element({"p"})
				->text("Hello ")
				->create_next_sibling()
				->entity("XML")
				->text(" world");

			{
				auto intdtd=empty_document->writelock()
					->create_internal_dtd("-//LOCAL",
							      LIBCXX_NAMESPACE::uriimpl(o.str()));

				intdtd->create_parsed_entity("XML", "",
							     "filename.xml");
			}

			std::ostringstream o;

			empty_document->readlock()
				->save_to(std::ostreambuf_iterator<char>(o),
					  false);
			o.str();
		});

	bool caught=false;
	unlink("filename.xml");

	try {
		LIBCXX_NAMESPACE::xml::doc::create(docstr.begin(),
						   docstr.end(), "test31",
						   "dtdload");
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		caught=true;
		std::cerr << "Caught expected exception: " << e << std::endl;
	}

	if (!caught)
		throw EXCEPTION("test31: did not catch expected expection for failing to load external entity");

	std::ofstream("filename.xml") << "<i>XML</i>" << std::flush;

	auto doc=LIBCXX_NAMESPACE::xml::doc::create(docstr.begin(),
						    docstr.end(), "test31",
						    "noent");

	auto lock=doc->readlock();
	lock->get_root();
	lock->get_first_child();
	lock->get_first_child();

	if (lock->get_text() != "Hello XML world")
		throw EXCEPTION("Parsed entity expansion fail in test31");
	unlink("filename.xml");
}

void test32()
{
	std::string docstr=({
			auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

			empty_document->writelock()
				->create_child()->element({"html"})
				->element({"body"})
				->element({"p"})
				->text("Hello ")
				->create_next_sibling()
				->entity("XML")
				->text(" world");

			{
				auto intdtd=empty_document->writelock()
					->create_internal_dtd("-//W3C//DTD XHTML 1.0 Strict//EN",
							      LIBCXX_NAMESPACE::uriimpl("http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"));

				intdtd->create_unparsed_entity("XML", "",
							       "filename.xml",
							       "txt");
			}

			std::ostringstream o;

			empty_document->readlock()
				->save_to(std::ostreambuf_iterator<char>(o),
					  false);
			o.str();
		});

	std::ofstream("filename.xml") << "<i>XML</i>" << std::flush;

	bool caught=false;

	try {
		LIBCXX_NAMESPACE::xml::doc::create(docstr.begin(),
						   docstr.end(), "test32",
						   "noent");
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		caught=true;
		std::cerr << "Caught expected exception: " << e << std::endl;
	}

	unlink("filename.xml");

	if (!caught)
		throw EXCEPTION("test32: did not catch expected expection for failing to load unparsed entity");
}

void test33()
{
	std::string docstr=({
			auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

			empty_document->writelock()
				->create_child()->element({"html"})
				->element({"body"})
				->element({"p"})
				->text("Hello ")
				->create_next_sibling()
				->entity("XML")
				->text(" world");

			{
				auto intdtd=empty_document->writelock()
					->create_internal_dtd("-//W3C//DTD XHTML 1.0 Strict//EN",
							      LIBCXX_NAMESPACE::uriimpl("http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"));

				intdtd->create_external_parameter_entity("entities",
									 "",
									 "entities.xml");
				intdtd->include_parameter_entity("entities");
			}

			std::ostringstream o;

			empty_document->readlock()
				->save_to(std::ostreambuf_iterator<char>(o),
					  false);
			o.str();
		});

	unlink("entities.xml");

	bool caught=false;
	try {
		LIBCXX_NAMESPACE::xml::doc::create(docstr.begin(),
						   docstr.end(), "test33",
						   "noent");
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		caught=true;
		std::cerr << "Caught expected exception: " << e << std::endl;
	}

	if (!caught)
		throw EXCEPTION("test33: did not catch expected expection for failing to load external entity");

	std::ofstream("entities.xml") << "<!ENTITY XML \"<i>XML</i>\">\n"
				      << std::flush;

	auto doc=LIBCXX_NAMESPACE::xml::doc::create(docstr.begin(),
						    docstr.end(), "test34",
						    "noent");

	auto lock=doc->readlock();
	lock->get_root();
	lock->get_first_child();
	lock->get_first_child();

	if (lock->get_text() != "Hello XML world")
		throw EXCEPTION("Parsed entity expansion fail in test33");
	unlink("entities.xml");
}

void test34()
{
	std::string docstr=({
			auto empty_document=LIBCXX_NAMESPACE::xml::doc::create();

			empty_document->writelock()
				->create_child()->element({"html"})
				->element({"body"})
				->element({"p"})
				->text("Hello ")
				->create_next_sibling()
				->entity("XML")
				->text(" world");

			{
				auto intdtd=empty_document->writelock()
					->create_internal_dtd("-//W3C//DTD XHTML 1.0 Strict//EN",
							      LIBCXX_NAMESPACE::uriimpl("http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"));

				intdtd->create_internal_parameter_entity("entities",
									 "<!ENTITY XML \"<i>XML</i>\">");
				intdtd->include_parameter_entity("entities");
			}

			std::ostringstream o;

			empty_document->readlock()
				->save_to(std::ostreambuf_iterator<char>(o),
					  false);
			o.str();
		});

	auto doc=LIBCXX_NAMESPACE::xml::doc::create(docstr.begin(),
						    docstr.end(), "test34",
						    "noent");

	auto lock=doc->readlock();
	lock->get_root();
	lock->get_first_child();
	lock->get_first_child();

	if (lock->get_text() != "Hello XML world")
		throw EXCEPTION("Parsed entity expansion fail in test34");
}

static void test40()
{
	std::string docstr="<root><child><subchild><subsubchild/></subchild><subchild><subsubchild /></subchild></child><bool /></root>";

	auto doc=LIBCXX_NAMESPACE::xml::doc::create(docstr.begin(),
						    docstr.end(),
						    "test40");

	{
		auto readlock=doc->readlock();

		readlock->get_root();

		readlock->get_xpath("child")->to_node();

		if (readlock->path() != "/root/child")
			throw EXCEPTION("to_node() failed");

		bool caught=false;

		try {
			readlock->get_xpath("subchild[");
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cerr << "Expected exception: [" << e
				  << "]" << std::endl;
			caught=true;
		}

		if (!caught)
			throw EXCEPTION("Did not catch expected exception");

		caught=false;

		try {
			readlock->get_xpath("subchild")->to_node();
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cerr << "Expected exception: [" << e
				  << "]" << std::endl;
			caught=true;
		}

		if (!caught)
			throw EXCEPTION("Did not catch expected exception");

		caught=false;

		try {
			readlock->get_xpath("subchild")->to_node(3);
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cerr << "Expected exception: [" << e
				  << "]" << std::endl;
			caught=true;
		}

		if (!caught)
			throw EXCEPTION("Did not catch expected exception");

		if (readlock->path() != "/root/child")
			throw EXCEPTION("Lost read lock position at some point");
		readlock->get_xpath("subchild")->to_node(2);

		if (readlock->path()
		    != "/root/child/subchild[2]")
			throw EXCEPTION("New read lock position is unexpected in test40");
	}

	{
		auto writelock=doc->writelock();

		writelock->get_root();

		writelock->get_xpath("child/subchild")->to_node(2);
		if (writelock->path() != "/root/child/subchild[2]")
			throw EXCEPTION("set_node() failed");
	}

	{
		auto writelock=doc->writelock();

		writelock->get_root();

		auto xpath=writelock->get_xpath("child/subchild/subsubchild");

		if (xpath->count() != 2)
			throw EXCEPTION("Expected 2 subchild elements");

		writelock->get_xpath("bool")->to_node();

		if (xpath->count() != 2)
			throw EXCEPTION("Expected 2 subchild elements");

		xpath->to_node(1);
		if (writelock->path() != "/root/child/subchild[1]/subsubchild")
			throw EXCEPTION("xpath 1 is wrong");
		xpath->to_node(2);
		if (writelock->path() != "/root/child/subchild[2]/subsubchild")
			throw EXCEPTION("xpath 2 is wrong");
		writelock->get_parent();
		writelock->remove();

		if (xpath->count() != 2)
			throw EXCEPTION("xpath is not valid any more");

		if (writelock->path() != "/root/child")
			throw EXCEPTION("did not position to parent node");

		xpath->to_node(1);
		if (writelock->path() != "/root/child/subchild/subsubchild")
			throw EXCEPTION("xpath 1 is wrong after remove");

		bool thrown=false;

		try {
			xpath->to_node(2);
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cout << "Expected exception: " << e << std::endl;
			thrown=true;
		}

		if (!thrown)
			throw EXCEPTION("to_node succeeded after removal");
	}
}

static void test50()
{
	std::string docstr=
		"<html xmlns:xi=\"http://www.w3.org/2001/XInclude\">"
		"<xi:include href=\"xincluded.xml\" />"
		"</html>";

	std::ofstream("xincluded.xml") << "<p>Hello world</p>" << std::flush;

	auto doc=LIBCXX_NAMESPACE::xml::doc::create(docstr.begin(),
						    docstr.end(),
						    "test50",
						    "xinclude");

	doc=doc->readlock()->clone_document();
	auto lock=doc->readlock();

	lock->get_root();
	lock->get_xpath("p")->to_node();

	if (lock->get_text() != "Hello world")
		throw EXCEPTION("test50 failed");

	std::ofstream("xincluded.xml") << "<p>Hello world" << std::flush;

	bool caught=false;
	try {
		LIBCXX_NAMESPACE::xml::doc::create(docstr.begin(),
						   docstr.end(),
						   "test50",
						   "xinclude");
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		caught=true;
		std::cout << "Caught expected exception: [" << e << "]"
			  << std::endl;
	}

	unlink("xincluded.xml");

	if (!caught)
		throw EXCEPTION("test50 did not catch the expected exception");
}

void test70()
{
	auto d=LIBCXX_NAMESPACE::xml::doc::create();

	auto l=d->writelock();
	auto c=l->create_child();

	c=c->element({"root"});

	c->attribute({"id",
		      "\372\304\322\301\327\323\324\327\325\312\324\305"});
	c->text("[\372\304\322\301\327\323\324\327\325\312\324\305]");

	std::ostringstream o;

	l->save_to(std::ostreambuf_iterator<char>(o), false);

	auto s=o.str();

	if (s !=
	    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	    "<root id=\"Здравствуйте\">[Здравствуйте]</root>\n")
		throw EXCEPTION("test70: generated:\n" << s);

	d=parse(s);

	l=d->writelock();

	l->get_root();

	if (l->get_any_attribute("id") !=
	    "\372\304\322\301\327\323\324\327\325\312\324\305")
		throw EXCEPTION("test70: didn't transcode attribute");

	if (l->get_text() !=
	    "[\372\304\322\301\327\323\324\327\325\312\324\305]")
		throw EXCEPTION("test70: didn't transcode text");
}

void test80()
{
	auto d=LIBCXX_NAMESPACE::xml::doc::create();

	auto l=d->writelock();
	auto c=l->create_child();

	c=c->element({"root"});

	c->text(LIBCXX_NAMESPACE::number<int,void>{-20});

	std::ostringstream o;

	l->save_to(std::ostreambuf_iterator<char>(o), false);

	if (o.str() != "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	    "<root>-20</root>\n")
		throw EXCEPTION("test80 failed");
}

void test90()
{
	auto doc=({
			std::string dummy=
				"<root xmlns:x='http://www.example.com'>"
				"<child x:attribute='40.2' x:n='5'>10</child>"
				"</root>";

			LIBCXX_NAMESPACE::xml::doc::create(dummy.begin(),
							   dummy.end(),
							   "string");
		});

	auto lock=doc->readlock();

	lock->get_root();

	lock->get_xpath("/root/child")->to_node();

	if (lock->get_attribute<double>("attribute",
					"http://www.example.com") != 40.2)
		throw EXCEPTION("get_attribute<double> failed");
	if (lock->get_any_attribute<double>("attribute") != 40.2)
		throw EXCEPTION("get_attribute<double> failed");

	if (lock->get_text<float>() != 10)
		throw EXCEPTION("get_text<float> failed");

	if (lock->get_text<unsigned>() != 10)
		throw EXCEPTION("get_text<unsigned> failed");

	bool caught=false;

	try {
		lock->get_any_attribute<int>("attribute");
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "Expected exception: " << e << std::endl;
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("Expected exception from get_any_attribute "
				"was not thrown");

	if (lock->get_text<LIBCXX_NAMESPACE::number<int, void>>() != 10 ||
	    lock->get_attribute<LIBCXX_NAMESPACE::number<int, void>>(
		    "n",
		    "http://www.example.com") != 5 ||
	    lock->get_any_attribute<LIBCXX_NAMESPACE::number<int, void>>(
		    "n"
	    ) != 5)
		throw EXCEPTION("get<number> failed");

}

void test100()
{
	auto doc1=({
			std::string doc="<root><child1><child2>text</child2></child1></root>";

			LIBCXX_NAMESPACE::xml::doc::create(doc.begin(),
							   doc.end(), "STRING");
		});

	auto doc2=LIBCXX_NAMESPACE::xml::doc::create();

	auto lock=doc2->writelock();

	lock->create_child()->element({"root2"});

	auto rlock=doc1->readlock();

	rlock->get_root();

	rlock->get_xpath("/root/child1")->to_node();

	lock->create_child()->clone(rlock);

	std::string s;

	lock->save_to(std::back_insert_iterator<std::string>(s));

	if (s != "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	    "<root2><child1><child2>text</child2></child1></root2>\n")
		throw EXCEPTION("clone() failed");

}

void test110()
{
	auto doc1=({
			std::string doc="<root xmlns:x='http://www.example.com'><child2/><x:child1 xmlns:y='http://www.example.com/y'>text</x:child1></root>";

			LIBCXX_NAMESPACE::xml::doc::create(doc.begin(),
							   doc.end(), "STRING");
		});

	auto lock=doc1->readlock();

	lock->get_root();

	lock->get_xpath("/root/child2")->to_node();

	lock->get_xpath("/root/z:child1", {
			{ "z", "http://www.example.com"},
			{ "x", "http://www.example.com/wrongx"}
		})->to_node();

	if (lock->get_text() != "text")
		throw EXCEPTION("Namespace xpath 1 failed");

	if (lock->namespaces() != std::unordered_map<std::string, std::string>{
			{"x", "http://www.example.com" },
			{"y", "http://www.example.com/y" }
		})
		throw EXCEPTION("namespaces() failed");

	lock->get_xpath("/root/child2")->to_node();

	lock->get_xpath("/root/x:child1", {
			{ "x", LIBCXX_NAMESPACE::uriimpl{
					"http://www.example.com"
				}},
			{ "sc", "http://www.example.com"},
			{ "ss", std::string{"http://www.example.com"}},
		})->to_node();

	if (lock->get_text() != "text")
		throw EXCEPTION("Namespace xpath 2 failed");

	lock->get_xpath("/root/child2")->to_node();
	lock->get_xpath("/root/x:child1")->to_node();

	if (lock->get_text() != "text")
		throw EXCEPTION("Namespace xpath 3 failed");

	{

	auto doc1=({
			std::string doc="<?xml version=\"1.0\"?>\n\
<libcxx:windows xmlns:libcxx=\"https://www.libcxx.org/w\" libcxx:version=\"1640739574.805326267\"></libcxx:windows>";

			LIBCXX_NAMESPACE::xml::doc::create(doc.begin(),
							   doc.end(), "STRING");
		});
	}

	lock=doc1->readlock();
	lock->get_root();

	if (lock->get_xpath("libcxx:window/libcxx:table", {
				{"libcxx", "https://www.libcxx.org/w"},
			})->count() != 0)
		throw EXCEPTION("Empty nodeset's count is not 0");
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
		test9();
		test10();

		test20();
		test21();
		test22();
		test30();
		test31();
		test32();
		test33();
		test34();

		test40();
		test50();

		LIBCXX_NAMESPACE::locale::create("ru_RU.KOI8-R")->global();
		test70();

		test80();
		test90();
		test100();
		test110();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
