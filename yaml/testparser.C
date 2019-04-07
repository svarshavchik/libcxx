/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/yaml/parser.H"
#include "x/yaml/mappingnode.H"
#include "x/yaml/scalarnode.H"
#include "x/yaml/sequencenode.H"
#include "x/options.H"
#include <iostream>
#include <cstdlib>

void testparser()
{
	std::string document_text=
		"yaml:\n"
		"    name1: &aliasevent\n"
		"        key1: value1\n"
		"        key2: value2\n"
		"    name2: *aliasevent\n"
		"---\n"
		"- list1\n"
		"- list2\n";

	auto parser=LIBCXX_NAMESPACE::yaml::parser
		::create(document_text.begin(),
			 document_text.end());

	if (parser->documents.size() != 2)
		throw EXCEPTION("Did not parse two documents");

	auto first_document=parser->documents.front();

	LIBCXX_NAMESPACE::yaml::mappingnode node=first_document->root();

	if (node->size() != 1)
		throw EXCEPTION("First document's root node should be a mapping with 1 element");

	std::pair<LIBCXX_NAMESPACE::yaml::scalarnode,
		  LIBCXX_NAMESPACE::yaml::mappingnode>
		yaml_map=node->get(0);

	if (yaml_map.first->value != "yaml")
		throw EXCEPTION("First document's root node's map key should be 'yaml'");

	if (yaml_map.second->size() != 2)
		throw EXCEPTION("The yaml mapping should contain 2 elements");

	std::ostringstream o;

	yaml_map.second->for_each
		([&]
		 (LIBCXX_NAMESPACE::yaml::scalarnode &&key,
		  LIBCXX_NAMESPACE::yaml::mappingnode &&value)
		 {
			 o << key->value << ":";

			 value->for_each([&]
					 (LIBCXX_NAMESPACE::yaml::scalarnode &&key,
					  LIBCXX_NAMESPACE::yaml::scalarnode &&value)
					 {
						 o << key->value
						   << "="
						   << value->value << "|";
					 });

		 });

	if (o.str() != "name1:key1=value1|key2=value2|name2:key1=value1|key2=value2|")
	{
		std::cout << "Parsing failed for first document"
			  << std::endl;
	}

	LIBCXX_NAMESPACE::yaml::sequencenode seq=
		parser->documents.back()->root();

	if (seq->size() != 2)
		throw EXCEPTION("Sequence not of size 2");
	seq->get(0);

	std::ostringstream o2;

	seq->for_each([&]
		      (LIBCXX_NAMESPACE::yaml::scalarnode &&n)
		      {
			      o2 << n->value << "|";
		      });
	if (o2.str() != "list1|list2|")
		throw EXCEPTION("Sequence not what is expected");
}

class exception_iter {

public:
	const char *p;

	exception_iter(const char *pArg): p(pArg)
	{
	}

	~exception_iter() {}

	char operator*()
	{
		if (*p == '1')
			throw EXCEPTION("Expected exception");

		return *p;
	}

	exception_iter &operator++()
	{
		++p;
		return *this;
	}

	const char *operator++(int)
	{
		const char *q=p;

		++p;
		return q;
	}

	bool operator==(const exception_iter &o) const
	{
		return p == o.p;
	}

	bool operator!=(const exception_iter &o) const
	{
		return p != o.p;
	}

};

void testexception()
{
	static const char yaml[]=
		"- list1\n"
		"- list2\n";

	bool caught=false;

	try {
		LIBCXX_NAMESPACE::yaml::parser
			::create(exception_iter(yaml),
				 exception_iter(yaml+sizeof(yaml)-1));
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("Exception was not rethrown");
}

void testcircular()
{
	typedef LIBCXX_NAMESPACE::yaml::mappingnode m_t;

	std::string document_text=
		"yaml:\n"
		"    name1: &alias1\n"
		"        key1: value1\n"
		"        key2: *alias1\n";

	auto parser=LIBCXX_NAMESPACE::yaml::parser
		::create(document_text.begin(),
			 document_text.end());

	if (parser->documents.size() != 1)
		throw EXCEPTION("Did not parse two documents");

	if (LIBCXX_NAMESPACE::yaml::scalarnode
	    (m_t(m_t(m_t(m_t(m_t(parser->documents.front()->root())
			     ->get(0).second)->get(0).second)
		     ->get(1).second)->get(1).second)->get(0).second)
	    ->value != "value1")
		throw EXCEPTION("Circular reference parse failed");
}

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::list
		options(LIBCXX_NAMESPACE::option::list::create());

	options->addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser
		parser(LIBCXX_NAMESPACE::option::parser::create());

	parser->setOptions(options);

	if (parser->parseArgv(argc, argv) ||
	    parser->validate())
		exit(1);

	try {
		testparser();
		testcircular();
		testexception();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}

