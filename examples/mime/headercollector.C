#include <x/mime/newlineiter.H>
#include <x/mime/bodystartiter.H>
#include <x/mime/headeriter.H>
#include <x/mime/headercollector.H>
#include <map>
#include <iterator>
#include <iostream>

int main()
{
	typedef x::mime::header_iter<x::mime::header_collector> header_iter_t;

	typedef x::mime::bodystart_iter<header_iter_t> bodystart_iter_t;

	typedef x::mime::newline_iter<bodystart_iter_t> newline_iter_t;

	auto iter=std::copy(std::istreambuf_iterator<char>(std::cin),
			    std::istreambuf_iterator<char>(),
			    newline_iter_t::create
			    (bodystart_iter_t::create
			     (header_iter_t::create
			      (x::mime::header_collector::create
			       ([] (const std::string &name,
				    const std::string &name_lc,
				    const std::string &value)
		{
			std::cout << "Header: " << name
			<< ", value: " << value
			<< std::endl;
		})))));

	iter.get()->eof();
	return 0;
}
