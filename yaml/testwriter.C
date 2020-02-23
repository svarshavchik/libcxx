/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/yaml/writer.H"
#include "x/yaml/newaliasnode.H"
#include "x/yaml/newsequencenode.H"
#include "x/yaml/newmappingnode.H"
#include "x/yaml/newscalarnode.H"
#include "x/yaml/newnode.H"
#include "x/yaml/parser.H"
#include "x/yaml/mappingnode.H"
#include "x/yaml/scalarnode.H"
#include "x/yaml/sequencenode.H"
#include "x/join.H"
#include "x/options.H"
#include <iterator>
#include <iostream>
#include <cstdlib>

typedef LIBCXX_NAMESPACE::yaml::newmapping map_t;
typedef LIBCXX_NAMESPACE::yaml::newsequence seq_t;
typedef LIBCXX_NAMESPACE::yaml::newscalarnode scalar_t;
typedef LIBCXX_NAMESPACE::yaml::newaliasnode alias_t;

static LIBCXX_NAMESPACE::yaml::newnode create_seq1()
{
	return seq_t::create
		([]
		 (seq_t &info)
		 {
			 info.anchor="alias1";

			 std::list<scalar_t> scalar_list;
			 scalar_list.push_back(scalar_t::create("scalar1"));
			 scalar_list.push_back(scalar_t::create("scalar2"));

			 info(scalar_list.begin(), scalar_list.end());
		 });
}


static LIBCXX_NAMESPACE::yaml::newnode create_map1()
{
	return map_t::create
		([]
		 (map_t &info)
		 {
			 auto newseq1=create_seq1();

			 std::list<std::pair<LIBCXX_NAMESPACE::yaml::newnode,
					     LIBCXX_NAMESPACE::yaml::newnode>
				   > scalar_map;

			 scalar_map.push_back(std::make_pair
					      (scalar_t::create("seq1"),
					       newseq1));

			 scalar_map.push_back(std::make_pair
					      (scalar_t::create("seq2"),
					       alias_t::create("alias1")));
			 info(scalar_map.begin(), scalar_map.end());
		 });
}

void testwriter()
{
	auto testdocument=LIBCXX_NAMESPACE::yaml::make_newdocumentnode
		([]
		 {
			 return map_t::create
				 ([]
				  (map_t &info)
				  {
					  auto entry=std::make_pair
						  (scalar_t::create("maps"),
						   create_map1());
					  info(&entry, &entry+1);
				  });
		 });


	std::vector<LIBCXX_NAMESPACE::yaml::newdocumentnode> dummy;

	dummy.push_back(testdocument);
	dummy.push_back(testdocument);

	std::string s;

	LIBCXX_NAMESPACE::yaml::writer::write(dummy,
					      std::back_insert_iterator
					      <std::string>(s));

	std::cout << s << std::flush;

	auto parser=LIBCXX_NAMESPACE::yaml::parser::create(s.begin(), s.end());

	std::ostringstream parsed;

	for (const auto &document:parser->documents)
	{
		parsed << "doc:";

		LIBCXX_NAMESPACE::yaml::mappingnode map=document->root();

		for (std::pair<LIBCXX_NAMESPACE::yaml::scalarnode,
			     LIBCXX_NAMESPACE::yaml::mappingnode> outer: *map)
		{
			parsed << outer.first->value << "=";

			for (std::pair<LIBCXX_NAMESPACE::yaml::scalarnode,
				     LIBCXX_NAMESPACE::yaml::sequencenode>
				     inner: *outer.second)
			{
				parsed << inner.first->value << ":";

				const char *sep="";

				for (LIBCXX_NAMESPACE::yaml::scalarnode n
					     :*inner.second)
				{
					parsed << sep
					       << n->value;
					sep=",";
				}
				parsed << "|";
			}
		}
	}

	if (parsed.str() !=
	    "doc:maps=seq1:scalar1,scalar2|seq2:scalar1,scalar2|"
	    "doc:maps=seq1:scalar1,scalar2|seq2:scalar1,scalar2|")
		throw EXCEPTION("Writer failed");
}

void testwriter2()
{
	std::string s;

	LIBCXX_NAMESPACE::yaml::writer::write_one_document
		(LIBCXX_NAMESPACE::yaml::make_newdocumentnode
		 ([]
		  {
			  return LIBCXX_NAMESPACE::yaml::newmapping::create
				  ([]
				   (LIBCXX_NAMESPACE::yaml::newmapping &info)
				   {
					   std::list<std::pair<LIBCXX_NAMESPACE
							       ::yaml::newnode,
							       LIBCXX_NAMESPACE
							       ::yaml::newnode>
						     > scalar_map;

					   scalar_map.push_back
						   (std::make_pair
						    (LIBCXX_NAMESPACE
						     ::yaml::newscalarnode
						     ::create("fruit"),
						     LIBCXX_NAMESPACE
						     ::yaml::newscalarnode
						     ::create("apple")
						     ));
					   scalar_map.push_back
						   (std::make_pair
						    (LIBCXX_NAMESPACE
						     ::yaml::newscalarnode
						     ::create("vegetable"),
						     LIBCXX_NAMESPACE
						     ::yaml::newscalarnode
						     ::create("carrot")
						     ));
					   info(scalar_map.begin(),
						scalar_map.end());
				   });
		  }),

		 std::back_insert_iterator<std::string>(s));

	std::cout << s << std::endl;
}

class exception_iter {

public:
	exception_iter &operator*()
	{
		return *this;
	}

	exception_iter &operator++()
	{
		return *this;
	}

	exception_iter &operator++(int)
	{
		return *this;
	}

	void operator=(char c)
	{
		throw EXCEPTION("Expected exception");
	}
};

void testexception()
{
	bool caught=false;

	try {
		LIBCXX_NAMESPACE::yaml::writer::write_one_document
			(LIBCXX_NAMESPACE::yaml::make_newdocumentnode
			 ([]
			  {
				  return LIBCXX_NAMESPACE::yaml::newscalarnode
					  ::create("alpha");
			  }),
			 exception_iter());
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("Failed to catch a thrown exception");
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
		testwriter();
		testwriter2();
		testexception();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}

