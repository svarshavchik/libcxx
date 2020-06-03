/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/yaml/writer.H"
#include "x/yaml/newnode.H"
#include "x/yaml/newdocumentnode.H"
#include "x/yaml/newscalarnode.H"
#include "x/yaml/newaliasnode.H"
#include "x/exception.H"
#include "x/messages.H"
#include <vector>
#include "gettext_in.h"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::yaml::writerObj);

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

extern "C" {

	static int write_callback(void *data, unsigned char *buffer,
				   size_t size)
	{
		return reinterpret_cast<writer::output_iterator_base *>(data)
			->write(buffer, size);
	}
};

#define EMIT(event,name)						\
	do {								\
	if (!yaml_emitter_emit(&writer, &event))			\
		throw EXCEPTION(gettextmsg				\
				(_("yaml_emitter_emit(%1%) failed: %2%"), \
				 #name,					\
				 (writer.error ? writer.problem:	\
				  "No further information is available"))); \
	} while (0)

writerObj::writerObj(writer::output_iterator_base &output_iter,
		       const writer::options &options) : initialized(false)
{
	if (!yaml_emitter_initialize(&writer))
		throw EXCEPTION(_("yaml_emitter_initialize failed"));

	initialized=true;

	yaml_emitter_set_output(&writer, write_callback,
				reinterpret_cast<void *>(&output_iter));

	yaml_emitter_set_encoding(&writer, options.encoding_opt);
	yaml_emitter_set_canonical(&writer, options.canonical_opt);
	yaml_emitter_set_indent(&writer, options.indent);
	yaml_emitter_set_width(&writer, options.width);
	yaml_emitter_set_unicode(&writer, options.unicode_opt);
	yaml_emitter_set_break(&writer, options.break_opt);

	yaml_event_t start_event;

	if (!yaml_stream_start_event_initialize(&start_event,
						options.encoding_opt))
		throw EXCEPTION(_("yaml_stream_start_event_initialize() failed"));

	EMIT(start_event, stream_start);
	LOG_TRACE("STREAM_START");
}

void writerObj::finish()
{
	yaml_event_t end_event;

	if (!yaml_stream_end_event_initialize(&end_event))
		throw EXCEPTION(_("yaml_stream_end_event_initialize() failed"));

	EMIT(end_event, stream_end);
	LOG_TRACE("STREAM_END");

	if (!yaml_emitter_flush(&writer))
		throw EXCEPTION(_("yaml_emitter_flush() failed"));
	LOG_TRACE("FLUSH");

}

void writerObj::write(const const_newdocumentnode &newdocument)
{
	{
		std::vector<yaml_tag_directive_t> tagvec;

		tagvec.reserve(newdocument->tags.size());

		for (const auto &tag:newdocument->tags)
		{
			yaml_tag_directive_t d=yaml_tag_directive_t();

			d.handle=reinterpret_cast<yaml_char_t *>
				(const_cast<char *>(tag.first.c_str()));
			d.prefix=reinterpret_cast<yaml_char_t *>
				(const_cast<char *>(tag.second.c_str()));
			tagvec.push_back(d);
		}

		yaml_tag_directive_t *beg=nullptr, *end=nullptr;

		if (!tagvec.empty())
		{
			beg=&*tagvec.begin();
			end=beg+tagvec.size();
		}

		yaml_event_t event;

		if (!yaml_document_start_event_initialize(&event,
							  newdocument->version
							  .major ||
							  newdocument->version
							  .minor
							  ?
							  const_cast<yaml_version_directive_t *>
							  (&newdocument->version)
							  : 0,
							  beg, end,
							  newdocument->
							  start_implicit))
			throw EXCEPTION(_("yaml_document_start_event_initialize"
					  " failed"));

		EMIT(event, document_start);
		LOG_TRACE("DOCUMENT_START");
	}

	newdocument->root()->write(*this);

	yaml_event_t event;

	if (!yaml_document_end_event_initialize(&event,
						newdocument->end_implicit))
		throw EXCEPTION(_("yaml_document_end_event_initialize"
				  " failed"));

	EMIT(event, document_end);
	LOG_TRACE("DOCUMENT_END");
}

void writerObj::write_scalar(const newscalarnodeObj &scalar)
{
	yaml_event_t event;

	if (!yaml_scalar_event_initialize(&event,
					  scalar.anchor.size() ?
					  reinterpret_cast<yaml_char_t *>
					  (const_cast<char *>(scalar.anchor
							      .c_str())):0,
					  scalar.tag.size() ?
					  reinterpret_cast<yaml_char_t *>
					  (const_cast<char *>(scalar.tag
							      .c_str())):0,
					  reinterpret_cast<yaml_char_t *>
					  (const_cast<char *>(scalar.value
							      .c_str())),
					  scalar.value.size(),
					  scalar.plain_implicit,
					  scalar.quoted_implicit,
					  scalar.style))
		throw EXCEPTION(_("yaml_scalar_event_initialize failed"));
	EMIT(event, scalar);
	LOG_TRACE("SCALAR");
}

void writerObj::write_alias(const newaliasnodeObj &alias)
{
	yaml_event_t event;

	if (!yaml_alias_event_initialize(&event,
					  reinterpret_cast<yaml_char_t *>
					  (const_cast<char *>(alias.anchor
							      .c_str()))))
		throw EXCEPTION(_("yaml_alias_event_initialize failed"));
	EMIT(event, alias);
	LOG_TRACE("ALIS");
}

void writerObj::write_sequence_start(const std::string &anchor,
				     const std::string &tag,
				     bool implicit,
				     yaml_sequence_style_t style)
{
	yaml_event_t event;

	if (!yaml_sequence_start_event_initialize(&event,
						  anchor.size() ?
						  reinterpret_cast<yaml_char_t
						  *>
						  (const_cast<char *>
						   (anchor.c_str())):0,
						  tag.size() ?
						  reinterpret_cast<yaml_char_t
						  *>
						  (const_cast<char *>
						   (tag.c_str())):0,
						  implicit, style))

		throw EXCEPTION(_("yaml_sequence_start_event_initialize failed")
				);

	EMIT(event, sequence_start);
	LOG_TRACE("SEQUENCE_START");
}

void writerObj::write_sequence_end()
{
	yaml_event_t event;

	if (!yaml_sequence_end_event_initialize(&event))
		throw EXCEPTION(_("yaml_sequence_end_event_initialize failed"));

	EMIT(event, sequence_end);
	LOG_TRACE("SEQUENCE_END");
}

void writerObj::write_mapping_start(const std::string &anchor,
				     const std::string &tag,
				     bool implicit,
				     yaml_mapping_style_t style)
{
	yaml_event_t event;

	if (!yaml_mapping_start_event_initialize(&event,
						 anchor.size() ?
						 reinterpret_cast<yaml_char_t
						 *>
						 (const_cast<char *>
						  (anchor.c_str())):0,
						 tag.size() ?
						 reinterpret_cast<yaml_char_t
						 *>
						 (const_cast<char *>
						  (tag.c_str())):0,
						 implicit, style))

		throw EXCEPTION(_("yaml_mapping_start_event_initialize failed")
				);

	EMIT(event, mapping_start);
	LOG_TRACE("MAPPING_START");
}

void writerObj::write_mapping_end()
{
	yaml_event_t event;

	if (!yaml_mapping_end_event_initialize(&event))
		throw EXCEPTION(_("yaml_mapping_end_event_initialize failed"));

	EMIT(event, mapping_end);
	LOG_TRACE("MAPPING_END");
}

writerObj::~writerObj()
{
	if (initialized)
		yaml_emitter_delete(&writer);
}

writer::options::options()
	: encoding_opt(YAML_ANY_ENCODING), break_opt(YAML_ANY_BREAK),
	  canonical_opt(false), unicode_opt(false), indent(4),
	  width(-1)
{
}

#if 0
{
#endif
}
