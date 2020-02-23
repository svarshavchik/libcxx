/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_writerobj_H
#define x_yaml_writerobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/yaml/writerfwd.H>
#include <x/yaml/newaliasnodefwd.H>
#include <x/yaml/newscalarnodefwd.H>
#include <x/yaml/newsequencenodefwd.H>
#include <x/yaml/newmappingnodefwd.H>
#include <x/yaml/newdocumentnodefwd.H>
#include <x/logger.H>
#include <yaml.h>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! A container for a yaml_emitter_t

class writerObj : virtual public obj {

	//! The logger class
	LOG_CLASS_SCOPE;

	//! Whether the writer object has been initialized by LibYMAL.

	bool initialized;

	//! The LibYAML writer object.

	yaml_emitter_t writer;

public:
	//! Construct the writer object

	writerObj(writer::output_iterator_base &output_iter,
		   const writer::options &options);

	//! Destructor
	~writerObj();

	friend class writer;
	friend class newscalarnodeObj;
	friend class newaliasnodeObj;
	friend class newsequence;
	friend class newmapping;

private:
	//! Emit a document
	void write(const const_newdocumentnode &newdocument);

	//! Emit a scalar
	void write_scalar(const newscalarnodeObj &scalar);

	//! Emit an alias
	void write_alias(const newaliasnodeObj &scalar);

	//! Emit sequence start

	void write_sequence_start(const std::string &anchor,
				 const std::string &tag,
				 bool implicit,
				 yaml_sequence_style_t style);

	//! Emit sequence end

	void write_sequence_end();

	//! Emit mapping start

	void write_mapping_start(const std::string &anchor,
				 const std::string &tag,
				 bool implicit,
				 yaml_mapping_style_t style);

	//! Emit mapping end

	void write_mapping_end();

	//! Called by write() after it's done writing the last document

	void finish();

};

#if 0
{
	{
#endif
	}
}
#endif
