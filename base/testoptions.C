/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "options.H"
#include "exception.H"
#include "tostring.H"
#include "fmtsize.H"
#include "locale.H"
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

	options->setAppName(L"testoptions");
	options->add(retries_value,
		     'r', L"retries",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"How many times to retry",
		     L"count")
		.add(filename_values,
		     'f', L"",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Filename to process",
		     L"filename")
		.add(debug_alloc_value,
		     0,
		     L"debug-alloc",
		     0,
		     L"Debug allocations")
		.add(debug_auth_value,
		     0,
		     L"debug-auth",
		     0,
		     L"Debug authentication")
		.addArgument(L"flag", LIBCXX_NAMESPACE::option::list::base::required)
		.addArgument(L"filename", LIBCXX_NAMESPACE::option::list::base::repeated);

	options->usage(std::wcout, 40);
	options->help(std::wcout, 40);
	std::wcout << std::flush;
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
	options->setAppName(L"testoptions");

	options->add(a_value,
		     'a', L"",
		     0,
		     L"A parameter")
		.add(b_value,
		     'b', L"",
		     0,
		     L"B parameter")
		.add(c_value,
		     'c', L"",
		     0,
		     L"C parameter")
		.add(d_value,
		     'd', L"",
		     0,
		     L"D parameter")
		.add(e_value,
		     'e', L"",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"E parameter")
		.add(f_value,
		     'f', L"",
		     0,
		     L"F parameter")
		.add(g_value,
		     'g', L"",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"G parameter");

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);
	p->parseString(" -abc word1 -de=earg word2 -fg garg word3 -- -z word4");

	std::wcout << L"test2: "
		  << p->validate()
		  << std::endl;

	LIBCXX_NAMESPACE::locale l(LIBCXX_NAMESPACE::locale::base::environment());

	std::wcout << "A: " << (a_value->value ? 1:0) << std::endl;
	std::wcout << "B: " << (b_value->value ? 1:0) << std::endl;
	std::wcout << "C: " << (c_value->value ? 1:0) << std::endl;
	std::wcout << "D: " << (d_value->value ? 1:0) << std::endl;
	std::wcout << "E: " << LIBCXX_NAMESPACE::towstring(e_value->value, l)
		   << std::endl;
	std::wcout << "F: " << (f_value->value ? 1:0) << std::endl;
	std::wcout << "G: " << LIBCXX_NAMESPACE::towstring(g_value->value, l)
		   << std::endl;

	std::list<std::string>::iterator b=p->args.begin(),
		e=p->args.end();

	while (b != e)
	{
		std::wcout << L"Param: "
			   << LIBCXX_NAMESPACE::towstring(*b++, l) << std::endl;
	}
}

static void testoptions31()
{
	LIBCXX_NAMESPACE::option::int_value a_value(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName(L"testoptions");
	options->add(a_value,
		     0, L"a",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"A parameter");

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--a=31");

	std::wcout << L"test3.1: "
		  << p->validate()
		  << std::endl;

	std::wcout << "A: " << a_value->value << std::endl;
}

static void testoptions32()
{
	LIBCXX_NAMESPACE::option::int_value a_value(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName(L"testoptions");
	options->add(a_value,
		     0, L"a",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"A parameter");

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--a 32");

	std::wcout << L"test3.2: "
		  << p->validate()
		  << std::endl;

	std::wcout << "A: " << a_value->value << std::endl;
}

static void testoptions33()
{
	LIBCXX_NAMESPACE::option::int_value a_value(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName(L"testoptions");
	options->add(a_value,
		     0, L"a",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"A parameter");

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--a x");

	std::wcout << L"test3.3: "
		   << p->validate() << L": ";

	std::wcout << LIBCXX_NAMESPACE::towstring(p->err_option,
						LIBCXX_NAMESPACE::locale::base::environment())
		   << std::endl;
}

static int function_opt() noexcept
{
	std::wcout << L"--function option invoked" << std::endl;
	return 0;
}

static int intfunction_opt(int arg) noexcept
{
	std::wcout << L"--intfunction=" << arg
		   << L" option invoked" << std::endl;
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

	options->setAppName(L"testoptions");
	options->add(a_value,
		     'a', L"A",
		     0,
		     L"A parameter")
		.add(b_value,
		     'b', L"B",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"B parameter")
		.add(c_value,
		     'c', L"C",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"C parameter")
		.add(d_value,
		     'd', L"D",
		     0,
		     L"D parameter")
		.add(e_value,
		     'e', L"E",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"E parameter")
		.add(f_value,
		     'f', L"F",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"F parameter")
		.add(&function_opt,
		     0, L"function")
		.add(&intfunction_opt,
		     0, L"intfunction");

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--function --intfunction=2 --A --B bval --C=cval --D word1 -eearg --F");

	std::wcout << L"test4: "
		  << p->validate()
		  << std::endl;

	LIBCXX_NAMESPACE::locale l(LIBCXX_NAMESPACE::locale::base::environment());

	std::wcout << "A: " << LIBCXX_NAMESPACE::towstring(a_value->value, l) << std::endl;
	std::wcout << "B: " << LIBCXX_NAMESPACE::towstring(b_value->value, l) << std::endl;
	std::wcout << "C: " << LIBCXX_NAMESPACE::towstring(c_value->value, l) << std::endl;
	std::wcout << "D: " << LIBCXX_NAMESPACE::towstring(d_value->value, l) << std::endl;
	std::wcout << "E: " << LIBCXX_NAMESPACE::towstring(e_value->value, l) << std::endl;
	std::wcout << "F: " << LIBCXX_NAMESPACE::towstring(f_value->value, l) << std::endl;

	std::list<std::string>::iterator b=p->args.begin(),
		e=p->args.end();

	while (b != e)
	{
		std::wcout << L"Param: "
			   << LIBCXX_NAMESPACE::towstring(*b++, l) << std::endl;
	}
}

static void testoptions5()
{
	LIBCXX_NAMESPACE::option::int_value a_value(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName(L"testoptions");

	options->add(a_value,
		     0, L"a",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"A parameter");

	std::ofstream o("testoptions0.tmp");

	if (!o.is_open())
		throw EXCEPTION("testoptions0.tmp");

	o << "--a 4";
	o.close();

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("@testoptions0.tmp");

	std::wcout << L"test5: "
		  << p->validate()
		  << std::endl;
	unlink("testoptions0.tmp");
	std::wcout << "A: " << a_value->value << std::endl;
}

static void testoptions6()
{
	LIBCXX_NAMESPACE::option::int_value a_value(LIBCXX_NAMESPACE::option::int_value::create());
	LIBCXX_NAMESPACE::option::int_value b_value(LIBCXX_NAMESPACE::option::int_value::create());
	LIBCXX_NAMESPACE::option::int_value c_value(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName(L"testoptions");

	options->add(a_value,
		     0, L"a",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue | LIBCXX_NAMESPACE::option::list::base::required,
		     L"A parameter", L"avalue")
		.add(b_value,
		     'b', L"",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue | LIBCXX_NAMESPACE::option::list::base::required,
		     L"B parameter", L"bvalue")
		.add(c_value,
		     0, L"c",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"C parameter")
		.addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--a=1");

	std::wcout << L"test6: "
		   << p->validate() << L": ";

	std::wcout << LIBCXX_NAMESPACE::towstring(p->err_option,
						LIBCXX_NAMESPACE::locale::create())
		   << std::endl;
	std::wcout << "A: " << a_value->value << std::endl;
	std::wcout << "B: " << b_value->value << std::endl;
	std::wcout << "C: " << c_value->value << std::endl;
	p->setOptions(options);
	p->parseString("--help=40");
	std::wcout << L"test6.2: "
		   << p->validate() << std::endl;
	p->setOptions(options);
	p->parseString("--usage=40");
	std::wcout << L"test6.3: "
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
			      0, L"add", 0, L"Add a sprocket")
		.add(del_sprocket,
		     0, L"del", 0, L"Delete a sprocket")
		.add(grok_sprocket,
		     0, L"grok", 0, L"Grok a sprocket")
		.add(frob_sprocket,
		     0, L"from", 0, L"From a sprocket");

	LIBCXX_NAMESPACE::option::list spricket_options(LIBCXX_NAMESPACE::option::list::create());

	spricket_options->add(add_spricket,
			      0, L"add", 0, L"Add a spricket")
		.add(del_spricket,
		     0, L"del", 0, L"Delete a spricket");

	LIBCXX_NAMESPACE::option::int_value do_groz(LIBCXX_NAMESPACE::option::int_value::create());

	LIBCXX_NAMESPACE::option::list groz_options(LIBCXX_NAMESPACE::option::list::create());

	groz_options->addArgument(L"grozA", 0)
		.addArgument(L"grozB", 0);

	LIBCXX_NAMESPACE::option::list main_options(LIBCXX_NAMESPACE::option::list::create());

	main_options->setAppName("testoptions");

	main_options->add(do_sprockets, 0, L"sprockets", sprocket_options,
		     L"Do something with sprockets")
		.add(do_sprickets, 0, L"sprickets", spricket_options,
		     L"Do something with sprickets")
		.add(do_groz, 0, L"groz", groz_options,
		     L"Do something with grozes")
		.add(do_gadget, 'g', L"", 0, L"Dummy flag")
		.addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser opt_parser(LIBCXX_NAMESPACE::option::parser::create());

	opt_parser->setOptions(main_options);

	opt_parser->parseString("--help=40");
	opt_parser->validate();
	opt_parser->reset();
	opt_parser->parseString("--usage=40");
	opt_parser->validate();
	opt_parser->reset();

	std::wcout << "Status before: " << STATUS_ALL << std::endl;

	if (opt_parser->parseString("--sprickets --del") == 0 &&
	    opt_parser->validate() == 0)
	{
		std::wcout << "Status after --sprickets --del: "
			   << STATUS_ALL << std::endl;
	}

	opt_parser->reset();
	std::wcout << "Status before: " << STATUS_ALL << std::endl;

	if (opt_parser->parseString("--sprockets --add") == 0 &&
	    opt_parser->validate() == 0)
	{
		std::wcout << "Status after --spoickets --add: "
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

	opts->setAppName(L"testoptions");

	opts->add(a, 'a', L"", 0, L"Option A")
		.add(b, 'b', L"", 0, L"Option B");

	LIBCXX_NAMESPACE::option::parser opt_parser(LIBCXX_NAMESPACE::option::parser::create());

	opt_parser->setOptions(opts);

	opt_parser->parseString("-a");
	std::wcout << L"Mutex test 1: " << opt_parser->validate()
		   << L", a=" << a->value
		   << L", b=" << b->value << std::endl;

	opt_parser->reset();

	opt_parser->parseString("-b");
	std::wcout << L"Mutex test 2: " << opt_parser->validate()
		   << L", a=" << a->value
		   << L", b=" << b->value << std::endl;

	opt_parser->reset();
	opt_parser->parseString("-a -b");

	std::wcout << L"Mutex test 2: " << opt_parser->validate()
		   << L", a=" << a->value
		   << L", b=" << b->value << std::endl;

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

	opts->setAppName(L"testoptions");

	opts->add(hms_value, 't', L"",
		  LIBCXX_NAMESPACE::option::list::base::hasvalue,
		  L"Time")
		.add(ymd_value, 'd', L"",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, L"Date")
		.add(ymd_interval_value, 'i', L"",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, L"Interval")
		.add(uri_value, 'u', L"",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, L"URI");

	LIBCXX_NAMESPACE::option::parser opt_parser(LIBCXX_NAMESPACE::option::parser::create());

	opt_parser->setOptions(opts);

	opt_parser->parseString("-t \"1 hour\" -d 2010-05-01 -i \"1 week\""
				" -u http://localhost/cgi-bin/test.cgi");

	std::ostreambuf_iterator<wchar_t> o(std::wcout.rdbuf());

	std::wcout << "Time: ";

	hms_value->value.toString(o);

	std::wcout << std::endl << "Date: ";
	
	ymd_value->value.toWideString(o);

	std::wcout << std::endl << "Interval: ";

	auto w=LIBCXX_NAMESPACE::towstring(ymd_interval_value->value);

	std::copy(w.begin(), w.end(), o);

	std::wcout << std::endl << "URI path: ";

	{
		std::wstring w;
		const std::string &s=uri_value->value.getPath();

		std::copy(s.begin(), s.end(),
			  std::back_insert_iterator<std::wstring>(w));

		std::wcout << w << std::endl;
	}
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

	opts->setAppName(L"testoptions");

	opts->add(hms_list, 't', L"",
		  LIBCXX_NAMESPACE::option::list::base::hasvalue,
		  L"Time")
		.add(ymd_list, 'd', L"",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, L"Date")
		.add(ymd_interval_list, 'i', L"",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, L"Interval")
		.add(uri_list, 'u', L"",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue, L"URI");

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

	options->setAppName(L"testoptions");

	options->add(memsize, L's', L"memsize",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Memory size",
		     L"size");

	options->usage(std::wcout, 40);
	options->help(std::wcout, 40);

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--memsize=4KB");

	std::wcout << L"bytes: " << memsize->value.bytes << std::endl;
}

static void testoptions12()
{
	LIBCXX_NAMESPACE::option::wstring_value
		wstring=LIBCXX_NAMESPACE::option::wstring_value::create();

	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	options->setAppName(L"testoptions");

	options->add(wstring, L's', L"wstring",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Wide string",
		     L"string");

	options->usage(std::wcout, 40);
	options->help(std::wcout, 40);

	LIBCXX_NAMESPACE::option::parser p(LIBCXX_NAMESPACE::option::parser::create());

	p->setOptions(options);

	p->parseString("--wstring=foo");

	if (wstring->value != L"foo")
		throw EXCEPTION("test12 failed");
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
		testoptions12();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::wcout << LIBCXX_NAMESPACE::towstring((std::string)*e,
							LIBCXX_NAMESPACE::locale::create()) << std::endl;
	}
	return 0;
}
