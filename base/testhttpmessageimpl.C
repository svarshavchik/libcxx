/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "chrcasecmp.H"
#include "http/requestimpl.H"
#include "http/responseimpl.H"
#include "http/senderimpl.H"
#include "http/receiverimpl.H"
#include "http/accept_header.H"
#include "http/exception.H"
#include "mime/structured_content_header.H"
#include "strtok.H"
#include <iostream>
#include <iterator>

static void parsemsg(const std::string &str,
		     LIBCXX_NAMESPACE::http::requestimpl &msg)

{
	if (msg.parse(str.begin(), str.end(), 0) != str.end())
		throw EXCEPTION("Did not parse the full message");
}

std::string dumpmsg(LIBCXX_NAMESPACE::http::requestimpl &msg)

{
	std::string s;

	msg.toString(std::back_insert_iterator<std::string>(s));

	return s;
}

static void testparse()
{
	std::string lmsg=
		"get /images http/1.1\r\n"
		"Host: localhost\r\n"
		"Accept-Encoding: */*\r\n"
		"\r\n";

	LIBCXX_NAMESPACE::http::requestimpl msg;

	parsemsg(lmsg, msg);

	if (LIBCXX_NAMESPACE::chrcasecmp::str_not_equal_to()(dumpmsg(msg), lmsg))
		throw EXCEPTION("Dumped message did not match original: \n"
				+ dumpmsg(msg));
}

class testparsetokenheadercb {

public:

	std::string value;

	void operator()(const std::string &token, char terminator)
	{
		value += token;
		if (terminator)
			value.push_back(terminator);
		value.push_back('\n');
	}
};

static void testparsetokenheader()
{
	std::string lmsg=
		"get /images http/1.1\r\n"
		"Host: localhost\r\n"
		"Accept: */* ; q=0.100; qvalue=\"quoted, value\" , text/*; q=\"0.200\"\r\n"
		"accept: text/csv; headers=4 ; rows=6\r\n"
		"\r\n";

	LIBCXX_NAMESPACE::http::requestimpl msg;

	parsemsg(lmsg, msg);

	if (LIBCXX_NAMESPACE::chrcasecmp::str_not_equal_to()(dumpmsg(msg), lmsg))
		throw EXCEPTION("Dumped message did not match original: \n"
				+ dumpmsg(msg));

	testparsetokenheadercb cb;

	msg.process("accept",
		    [&]
		    (std::string::const_iterator b,
		     std::string::const_iterator e)
		    {
			    LIBCXX_NAMESPACE::
				    headersbase::parse_structured_header
				    (LIBCXX_NAMESPACE::headersbase
				     ::comma_or_semicolon,
				     [&]
				     (char sep, const std::string &w)
				     {
					     cb(w, sep);
				     }, b, e);
		    });

	if (cb.value !=
	    "*/*;\n"
	    "q=0.100;\n"
	    "qvalue=quoted, value,\n"
	    "text/*;\n"
	    "q=0.200\n"
	    "text/csv;\n"
	    "headers=4;\n"
	    "rows=6\n")
		throw EXCEPTION("parsetokenheader() sanity check failed: " +
				cb.value);
}

static void testacceptheader()
{
	std::string lmsg=
		"get /images http/1.1\r\n"
		"Host: localhost\r\n"
		"Accept: */*; q=0.100, text/*; q=0.2, text/html; q=0.3, text/html; version=2; q=0.4\r\n"
		"\r\n";

	LIBCXX_NAMESPACE::http::requestimpl msg;

	parsemsg(lmsg, msg);

	LIBCXX_NAMESPACE::http::accept_header hdr(msg);

	static const char *testcases[]={
		"image/gif:0",
		"image/gif;text/plain:1",
		"text/plain;image/gif:0",
		"text/plain;text/html:1",
		"text/html;text/plain:0",
		"text/plain;text/html,version=1:1",
		"text/html;text/html,version=1:0,1",
		"text/html;text/html,version=2:1",
		"text/html,version=2;text/html:0",
		0};

	typedef	std::vector< std::vector<LIBCXX_NAMESPACE::http
					 ::accept_header
					 ::media_type_t>
			     ::const_iterator>
	res_iter_vector_t;

	for (size_t i=0; testcases[i]; ++i)
	{
		std::vector<std::string> vec;

		LIBCXX_NAMESPACE::strtok_str(testcases[i], ":", vec);

		std::vector<std::string> res_str;

		LIBCXX_NAMESPACE::strtok_str(vec[1], ",", res_str);

		std::vector<int> res_int;

		for (size_t n=0; n<res_str.size(); ++n)
		{
			std::istringstream i(res_str[n]);

			res_int.push_back(0);
			i >> res_int[n];
		}

		std::vector<std::string> medias;

		LIBCXX_NAMESPACE::strtok_str(vec[0], ";", medias);

		std::vector<LIBCXX_NAMESPACE::http::accept_header
			    ::media_type_t> candidates;

		res_iter_vector_t res_iter;

		for (std::vector<std::string>::const_iterator
			     b(medias.begin()), e(medias.end()); b != e; ++b)
		{
			LIBCXX_NAMESPACE::http::accept_header::media_type_t c;

			size_t p=b->find('/');

			c.type=b->substr(0, p);

			std::string rest(b->substr(p+1));

			p=rest.find(',');
			c.subtype=rest.substr(0, p);

			if (p != std::string::npos)
			{
				std::vector<std::string> params;

				LIBCXX_NAMESPACE::strtok_str(rest.substr(p+1),
							   ",", params);

				for (std::vector<std::string>::const_iterator
					     b(params.begin()), e(params.end());
				     b != e; ++b)
				{
					LIBCXX_NAMESPACE::mime::parameter_t p;

					p.name=b->substr(0, b->find('='));
					p.value=b->substr(b->find('=')+1);
					c.media_parameters.insert(p);
				}
			}
			candidates.push_back(c);
		}

		hdr.find(candidates.begin(),
			 candidates.end(),
			 std::back_insert_iterator<res_iter_vector_t>
			 (res_iter));

		if (res_iter.size() == res_int.size())
		{
			size_t z;

			for (z=res_int.size(); z > 0; )
			{
				--z;
				if (res_iter[z] - candidates.begin() !=
				    res_int[z])
					break;
			}

			if (z == 0)
				continue;
		}

		throw EXCEPTION("accept_header.find() failed for "
				+ std::string(testcases[i]));
	}

	msg=LIBCXX_NAMESPACE::http::requestimpl();

	std::vector<LIBCXX_NAMESPACE::http::accept_header
		    ::media_type_t> candidates;

	candidates.push_back(LIBCXX_NAMESPACE::http::accept_header
			     ::media_type_t());

	res_iter_vector_t res;

	hdr.find(candidates.begin(),
		 candidates.end(), 
		 std::back_insert_iterator<res_iter_vector_t>(res));

	if (res.size() != 1)
		throw EXCEPTION("Missing Accept: header not treated as a acceptable wildcard");


	LIBCXX_NAMESPACE::http::accept_header
		h("text/plain; charset=us-ascii, "
		    "text/plain; q=.9, "
		    "text/plain; version=\"12[]\"; q=.5, "
		    "text/plain; version=\"\\\\\"; q=.003");

	std::ostringstream o;

	h.toString(std::ostreambuf_iterator<char>(o));

	if (o.str() != "text/plain; charset=us-ascii; q=1.000, text/plain; q=0.900, text/plain; version=\"12[]\"; q=0.500, text/plain; version=\"\\\\\\\\\"; q=0.003")
		throw EXCEPTION("Unexpected accept_header.toString result: " + o.str());
}

static void testcontenttypeheader()
{
	std::string s=LIBCXX_NAMESPACE::mime::structured_content_header
		("Content-Type: text/plain;  charset=\"utf-8\"");

	if (s != "Content-Type: text/plain; charset=utf-8")
		throw EXCEPTION("Unexpected content_type_header.toString result: " + s);

}

static void testsendcontentlength()
{
	std::string lmsg=
		"POST /content-length http/1.0\r\n"
		"Host: localhost\r\n"
		"\r\n";

	LIBCXX_NAMESPACE::http::requestimpl msg;

	parsemsg(lmsg, msg);

	std::ostringstream o;

	LIBCXX_NAMESPACE::http::senderimpl<std::ostreambuf_iterator<char> >
		formatter=
		LIBCXX_NAMESPACE::http::senderimpl<std::ostreambuf_iterator<char> >
		(std::ostreambuf_iterator<char>(o));

	formatter.setPeerHttp11();

	{
		LIBCXX_NAMESPACE::http::senderimpl_encode::wait_continue dummy;

		std::string str("Dummy\r\n");
		formatter.send(msg, str.begin(), str.end(), dummy);
		{
			std::string s(o.str());

			std::cout << s;

			LIBCXX_NAMESPACE::http::receiverimpl<LIBCXX_NAMESPACE::http::requestimpl, std::string::const_iterator> receiver(s.begin(), s.end(), 100);

			LIBCXX_NAMESPACE::http::requestimpl msg2;

			receiver.message(msg2);

			std::string s2;

			std::copy(receiver.begin(),
				  receiver.end(),
				  std::back_insert_iterator<std::string>(s2));

			if (s2 != "Dummy\r\n")
				throw EXCEPTION("Decoding failure (use content-length: header)");
		}

		o.str("");

		std::stringstream lst(str);

		formatter.send(msg, std::istreambuf_iterator<char>(lst.rdbuf()),
			       std::istreambuf_iterator<char>(), dummy);
		{
			std::string s(o.str());

			std::cout << s;

			LIBCXX_NAMESPACE::http::receiverimpl<LIBCXX_NAMESPACE::http::requestimpl, std::string::const_iterator> receiver(s.begin(), s.end(), 100);

			LIBCXX_NAMESPACE::http::requestimpl msg2;

			receiver.message(msg2);

			msg2.erase("content-length");

			std::string s2;

			std::copy(receiver.begin(),
				  receiver.end(),
				  std::back_insert_iterator<std::string>(s2));

			if (s2 != "Dummy\r\n")
				throw EXCEPTION("Decoding failure (decode till EOF)");
		}
	}
}

static void testsendchunked(size_t chunksize)
{
	std::string lmsg=
		"POST /content-length http/1.1\r\n"
		"Host: localhost\r\n"
		"\r\n";

	LIBCXX_NAMESPACE::http::requestimpl msg;

	parsemsg(lmsg, msg);

	LIBCXX_NAMESPACE::http::senderimpl_encode::chunksize.setValue(chunksize);

	std::ostringstream o;

	LIBCXX_NAMESPACE::http::senderimpl<std::ostreambuf_iterator<char> >
		formatter=
		LIBCXX_NAMESPACE::http::senderimpl<std::ostreambuf_iterator<char> >
		(std::ostreambuf_iterator<char>(o));

	formatter.setPeerHttp11();

	{
		LIBCXX_NAMESPACE::http::senderimpl_encode::wait_continue dummy;

		std::string str("Dummy string\r\n");

		std::stringstream lst(str);

		formatter.send(msg,
			       std::istreambuf_iterator<char>(lst.rdbuf()),
			       std::istreambuf_iterator<char>(), dummy);

		{
			std::string s(o.str());

			std::cout << s;

			LIBCXX_NAMESPACE::http::receiverimpl<LIBCXX_NAMESPACE::http::requestimpl, std::string::const_iterator> receiver(s.begin(), s.end(), 100);

			LIBCXX_NAMESPACE::http::requestimpl msg2;

			receiver.message(msg2);

			std::string s2;

			std::copy(receiver.begin(),
				  receiver.end(),
				  std::back_insert_iterator<std::string>(s2));

			if (s2 != "Dummy string\r\n")
				throw EXCEPTION("Decoding failure (chunked)");
		}
	}
}

void testbindings()
{
	LIBCXX_NAMESPACE::http::receiverimpl
		<LIBCXX_NAMESPACE::http::requestimpl, std::istreambuf_iterator<char> >
		reqreceiver=
		LIBCXX_NAMESPACE::http::receiverimpl
		<LIBCXX_NAMESPACE::http::requestimpl,
		 std::istreambuf_iterator<char> >
		(std::istreambuf_iterator<char>(),
		 std::istreambuf_iterator<char>(), 100);

	LIBCXX_NAMESPACE::http::requestimpl req;

	reqreceiver.message(req);

	LIBCXX_NAMESPACE::http::receiverimpl
		<LIBCXX_NAMESPACE::http::responseimpl,
		 std::istreambuf_iterator<char> >
		respreceiver=
		LIBCXX_NAMESPACE::http::receiverimpl
		<LIBCXX_NAMESPACE::http::responseimpl,
		 std::istreambuf_iterator<char> >
		(std::istreambuf_iterator<char>(),
		 std::istreambuf_iterator<char>(), 100);

	LIBCXX_NAMESPACE::http::responseimpl resp;
	respreceiver.message(resp, req);
}
	

static void testbadmessage(const std::string &message)

{
	LIBCXX_NAMESPACE::http::receiverimpl<LIBCXX_NAMESPACE::http::responseimpl,
					   std::string::const_iterator>
		parser(message.begin(), message.end(), 100);

	LIBCXX_NAMESPACE::http::responseimpl resp;

	std::string dummy;

	try {
		parser.message(resp,
			       LIBCXX_NAMESPACE::http::requestimpl());
		std::copy(parser.begin(), parser.end(),
			  std::back_insert_iterator<std::string>(dummy));
	} catch (LIBCXX_NAMESPACE::http::response_exception &resp)
	{
		return;
	}

	throw EXCEPTION("Did not throw an exception for a misformatted message");
}

int main(int argc, char **argv)
{
	try {
		testparse();
		testparsetokenheader();
		testacceptheader();
		testcontenttypeheader();
		testsendcontentlength();
		testsendchunked(100);
		testsendchunked(10);
		testsendchunked(14);

		testbadmessage("200 Ok\r\n");
		testbadmessage("HTTP/1.1 200 Ok\r\n"
			       "Content-Length: 20\r\n"
			       "\r\n"
			       "dummy\r\n");
		testbadmessage("HTTP/1.1 200 Ok\r\n"
			       "Transfer-Encoding: chunked\r\n"
			       "\r\n"
			       "8\n"
			       "dummy\r\n");
		testbadmessage("HTTP/1.1 200 Ok\r\n"
			       "Transfer-Encoding: chunked\r\n"
			       "\r\n"
			       "7\n"
			       "dummy\r\n");
		testbadmessage("HTTP/1.1 200 Ok\r\n"
			       "Transfer-Encoding: chunked\r\n"
			       "\r\n"
			       "7\n"
			       "dummy\r\n"
			       "0\r\n");
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

