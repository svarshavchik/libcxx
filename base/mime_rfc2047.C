/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mime/rfc2047.H"
#include "x/mime/headeriter.H"
#include "x/tostring.H"
#include "x/iconviofilter.H"
#include "x/qp.H"
#include "x/base64.H"
#include "x/chrcasecmp.H"
#include "x/exception.H"

#include "gettext_in.h"

#include <sstream>

namespace LIBCXX_NAMESPACE {
	namespace mime {
#if 0
	};
};
#endif

#define UTF8 "UTF-8"

#define IS_UTF8TAIL(c)  (((int)(c) & 0xC0) == 0x80)

rfc2047_encode_base::rfc2047_encode_base()
{
}

rfc2047_encode_base::~rfc2047_encode_base()
{
}

class LIBCXX_HIDDEN rfc2047_std_encoding : public rfc2047_encode_base {
 public:

	rfc2047_std_encoding() {}
	~rfc2047_std_encoding() {}

	bool do_encode(char32_t c) override
	{
		return false;
	}
};

std::string to_rfc2047(const std::u32string &str)
{
	return to_rfc2047("", str);
}

std::string to_rfc2047(const std::string &language,
		       const std::u32string &str)
{
#pragma GCC visibility push(hidden)

	rfc2047_std_encoding encoder;

	encoder.encode(language, str);

	return encoder.formatted_string;
#pragma GCC visibility pop
}

// ------------------------------------------------------------------------

#define WHITESPACE(c) ((c) == ' ' || (c) == '\t')

std::u32string::const_iterator
rfc2047_encode_base::next_word(std::u32string::const_iterator b,
			       std::u32string::const_iterator e,
			       bool &encode_it)
{
	encode_it=false;

	while (b != e)
	{
		if (WHITESPACE(*b))
			break;
		if (do_encode(*b) || *b < ' ' || *b > 127)
			encode_it=true;
		++b;
	}

	return b;
}

std::string
rfc2047_encode_base::do_encode(const std::string &utf8_str,
			       size_t maximum_length,
			       const std::string &language)
{
	std::string qp=do_qp(utf8_str, maximum_length, language);
	std::string b64=do_base64(utf8_str, maximum_length, language);

	return qp.size() < b64.size() ? qp:b64;
}

class LIBCXX_HIDDEN rfc2047_encode_base::qp_traits {

 public:
	rfc2047_encode_base &base;

	qp_traits(rfc2047_encode_base &baseArg) : base(baseArg) {}

	bool encode(char c)
	{
		return c == '_' || c == '?'
			|| (unsigned char)c < ' '
			|| (c <= 0x7F && base.do_encode(c));
	}
};

static void enc_emit(std::ostringstream &o,
		     std::string::iterator b,
		     std::string::iterator e,
		     const char *&sep,
		     const char method,
		     const std::string &language)
{
	std::replace(b, e, ' ', '_');
	o << sep << "=?" UTF8;

	if (language.size())
		o << "*" << language;

	o << "?" << method << '?' << std::string(b, e) << "?=";
	sep=" ";
}

static void qp_emit(std::ostringstream &o,
		    std::string::iterator b,
		    std::string::iterator e,
		    const char *&sep,
		    const std::string &language)
{
	enc_emit(o, b, e, sep, 'Q', language);
}

static void b64_emit(std::ostringstream &o,
		     std::string::const_iterator b,
		     std::string::const_iterator e,
		     const char *&sep,
		     const std::string &language)
{
	std::string encoded;

	std::copy(b, e,
		  base64<>::encoder<std::back_insert_iterator<std::string>
		  >(std::back_insert_iterator<std::string>(encoded), 0)).eof();

	enc_emit(o, encoded.begin(), encoded.end(), sep, 'B', language);
}

std::string rfc2047_encode_base::do_qp(const std::string &utf8_str,
				       size_t maximum_length,
				       const std::string &language)
{
	std::ostringstream o;

	std::string buf;

	const char *sep="";

	auto encoder=
		qp_encoder<std::back_insert_iterator<std::string>,
			   qp_traits>(std::back_insert_iterator<
				      std::string>(buf), 0, false,
				      qp_traits(*this));

	size_t i=0;

	for (char c:utf8_str)
	{
		if (!IS_UTF8TAIL(c))
		{
			if (buf.size() > maximum_length)
			{
				qp_emit(o, buf.begin(), buf.begin()+i, sep,
					language);
				buf=buf.substr(i);
			}
			i=buf.size();
		}
		*encoder++=c;
	}

	if (buf.begin() != buf.end())
		qp_emit(o, buf.begin(), buf.end(), sep, language);
	return o.str();
}

std::string rfc2047_encode_base::do_base64(const std::string &utf8_str,
					   size_t maximum_length,
					   const std::string &language)
{
	std::ostringstream o;
	const char *sep="";
	size_t start=0;
	size_t i=0;

	size_t n;

	for (n=0; n<utf8_str.size(); ++n)
	{
		if (!IS_UTF8TAIL(utf8_str[n]))
		{
			if ( (n-start) > maximum_length / 4 * 3)
			{
				b64_emit(o, utf8_str.begin()+start,
					 utf8_str.begin()+i, sep,
					 language);
				start=i;
			}
			i=n;
		}
	}

	if (start < n)
		b64_emit(o, utf8_str.begin()+start, utf8_str.begin()+n, sep,
			 language);
	return o.str();
}

std::string rfc2047_encode_base::to_utf8(std::u32string::const_iterator b,
					 std::u32string::const_iterator e)
{
	return unicode::iconvert::convert
		(std::u32string(b, e), unicode::utf_8);
}

void rfc2047_encode_base::encode(const std::string &language,
				 const std::u32string &text)
{
	std::ostringstream o;

	std::u32string::const_iterator b=text.begin(), e=text.end();

	size_t overhead=sizeof("=?" UTF8 "?Q?" "?=")
		+ (language.size() ? language.size()+1:0);

	if (overhead > 50)
		overhead=50; // GIGO

	size_t maximum_length=70-overhead;

	while (b != e)
	{
		if (WHITESPACE(*b))
		{
			o << (char)*b;
			++b;
			continue;
		}

		std::u32string::const_iterator p=b;
		bool encode_it;

		b=next_word(b, e, encode_it);

		if (language.size())
			encode_it=true;

		if (encode_it)
		{
			// If the following word will also get encoded,
			// it must be merged together with this word.

			std::u32string::const_iterator q=b;

			while (b != e)
			{
				// Find the start of the next word.
				if (WHITESPACE(*b))
				{
					++b;
					continue;
				}

				b=next_word(b, e, encode_it);

				if (language.size())
					encode_it=true;

				if (!encode_it)
					break;
				q=b;
			}
			b=q;

			o << do_encode(to_utf8(p, b), maximum_length,
				       language);
			continue;
		}

		std::string s=to_utf8(p, b);

		o << (s.size() > maximum_length ?
		      do_encode(s, maximum_length, language):s);
	}
	formatted_string=o.str();
}

// --------------------------------------------------------------------------

std::u32string from_rfc2047(const std::string &string,
				       const std::string &native_charset)
{
	return from_rfc2047_lang(string, native_charset).second;
}

std::pair<std::string, std::u32string>
from_rfc2047_lang(const std::string &string,
		  const std::string &native_charset)
{
	std::vector<std::pair<std::string, std::u32string> > res;

	from_rfc2047(string, native_charset, res);

	std::pair<std::string, std::u32string> ret;

	size_t size=0;

	for (const auto &fragment:res)
	{
		if (fragment.first.size() && !ret.first.size())
			ret.first=fragment.first; // Capture language.

		size += fragment.second.size();
	}

	ret.second.reserve(size);

	for (const auto &fragment:res)
	{
		ret.second.insert(ret.second.end(),
				  fragment.second.begin(),
				  fragment.second.end());
	}

	return ret;
}

static void transcode(const std::string &str,
		      std::u32string &ret,
		      const std::string &charset)
{
	if (!unicode::iconvert::convert(str, charset, ret))
	{
		std::stringstream o;

		o << libmsg(_txt("Invalid character sequence"));

		ret=std::u32string(std::istreambuf_iterator<char>(o),
					      std::istreambuf_iterator<char>());
	}
}

// Does something stats with =? ?

static inline bool is_start_of_encoded_word(std::string::const_iterator b,
					    std::string::const_iterator e)
{
	if (b != e && *b++ == '=')
	{
		if (b != e && *b == '?')
			return true;
	}
	return false;
}

// Something starting with =?

static inline std::string::const_iterator
decode_next_word(std::string::const_iterator b,
		 std::string::const_iterator e,
		 std::string &next_lang,
		 std::u32string &next_str,
		 const std::string &native_charset)
{
	auto start=b;

	b += 2;

	auto q=std::find(b, e, '?');

	std::string charset=std::string(b, q);

	auto c_b=charset.begin(), c_e=charset.end(),
		c_asterisk=std::find(c_b, c_e, '*');

	if (c_asterisk != c_e) // RFC 2231 extension, language indicator
	{
		next_lang=std::string(c_asterisk+1, c_e);
		std::transform(next_lang.begin(),
			       next_lang.end(),
			       next_lang.begin(),
			       std::ptr_fun(chrcasecmp::toupper));
		charset=std::string(c_b, c_asterisk);
	}

	b=q;
	if (b != e && ++b != e)
	{
		char method=*b;

		b=std::find(b, e, '?');
		if (b != e)
		{
			auto encoded_begin= ++b;

			char prev=0;

			typedef std::back_insert_iterator<std::string> ins_iter;

			typedef qp_decoder<ins_iter> qp_t;

			while (b != e)
			{
				if (*b == '=' && prev == '?')
				{
					std::string s;

					switch (method) {
					default:
						break;
					case 'b':
					case 'B':
						base64<>::decode(encoded_begin,
								 b-1,
								 ins_iter(s));
						transcode(s, next_str, charset);
						return ++b;

					case 'q':
					case 'Q':

						auto qp=qp_t(ins_iter(s));

						--b;
						while (encoded_begin != b)
						{
							char c=*encoded_begin;

							if (c == '_')
								c=' ';

							*qp++=c;
							++encoded_begin;
						}

						transcode(s, next_str, charset);
						return b+2;
					}
					break;
				}

				prev= *b;
				++b;
			}
		}
	}

	// We couldn't parse it, so pass through everything we skipped.

	transcode(std::string(start, b), next_str, native_charset);
	return b;
}

void from_rfc2047(const std::string &string,
		  const std::string &native_charset,
		  std::vector<std::pair<std::string, std::u32string> > &res)
{
	bool previous_word_was_decoded=false;

	std::string::const_iterator b=string.begin(), e=string.end();

	while (b != e)
	{
		auto p=std::find_if(b, e, []
				    (char c)
				    {
					    return !WHITESPACE(c);
				    });
		auto s=p;

		bool next_word_is_encoded=is_start_of_encoded_word(p, e);

		if (previous_word_was_decoded && next_word_is_encoded)
			b=p; // Drop whitespace between encoded words.

		std::string next_lang;
		std::u32string next_str;

		if (!next_word_is_encoded)
		{
			p=std::find_if(s, e, []
				       (char c)
				       {
					       return WHITESPACE(c);
				       });

			transcode(std::string(b, p), next_str, native_charset);
			b=p;
		}
		else if (s != b) // Whitespace before 1st encoded word
		{
			transcode(std::string(b, s), next_str, native_charset);
			next_word_is_encoded=false;
			b=s;
		}
		else
		{
			b=decode_next_word(b, e, next_lang, next_str,
					   native_charset);
		}

		previous_word_was_decoded=next_word_is_encoded;

		// Two strings in a row, same language, combine them.

		if (!res.empty() && res.back().first == next_lang)
		{
			auto &last=res.back().second;

			last.insert(last.end(),
				    next_str.begin(),
				    next_str.end());
		}
		else
			res.push_back(std::make_pair(next_lang, next_str));
	}
}

std::string from_rfc2047_as_utf8(const std::string &string,
				 const std::string &native_charset)
{
	auto ret=from_rfc2047(string, native_charset);

	return unicode::iconvert::convert(ret, unicode::utf_8);
}

#if 0
{
	{
#endif
	}
}
