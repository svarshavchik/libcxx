/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/options.H"
#include "x/exception.H"
#include "x/to_string.H"
#include "x/fmtsize.H"
#include "x/locale.H"
#include <iostream>
#include <iterator>
#include <fstream>
#include <unistd.h>

static void testoptions1()
{
	LIBCXX_NAMESPACE::option::int_value debug_value(LIBCXX_NAMESPACE::option::int_value::create(0));
	LIBCXX_NAMESPACE::option::bool_value verbose_value(LIBCXX_NAMESPACE::option::bool_value::create());
	LIBCXX_NAMESPACE::option::int_value retries_value(LIBCXX_NAMESPACE::option::int_value::create(4));


	LIBCXX_NAMESPACE::option::string_list filename_values(LIBCXX_NAMESPACE::option::string_list::create()
					       );
	LIBCXX_NAMESPACE::option::or_op<int>::value
		debug_alloc_value(LIBCXX_NAMESPACE::option::or_op<int>::value::create(debug_value, 1)),
		debug_auth_value(LIBCXX_NAMESPACE::option::or_op<int>::value::create(debug_value, 2));

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName("testoptions");
	options->add(retries_value,
		     'r', "retries",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "How many times to retry",
		     "count")
		.add(filename_values,
		     'f', "",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "Filename to process",
		     "filename")
		.add(debug_alloc_value,
		     0,
		     "debug-alloc",
		     0,
		     "Debug allocations")
		.add(debug_auth_value,
		     0,
		     "debug-auth",
		     0,
		     "Debug authentication")
		.addArgument("flag", LIBCXX_NAMESPACE::option::list::base::required)
		.addArgument("filename", LIBCXX_NAMESPACE::option::list::base::repeated);

	options->usage(std::cout, 40);
	options->help(std::cout, 40);
	std::cout << std::flush;
}

static void testoptions2()
{
	LIBCXX_NAMESPACE::option::bool_value a_value(LIBCXX_NAMESPACE::option::bool_value::create());
	LIBCXX_NAMESPACE::option::bool_value b_value(LIBCXX_NAMESPACE::option::bool_value::create());
	LIBCXX_NAMESPACE::option::bool_value c_value(LIBCXX_NAMESPACE::option::bool_value::create());

	LIBCXX_NAMESPACE::option::bool_value d_value(LIBCXX_NAMESPACE::option::bool_value::create());
	LIBCXX_NAMESPACE::option::string_value e_value(LIBCXX_NAMESPACE::option::string_value::create());

	LIBCXX_NAMESPACE::option::bool_value f_value(LIBCXX_NAMESPACE::option::bool_value::create());
	LIBCXX_NAMESPACE::option::string_value g_value(LIBCXX_NAMESPACE::option::string_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());
	options->setAppName("testoptions");

	options->add(a_value,
		     'a', "",
		     0,
		     "A parameter")
		.add(b_value,
		     'b', "",
		     0,
		     "B parameter")
		.add(c_value,
		     'c', "",
		     0,
		     "C parameter")
		.add(d_value,
		     'd', "",
		     0,
		     "D parameter")
		.add(e_value,
		     'e', "",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "E parameter")
		.add(f_value,
		     'f', "",
		     0,
		     "F parameter")
		.add(g_value,
		     'g', "",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "G parameter");

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);
	p->parseString(" -abc word1 -de=earg word2 -fg garg word3 -- -z word4");

	std::cout << "test2: "
		  << p->validate()
		  << std::endl;

	auto l=LIBCXX_NAMESPACE::locale::base::environment();

	std::cout << "A: " << (a_value->value ? 1:0) << std::endl;
	std::cout << "B: " << (b_value->value ? 1:0) << std::endl;
	std::cout << "C: " << (c_value->value ? 1:0) << std::endl;
	std::cout << "D: " << (d_value->value ? 1:0) << std::endl;
	std::cout << "E: " << e_value->value << std::endl;
	std::cout << "F: " << (f_value->value ? 1:0) << std::endl;
	std::cout << "G: " << g_value->value << std::endl;

	std::list<std::string>::iterator b=p->args.begin(),
		e=p->args.end();

	while (b != e)
	{
		std::cout << "Param: "
			   << *b++ << std::endl;
	}
}

static void testoptions31()
{
	LIBCXX_NAMESPACE::option::int_value a_value(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName("testoptions");
	options->add(a_value,
		     0, "a",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "A parameter");

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--a=31");

	std::cout << "test3.1: "
		  << p->validate()
		  << std::endl;

	std::cout << "A: " << a_value->value << std::endl;
}

static void testoptions32()
{
	LIBCXX_NAMESPACE::option::int_value a_value(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName("testoptions");
	options->add(a_value,
		     0, "a",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "A parameter");

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--a 32");

	std::cout << "test3.2: "
		  << p->validate()
		  << std::endl;

	std::cout << "A: " << a_value->value << std::endl;
}

static void testoptions33()
{
	LIBCXX_NAMESPACE::option::int_value a_value(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName("testoptions");
	options->add(a_value,
		     0, "a",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "A parameter");

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--a x");

	std::cout << "test3.3: "
		   << p->validate() << ": ";

	std::cout << p->err_option << std::endl;
}

static int function_opt() noexcept
{
	std::cout << "--function option invoked" << std::endl;
	return 0;
}

static int intfunction_opt(int arg) noexcept
{
	std::cout << "--intfunction=" << arg
		   << " option invoked" << std::endl;
	return 0;
}

static void testoptions4()
{
	LIBCXX_NAMESPACE::option::string_value a_value(LIBCXX_NAMESPACE::option::string_value::create());
	LIBCXX_NAMESPACE::option::string_value b_value(LIBCXX_NAMESPACE::option::string_value::create());
	LIBCXX_NAMESPACE::option::string_value c_value(LIBCXX_NAMESPACE::option::string_value::create());
	LIBCXX_NAMESPACE::option::string_value d_value(LIBCXX_NAMESPACE::option::string_value::create());
	LIBCXX_NAMESPACE::option::string_value e_value(LIBCXX_NAMESPACE::option::string_value::create());
	LIBCXX_NAMESPACE::option::string_value f_value(LIBCXX_NAMESPACE::option::string_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName("testoptions");
	options->add(a_value,
		     'a', "A",
		     0,
		     "A parameter")
		.add(b_value,
		     'b', "B",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "B parameter")
		.add(c_value,
		     'c', "C",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "C parameter")
		.add(d_value,
		     'd', "D",
		     0,
		     "D parameter")
		.add(e_value,
		     'e', "E",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "E parameter")
		.add(f_value,
		     'f', "F",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "F parameter")
		.add(&function_opt,
		     0, "function")
		.add(&intfunction_opt,
		     0, "intfunction");

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--function --intfunction=2 --A --B bval --C=cval --D word1 -eearg --F");

	std::cout << "test4: "
		  << p->validate()
		  << std::endl;

	auto l=LIBCXX_NAMESPACE::locale::base::environment();

	std::cout << "A: " << a_value->value << std::endl;
	std::cout << "B: " << b_value->value << std::endl;
	std::cout << "C: " << c_value->value << std::endl;
	std::cout << "D: " << d_value->value << std::endl;
	std::cout << "E: " << e_value->value << std::endl;
	std::cout << "F: " << f_value->value << std::endl;

	std::list<std::string>::iterator b=p->args.begin(),
		e=p->args.end();

	while (b != e)
	{
		std::cout << "Param: " << *b++ << std::endl;
	}
}

static void testoptions5()
{
	LIBCXX_NAMESPACE::option::int_value a_value(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName("testoptions");

	options->add(a_value,
		     0, "a",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "A parameter");

	std::ofstream o("testoptions0.tmp");

	if (!o.is_open())
		throw EXCEPTION("testoptions0.tmp");

	o << "--a 4";
	o.close();

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("@testoptions0.tmp");

	std::cout << "test5: "
		  << p->validate()
		  << std::endl;
	unlink("testoptions0.tmp");
	std::cout << "A: " << a_value->value << std::endl;
}

static void testoptions6()
{
	LIBCXX_NAMESPACE::option::int_value a_value(LIBCXX_NAMESPACE::option::int_value::create());
	LIBCXX_NAMESPACE::option::int_value b_value(LIBCXX_NAMESPACE::option::int_value::create());
	LIBCXX_NAMESPACE::option::int_value c_value(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName("testoptions");

	options->add(a_value,
		     0, "a",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue | LIBCXX_NAMESPACE::option::list::base::required,
		     "A parameter", "avalue")
		.add(b_value,
		     'b', "",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue | LIBCXX_NAMESPACE::option::list::base::required,
		     "B parameter", "bvalue")
		.add(c_value,
		     0, "c",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "C parameter")
		.addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--a=1");

	std::cout << "test6: "
		   << p->validate() << ": ";

	std::cout << p->err_option << std::endl;
	std::cout << "A: " << a_value->value << std::endl;
	std::cout << "B: " << b_value->value << std::endl;
	std::cout << "C: " << c_value->value << std::endl;
	p->setOptions(options);
	p->parseString("--help=40");
	std::cout << "test6.2: "
		   << p->validate() << std::endl;
	p->setOptions(options);
	p->parseString("--usage=40");
	std::cout << "test6.3: "
		   << p->validate() << std::endl;
}

static void testoptions7()
{
	LIBCXX_NAMESPACE::option::int_value do_sprockets(LIBCXX_NAMESPACE::option::int_value::create()),
		do_sprickets(LIBCXX_NAMESPACE::option::int_value::create()),

		add_sprocket(LIBCXX_NAMESPACE::option::int_value::create()),
		del_sprocket(LIBCXX_NAMESPACE::option::int_value::create()),
		frob_sprocket(LIBCXX_NAMESPACE::option::int_value::create()),
		grok_sprocket(LIBCXX_NAMESPACE::option::int_value::create()),

		add_spricket(LIBCXX_NAMESPACE::option::int_value::create()),
		del_spricket(LIBCXX_NAMESPACE::option::int_value::create()),

		do_gadget(LIBCXX_NAMESPACE::option::int_value::create());

#define STATUS_SHIFT(p,n) (((p) << 1) | (n)->value)

	// Sorry:

#define STATUS_ALL						\
	STATUS_SHIFT						\
		(STATUS_SHIFT					\
		(STATUS_SHIFT					\
		(STATUS_SHIFT					\
		(STATUS_SHIFT					\
		(STATUS_SHIFT					\
		 (STATUS_SHIFT(0, do_sprockets),		\
		  do_sprickets),				\
		 add_sprocket),					\
		 del_sprocket),					\
		 add_spricket),					\
		 del_spricket),					\
		 do_gadget)



	LIBCXX_NAMESPACE::option::list sprocket_options(LIBCXX_NAMESPACE::option::list::create());

	sprocket_options->add(add_sprocket,
			      0, "add", 0, "Add a sprocket")
		.add(del_sprocket,
		     0, "del", 0, "Delete a sprocket")
		.add(grok_sprocket,
		     0, "grok", 0, "Grok a sprocket")
		.add(frob_sprocket,
		     0, "from", 0, "From a sprocket");

	LIBCXX_NAMESPACE::option::list spricket_options(LIBCXX_NAMESPACE::option::list::create());

	spricket_options->add(add_spricket,
			      0, "add", 0, "Add a spricket")
		.add(del_spricket,
		     0, "del", 0, "Delete a spricket");

	LIBCXX_NAMESPACE::option::int_value do_groz(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list groz_options(LIBCXX_NAMESPACE::option::list::create());

	groz_options->addArgument("grozA", 0)
		.addArgument("grozB", 0);

	LIBCXX_NAMESPACE::option::list main_options(LIBCXX_NAMESPACE::option::list::create());

	main_options->setAppName("testoptions");

	main_options->add(do_sprockets, 0, "sprockets", sprocket_options,
		     "Do something with sprockets")
		.add(do_sprickets, 0, "sprickets", spricket_options,
		     "Do something with sprickets")
		.add(do_groz, 0, "groz", groz_options,
		     "Do something with grozes")
		.add(do_gadget, 'g', "", 0, "Dummy flag")
		.addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser opt_parser(LIBCXX_NAMESPACE::option::parser::create());

	opt_parser->setOptions(main_options);

	opt_parser->parseString("--help=40");
	opt_parser->validate();
	opt_parser->reset();
	opt_parser->parseString("--usage=40");
	opt_parser->validate();
	opt_parser->reset();

	std::cout << "Status before: " << STATUS_ALL << std::endl;

	if (opt_parser->parseString("--sprickets --del") == 0 &&
	    opt_parser->validate() == 0)
	{
		std::cout << "Status after --sprickets --del: "
			   << STATUS_ALL << std::endl;
	}

	opt_parser->reset();
	std::cout << "Status before: " << STATUS_ALL << std::endl;

	if (opt_parser->parseString("--sprockets --add") == 0 &&
	    opt_parser->validate() == 0)
	{
		std::cout << "Status after --spoickets --add: "
			   << STATUS_ALL << std::endl;
	}
}

static void testoptions8()
{
	LIBCXX_NAMESPACE::option::int_value a(LIBCXX_NAMESPACE::option::int_value::create()),
		b(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::mutex m(LIBCXX_NAMESPACE::option::mutex::create());

	m->add(a).add(b);

	LIBCXX_NAMESPACE::option::list opts(LIBCXX_NAMESPACE::option::list::create());

	opts->setAppName("testoptions");

	opts->add(a, 'a', "", 0, "Option A")
		.add(b, 'b', "", 0, "Option B");

	LIBCXX_NAMESPACE::option::parser opt_parser(LIBCXX_NAMESPACE::option::parser::create());

	opt_parser->setOptions(opts);

	opt_parser->parseString("-a");
	std::cout << "Mutex test 1: " << opt_parser->validate()
		   << ", a=" << a->value
		   << ", b=" << b->value << std::endl;

	opt_parser->reset();

	opt_parser->parseString("-b");
	std::cout << "Mutex test 2: " << opt_parser->validate()
		   << ", a=" << a->value
		   << ", b=" << b->value << std::endl;

	opt_parser->reset();
	opt_parser->parseString("-a -b");

	std::cout << "Mutex test 2: " << opt_parser->validate()
		   << ", a=" << a->value
		   << ", b=" << b->value << std::endl;

}

static void testoptions9()
{
	LIBCXX_NAMESPACE::option::hms_value hms_value(LIBCXX_NAMESPACE::option::hms_value::create());

	LIBCXX_NAMESPACE::option::ymd_value ymd_value(LIBCXX_NAMESPACE::option::ymd_value::create());

	LIBCXX_NAMESPACE::option::ymd_interval_value
		ymd_interval_value(LIBCXX_NAMESPACE::option::ymd_interval_value::create());

	LIBCXX_NAMESPACE::option::uri_value
		uri_value(LIBCXX_NAMESPACE::option::uri_value::create());

	LIBCXX_NAMESPACE::option::list opts(LIBCXX_NAMESPACE::option::list::create());

	opts->setAppName("testoptions");

	opts->add(hms_value, 't', "",
		  LIBCXX_NAMESPACE::option::list::base::hasvalue,
		  "Time")
		.add(ymd_value, 'd', "",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, "Date")
		.add(ymd_interval_value, 'i', "",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, "Interval")
		.add(uri_value, 'u', "",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, "URI");

	LIBCXX_NAMESPACE::option::parser opt_parser(LIBCXX_NAMESPACE::option::parser::create());

	opt_parser->setOptions(opts);

	opt_parser->parseString("-t \"1 hour\" -d 2010-05-01 -i \"1 week\""
				" -u http://localhost/cgi-bin/test.cgi");

	std::ostreambuf_iterator<char> o(std::cout.rdbuf());

	std::cout << "Time: ";

	hms_value->value.to_string(o);

	std::cout << std::endl << "Date: ";

	ymd_value->value.to_string(o);

	std::cout << std::endl << "Interval: ";

	ymd_interval_value->value.to_string(o);

	std::cout << std::endl << "URI path: "
		  << uri_value->value.getPath() << std::endl;
}

static void testoptions10()
{
	LIBCXX_NAMESPACE::option::hms_list hms_list(LIBCXX_NAMESPACE::option::hms_list::create());

	LIBCXX_NAMESPACE::option::ymd_list ymd_list(LIBCXX_NAMESPACE::option::ymd_list::create());

	LIBCXX_NAMESPACE::option::ymd_interval_list
		ymd_interval_list(LIBCXX_NAMESPACE::option::ymd_interval_list::create());

	LIBCXX_NAMESPACE::option::uri_list
		uri_list(LIBCXX_NAMESPACE::option::uri_list::create());

	LIBCXX_NAMESPACE::option::list opts(LIBCXX_NAMESPACE::option::list::create());

	opts->setAppName("testoptions");

	opts->add(hms_list, 't', "",
		  LIBCXX_NAMESPACE::option::list::base::hasvalue,
		  "Time")
		.add(ymd_list, 'd', "",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, "Date")
		.add(ymd_interval_list, 'i', "",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, "Interval")
		.add(uri_list, 'u', "",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, "URI");

	LIBCXX_NAMESPACE::option::parser opt_parser(LIBCXX_NAMESPACE::option::parser::create());

	opt_parser->setOptions(opts);

	opt_parser->parseString("-t \"1 hour\" -d 2010-05-01 -i \"1 week\""
				" -u http://localhost/cgi-bin/test.cgi");
}

static void testoptions11()
{
	LIBCXX_NAMESPACE::option::memsize_value
		memsize=LIBCXX_NAMESPACE::option::memsize_value::create();

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName("testoptions");

	options->add(memsize, L's', "memsize",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "Memory size",
		     "size");

	options->usage(std::cout, 40);
	options->help(std::cout, 40);

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--memsize=4KB");

	std::cout << "bytes: " << memsize->value.bytes << std::endl;
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::locale::base::utf8()->global();
		testoptions1();
		testoptions2();
		testoptions31();
		testoptions32();
		testoptions33();
		testoptions4();
		testoptions5();
		testoptions6();
		testoptions7();
		testoptions8();
		testoptions9();
		testoptions10();
		testoptions11();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
	}
	return 0;
}
