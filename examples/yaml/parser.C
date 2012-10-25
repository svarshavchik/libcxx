#include <x/yaml/writer.H>
#include <x/yaml/parser.H>
#include <x/yaml/sequencenode.H>
#include <x/yaml/mappingnode.H>
#include <x/yaml/scalarnode.H>

#include <cstdlib>
#include <map>
#include <iostream>

void parseyaml()
{
	x::yaml::parser p=
		x::yaml::parser::create(std::istreambuf_iterator<char>(std::cin),
					std::istreambuf_iterator<char>());

	if (p->documents.size() != 1)
		throw EXCEPTION("stdin did not contain a single YAML document");

	x::yaml::document doc=p->documents.front();

	x::yaml::sequencenode seq=doc->root();

	for (x::yaml::mappingnode highest_point:*seq)
	{
		std::map<std::string, std::string> m;

		for (std::pair<x::yaml::scalarnode, x::yaml::scalarnode> mapping
			     : *highest_point)
		{
			m[mapping.first->value]=mapping.second->value;
		}

		std::cout << "In "
			  << m["continent"]
			  << ", the highest point is "
			  << m["highest_point"]
			  << ", located in "
			  << m["country"] << std::endl;
	}

}

int main(int argc, char **argv)
{
	try {
		parseyaml();
	} catch (const x::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
