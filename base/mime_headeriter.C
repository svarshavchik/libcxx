/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mime/headeriter.H"

namespace LIBCXX_NAMESPACE {
	namespace mime {
#if 0
	};
};
#endif

#define SWITCH state=&header_iterbase::

header_iterbase::header_iterbase() : state(&header_iterbase::start_header),
				     in_header(false)
{
}

// Start of a new line in the header section.

// If in_header is set, we have header_contents open from the previous header.
//
// * body_start or eof transitions to in_body.
// * newline_start transitions to in_endofline.
// * If in_header is set, a space() emits header_fold_start, transitions to
//   in_headerfold.
// * Otherwise, if in_header is set, emit header_contents_end, set
//   in_header=true, emit header_name_start, transition to in_headername

void header_iterbase::start_header(int c)
{
	if (c == separator_line_start || c == body_start || c == eof)
	{
		SWITCH in_body;

		if (in_header)
			emit(header_contents_end);

		emit(c);
		return;
	}

	if (c == newline_start)
	{
		SWITCH in_endofline;
		in_endofline(c);
		return;
	}

	if (in_header && space(c))
	{
		SWITCH in_headerfold;
		emit(header_fold_start);
		in_headerfold(c);
		return;
	}

	if (in_header)
		emit(header_contents_end);
	in_header=true;
	SWITCH in_headername;
	emit(header_name_start);
	in_headername(c);
}

// in_body: pass through, without further processing.

void header_iterbase::in_body(int c)
{
	emit(c);
}

// In a middle of a newline sequence.
// If in_header is set, we have header_contents open from the previous header.

// * newline_end transitions to start_header.

void header_iterbase::in_endofline(int c)
{
	if (c == newline_end)
		SWITCH start_header;
	emit(c);
}

// In a middle of whitespace of a header continued on a next line.
// We have emitted header_contents_start and header_fold_start.
//
// * newline_start emits header_fold_end, and transitions to in_endofline.
//
// * anything other than a space emits header_fold_end, and transitions to
//   in_contents.

void header_iterbase::in_headerfold(int c)
{
	if (c == newline_start)
	{
		SWITCH in_endofline;
		emit(header_fold_end);
		in_endofline(c);
		return;
	}

	if (!space(c))
	{
		emit(header_fold_end);
		SWITCH in_contents;
		in_contents(c);
		return;
	}
	emit(c);
}

// Emitting the header name. in_header is set to true, we've emitted
// header_name_start.

// * A newline emits header_name_end, header_content_start, and transitions
//   to in_endofline.
// * A colon emits header_name_end, the colon, and transitions to
//   in_starting_contents.

void header_iterbase::in_headername(int c)
{
	if (c == newline_start)
	{
		SWITCH in_endofline;
		emit(header_name_end);
		emit(header_contents_start);
		in_endofline(c);
		return;
	}

	if (c == ':')
	{
		SWITCH in_starting_contents;
		emit(header_name_end);
		emit(c);
		return;
	}

	emit(c);
}

// After the colon, flushing whitespace until the first space character.
// Transactions to in_starting_contents_newline upon a newline_start.

// Always transitions to in_contents, unless the character is a space.
// If we transition to in_starting_contents_newline, then back here, it's
// possible that we'll see a body_start or eof, if so, call start_header(),
// to process it.

void header_iterbase::in_starting_contents(int c)
{
	if (space(c))
	{
		emit(c);
		return;
	}

	if (c == newline_start)
	{
		SWITCH in_starting_contents_newline;
		emit(c);
		return;
	}

	emit(header_contents_start);

	if (c == separator_line_start || c == body_start || c == eof)
	{
		start_header(c);
		return;
	}

	SWITCH in_contents;
	in_contents(c);
}

// Newline before the first non-space character in a header.
// transaction back to in_starting_contents upon a newline_end.

void header_iterbase::in_starting_contents_newline(int c)
{
	if (c == newline_end)
		SWITCH in_starting_contents;
	emit(c);
}

// Emitting header contents
// in_header is set to true.

// * newline_start transitions to in_endofline.

void header_iterbase::in_contents(int c)
{
	if (c == newline_start)
		SWITCH in_endofline;

	emit(c);
}

#if 0
{
	{
#endif
	}
}
