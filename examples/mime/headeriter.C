#include <x/mime/newlineiter.H>
#include <x/mime/bodystartiter.H>
#include <x/mime/headeriter.H>
#include <map>
#include <iterator>
#include <iostream>

class headercollector {

public:
	typedef std::output_iterator_tag iterator_category;
	typedef void value_type;
	typedef void reference;
	typedef void difference_type;
	typedef void pointer;

	std::multimap<std::string, std::string> *h;
	std::string name;

	bool seen_contents_start;
	bool in_header_name;
	bool in_newline;
	bool in_fold;

	std::multimap<std::string, std::string>::iterator value;
	headercollector(std::multimap<std::string, std::string> *hArg)
		: h(hArg), in_header_name(false), seen_contents_start(false)
	{
	}

	void operator=(int c)
	{
		switch (c) {
		case x::mime::header_name_start:
			name.clear();
			in_header_name=true;
			return;
		case x::mime::header_name_end:
			value=h->insert(std::make_pair(name, ""));
			in_header_name=false;
			return;
		case x::mime::header_contents_start:
			seen_contents_start=true;
			in_newline=false;
			in_fold=false;
			return;
		case x::mime::header_contents_end:
			seen_contents_start=false;
			return;
		case x::mime::header_fold_start:
			in_fold=true;
			return;
		case x::mime::header_fold_end:
			in_fold=false;
			value->second.push_back(' ');
			return;
		case x::mime::newline_start:
			in_newline=true;
			return;
		case x::mime::newline_end:
			in_newline=false;
			return;
		}

		if (!x::mime::nontoken(c))
			return;

		if (in_header_name)
		{
			name.push_back(c);
			return;
		}

		if (!seen_contents_start || in_fold || in_newline)
			return;

		value->second.push_back(c);
	}

	headercollector &operator*()
	{
		return *this;
	}

	headercollector &operator++()
	{
		return *this;
	}

	headercollector &operator++(int)
	{
		return *this;
	}
};

int main()
{
	std::multimap<std::string, std::string> h;

	typedef x::mime::header_iter<headercollector> header_iter_t;

	typedef x::mime::bodystart_iter<header_iter_t> bodystart_iter_t;

	typedef x::mime::newline_iter<bodystart_iter_t> newline_iter_t;

	auto iter=std::copy(std::istreambuf_iterator<char>(std::cin),
			    std::istreambuf_iterator<char>(),
			    newline_iter_t::create
			    (bodystart_iter_t::create
			     (header_iter_t::create(&h))));

	iter.get()->eof();

	for (const auto &c:h)
	{
		std::cout << "Header: " << c.first << ", value: "
			  << c.second << std::endl;
	}
	return 0;
}
