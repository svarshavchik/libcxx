/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tokens_H
#define x_tokens_H

#include <string>
#include <algorithm>
#include <x/headersbase.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A tokenizer class

//! This class generates a template for generic tokenization functions.
//! The template parameter is a class that defines a constexpr is_token()
//! function that takes a char and returns true if the given character belongs
//! to the token class.
//!
//! From that, an internal token_map[] lookup array gets calculated,
//! and several public functions.

template<typename token_class> class tokenizer {

	//! Tokenizer lookup table

	//! Use the token class's is_token() function to build a lookup table
	//! that contains a true value if the given character is in the token
	//! class.

	static constexpr bool token_map[256]={
		token_class::is_token(  0), token_class::is_token(  1), 
		token_class::is_token(  2), token_class::is_token(  3), 
		token_class::is_token(  4), token_class::is_token(  5), 
		token_class::is_token(  6), token_class::is_token(  7),

		token_class::is_token(  8), token_class::is_token(  9), 
		token_class::is_token( 10), token_class::is_token( 11), 
		token_class::is_token( 12), token_class::is_token( 13), 
		token_class::is_token( 14), token_class::is_token( 15),

		token_class::is_token( 16), token_class::is_token( 17), 
		token_class::is_token( 18), token_class::is_token( 19), 
		token_class::is_token( 20), token_class::is_token( 21), 
		token_class::is_token( 22), token_class::is_token( 23),

		token_class::is_token( 24), token_class::is_token( 25), 
		token_class::is_token( 26), token_class::is_token( 27), 
		token_class::is_token( 28), token_class::is_token( 29), 
		token_class::is_token( 30), token_class::is_token( 31),

		token_class::is_token( 32), token_class::is_token( 33), 
		token_class::is_token( 34), token_class::is_token( 35), 
		token_class::is_token( 36), token_class::is_token( 37), 
		token_class::is_token( 38), token_class::is_token( 39),

		token_class::is_token( 40), token_class::is_token( 41), 
		token_class::is_token( 42), token_class::is_token( 43), 
		token_class::is_token( 44), token_class::is_token( 45), 
		token_class::is_token( 46), token_class::is_token( 47),

		token_class::is_token( 48), token_class::is_token( 49), 
		token_class::is_token( 50), token_class::is_token( 51), 
		token_class::is_token( 52), token_class::is_token( 53), 
		token_class::is_token( 54), token_class::is_token( 55),

		token_class::is_token( 56), token_class::is_token( 57), 
		token_class::is_token( 58), token_class::is_token( 59), 
		token_class::is_token( 60), token_class::is_token( 61), 
		token_class::is_token( 62), token_class::is_token( 63),

		token_class::is_token( 64), token_class::is_token( 65), 
		token_class::is_token( 66), token_class::is_token( 67), 
		token_class::is_token( 68), token_class::is_token( 69), 
		token_class::is_token( 70), token_class::is_token( 71),

		token_class::is_token( 72), token_class::is_token( 73), 
		token_class::is_token( 74), token_class::is_token( 75), 
		token_class::is_token( 76), token_class::is_token( 77), 
		token_class::is_token( 78), token_class::is_token( 79),

		token_class::is_token( 80), token_class::is_token( 81), 
		token_class::is_token( 82), token_class::is_token( 83), 
		token_class::is_token( 84), token_class::is_token( 85), 
		token_class::is_token( 86), token_class::is_token( 87),

		token_class::is_token( 88), token_class::is_token( 89), 
		token_class::is_token( 90), token_class::is_token( 91), 
		token_class::is_token( 92), token_class::is_token( 93), 
		token_class::is_token( 94), token_class::is_token( 95),

		token_class::is_token( 96), token_class::is_token( 97), 
		token_class::is_token( 98), token_class::is_token( 99), 
		token_class::is_token(100), token_class::is_token(101), 
		token_class::is_token(102), token_class::is_token(103),

		token_class::is_token(104), token_class::is_token(105), 
		token_class::is_token(106), token_class::is_token(107), 
		token_class::is_token(108), token_class::is_token(109), 
		token_class::is_token(110), token_class::is_token(111),

		token_class::is_token(112), token_class::is_token(113), 
		token_class::is_token(114), token_class::is_token(115), 
		token_class::is_token(116), token_class::is_token(117), 
		token_class::is_token(118), token_class::is_token(119),

		token_class::is_token(120), token_class::is_token(121), 
		token_class::is_token(122), token_class::is_token(123), 
		token_class::is_token(124), token_class::is_token(125), 
		token_class::is_token(126), token_class::is_token(127),

		token_class::is_token(128), token_class::is_token(129), 
		token_class::is_token(130), token_class::is_token(131), 
		token_class::is_token(132), token_class::is_token(133), 
		token_class::is_token(134), token_class::is_token(135),

		token_class::is_token(136), token_class::is_token(137), 
		token_class::is_token(138), token_class::is_token(139), 
		token_class::is_token(140), token_class::is_token(141), 
		token_class::is_token(142), token_class::is_token(143),

		token_class::is_token(144), token_class::is_token(145), 
		token_class::is_token(146), token_class::is_token(147), 
		token_class::is_token(148), token_class::is_token(149), 
		token_class::is_token(150), token_class::is_token(151),

		token_class::is_token(152), token_class::is_token(153), 
		token_class::is_token(154), token_class::is_token(155), 
		token_class::is_token(156), token_class::is_token(157), 
		token_class::is_token(158), token_class::is_token(159),

		token_class::is_token(160), token_class::is_token(161), 
		token_class::is_token(162), token_class::is_token(163), 
		token_class::is_token(164), token_class::is_token(165), 
		token_class::is_token(166), token_class::is_token(167),

		token_class::is_token(168), token_class::is_token(169), 
		token_class::is_token(170), token_class::is_token(171), 
		token_class::is_token(172), token_class::is_token(173), 
		token_class::is_token(174), token_class::is_token(175),

		token_class::is_token(176), token_class::is_token(177), 
		token_class::is_token(178), token_class::is_token(179), 
		token_class::is_token(180), token_class::is_token(181), 
		token_class::is_token(182), token_class::is_token(183),

		token_class::is_token(184), token_class::is_token(185), 
		token_class::is_token(186), token_class::is_token(187), 
		token_class::is_token(188), token_class::is_token(189), 
		token_class::is_token(190), token_class::is_token(191),

		token_class::is_token(192), token_class::is_token(193), 
		token_class::is_token(194), token_class::is_token(195), 
		token_class::is_token(196), token_class::is_token(197), 
		token_class::is_token(198), token_class::is_token(199),

		token_class::is_token(200), token_class::is_token(201), 
		token_class::is_token(202), token_class::is_token(203), 
		token_class::is_token(204), token_class::is_token(205), 
		token_class::is_token(206), token_class::is_token(207),

		token_class::is_token(208), token_class::is_token(209), 
		token_class::is_token(210), token_class::is_token(211), 
		token_class::is_token(212), token_class::is_token(213), 
		token_class::is_token(214), token_class::is_token(215),

		token_class::is_token(216), token_class::is_token(217), 
		token_class::is_token(218), token_class::is_token(219), 
		token_class::is_token(220), token_class::is_token(221), 
		token_class::is_token(222), token_class::is_token(223),

		token_class::is_token(224), token_class::is_token(225), 
		token_class::is_token(226), token_class::is_token(227), 
		token_class::is_token(228), token_class::is_token(229), 
		token_class::is_token(230), token_class::is_token(231),

		token_class::is_token(232), token_class::is_token(233), 
		token_class::is_token(234), token_class::is_token(235), 
		token_class::is_token(236), token_class::is_token(237), 
		token_class::is_token(238), token_class::is_token(239),

		token_class::is_token(240), token_class::is_token(241), 
		token_class::is_token(242), token_class::is_token(243), 
		token_class::is_token(244), token_class::is_token(245), 
		token_class::is_token(246), token_class::is_token(247),

		token_class::is_token(248), token_class::is_token(249), 
		token_class::is_token(250), token_class::is_token(251), 
		token_class::is_token(252), token_class::is_token(253), 
		token_class::is_token(254), token_class::is_token(255)
	};

	//! Functor for testing that a char is not a token.

	class is_not_token {

	public:

		//! Return \c true if character is not an atom.
		inline bool operator()(const char c) const
		{
			return !token_map[(int)(unsigned char)c];
		} 
	};
public:

	//! Return true if the given sequence of chars can be an RFC 2822 atom

	template<typename iter_type>
	static bool is_token(//! Beginning iterator
			    iter_type b,

			    //! Ending iterator
			    iter_type e)
	{
		return std::find_if(b, e, is_not_token()) == e;
	}

	//! Return true if the given container contains an atom.

	template<typename container_type>
	static bool is_token(//! Container
			    const container_type &c)
	{
		return is_token(std::begin(c), std::end(c));
	}

	//! Emit either an atom or a quoted word, into an output iterator

	template<typename output_iter, typename input_iter>
	static output_iter emit_token_or_quoted_word(//! Output iterator
						     output_iter out,

						     //! Beginning iterator
						     input_iter b,

						     //! Ending iterator
						     input_iter e)
	{
		if (is_token(b, e))
			return std::copy(b, e, out);

                return headersbase::emit_quoted_string(out, b, e);
	}

	//! Emit either an atom or a quoted word, into an output iterator

	template<typename output_iter, typename container_type>
	static output_iter emit_token_or_quoted_word(//! Output iterator
						    output_iter out,

						    //! Container
						    const container_type
						    &container)
	{
		return emit_token_or_quoted_word(out,
						 std::begin(container),
						 std::end(container));
	}

	//! Return a string with a quoted word.

	template<typename input_iter>
	static std::basic_string<typename
				 std::iterator_traits<input_iter>::value_type>
	token_or_quoted_word(//! Beginning iterator
			     input_iter b,

			     //! Ending iterator
			     input_iter e)
	{
		typedef typename std::iterator_traits<input_iter>::value_type
			char_type;
		typedef std::basic_string<char_type> string_type;

		string_type s;

		emit_token_or_quoted_word(std::back_insert_iterator<string_type>
					  (s), b, e);
		return s;
	}

	//! Return a string with a quoted word.

	template<typename container_type>
	static auto token_or_quoted_word(//! Container
					 const container_type &container)
#ifndef DOXYGEN
	// temporary confusion

		-> decltype(token_or_quoted_word(std::begin(container),
						 std::end(container)))
#endif
	{
		return token_or_quoted_word(std::begin(container),
					    std::end(container));
	}
};

template<typename token_class> constexpr bool tokenizer<token_class>::token_map[256];

//! Instantiate a tokenizers for RFC 2616 tokens

class is_http_token {

public:

	//! Return true if the given character can be in a token (see section 2.2)

	static constexpr bool is_token(char c)
	{
		return ((unsigned char)c) > ' ' &&
			c != '(' && c != ')' && c != '<' && c != '>' &&
			c != '@' && c != ',' && c != ';' && c != ':' &&
			c != '\\' && c != '"' && c != '/' && c != '[' &&
			c != ']' && c != '?' && c != '='
			&& c != '{' && c != '}';
	}
};

#define LIBCXX_TEMPLATE_DECL extern
#include <x/tokens_to.H>
#undef LIBCXX_TEMPLATE_DECL

#if 0
{
#endif
}
#endif
