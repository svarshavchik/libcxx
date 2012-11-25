/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/refiterator.H"
#include "x/ref.H"
#include "x/obj.H"
#include "x/exception.H"
#include "x/options.H"
#include <iterator>
#include <vector>
#include <algorithm>
#include <iostream>

class dummyOutputIterObj : virtual public LIBCXX_NAMESPACE::obj {
public:

	typedef std::iterator<std::output_iterator_tag, void, void, void,
			      void> iterator_traits;

	std::string buffer;

public:
	dummyOutputIterObj() {}
	~dummyOutputIterObj() noexcept {}

	void operator++() {}

	LIBCXX_NAMESPACE::ref<dummyOutputIterObj> before_postoper()
	{
		return LIBCXX_NAMESPACE::ref<dummyOutputIterObj>(this);
	}

	dummyOutputIterObj &operator*()
	{
		return *this;
	}

	void operator=(char c)
	{
		buffer.push_back(c);
	}

	dummyOutputIterObj *operator->()
	{
		return this;
	}
};

void testoutputrefiterator()
{
	auto iter=LIBCXX_NAMESPACE::make_refiterator(LIBCXX_NAMESPACE::ref<dummyOutputIterObj>::create());

	std::string s="fooba";

	iter=std::copy(s.begin(), s.end(), iter);

	*iter++='r';

	if (iter.get()->buffer != "foobar" ||
	    iter->buffer != "foobar") // Test operator->
		throw EXCEPTION("testoutputrefiter failed");
}

class dummyInputIterObj : virtual public LIBCXX_NAMESPACE::obj {
public:

	typedef std::iterator<std::input_iterator_tag, void, void, void,
			      void> iterator_traits;

	const char *str;

public:
	dummyInputIterObj(const char *strArg) : str(strArg) {}
	~dummyInputIterObj() noexcept {}

	LIBCXX_NAMESPACE::ref<dummyInputIterObj> before_postoper()
	{
		return LIBCXX_NAMESPACE::ref<dummyInputIterObj>
			::create(str);
	}

	dummyInputIterObj &operator*()
	{
		return *this;
	}

	void operator++()
	{
		++str;
	}

	operator char() const
	{
		return *str;
	}

	bool operator==(const dummyInputIterObj &iter) const
	{
		return str == iter.str;
	}

	bool operator!=(const dummyInputIterObj &iter) const
	{
		return !operator==(iter);
	}

};

void testinputrefiterator()
{
	static const char foobar[]="foobar";

	auto b=LIBCXX_NAMESPACE::make_refiterator(LIBCXX_NAMESPACE::ref<dummyInputIterObj>::create(foobar)),
		e=LIBCXX_NAMESPACE::make_refiterator(LIBCXX_NAMESPACE::ref<dummyInputIterObj>::create(foobar+6));

	std::string s;

	s.push_back(*b++);

	std::copy(b, e,
		  std::back_insert_iterator<std::string>(s));

	if (s != "foobar")
		throw EXCEPTION("testinputrefiterator failed");
}

class dummyInputIterator2 : public LIBCXX_NAMESPACE::inputrefiteratorObj<char> {

public:
	mutable std::string s;

	dummyInputIterator2(const std::string sArg) : s(sArg)
	{
	}

	~dummyInputIterator2() noexcept
	{
	}

	void fill() const
	{
		buffer.clear();
		buffer.insert(buffer.end(), s.begin(), s.end());
		s.clear();
	}
};

void testinputrefiterator2()
{
	std::string s;

	std::copy(LIBCXX_NAMESPACE::inputrefiterator<char>
		  (LIBCXX_NAMESPACE::ref<dummyInputIterator2>
		   ::create("foobar")),
		  LIBCXX_NAMESPACE::inputrefiterator<char>::create(),
		  std::back_insert_iterator<std::string>(s));

	if (s != "foobar")
		throw EXCEPTION("testinputrefiterator2 failed");
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::option::list
			options(LIBCXX_NAMESPACE::option::list::create());

		options->addDefaultOptions();

		LIBCXX_NAMESPACE::option::parser
			opt_parser(LIBCXX_NAMESPACE::option::parser::create());

		opt_parser->setOptions(options);
		int err=opt_parser->parseArgv(argc, argv);

		if (err == 0) err=opt_parser->validate();

		if (err)
		{
			if (err == LIBCXX_NAMESPACE::option::parser::base
			    ::err_builtin)
				exit(0);

			std::cerr << opt_parser->errmessage();
			std::cerr.flush();
			exit(1);
		}
		testoutputrefiterator();
		testinputrefiterator();
		testinputrefiterator2();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
