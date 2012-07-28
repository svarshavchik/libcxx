/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_base64_H
#define x_base64_H

#include <x/namespace.h>
#include <cstring>
#include <utility>
#include <iterator>
#include <algorithm>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base64 encoding and decoding algorithms

//! This class implements the base64 algorithm

class base64 {

public:
	//! Apply base64 encoding to a sequence

	//! \return The output iterator after the encoded block is emitted.
	//!

	template<typename input_iter, typename output_iter>
	static output_iter encode(//! Beginning iterator of data to be encoded
				  input_iter begin,

				  //! Ending iterator of data to be encoded
				  input_iter end,

				  //! Output iterator for encoded data
				  output_iter output,

				  //! Base64 encoding alphabet
				  const char *alphabet,

				  //! Padding character for any partially encoded block
				  char padchar='=') noexcept;

	//! Decode a base64-encoded block

	//! \return A pair containing the output iterator after the decoded
	//! block is emitted, and a boolean flag. The boolean flag is \c true
	//! if the encoded block was succesfully decoded, or \c false if
	//! there was a decoding error.

	template<typename input_iter, typename output_iter>
	static std::pair<output_iter, bool> decode(//! Beginning iterator of base64-encoded data
						   input_iter begin,

						   //! Ending iterator of base64-encoded data
						   input_iter end,

						   //! Output iterator of decoded data
						   output_iter output,

						   //! Base64 encoding alphabet

						   const char *alphabet,

						   //! Padding character used when encoding the data
						   char padchar='=') noexcept;

	//! Calculate the size of the base64-encoded data

	static inline size_t encoded_size(//! Number of bytes to be base64-encoded
					  size_t cnt) noexcept
	{
		return ((cnt+2)/3*4);
	}

	//! Calculate the size of the base64-encoded data

	//! \overload
	//!
	template<typename input_iter>
	static inline size_t encoded_size(//! Beginning iterator of the data to be base64-encoded
					  const input_iter &begin,

					  //! Ending iterator of the data to be base64-encoded
					  const input_iter &end) noexcept
	{
		return encoded_size(std::distance(begin, end));
	}

	//! Calculate the size of the base64-encoded data

	//! \return the size of buffer big enough to hold the decoded data.
	//! The actual decoded data may be slightly less if the last encoded
	//! block is partial.

	static size_t decoded_size(//! The size of the encoded data
				   size_t cnt) noexcept
	{
		return (cnt+3)/4*3;
	}

	//! Calculate the size of the base64-encoded data

	//! \overload
	//!

	template<typename input_iter>
	static size_t decoded_size(//! Beginning iterator of the data to be base64-encoded
				   const input_iter &begin,

				   //! Ending iterator of the data to be base64-encoded
				   const input_iter &end) noexcept
	{
		return decoded_size(std::distance(begin, end));
	}
};

template<typename input_iter, typename output_iter>
output_iter base64::encode(input_iter begin,
			   input_iter end,
			   output_iter output,
			   const char *alphabet,
			   char padchar) noexcept
{
	while (begin != end)
	{
		unsigned char c=*begin++;

		bool eof= begin == end;

		unsigned char d = !eof ? *begin++ : 0;

		bool eof2= begin == end;

		unsigned char e = !eof2 ? *begin++ : 0;

		*output++ = alphabet[ (c >> 2) & 63];
		*output++ = alphabet[ ((c << 4) | (d >> 4) ) & 63];
		*output++ = eof ? padchar:alphabet[ ((d << 2) | (e >> 6)) & 63];
		*output++ = eof2 ? padchar:alphabet[ e & 63 ];
	}

	return output;
}

template<typename input_iter, typename output_iter>
std::pair<output_iter, bool> base64::decode(input_iter begin,
					    input_iter end,
					    output_iter output,
					    const char *alphabet,
					    char padchar) noexcept
{
	while (begin != end)
	{
		const char *cp=strchr(alphabet, *begin++);

		if (cp == NULL || begin == end)
		{
		error:
			return std::make_pair(output, false);
		}

		unsigned char c=cp-alphabet;

		cp=strchr(alphabet, *begin++);

		if (cp == NULL)
			goto error;

		unsigned char d=cp-alphabet;

		*output++=(c << 2) | (d >> 4);

		if (begin == end)
			break;

		char ch=*begin++;

		if (ch == padchar)
		{
			if (begin == end || *begin++ != padchar)
				goto error;
			break;
		}

		if ((cp=strchr(alphabet, ch)) == NULL)
			goto error;

		unsigned char e=cp-alphabet;

		*output++=(d << 4) | (e >> 2);

		if (begin == end || (ch=*begin++) == padchar)
			break;

		if ((cp=strchr(alphabet, ch)) == NULL)
			goto error;

		unsigned char f=cp-alphabet;

		*output++=(e << 6) | f;
	}

	return std::make_pair(output, begin == end);
}

#if 0
{
#endif
}
#endif
