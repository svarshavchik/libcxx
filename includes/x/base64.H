/*
** Copyright 2012-2019 Double Precision, Inc.
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

//! Generate a \c base64 alphabet.

//! The optional two template parameters specify the 62nd and the 63rd value
//! in the base64 alphabet, defaulting to the standard %base64 alphabet.
//!
//! This is passed as the first template parameter to
//! \ref base64 "INSERT_LIBX_NAMESPACE::base64".

template<char val62='+', char val63='/'>
class base64alphabet {

public:
	//! The base64 alphabet

	static constexpr char alphabet[64] ={
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
		'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
		'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', val62, val63};

private:

	//! Helper function for generating the decoding lookup table.

	static constexpr int s(char c)
	{
		return (c) == alphabet[0] ? 0:
			(c) == alphabet[1] ? 1:
			(c) == alphabet[2] ? 2:
			(c) == alphabet[3] ? 3:
			(c) == alphabet[4] ? 4:
			(c) == alphabet[5] ? 5:
			(c) == alphabet[6] ? 6:
			(c) == alphabet[7] ? 7:
			(c) == alphabet[8] ? 8:
			(c) == alphabet[9] ? 9:
			(c) == alphabet[10] ? 10:
			(c) == alphabet[11] ? 11:
			(c) == alphabet[12] ? 12:
			(c) == alphabet[13] ? 13:
			(c) == alphabet[14] ? 14:
			(c) == alphabet[15] ? 15:
			(c) == alphabet[16] ? 16:
			(c) == alphabet[17] ? 17:
			(c) == alphabet[18] ? 18:
			(c) == alphabet[19] ? 19:
			(c) == alphabet[20] ? 20:
			(c) == alphabet[21] ? 21:
			(c) == alphabet[22] ? 22:
			(c) == alphabet[23] ? 23:
			(c) == alphabet[24] ? 24:
			(c) == alphabet[25] ? 25:
			(c) == alphabet[26] ? 26:
			(c) == alphabet[27] ? 27:
			(c) == alphabet[28] ? 28:
			(c) == alphabet[29] ? 29:
			(c) == alphabet[30] ? 30:
			(c) == alphabet[31] ? 31:
			(c) == alphabet[32] ? 32:
			(c) == alphabet[33] ? 33:
			(c) == alphabet[34] ? 34:
			(c) == alphabet[35] ? 35:
			(c) == alphabet[36] ? 36:
			(c) == alphabet[37] ? 37:
			(c) == alphabet[38] ? 38:
			(c) == alphabet[39] ? 39:
			(c) == alphabet[40] ? 40:
			(c) == alphabet[41] ? 41:
			(c) == alphabet[42] ? 42:
			(c) == alphabet[43] ? 43:
			(c) == alphabet[44] ? 44:
			(c) == alphabet[45] ? 45:
			(c) == alphabet[46] ? 46:
			(c) == alphabet[47] ? 47:
			(c) == alphabet[48] ? 48:
			(c) == alphabet[49] ? 49:
			(c) == alphabet[50] ? 50:
			(c) == alphabet[51] ? 51:
			(c) == alphabet[52] ? 52:
			(c) == alphabet[53] ? 53:
			(c) == alphabet[54] ? 54:
			(c) == alphabet[55] ? 55:
			(c) == alphabet[56] ? 56:
			(c) == alphabet[57] ? 57:
			(c) == alphabet[58] ? 58:
			(c) == alphabet[59] ? 59:
			(c) == alphabet[60] ? 60:
			(c) == alphabet[61] ? 61:
			(c) == alphabet[62] ? 62:
			(c) == alphabet[63] ? 63:127;
	}

public:

	//! The decoding alphabet table.

	//! decode_alphabet[c] = i if c is the ith character in the base64
	//! alphabet. Non-alphabet characters have their index set to 127.

	static constexpr char decode_alphabet[256]={
		s(  0),s(  1),s(  2),s(  3),s(  4),s(  5),s(  6),s(  7),
		s(  8),s(  9),s( 10),s( 11),s( 12),s( 13),s( 14),s( 15),
		s( 16),s( 17),s( 18),s( 19),s( 20),s( 21),s( 22),s( 23),
		s( 24),s( 25),s( 26),s( 27),s( 28),s( 29),s( 30),s( 31),
		s( 32),s( 33),s( 34),s( 35),s( 36),s( 37),s( 38),s( 39),
		s( 40),s( 41),s( 42),s( 43),s( 44),s( 45),s( 46),s( 47),
		s( 48),s( 49),s( 50),s( 51),s( 52),s( 53),s( 54),s( 55),
		s( 56),s( 57),s( 58),s( 59),s( 60),s( 61),s( 62),s( 63),
		s( 64),s( 65),s( 66),s( 67),s( 68),s( 69),s( 70),s( 71),
		s( 72),s( 73),s( 74),s( 75),s( 76),s( 77),s( 78),s( 79),
		s( 80),s( 81),s( 82),s( 83),s( 84),s( 85),s( 86),s( 87),
		s( 88),s( 89),s( 90),s( 91),s( 92),s( 93),s( 94),s( 95),
		s( 96),s( 97),s( 98),s( 99),s(100),s(101),s(102),s(103),
		s(104),s(105),s(106),s(107),s(108),s(109),s(110),s(111),
		s(112),s(113),s(114),s(115),s(116),s(117),s(118),s(119),
		s(120),s(121),s(122),s(123),s(124),s(125),s(126),s(127),
		s(128),s(129),s(130),s(131),s(132),s(133),s(134),s(135),
		s(136),s(137),s(138),s(139),s(140),s(141),s(142),s(143),
		s(144),s(145),s(146),s(147),s(148),s(149),s(150),s(151),
		s(152),s(153),s(154),s(155),s(156),s(157),s(158),s(159),
		s(160),s(161),s(162),s(163),s(164),s(165),s(166),s(167),
		s(168),s(169),s(170),s(171),s(172),s(173),s(174),s(175),
		s(176),s(177),s(178),s(179),s(180),s(181),s(182),s(183),
		s(184),s(185),s(186),s(187),s(188),s(189),s(190),s(191),
		s(192),s(193),s(194),s(195),s(196),s(197),s(198),s(199),
		s(200),s(201),s(202),s(203),s(204),s(205),s(206),s(207),
		s(208),s(209),s(210),s(211),s(212),s(213),s(214),s(215),
		s(216),s(217),s(218),s(219),s(220),s(221),s(222),s(223),
		s(224),s(225),s(226),s(227),s(228),s(229),s(230),s(231),
		s(232),s(233),s(234),s(235),s(236),s(237),s(238),s(239),
		s(240),s(241),s(242),s(243),s(244),s(245),s(246),s(247),
		s(248),s(249),s(250),s(251),s(252),s(253),s(254),s(255)
	};
};

//! Base64 encoding and decoding

//! The first template parameter is the padding character, defaulting to '='.
//! A padding character of '\0', or 0, disables padding completely.
//!
//! The second template parameter specifies a
//! \ref base64alphabet "INSERT_LIBX_NAMESPACE::base64alphabet", which defaults to
//! the standard %base64 alphabet.
//!
//! This template class defines
//! \ref encoder and \ref decoder, output iterators that implement the
//! actual encoding and decoding, as iterators, and encode() and decode(),
//! two convenience functions that construct the encoder or the decoder class,
//! encode/decode some input sequence, and take care of any requested padding.

template<char pad='=', typename alphabet=base64alphabet<>>
class base64 {

public:
	//! Convenient typedef alias.

	typedef alphabet alphabet_t;

	//! An encoding output iterator

	//! The template parameter is an output iterator type. The constructor
	//! takes an instance of it, and constructs an instance that writes
	//! the base64-encoded character stream to it.

	template<typename out_iter_type>
	class encoder : public std::iterator<std::output_iterator_tag,
				     void, void, void, void> {

		//! Output iterator base64-encoded content gets written to
		out_iter_type out;

		//! Previous character
		char curval;

		//! Current base64 encoding phase.
		char phase;

		//! Emit newline after these many characters
		size_t linesize;

		//! Current line size
		size_t linecnt;

		//! Whether to emit crlf instead of lf as the newline
		bool crlf;

	public:
		//! Constructor
		encoder(//! Output iterator base64-encoded content gets written to
			const out_iter_type &outArg,

			//! Emit newline after these many characters (0-disable)
			size_t linesizeArg=76,

			//! Use CRLF instead of LF

			bool crlfArg=false)
			: out(outArg), curval(0), phase(0),
			  linesize(linesizeArg), linecnt(0), crlf(crlfArg) {}

		//! Move constructor
		encoder(//! Output iterator base64-encoded content gets written to
			out_iter_type &&outArg,

			//! Emit newline after these many characters (0-disable)
			size_t linesizeArg=76,

			//! Use CRLF instead of LF

			bool crlfArg=false)
			: out(std::move(outArg)), curval(0), phase(0),
			  linesize(linesizeArg), linecnt(0), crlf(crlfArg) {}

		//! No-op output iterator operator
		encoder &operator++() noexcept { return *this; }

		//! No-op output iterator operator
		encoder &operator++(int) noexcept { return *this; }

		//! No-op output iterator operator
		encoder &operator*() noexcept { return *this; }

		//! Encoding iterator operator
		encoder &operator=(char c)
		{
			emit(c, false);
			return *this;
		}

	private:

		//! Emit the next base64-encoded character(s)

		//! Writes one or more base64-encoded characters in response
		//! to processing the next unencoded character.

		void emit(//! The next unencoded character
			  char c,

			  //! This was invoked from eof()
			  bool eof)
		{
			switch (++phase) {
			case 1:
				//! See if we need to emit a newline, first.

				if (linesize && linecnt >= linesize)
				{
					linecnt=0;
					if (crlf)
						*out++='\r';
					*out++='\n';
				}
				linecnt += 4;
				*out++=alphabet_t::alphabet[ (c >> 2) & 63];
				break;
			case 2:
				*out++=alphabet
					::alphabet[ ((curval << 4)
						     | ((unsigned char)c >> 4) )
						    & 63];
				break;
			case 3:
				*out++=alphabet
					::alphabet[ ((curval << 2)
						     | ((unsigned char)c >> 6))
						    & 63];

				if (eof)
				{
					if (pad)
						*out++=pad;
				}
				else
				{
					*out++ = alphabet_t::alphabet[ c & 63 ];
				}
				phase=0;
				break;
			}
			curval=c;
		}
	public:
		//! Base64 encoding is complete.

		//! Emits any required padding. Returns the new output iterator.
		out_iter_type eof()
		{
			switch (phase) {
			case 1:
				// One character was partially base64-encoded.
				// Encode a fake 0 character, which results
				// in two encoded base64 characters being
				// written total, then two more padding chars.
				emit(0, true);
				if (pad)
				{
					*out++=pad;
					*out++=pad;
				}
				break;
			case 2:
				// Two characters were base64-encoded. Encode
				// a fake 0 character, to make it an even three.
				// This emits two more characters, and the
				// true flag emits the padding character for
				// the second (fourth encoded) character.
				emit(0, true);
				break;
			}

			if (linecnt && linesize)
			{
				//! newline after the last partial line.

				if (crlf)
					*out++='\r';
				*out++='\n';
			}
			return out;
		}
	};

	//! Apply base64 encoding to a sequence

	//! Constructs an encoder, encodes an input sequence, appends any
	//! required padding.
	//!
	//! \return The output iterator after the encoded block is emitted.

	template<typename input_iter, typename output_iter>
	static output_iter encode(//! Beginning iterator of data to be encoded
				  input_iter begin,

				  //! Ending iterator of data to be encoded
				  input_iter end,

				  //! Output iterator for encoded data
				  output_iter output,

				  //! Emit newline after these many characters (0-disable)
				  size_t linesize=76,

				  //! Use CRLF instead of LF

				  bool crlf=false)
	{
		return std::copy(begin, end,
				 encoder<output_iter>(output, linesize, crlf))
			.eof();
	}

	//! A decoding output iterator

	//! The template parameter is an output iterator type. The constructor
	//! takes an instance of it, and constructs an instance that writes
	//! the base64-decoded character stream to it.

	template<typename out_iter_type>
	class decoder : public std::iterator<std::output_iterator_tag,
					     void, void, void, void> {

		//! Output iterator base64-decoded content gets written to
		out_iter_type out;

		//! Previous character
		char curval;

		//! Flag to indicate that an unknown character was encountered.
		bool errflag;

		//! Current base64 encoding phase.
		short phase;

		//! Do not consider the following character to be a pad char.
		char padchar;

		//! Seen a padding character
		bool seen_pad;

	public:
		//! Constructor
		decoder(//! Output iterator base64-encoded content gets written to
			const out_iter_type &outArg)
			: out(outArg), curval(0), errflag(false), phase(0),
			  padchar(pad), seen_pad(false) {}

		//! Move constructor
		decoder(//! Output iterator base64-encoded content gets written to
			out_iter_type &&outArg)
			: out(std::move(outArg)), curval(0), errflag(false),
			  phase(0), padchar(pad), seen_pad(false) {}

		//! No-op output iterator operator
		decoder &operator++() noexcept { return *this; }

		//! No-op output iterator operator
		decoder &operator++(int) noexcept { return *this; }

		//! No-op output iterator operator
		decoder &operator*() noexcept { return *this; }

		//! Encoding iterator operator
		decoder &operator=(char c)
		{
			if (c == '\r' || c == '\n')
				return *this;

			if (padchar && c == padchar)
			{
				// A padding character may appear in place of
				// a third or the fourth character.
				switch (++phase) {
				case 3:
					break;
				case 4:
					phase=0;
					padchar=0;
					// With padchar=0, and seen_pad=true,
					// all further calls become no-ops
					// due to the check below.
				}
				seen_pad=true;
				return *this;
			}

			int n=alphabet_t::decode_alphabet[(unsigned char)c];

			// One the pad character is seen, we can only see
			// a pad character.
			if (n > 63 || seen_pad)
			{
				errflag=true;
				return *this;
			}

			switch (++phase) {
			case 1:
				// First base64-encoded character gets buffered
				break;
			case 2:
				// Emit 1st decoded character from the first
				// and the second base64-encoded character.
				*out++=(curval << 2) | (n >> 4);
				break;
			case 3:
				// Emit 2nd decoded character from the second
				// and the third base64-encoded character.
				*out++=(curval << 4) | (n >> 2);
				break;
			case 4:
				// Emit 3rd decoded character from the third
				// and the fourth base64-encoded character.
				*out++=(curval << 6) | n;
				phase=0;
				break;
			}

			curval=n;
			return *this;
		}

		//! Return the current iterator value

		//! \return
		//! A std::pair, with \c first being the current output iterator
		//! value, and the \c second being true if the sequence was
		//! decoded with no errors.
		std::pair<out_iter_type, bool> eof() const
		{
			return std::make_pair(out, !errflag);
		}
	};

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
						   output_iter output)
	{
		return std::copy(begin, end,
				 decoder<output_iter>(output)).eof();
	}

	//! Calculate the size of the base64-encoded data

	static inline size_t encoded_size(//! Number of bytes to be base64-encoded
					  size_t cnt,

					  //! Emit newline after these many characters (0-disable)
					  size_t linesize=76,

					  //! Use CRLF instead of LF
					  bool crlf=false) noexcept
	{
		const size_t n=pad ? ((cnt+2)/3*4)
			: (cnt/3*4) + (cnt % 3 ? (cnt%3)+1:0);

		// Size of base64-encoded data

		linesize = linesize ? (linesize+3)/4*4:0;
		// emit() always rounds up linesize to an even multiple of 4.

		return n + (linesize ? ( (n+linesize-1)/linesize ) *
			    (crlf ? 2:1) : 0);
	}

	//! Calculate the size of the base64-encoded data

	//! \overload
	//!
	template<typename input_iter>
	static inline size_t encoded_size(//! Beginning iterator of the data to be base64-encoded
					  const input_iter &begin,

					  //! Ending iterator of the data to be base64-encoded
					  const input_iter &end,

					  //! Emit newline after these many characters (0-disable)
					  size_t linesize=76,

					  //! Use CRLF instead of LF

					  bool crlf=false) noexcept
	{
		return encoded_size(std::distance(begin, end), linesize, crlf);
	}

	//! Calculate the size of the base64-encoded data

	//! \return the size of buffer big enough to hold the decoded data.
	//! The actual decoded data may be slightly less if the last encoded
	//! block is partial.

	static inline size_t decoded_size(//! The size of the encoded data
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

template<char val62, char val63>
constexpr char base64alphabet<val62, val63>::alphabet[64];

template<char val62, char val63>
constexpr char base64alphabet<val62, val63>::decode_alphabet[256];

extern template class base64alphabet<>;
extern template class base64<>;

//! base64 encoder that uses no padding

typedef base64<0> base64_nopad;

extern template class base64<0>;

#if 0
{
#endif
}
#endif
