#ifndef DOXYGEN

LIBCXX_TEMPLATE_DECL
template const bool tokenizer<is_http_token>::token_map[256];

LIBCXX_TEMPLATE_DECL
template class tokenizer<is_http_token>::is_not_token;

LIBCXX_TEMPLATE_DECL
template bool tokenizer<is_http_token>::is_token(const std::string &);

#define LIBCXX_TEMPLATE_CONST_ITERATOR std::string::const_iterator
#define LIBCXX_TEMPLATE_MUTABLE_ITERATOR std::string::iterator
#include <x/tokens_to2.H>
#undef LIBCXX_TEMPLATE_CONST_ITERATOR
#undef LIBCXX_TEMPLATE_MUTABLE_ITERATOR

#endif

