/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_yaml_writer_H
#define x_yaml_writer_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/ref.H>
#include <x/exception.H>
#include <x/yaml/writerfwd.H>
#include <x/yaml/writerobj.H>
#include <x/yaml/newdocumentnode.H>
#include <yaml.h>

#include <vector>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

//! writer options
class writer::options {

public:
	//! Encoding option

	yaml_encoding_t encoding_opt;

	//! Set the encoding option

	options &setencoding(yaml_encoding_t opt)
	{
		encoding_opt=opt;
		return *this;
	}

	//! Break option

	yaml_break_t break_opt;

	//! Set the break option

	options &setbreak(yaml_break_t opt)
	{
		break_opt=opt;
		return *this;
	}

	//! Canonical option

	bool canonical_opt;

	//! Set the canonical option

	options &setcanonical(bool opt)
	{
		canonical_opt=opt;
		return *this;
	}

	//! Unicode option

	bool unicode_opt;

	//! Set the unicode option

	options &setunicode(bool opt)
	{
		unicode_opt=opt;
		return *this;
	}

	//! Indentation option

	int indent;

	//! Set the indentation option

	options &setindent(int opt)
	{
		indent=opt;
		return *this;
	}

	//! Width option

	int width;

	//! Set the width option

	options &setwidth(int opt)
	{
		width=opt;
		return *this;
	}

	//! Constructor
	options();
};

//! Output iterator superclass

class writer::output_iterator_base {

public:

	//! write() callback implemented in the subclass

	virtual int write(const unsigned char *buffer,
			   size_t size)=0;
};

//! Callback from LibYAML that writes the contents of the document to an output iterator.

//! If an exception gets caught, it gets rethrown upon return from LibYAML.
//!

template<typename output_iterator_type>
class writer::output_iterator : public output_iterator_base {

public:
	//! Current value of the output iterator

	output_iterator_type iterator;

	//! Constructor
	output_iterator(output_iterator_type iteratorArg)
		: iterator(iteratorArg), caught_exception_flag(false)
	{
	}

	//! An exception was caught

	bool caught_exception_flag;

	//! An exception that got caught inside the C callback from yaml gets rethrown here

	exception caught_exception;

	//! Implement the write() callback from LibYAML

	int write(const unsigned char *buffer, size_t size) override
	{
		if (caught_exception_flag)
			return 0; // Don't even try

		try {
			while (size)
			{
				*iterator = *buffer;
				++iterator;
				++buffer;
				--size;
			}
		} catch (const exception &e)
		{
			caught_exception=e;
			caught_exception_flag=true;
			return 0;
		}
		return 1;
	}
};

#ifndef DOXYGEN_FIXME

template<typename input_container_type,
	 typename output_iterator_type>
output_iterator_type writer::write(const input_container_type &container,
				   output_iterator_type out_iter)
{
	return write(std::begin(container), std::end(container), out_iter);
}

template<typename input_iterator_type,
	 typename output_iterator_type>
output_iterator_type writer::write(input_iterator_type beg_iter,
				   input_iterator_type end_iter,
				   output_iterator_type out_iter)
{
	return write(options(), beg_iter, end_iter, out_iter);
}

template<typename input_container_type,
	 typename output_iterator_type>
output_iterator_type writer::write(const options &opts,
				   const input_container_type &container,
				   output_iterator_type out_iter)
{
	return write(options(), std::begin(container), std::end(container),
		    out_iter);
}

template<typename input_iterator_type,
	 typename output_iterator_type>
output_iterator_type writer::write(const options &opts,
				   input_iterator_type beg_iter,
				   input_iterator_type end_iter,
				   output_iterator_type out_iter)
{
	output_iterator<output_iterator_type> out(out_iter);

	try {
		auto e=ref<writerObj>::create(out, opts);

		while (beg_iter != end_iter)
		{
			e->write(*beg_iter);
			++beg_iter;
		}

		e->finish();

		// Rethrowing the original exception in the callback takes
		// precedence over all other thrown exceptions.

	} catch (...) {
		if (!out.caught_exception_flag)
			throw;
	}
	if (out.caught_exception_flag)
		out.caught_exception->rethrow();

	return out.iterator;
}

template<typename output_iterator_type>
output_iterator_type writer::write_one_document(const const_newdocumentnode &d,
						output_iterator_type iter)
{
	return write_one_document(options(), d, iter);
}

template<typename output_iterator_type>
output_iterator_type writer::write_one_document(const options &options,
						const const_newdocumentnode &d,
						output_iterator_type iter)
{
	std::vector<const_newdocumentnode> vec={d};

	return write(options, vec, iter);
}
#endif

#if 0
{
#endif
}
#endif
