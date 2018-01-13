/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_yaml_parserobj_H
#define x_yaml_parserobj_H

#include <x/namespace.h>
#include <x/exception.H>
#include <x/yaml/document.H>

#include <iterator>
#include <iostream>
#include <list>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

//! Parse a YAML document.

class parserObj : virtual public obj {

public:

	//! Base class for implementing read handler callback from libyaml.

	//! \internal

	class read_handler_base {

	public:

		//! Implement the read request.

		virtual int read(unsigned char *buffer, size_t size,
				 size_t *size_read)=0;
		//! An exception was caught

		bool caught_exception_flag;

		//! An exception that got caught inside the C callback from yaml gets rethrown here

		exception caught_exception;

		//! Constructor
		read_handler_base() : caught_exception_flag(false) {}
	};

private:
	//! Read handler callback, read from an input iterator sequence

	template<typename iter_type>
	class read_handler : public read_handler_base {

	public:

		//! Current iterator
		iter_type b;

		//! Ending iterator value
		iter_type e;

		//! Constructor

		read_handler(const iter_type &bArg,
			     const iter_type &eArg) : b(bArg), e(eArg)
		{
		}

		//! Implement the callback from libyaml

		int read(unsigned char *buffer, size_t size,
			 size_t *size_read) override
		{
			*size_read=0;

			try {
				while (b != e && size)
				{
					*buffer++ = *b;
					++b;
					--size;
					++*size_read;
				}
			} catch (const x::exception &e)
			{
				caught_exception=e;
				caught_exception_flag=true;
			}
			return 1;
		}
	};
public:

	//! The constructor puts parsed documents here.

	std::list<document> documents;

	//! Constructor, parse a YAML file from an arbitrary input sequence.

	template<typename iter_type>
	parserObj(iter_type b, iter_type e)
	{
		read_handler<iter_type> handler(b, e);

		parse(handler);
	}

	//! Destructor
	~parserObj();

private:

	//! Internal parser implementation

	void parse(read_handler_base &b);

	//! Internal parser implementation

	//! \internal
	void parse(read_handler_base &b, yaml_parser_t &parser)
		LIBCXX_INTERNAL;
};

//! Specialized implementation that uses the const_iterator-based implemenation

template<>
parserObj::parserObj(std::string::iterator, std::string::iterator);

#ifndef DOXYGEN
extern template class parserObj::read_handler<std::string::const_iterator>;
extern template class parserObj::read_handler<std::istreambuf_iterator<char> >;

extern template parserObj::parserObj(std::string::const_iterator,
				     std::string::const_iterator);

extern template parserObj::parserObj(std::istreambuf_iterator<char>,
				     std::istreambuf_iterator<char>);
#endif

#if 0
{
	{
#endif
	}
}
#endif
