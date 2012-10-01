#ifndef DOXYGEN

#ifndef LIBCXX_TEMPLATE_SPEC
template<>
template<>
bool tokenizer<is_http_token>::is_token(LIBCXX_TEMPLATE_MUTABLE_ITERATOR,
					LIBCXX_TEMPLATE_MUTABLE_ITERATOR);
#else
template<>
template<>
bool tokenizer<is_http_token>::is_token(LIBCXX_TEMPLATE_MUTABLE_ITERATOR b,
					LIBCXX_TEMPLATE_MUTABLE_ITERATOR e)
{
	return is_token(LIBCXX_TEMPLATE_CONST_ITERATOR(b),
			LIBCXX_TEMPLATE_CONST_ITERATOR(e));
}
#endif

LIBCXX_TEMPLATE_DECL
template bool tokenizer<is_http_token>::is_token(LIBCXX_TEMPLATE_CONST_ITERATOR,
						 LIBCXX_TEMPLATE_CONST_ITERATOR);

#define LIBCXX_OUTPUT_ITERATOR std::ostreambuf_iterator<char>
#include <x/tokens_to3.H>
#undef LIBCXX_OUTPUT_ITERATOR

#define LIBCXX_OUTPUT_ITERATOR std::back_insert_iterator<std::string>
#include <x/tokens_to3.H>
#undef LIBCXX_OUTPUT_ITERATOR

#endif

