/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "uuid.H"
#include "exception.H"
#include "serialize.H"
#include "deserialize.H"
#include "tostring.H"
#include <iostream>
#include <cstdlib>
#include <random>

template<char padChar>
static void testbase64()
{
	std::mt19937 re;

	re.seed(time(NULL));

	std::uniform_int_distribution<char> chars;
	std::uniform_int_distribution<int> randlen(15, 29);
	std::uniform_int_distribution<int> randlinesize(0, 20);
	std::uniform_int_distribution<int> randbool(0, 2);

	typedef LIBCXX_NAMESPACE::base64<padChar> base64_t;

	for (size_t i=0; i<10; ++i)
	{
		size_t l=randlen(re);
		int randbool_v=randbool(re);
		size_t ls=0;

		if (randbool_v)
		{
			--randbool_v;

			ls=randlinesize(re);
		}

		char buf[l];

		std::generate(buf, buf+l, [&]
			      {
				      return (char)chars(re);
			      });

		size_t needed=base64_t::encoded_size(l, ls, randbool_v);
		size_t needed2=base64_t::encoded_size(&buf[0], &buf[l], ls, randbool_v);

		if (needed != needed2)
			throw EXCEPTION("encoded_size() does not agree with itself?");
		char ebuf[needed+10];

		size_t p=base64_t::encode(buf, buf+l, ebuf, ls, randbool_v)
			- ebuf;

		if (p != needed)
			throw EXCEPTION("encoded_size() failed with padded="
					+ std::string(padChar ? "1":"0")
					+ ", l="
					+ x::tostring(l)
					+ ", ls=" + x::tostring(ls)
					+ ", crlf=" + x::tostring(randbool_v)
					+ ", needed=" + x::tostring(needed)
					+ ", actual=" + x::tostring(p)
					+ ":\n"
					+ std::string(ebuf, ebuf+p));

		char buf2[l];

		auto res=base64_t::decode(ebuf, ebuf+p, buf2);

		if (!res.second)
			throw EXCEPTION("Decoding failed");

		if (res.first-buf2 != l
		    || !std::equal(buf, buf+l, buf2))
			throw EXCEPTION("did not decode to the same size");

		std::cout << std::string(ebuf, ebuf+p) << std::endl;
	}
}

static void testbase642()
{
	typedef LIBCXX_NAMESPACE::base64<0> base64_t;

	std::stringstream o;

	typedef std::ostreambuf_iterator<char> o_iter;

	auto enciter=base64_t::encoder<o_iter>(o_iter(o), 0);

	*enciter++='A';
	enciter.eof();

	o_iter value=enciter.eof();

	if (o.str().size() != 2)
		throw EXCEPTION("Padless base64 encoding wasn't two chars: "
				+ o.str());

	char buf[2];

	base64_t::decoder<char *> deciter(buf);

	*std::copy(std::istreambuf_iterator<char>(o),
		   std::istreambuf_iterator<char>(), deciter).eof().first=0;

	if (buf[0] != 'A' || buf[1])
		throw EXCEPTION("Huh?");
}

static void testuuid()
{
	std::cout << LIBCXX_NAMESPACE::tostring(LIBCXX_NAMESPACE::uuid())
		  << std::endl;
	std::cout << LIBCXX_NAMESPACE::tostring(LIBCXX_NAMESPACE::uuid())
		  << std::endl;

	LIBCXX_NAMESPACE::uuid uuid1;

	std::string s=LIBCXX_NAMESPACE::tostring(uuid1);

	LIBCXX_NAMESPACE::uuid uuid2(s);

	std::string t=LIBCXX_NAMESPACE::tostring(uuid2);

	if (s != t)
		throw EXCEPTION("Failed to save and restore a UUID");

	std::vector<char> sbuf;

	sbuf.resize(LIBCXX_NAMESPACE::serialize::object(uuid2));
	LIBCXX_NAMESPACE::serialize::object(uuid2, sbuf.begin());

	LIBCXX_NAMESPACE::deserialize::object(uuid1, sbuf);

	std::cout << LIBCXX_NAMESPACE::tostring(uuid1) << std::endl;

}

int main(int argc, char **argv)
{
	alarm(10);

	try {
		testbase64<'='>();
		testbase64<0>();
		testbase642();
		testuuid();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}
