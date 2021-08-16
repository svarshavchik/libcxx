/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_writerfwd_H
#define x_yaml_writerfwd_H

#include <x/namespace.h>
#include <x/yaml/newdocumentnodefwd.H>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

class writerObj;

//! Write a YAML document

//! This class defines several overloaded versions of write() that iterate
//! over an input sequence of \ref newdocumentnode "INSERT_LIBX_NAMESPACE::yaml::newdocumentnode"s.
//! The sequence is defined by explicit beginning and ending iterators,
//! or by a container that's understood by std::begin() and std::end().
//! The last parameter to write() is an output iterator over chars, where the
//! YAML documents get written to.
//!
//! Multiple YAML documents, in one go, get written into the output iterator by
//! supplying an input sequence, or a container, with more than one
//! \ref newdocumentnode "INSERT_LIBX_NAMESPACE::yaml::newdocumentnode".
//! write() returns the value of the output
//! iterator after writing all YAML documents to the output iterator.
//!
//! write() also takes an optional first parameter that set various
//! \ref writer::options "options" for writing the YAML documents.

class writer {

public:

	class options;

	class output_iterator_base;

	template<typename output_iterator_type> class output_iterator;

	//! Write an input sequence of documents, with options

	template<typename input_iterator_type,
		 typename output_iterator_type>
	static output_iterator_type write(const options &,
					 input_iterator_type,
					 input_iterator_type,
					 output_iterator_type);

	//! Write an input sequence of documents from a container, with options

	template<typename input_container_type,
		 typename output_iterator_type>
	static output_iterator_type write(const options &,
					 const input_container_type &,
					 output_iterator_type);

	//! Write an input sequence of documents, using default options

	template<typename input_iterator_type,
		 typename output_iterator_type>
	static output_iterator_type write(input_iterator_type,
					 input_iterator_type,
					 output_iterator_type);

	//! Write an input sequence of documents from a container, using default options

	template<typename input_container_type,
		 typename output_iterator_type>
	static output_iterator_type write(const input_container_type &,
					 output_iterator_type);

	//! The most common case of writing a single document using default options

	template<typename output_iterator_type>
	static output_iterator_type
	write_one_document(const const_newdocumentnode &,
			  output_iterator_type);

	//! Write a single document, with custom options

	template<typename output_iterator_type>
	static output_iterator_type
	write_one_document(const options &,
			  const const_newdocumentnode &,
			  output_iterator_type);
};

#if 0
{
#endif
}
#endif
