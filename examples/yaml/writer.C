#include <x/yaml/writer.H>
#include <x/yaml/newdocumentnode.H>
#include <x/yaml/newsequencenode.H>
#include <x/yaml/newmappingnode.H>
#include <x/yaml/newscalarnode.H>

#include <cstdlib>

// A list of continents, the highest point in each continent, and which country
// it's in.

struct highest_point {
	const char *continent;
	const char *name;
	const char *country;
};

const highest_point continent_list[]={
	{"Africa", "Mount Kilimanjaro","Tanzania"},
	{"Antarctica", "Vinson Massif","Antarctica"},
	{"Asia", "Mount Everest","China/Nepal"},
	{"Australia", "Puncak Jaya","Papua, Indonesia"},
	{"Europe", "Mount Elbrus","Russia"},
	{"North America", "Mount McKinley","United States"},
	{"South America", "Aconcagua","Argentina"},
};

// Write a mapping from a highest_point object

// Constructs an x::yaml::newnode that represents a mapping created from a
// highest_point object.

x::yaml::newnode highest_point_to_yaml(const highest_point &hp)
{
	// x::yaml::newmapping::create constructs an x::yaml::newnode that
	// invokes a functor that creates a mapping, when writing the
	// document. Note that hp is captured by value, so that the
	// highest_point argument, that's passed by reference, is safely
	// tucked away in the lambda.

	return x::yaml::newmapping::create
		([hp]
		 (x::yaml::newmapping &info)
		 {
			 std::list<std::pair<x::yaml::newnode,
					     x::yaml::newnode> > list;

			 // Fill the container with the mapping.

			 list.push_back(std::make_pair
					(x::yaml::newscalarnode::create
					 ("continent"),
					 x::yaml::newscalarnode::create
					 (hp.continent)));
			 list.push_back(std::make_pair
					(x::yaml::newscalarnode::create
					 ("highest_point"),
					 x::yaml::newscalarnode::create
					 (hp.name)));
			 list.push_back(std::make_pair
					(x::yaml::newscalarnode::create
					 ("country"),
					 x::yaml::newscalarnode::create
					 (hp.country)));

			 info.style=YAML_FLOW_MAPPING_STYLE;
			 info(list.begin(), list.end());
		 });
}

x::yaml::newnode list_of_highest_points_to_yaml()
{
	// x::yaml::newsequence<>::create constructs an x::yaml::newnode that
	// invokes the given functor to create a sequence, when writing the
	// document.

	return x::yaml::newsequence::create
		([]
		 (x::yaml::newsequence &info)
		 {
			 // Iterate over the array. Use highest_point_to_yaml()
			 // to construct a node for each element in the array.

			 std::list<x::yaml::newnode> list;

			 for (const highest_point &hp:continent_list)
			 {
				 list.push_back(highest_point_to_yaml(hp));
			 }
			 info(list.begin(), list.end());
		 });
}

void writeyaml()
{
	x::yaml::newdocumentnode doc=x::yaml::make_newdocumentnode
		([]
		 {
			 return list_of_highest_points_to_yaml();
		 });

	x::yaml::writer
		::write_one_document(doc,
				     std::ostreambuf_iterator<char>(std::cout));
}

int main(int argc, char **argv)
{
	try {
		writeyaml();
	} catch (const x::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
