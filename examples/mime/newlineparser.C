#include <x/mime/newlineiter.H>
#include <vector>
#include <iterator>
#include <iostream>

int main()
{
	std::vector<int> seq;

	typedef std::back_insert_iterator<std::vector<int>> ins_iter_t;

	auto iter=std::copy(std::istreambuf_iterator<char>(std::cin),
			    std::istreambuf_iterator<char>(),
			    x::mime::newline_iter<ins_iter_t>
			    ::create(ins_iter_t(seq)));

	iter.get()->eof();

	ins_iter_t value=iter.get()->iter;

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
