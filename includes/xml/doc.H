/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_doc_H
#define x_xml_doc_H

#include <x/xml/docfwd.H>
#include <x/xml/docobj.H>
#include <x/xml/parser.H>
#include <x/ref.H>
#include <x/refiterator.H>

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

//! Base class for \ref doc "XML documents".

//! Refer to this class as %INSERT_LIBX_NAMESPACE::xml::doc::base.

class docBase {

public:

	//! Create an empty XML document
	static doc create();

	//! Load an XML document from a file

	static doc create(//! Filename
			  const std::string &filename);

	//! Load an XML document from a file, with non-default options
	static doc create(//! Filename
			  const std::string &filename,

			  //! Non-default options, see libxml/parser.h
			  const std::string &options);

	//! Load an XML document from an input sequence defined by iterators.
	template<typename iter_type>
	static doc create(//! Beginning iterator for an input sequence that defines an XML document
			  iter_type beg_iter,
			  //! Ending iterator for an input sequence that defines an XML document
			  iter_type end_iter,

			  //! What to call this document in any error messages.
			  const std::string &context,

			  //! Non-default options, see libxml/parser.h
			  const std::string &options="")
	{
		return std::copy(beg_iter, end_iter,
				 parser::create(context, options))
			.get()->done();
	}

	//! Object factory, used by \ref ref "INSERT_LIBX_NAMESPACE::ref::create()".

	template<typename ref_type> class objfactory {
	public:

		//! Forward create() call to the base class.

		template<typename ...Args>
		static ref_type create(Args && ...args)
		{
			return docBase::create(std::forward<Args>(args)...);
		}
	};
};

#if 0
{
	{
#endif
	}
}
#endif
