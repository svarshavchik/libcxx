#ifndef DOXYGEN

LIBCXX_TEMPLATE_DECL
template LIBCXX_OUTPUT_ITERATOR tokenizer<is_http_token>
::emit_token_or_quoted_word(LIBCXX_OUTPUT_ITERATOR, const std::string &);

#ifndef LIBCXX_TEMPLATE_SPEC
template<>
template<>
LIBCXX_OUTPUT_ITERATOR
tokenizer<is_http_token>
::emit_token_or_quoted_word(LIBCXX_OUTPUT_ITERATOR,
			    LIBCXX_TEMPLATE_MUTABLE_ITERATOR,
			    LIBCXX_TEMPLATE_MUTABLE_ITERATOR);
#else
template<>
template<>
LIBCXX_OUTPUT_ITERATOR
tokenizer<is_http_token>
::emit_token_or_quoted_word(LIBCXX_OUTPUT_ITERATOR out,
			    LIBCXX_TEMPLATE_MUTABLE_ITERATOR b,
			    LIBCXX_TEMPLATE_MUTABLE_ITERATOR e)
{
	return emit_token_or_quoted_word(out,
					 LIBCXX_TEMPLATE_CONST_ITERATOR(b),
					 LIBCXX_TEMPLATE_CONST_ITERATOR(e));
}
#endif

LIBCXX_TEMPLATE_DECL
template LIBCXX_OUTPUT_ITERATOR
tokenizer<is_http_token>
::emit_token_or_quoted_word(LIBCXX_OUTPUT_ITERATOR,
			    LIBCXX_TEMPLATE_CONST_ITERATOR,
			    LIBCXX_TEMPLATE_CONST_ITERATOR);

#endif
