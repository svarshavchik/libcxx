#include <x/mime/encoder.H>
#include <vector>
#include <iostream>
#include <iterator>

int main(int argc, char **argv)
{
	std::vector<x::mime::encoder> sections;

	for (int i=1; i<argc; ++i)
	{
		x::headersimpl<x::headersbase::lf_endl> headers;

		x::mime::structured_content_header ct(x::mime::filetype(argv[i],
									true));

		if (ct.value.substr(0, 5) == "text/")
			ct("charset", x::mime::filecharset(argv[i]));

		ct.append(headers, ct.content_type);

		x::mime::structured_content_header("attachment")
			.rfc2231("filename", argv[i], "UTF-8")
			.append(headers, x::mime::structured_content_header
				::content_disposition);

		sections.push_back(x::mime::make_section(headers,
							 x::mime::from_file(argv[i])));
	}

	x::headersimpl<x::headersbase::lf_endl> headers;
	headers.append("Mime-Version", "1.0");

	x::mime::encoder multipart=
		x::mime::make_multipart_section(headers, "mixed",
						sections.begin(),
						sections.end());

	std::copy(multipart->begin(),
		  multipart->end(),
		  std::ostreambuf_iterator<char>(std::cout));
	return 0;
}
