/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/parser.H"
#include "x/xml/doc.H"
#include "x/exception.H"
#include "x/fd.H"
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
				"<root attribute='value'>"
				"<child>Text<i>element</i></child>"
				"<child2>Text2<i>element2</i></child2>"
				"</root>";

			LIBCXX_NAMESPACE::xml::doc::create(dummy.begin(),
							   dummy.end(),
							   "string");
		});

	lock=document->readlock();
	if (!lock->get_root() || lock->type() != "element_node")
		throw EXCEPTION("Root node is not an element node");
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

}

int main(int argc, char **argv)
{
	try {
		test1(100);
		test1(LIBCXX_NAMESPACE::fdbaseObj::get_buffer_size());
		test2();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

