/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_mime_sectioninfoobj_H
#define x_mime_sectioninfoobj_H

#include <x/namespace.h>
#include <x/weakptrfwd.H>
#include <x/mime/sectioninfofwd.H>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <cstdio>
#include <x/sys/offt.h>
#include <type_traits>

namespace LIBCXX_NAMESPACE {

	//! Classes and templates for parsing MIME-formatted messages.

	namespace mime {
#if 0
	}
}
#endif

//! Where a MIME entity can be found in the original document

class sectioninfoObj : virtual public obj {

public:

	//! If this is not the top level MIME entity, here's its parent

	weakptr<sectioninfoptr> parent;

	//! Children entity of a multipart of a message/rfc822 MIME entity

	//! If this is a multipart entity, here are all the entities that it
	//! contains. A message/rfc822 MIME entity has one child entity.
	std::vector<const_sectioninfo> children;

	//! The byte offset where this entity starts in the output sequence.

	//! After iterating over the entire output sequence, this is the
	//! byte offset of this MIME entity where it started in the output
	//! sequence. The top level MIME entity always starts at offset 0.
	off64_t starting_pos;

	//! How many characters are in the header part of this MIME entity

	off64_t header_char_cnt;

	//! How many characters are in the body part of this MIME entity

	//! header_char_cnt+body_char_cnt is the total size of this MIME entity,
	//! in characters.
	off64_t body_char_cnt;

	//! Number of lines in the header part of this MIME entity
	size_t header_line_cnt;

	//! Number of lines in the body part of this MIME entity

	//! header_line_cnt+body_line_cnt gives the total numbre of lines in
	//! this MIME entity.
	size_t body_line_cnt;

	//! The index of this MIME entity in a multipart MIME entity

	//! If this is not a top level MIME entity, this is the index of this
	//! entity in parent.getptr()->children.
	size_t index;

	//! Whether this MIME entity did not terminate in a newline sequence.
	bool no_trailing_newline;

	//! Compute the size of this MIME entity with a CRLF newline sequence

	//! If this MIME entity was parsed using LF as a newline sequence,
	//! calculate what it would be if the newline sequence was CRLF

	off64_t crlf_size() const
	{
		return header_char_cnt + body_char_cnt
			// A CR for each line
			+ header_line_cnt + body_line_cnt
			// If there was no trailing newline
			- (no_trailing_newline ? 1:0);
	}

	//! Constructor
	sectioninfoObj();

	//! Destructor
	~sectioninfoObj() noexcept;

	//! Attach a new child section
	sectioninfo new_child();

	//! Return this section's "index name"

	//! The top level MIME entity is "1". If it's a multipart entity,
	//! the first part is "1.1", the second part is "1.2", and so on.
	//! If the second part itself is a multipart entity, its parts get
	//! numbered as "1.2.1", "1.2.2", and so on.
	//!
	//! A message/rfc822 entity is treated as if it is a multipart entity
	//! with one part.
	std::string index_name() const;

private:

	//! Helper function for constructing the index name

	//! \internal
	void index_name(std::ostream &o) const LIBCXX_HIDDEN;
};

#if 0
{
	{
#endif
	}
}
#endif
