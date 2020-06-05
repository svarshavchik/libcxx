#include <x/http/useragent.H>
#include <iterator>
#include <iostream>
#include <cstdlib>

void http_example()
{
	x::http::useragent::base::https_enable();

	x::http::useragent ua(x::http::useragent::create(x::http::noverifycert));

	x::http::useragent::base::response
		resp=ua->request(x::http::GET, "https://localhost");

	std::cout << resp->message.get_status_code()
		  << " " << resp->message.get_reason_phrase() << std::endl;

	for (auto hdr: resp->message)
	{
		std::cout << hdr.first << "="
			  << hdr.second.value() << std::endl;
	}

	if (resp->has_content())
	{
		std::copy(resp->begin(), resp->end(),
			  std::ostreambuf_iterator<char>(std::cout.rdbuf()));
		std::cout << std::flush;
	}

	if (resp->message.get_status_code_class() != x::http::resp_success)
		exit(1);

}

int main()
{
	http_example();
	return 0;
}
