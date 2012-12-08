#include <x/mime/newlineiter.H>
#include <x/mime/bodystartiter.H>
#include <vector>
#include <iterator>
#include <iostream>

int main()
{
	std::vector<int> seq;

	typedef std::back_insert_iterator<std::vector<int>> ins_iter_t;

	typedef x::mime::bodystart_iter<ins_iter_t> bodystart_iter_t;

	typedef x::mime::newline_iter<bodystart_iter_t> newline_iter_t;

	auto iter=std::copy(std::istreambuf_iterator<char>(std::cin),
			    std::istreambuf_iterator<char>(),
			    newline_iter_t::create
			    (bodystart_iter_t::create
			     (ins_iter_t(seq))));

	iter.get()->eof();

	ins_iter_t value=iter.get()->iter.get()->iter;

	for (int c:seq)
	{
		if (x::mime::nontoken(c))
			std::cout << (char)c;
		else
			std::cout << '<' << c << '>';
	}
	std::cout << std::endl << std::flush;
	return 0;
}
