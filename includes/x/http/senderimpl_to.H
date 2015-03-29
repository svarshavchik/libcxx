/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN
LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::content_length::send(requestimpl &,
					LIBCXX_HTTP_SENDER_OUTPUT_ITER,
					std::istreambuf_iterator<char>,
					std::istreambuf_iterator<char>,
					off_t,
					wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::content_length::send(responseimpl &,
					LIBCXX_HTTP_SENDER_OUTPUT_ITER,
					std::istreambuf_iterator<char>,
					std::istreambuf_iterator<char>,
					off_t,
					wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::content_length::send(requestimpl &,
					LIBCXX_HTTP_SENDER_OUTPUT_ITER,
					std::string::iterator,
					std::string::iterator,
					std::iterator_traits
					<std::string::iterator>
					::difference_type,
					wait_continue &, bool);


LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::content_length::send(responseimpl &,
					LIBCXX_HTTP_SENDER_OUTPUT_ITER,
					std::string::iterator,
					std::string::iterator,
					std::iterator_traits
					<std::string::iterator>
					::difference_type,
					wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::content_length::send(requestimpl &,
					LIBCXX_HTTP_SENDER_OUTPUT_ITER,
					std::string::const_iterator,
					std::string::const_iterator,
					std::iterator_traits
					<std::string::const_iterator>
					::difference_type,
					wait_continue &, bool);


LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::content_length::send(responseimpl &,
					LIBCXX_HTTP_SENDER_OUTPUT_ITER,
					std::string::const_iterator,
					std::string::const_iterator,
					std::iterator_traits
					<std::string::const_iterator>
					::difference_type,
					wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http10_request_encoding<std::iterator_traits<std::istreambuf_iterator<char> >::iterator_category >
::send(requestimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::istreambuf_iterator<char>,
       std::istreambuf_iterator<char>,
       wait_continue &);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http10_request_encoding<std::forward_iterator_tag>
::send(requestimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::string::iterator,
       std::string::iterator,
       wait_continue &);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http10_request_encoding<std::forward_iterator_tag>
::send(requestimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::string::const_iterator,
       std::string::const_iterator,
       wait_continue &);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http10_response_encoding<std::iterator_traits<std::istreambuf_iterator<char> >::iterator_category >
::send(responseimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::istreambuf_iterator<char>,
       std::istreambuf_iterator<char>,
       wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http10_response_encoding<std::iterator_traits<std::string::iterator>
				   ::iterator_category >
::send(responseimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::string::iterator,
       std::string::iterator,
       wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http10_response_encoding<std::iterator_traits<std::string::const_iterator>
					   ::iterator_category>
::send(responseimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::string::const_iterator,
       std::string::const_iterator,
       wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http11_encoding<std::iterator_traits<std::istreambuf_iterator<char> >::iterator_category >
::send(requestimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::istreambuf_iterator<char>,
       std::istreambuf_iterator<char>,
       wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http11_encoding<std::iterator_traits<std::istreambuf_iterator<char> >::iterator_category >
::send(responseimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::istreambuf_iterator<char>,
       std::istreambuf_iterator<char>,
       wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http11_encoding<std::iterator_traits<std::string::iterator>
				   ::iterator_category>
::send(requestimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::string::iterator,
       std::string::iterator,
       wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http11_encoding<std::iterator_traits<std::string::iterator>
				   ::iterator_category >
::send(responseimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::string::iterator,
       std::string::iterator,
       wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http11_encoding<std::iterator_traits<std::string::const_iterator>
				   ::iterator_category>
::send(requestimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::string::const_iterator,
       std::string::const_iterator,
       wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template
LIBCXX_HTTP_SENDER_OUTPUT_ITER
senderimpl_encode::http11_encoding<std::iterator_traits<std::string::const_iterator>
				   ::iterator_category>
::send(responseimpl &,
       LIBCXX_HTTP_SENDER_OUTPUT_ITER,
       std::string::const_iterator,
       std::string::const_iterator,
       wait_continue &, bool);

LIBCXX_TEMPLATE_DECL template class senderimpl<LIBCXX_HTTP_SENDER_OUTPUT_ITER >;

LIBCXX_TEMPLATE_DECL template
void senderimpl<LIBCXX_HTTP_SENDER_OUTPUT_ITER >
::send(requestimpl &,
       std::istreambuf_iterator<char>,
       std::istreambuf_iterator<char>,
       wait_continue &);

LIBCXX_TEMPLATE_DECL template
void senderimpl<LIBCXX_HTTP_SENDER_OUTPUT_ITER >
::send(responseimpl &,
       const requestimpl &,
       std::istreambuf_iterator<char>,
       std::istreambuf_iterator<char>,
       wait_continue &);

LIBCXX_TEMPLATE_DECL template
void senderimpl<LIBCXX_HTTP_SENDER_OUTPUT_ITER >
::send(requestimpl &,
       std::string::iterator,
       std::string::iterator,
       wait_continue &);

LIBCXX_TEMPLATE_DECL template
void senderimpl<LIBCXX_HTTP_SENDER_OUTPUT_ITER >
::send(responseimpl &,
       const requestimpl &,
       std::string::iterator,
       std::string::iterator,
       wait_continue &);

LIBCXX_TEMPLATE_DECL template
void senderimpl<LIBCXX_HTTP_SENDER_OUTPUT_ITER >
::send(requestimpl &,
       std::string::const_iterator,
       std::string::const_iterator,
       wait_continue &);

LIBCXX_TEMPLATE_DECL template
void senderimpl<LIBCXX_HTTP_SENDER_OUTPUT_ITER >
::send(responseimpl &,
       const requestimpl &,
       std::string::const_iterator,
       std::string::const_iterator,
       wait_continue &);

LIBCXX_TEMPLATE_DECL template
void senderimpl<LIBCXX_HTTP_SENDER_OUTPUT_ITER >
::sendimpl(requestimpl &);

LIBCXX_TEMPLATE_DECL template
void senderimpl<LIBCXX_HTTP_SENDER_OUTPUT_ITER >
::sendimpl(responseimpl &);

#endif
