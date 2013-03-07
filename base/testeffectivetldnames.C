/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/cookiejar.H"
#include "x/property_value.H"

static void checkPublicSuffix(const std::string &domain,
			      const std::string &public_suffix)
{
	auto s=LIBCXX_NAMESPACE::http::cookiejar::base::public_suffix(domain);

	if (s != public_suffix)
		throw EXCEPTION("Got public suffix \"" + s + "\" for "
				+ domain + ", but expected \""
				+ public_suffix + "\"");
}

static void testeffectivetldnames()
{
	LIBCXX_NAMESPACE::property::load_property(LIBCXX_NAMESPACE_WSTR
						  L"::http::effective_tld_names",
						  L"effective_tld_names.dat",
						  true, true);

	checkPublicSuffix("", "");
	// Mixed case.
	checkPublicSuffix("COM", "");
	checkPublicSuffix("example.COM", "example.com");
	checkPublicSuffix("WwW.example.COM", "example.com");
	// Leading dot.
	checkPublicSuffix(".com", "");
	checkPublicSuffix(".example", "");
	checkPublicSuffix(".example.com", "");
	checkPublicSuffix(".example.example", "");
	// Unlisted TLD.
	checkPublicSuffix("example", "");
	checkPublicSuffix("example.example", "example.example");
	checkPublicSuffix("b.example.example", "example.example");
	checkPublicSuffix("a.b.example.example", "example.example");
	// Listed, but non-Internet, TLD.
	//checkPublicSuffix("local", "");
	//checkPublicSuffix("example.local", "");
	//checkPublicSuffix("b.example.local", "");
	//checkPublicSuffix("a.b.example.local", "");
	// TLD with only 1 rule.
	checkPublicSuffix("biz", "");
	checkPublicSuffix("domain.biz", "domain.biz");
	checkPublicSuffix("b.domain.biz", "domain.biz");
	checkPublicSuffix("a.b.domain.biz", "domain.biz");
	// TLD with some 2-level rules.
	checkPublicSuffix("com", "");
	checkPublicSuffix("example.com", "example.com");
	checkPublicSuffix("b.example.com", "example.com");
	checkPublicSuffix("a.b.example.com", "example.com");
	checkPublicSuffix("uk.com", "");
	checkPublicSuffix("example.uk.com", "example.uk.com");
	checkPublicSuffix("b.example.uk.com", "example.uk.com");
	checkPublicSuffix("a.b.example.uk.com", "example.uk.com");
	checkPublicSuffix("test.ac", "test.ac");
	// TLD with only 1 (wildcard) rule.
	checkPublicSuffix("cy", "");
	checkPublicSuffix("c.cy", "");
	checkPublicSuffix("b.c.cy", "b.c.cy");
	checkPublicSuffix("a.b.c.cy", "b.c.cy");
	// More complex TLD.
	checkPublicSuffix("jp", "");
	checkPublicSuffix("test.jp", "test.jp");
	checkPublicSuffix("www.test.jp", "test.jp");
	checkPublicSuffix("ac.jp", "");
	checkPublicSuffix("test.ac.jp", "test.ac.jp");
	checkPublicSuffix("www.test.ac.jp", "test.ac.jp");
	checkPublicSuffix("kyoto.jp", "");
	checkPublicSuffix("test.kyoto.jp", "test.kyoto.jp");
	checkPublicSuffix("ide.kyoto.jp", "");
	checkPublicSuffix("b.ide.kyoto.jp", "b.ide.kyoto.jp");
	checkPublicSuffix("a.b.ide.kyoto.jp", "b.ide.kyoto.jp");
	checkPublicSuffix("c.kobe.jp", "");
	checkPublicSuffix("b.c.kobe.jp", "b.c.kobe.jp");
	checkPublicSuffix("a.b.c.kobe.jp", "b.c.kobe.jp");
	checkPublicSuffix("city.kobe.jp", "city.kobe.jp");
	checkPublicSuffix("www.city.kobe.jp", "city.kobe.jp");
	// TLD with a wildcard rule and exceptions.
	checkPublicSuffix("om", "");
	checkPublicSuffix("test.om", "");
	checkPublicSuffix("b.test.om", "b.test.om");
	checkPublicSuffix("a.b.test.om", "b.test.om");
	checkPublicSuffix("songfest.om", "songfest.om");
	checkPublicSuffix("www.songfest.om", "songfest.om");
	// US K12.
	checkPublicSuffix("us", "");
	checkPublicSuffix("test.us", "test.us");
	checkPublicSuffix("www.test.us", "test.us");
	checkPublicSuffix("ak.us", "");
	checkPublicSuffix("test.ak.us", "test.ak.us");
	checkPublicSuffix("www.test.ak.us", "test.ak.us");
	checkPublicSuffix("k12.ak.us", "");
	checkPublicSuffix("test.k12.ak.us", "test.k12.ak.us");
	checkPublicSuffix("www.test.k12.ak.us", "test.k12.ak.us");

}

int main(int argc, char **argv)
{
	try {
		testeffectivetldnames();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
