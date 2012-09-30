#include <x/hier.H>
#include <x/strtok.H>

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <list>

class booksObj : public x::obj {

public:
	booksObj() {}
	~booksObj() noexcept {}

	std::set<std::string> names;
};

typedef x::ref<booksObj> books;

typedef x::hier<int, books> classindex;

bool insert_book(const classindex &catalog,
		 const std::list<int> &number,
		 const std::string &name)
{
	bool updated=true;

	catalog->insert([&]
			{
				auto newbooks=books::create();

				newbooks->names.insert(name);
				return newbooks;
			}, number,
			[&]
			(books && existing_books)
			{
				updated=existing_books
					->names.insert(name).second;
				return false;
			});
	return updated;
}

std::list<int> tokey(const std::string &word)
{
	std::list<std::string> words;

	x::strtok_str(word, " ./\t\r", words);

	std::list<int> key;

	for (const std::string &word:words)
	{
		std::istringstream i(word);

		int n=0;

		if ((i >> n).fail())
		{
			key.clear();
			break;
		}

		key.push_back(n);
	}

	if (key.empty())
		std::cerr << "Invalid number" << std::endl;
	return key;
}

std::string fromkey(const std::list<int> &key)
{
	std::ostringstream o;

	const char *sep="";

	for (int n:key)
	{
		o << sep << n;
		sep=".";
	}
	return o.str();
}

int main()
{
	auto decimal_system=classindex::create();

	std::string line;

	while ((std::cout << "> " << std::flush),
	       !std::getline(std::cin, line).eof())
	{
		std::list<std::string> words;

		x::strtok_str(line, " \t\r", words);

		if (words.empty())
			continue;

		std::string cmd=words.front();

		words.pop_front();

		if (cmd == "add" && !words.empty())
		{
			std::list<int> key=tokey(words.front());

			if (key.empty())
				continue;

			std::cout << "Title: " << std::flush;

			if (std::getline(std::cin, line).eof())
				break;

			if (!insert_book(decimal_system, key, line))
				std::cerr << "Already exists" << std::endl;
			continue;
		}

		if (cmd == "get" && !words.empty())
		{
			std::list<int> key=tokey(words.front());

			classindex::base::readlock lock=
				decimal_system->create_readlock();

			if (!lock->to_child(key, true))
			{
				std::cerr << "Not found" << std::endl;
				continue;
			}

			std::cout << "Found: "
				  << fromkey(lock->name())
				  << std::endl;

			for (const auto &name:lock->entry()->names)
			{
				std::cout << "   " << name << std::endl;
			}
			continue;
		}

		if (cmd == "del" && !words.empty())
		{
			std::list<int> key=tokey(words.front());

			if (key.empty())
				continue;

			classindex::base::writelock lock=
				decimal_system->create_writelock();

			if (!lock->to_child(key, false))
			{
				std::cerr << "Not found" << std::endl;
				continue;
			}

			std::cout << "Found: "
				  << fromkey(lock->name())
				  << std::endl;

			for (const auto &name:lock->entry()->names)
			{
				std::cout << "   " << name << std::endl;
			}
			std::cout << "Delete? " << std::flush;

			if (std::getline(std::cin, line).eof())
				break;

			words.clear();
			x::strtok_str(line, " \t\r", words);

			if (words.empty())
				continue;

			if (words.front().substr(0, 1) == "y")
			{
				lock->erase();
				std::cout << "Removed" << std::endl;
			}
			continue;
		}

		if (cmd == "list")
		{
			for (const std::list<int> &key: *decimal_system)
			{
				std::cout << "    " << fromkey(key)
					  << std::endl;
			}
			continue;
		}
	}
	return 0;
}
