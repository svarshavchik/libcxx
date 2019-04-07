/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/cookiejar.H"
#include "x/http/responseimpl.H"
#include "x/options.H"

void storecookiejar(const LIBCXX_NAMESPACE::http::cookiejar &jar,
		    const std::string &uri,
		    const std::string &cookie)
{
	LIBCXX_NAMESPACE::http::responseimpl resp;

	std::string str="HTTP/1.0 200 Ok\r\n"
		"Set-Cookie: " + cookie + "\r\n\r\n";

	resp.parse(str.begin(), str.end(), 10000);

	jar->store(uri, resp);
}

void testcookiejar()
{
	auto jar=LIBCXX_NAMESPACE::http::cookiejar::create();

	storecookiejar(jar,
		       "http://subdomain.example.com/cgi-bin/script",
		       "name1=value1; path=\"/subdir\"; domain=.example.com");

	storecookiejar(jar,
		       "http://subdomain.example.com/cgi-bin/script",
		       "name2=value2; expires=\"" + ({
				       LIBCXX_NAMESPACE::http::cookie c;

				       c.expiration=time(NULL)+24*60*60;

				       c.expires_as_string();
			       }) + "\"; secure; httponly");

	jar->save("testcookiejar.txt");

	jar=LIBCXX_NAMESPACE::http::cookiejar::create();

	jar->load("testcookiejar.txt");
	unlink("testcookiejar.txt");

	std::list<LIBCXX_NAMESPACE::http::cookie> l(jar->begin(), jar->end());

	if (l.size() != 1)
		throw EXCEPTION("Did not load one expected cookies");

	auto &c=l.front();

	if (c.name != "name2" || c.value != "value2")
		throw EXCEPTION("Did not load name2 cookie");

	if (!c.has_expiration())
		throw EXCEPTION("Expiration not loaded");

	if (c.domain != "subdomain.example.com")
		throw EXCEPTION("Domain is " + c.domain);

	if (c.path != "/cgi-bin")
		throw EXCEPTION("Path is " + c.domain);

	if (!c.secure)
		throw EXCEPTION("Did not load the secure flag");

	if (!c.httponly)
		throw EXCEPTION("Did not load the httponly flag");
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
		testcookiejar();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}

