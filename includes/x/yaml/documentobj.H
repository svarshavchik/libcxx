/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_yaml_documentobj_H
#define x_yaml_documentobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/sipobj.H>
#include <x/mpobj.H>
#include <x/yaml/nodefwd.H>
#include <x/yaml/docnodefwd.H>
#include <yaml.h>

#include <set>
#include <vector>

namespace LIBCXX_NAMESPACE {

	//! Load and save documents in YAML format.

	//! This namespace implements a \ref parser "parser" for loading
	//! YAML-formatted documents, and a \ref writer "writer class" that
	//! generates YAML-formatted documents.

	namespace yaml {
#if 0
	}
}
#endif

class parserObj;
class scalarnodeObj;
class mappingnodeObj;
class sequencenodeObj;

//! libyaml document object

class documentObj : virtual public obj {

	//! The YAML document

	yaml_document_t y;

	//! Access to yaml_document_t

	//! Do not accesss y directly. Use document_t. Unfortunately, LibYAML
	//! API does not have the const-ness of pointers to yaml_document_t
	//! quite figured out. So what we do is put a pointer to yaml_document_t
	//! into the sipobj, and use sipobj to implement a readers/writer
	//! contract, but then dereference the pointer to obtain a mutable
	//! yaml_document_t.

	typedef sipobj<yaml_document_t *> document_t;

	//! The read lock on the mutex-protected document.

	typedef document_t::readlock readlock_t;

	//! The write lock on the mutex-protected document.

	typedef document_t::writelock writelock_t;

	//! The document object

	mutable document_t doc;

	//! The container for the tag cache.

	//! Instead of creating a string object for every node's tag, which
	//! can consume lots of memory, we'll cache the tags here.

	typedef mpobj<std::set<std::string> > tags_t;

	//! Cached tags
	mutable tags_t tags;

	//! Whether the parser initialized the document object

	bool initialized;

	class do_not_initialize;

public:
	//! Constructor used by the parser

	//! The argument referencing a nonexistent private class prevents
	//! instantiation except by the parser.

	documentObj(do_not_initialize *);

	friend class parserObj;
	friend class docnodeObj;

	//! Destructor
	~documentObj();

	//! Return the document's root node.

	node root() const;

	friend class scalarnodeObj;
	friend class mappingnodeObj;
	friend class sequencenodeObj;

private:

	//! Construct a node

	//! \internal
	node construct(const docnode &d,
		       const yaml_node_t &n) const LIBCXX_HIDDEN;

	//! Construct a node, by number

	node construct(yaml_node_item_t i) const;
};

#if 0
{
	{
#endif
	}
}
#endif
