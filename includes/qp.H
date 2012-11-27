/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_qp_H
#define x_qp_H

#include <x/namespace.h>
#include <cstring>
#include <utility>
#include <iterator>
#include <algorithm>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Default quoted-printable traits

struct default_qp_traits {

public:

	//! Do not encode any additional characters

	static bool encode(char c)
	{
		return false;
	}
};

//! Quoted-printable output iterator

//! The template parameter is an output iterator, and the constructor
//! receives an instance of the output iteratorm, the underlying output
//! iterator. Characters written to this
//! output iterator get qp-encoded and written to the underlying iterator.
//!
//! eof() must be called to signify end of output. This iterator does minor
//! buffering, and this writes out anything that's buffered, to the
//! underlying iterator. eof() returns the new value of the underlying output
//! iterator.
//!
//! The second optional template parameter specifies a traits class,
//! a class with an %encode() method that takes a character as a parameter and
//! returns a boolean indicating whether this character should be encoded. This
//! is used in contexts where additional characters, beyond the control and the
//! non-ASCII characters are subject to quoted-printable encoding.
//! %encode() needs to return \c true only for the additional characters that
//! are not otherwise encoded. Control characters and non-ASCII characters
//! get always encoded. An instance of the traits class gets passed to the
//! constructor, as its last optional argument.

template<typename output_iter_type, typename traits=default_qp_traits>
class qp_encoder : public std::iterator<std::output_iterator_tag, char,
					void, void, void> {

	//! The current iterator

	output_iter_type iter;

	//! Maximum line with
	size_t maxwidth;

	//! Use CRLF newline sequence
	bool use_crlf;

	//! Current line width
	size_t width;

	//! Seen a CR, when newline is CRLF
	bool seen_cr;

	//! Previous character was a SP or a TAB
	char sptab;

	//! The encoder traits instance
	traits Traits;
public:

	//! Constructor
	qp_encoder(//! The underlying iterator
		   const output_iter_type &iterArg,

		   //! Max line width, 0 do not insert line breaks
		   size_t maxwidthArg=76,

		   //! Whether to use CRLF for a newline sequence
		   bool use_crlfArg=false,

		   //! The default traits instance
		   traits TraitsArg=traits())
		: iter(iterArg),
		  maxwidth(maxwidthArg),
		  use_crlf(use_crlfArg),
		  width(0),
		  seen_cr(false), sptab(0), Traits(TraitsArg)
	{
	}

	//! Move constructor
	qp_encoder(//! The underlying iterator
		   output_iter_type &&iterArg,

		   //! Max line width, 0 do not insert line breaks
		   size_t maxwidthArg=76,

		   //! Whether to use CRLF for a newline sequence
		   bool use_crlfArg=false,

		   //! The default traits instance
		   traits TraitsArg=traits())
		: iter(std::move(iterArg)),
		  maxwidth(maxwidthArg),
		  use_crlf(use_crlfArg),
		  width(0),
		  seen_cr(false), sptab(0), Traits(TraitsArg)
	{
	}

	//! Iterator operator
	qp_encoder<output_iter_type, traits> &operator++() { return *this; }

	//! Iterator operator
	qp_encoder<output_iter_type, traits> &operator++(int) { return *this; }

	//! Iterator operator
	qp_encoder<output_iter_type, traits> &operator*() { return *this; }

	//! Encode the next character
	void operator=(unsigned char c)
	{
		encode(c);
	}

	//! EOF

	//! Returns the final value of the output iterator

	output_iter_type eof()
	{
		encode(-1);
		return iter;
	}

private:

	//! Encode the next character.

	void encode(int c)
	{
		if (seen_cr)
		{
			seen_cr=false;
			// Using CRLF, and the previous character was a CR

			if (c == '\n')
			{
				emit('\r');
				emit('\n');
				return;
			}

			// Emit the CR that did not follow with LF as q-p.

			emit_qp('\r');
		}

		if (maxwidth == 0 && (c == '\r' || c == '\n'))
		{
			emit_qp(c);
			return;
		}

		if (c == '\r' && use_crlf)
		{
			// Using CRLF, and this is a CR.

			seen_cr=true;
			return;
		}

		if (c == '\n' && !use_crlf)
		{
			// Not using CRLF, this is a newline.
			emit('\n');
			return;
		}

		if (c >= 0 && c != '\t' &&
		    (c < ' ' || c > 0x7F || c == '=' || Traits.encode(c)))
		{
			emit_qp(c);
			return;
		}
		emit(c);
	}

	//! Encode a character in hexadecimal.

	void emit_qp(unsigned char c)
	{
		emit('=');
		emit("0123456789ABCDEF"[c >> 4]);
		emit("0123456789ABCDEF"[c & 15]);
	}

	//! Emit qp-encoded character stream

	//! At this point, we have the following contract:
	//!
	//! - a CR or a LF must be a part of a newline sequence
	//! - a "=" will always be followed by two more hexadecimal characters.

	void emit(int c)
	{
		if (c == -1 || c == '\r' || c == '\n')
		{
			// Handle newlines.

			width=0;

			if (sptab) // Encode buffered SP/TAB before newline.
			{
				char c=sptab;

				sptab=0;
				emit_qp(c);
			}
			if (c != -1)
				*iter++=(char)c;
			return;
		}

		if (sptab)
		{
			// newline did not follow the preceding SP or TAB, so
			// emit it.
			emit_c(sptab);
			sptab=0;
		}

		if (c == ' ' || c == '\t')
		{
			// Buffer SP/TAB to see what's next.
			sptab=c;
			return;
		}
		emit_c(c);
	}

	//! Soft-break the emitted QP-stream.

	void emit_c(char c)
	{
		if (maxwidth)
		{
			if (width + (c == '=' ? 3:1) > maxwidth)
			{
				*iter++='=';
				if (use_crlf)
					emit('\r');
				emit('\n');
			}

			++width;
		}

		*iter++=c;
	}
};

//! Quoted-printable decoder

//! The template parameter is an output iterator, and the constructor
//! receives an instance of the output iteratorm, the underlying output
//! iterator. Characters written to this
//! output iterator get qp-decoded and written to the underlying iterator.
//!
//! eof() returns the new value of the underlying output iterator.
//! qp_decoder does not do any internal buffer, so eof() has no effect, other
//! than returning the new value of the underlying output iterator.

template<typename output_iter_type>
class qp_decoder : public std::iterator<std::output_iterator_tag, void, void,
					void, void> {

	//! The current iterator

	output_iter_type iter;

	//! Doing quoted-printable encoding

	size_t hex_chars;

	//! Quoted-printable character.
	unsigned char value;

public:

	//! Constructor
	qp_decoder(//! The underlying iterator
		   const output_iter_type &iterArg)
		: iter(iterArg), hex_chars(0)
	{
	}

	//! Move constructor
	qp_decoder(//! The underlying iterator
		   output_iter_type &&iterArg)
		: iter(std::move(iterArg)), hex_chars(0), value(0)
	{
	}

	//! Iterator operator
	qp_decoder<output_iter_type> &operator++() { return *this; }

	//! Iterator operator
	qp_decoder<output_iter_type> &operator++(int) { return *this; }

	//! Iterator operator
	qp_decoder<output_iter_type> &operator*() { return *this; }

	//! Decode the next character
	void operator=(unsigned char c)
	{
		if (hex_chars) // Decoding an encoded character.
		{
			static const char hex[]="0123456789ABCDEFabcdef";

			int nybble=std::find(hex, hex+22, c)-hex;

			if (nybble == 22)
			{
				if (c != '\r')
					hex_chars=0; // Assume soft break
				return;
			}
			if (nybble > 15)
				nybble -= 6;

			value=(value << 4)|nybble;
			if (--hex_chars)
				return;
			c=value;
		}
		else
		{
			if (c == '=')
			{
				value=0;
				hex_chars=2;
				return;
			}
		}
		*iter++=(char)c;
	}
};

#if 0
{
#endif
}
#endif
