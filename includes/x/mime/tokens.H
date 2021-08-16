/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_mime_tokens_H
#define x_mime_tokens_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::mime {
#if 0
}
#endif

//! A nontoken character

inline bool nontoken(int c) { return c == (unsigned char)c; }

//! EOF token
static constexpr int eof= -1;

//! Start of newline sequence
static constexpr int newline_start=0x0100;

//! End of newline sequence
static constexpr int newline_end=0x0101;

//! Start of message body
static constexpr int body_start=0x0102;

//! Start of header name
static constexpr int header_name_start=0x0103;

//! End of header name
static constexpr int header_name_end=0x0104;

//! Start of header contents
static constexpr int header_contents_start=0x0105;

//! End of header contents
static constexpr int header_contents_end=0x0106;

//! Start of multiline header fold
static constexpr int header_fold_start=0x0107;

//! End of multiline header fold
static constexpr int header_fold_end=0x0108;

//! Start of MIME section inside a multipart section
static constexpr int section_start=0x0109;

//! End of MIME section inside a multipart section
static constexpr int section_end=0x010A;

//! Start of a blank line that separates header from body
static constexpr int separator_line_start=0x010B;

#if 0
{
#endif
}
#endif
