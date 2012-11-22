/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_tokens_H
#define x_mime_tokens_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace mime {
#if 0
	}
}
#endif

//! A nontoken character

inline bool nontoken(int c) { return c == (unsigned char)c; }

//! EOF token
static const int eof= -1;

//! Start of newline sequence
static const int newline_start=0x0100;

//! End of newline sequence
static const int newline_end=0x0101;

//! Start of message body
static const int body_start=0x0102;

//! Start of header name
static const int header_name_start=0x0103;

//! End of header name
static const int header_name_end=0x0104;

//! Start of header contents
static const int header_contents_start=0x0105;

//! End of header contents
static const int header_contents_end=0x0106;

//! Start of multiline header fold
static const int header_fold_start=0x0107;

//! End of multiline header fold
static const int header_fold_end=0x0108;

//! Start of MIME section inside a multipart section
static const int section_start=0x0109;

//! End of MIME section inside a multipart section
static const int section_end=0x010A;

#if 0
{
	{
#endif
	}
}
#endif
