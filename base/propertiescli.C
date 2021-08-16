/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/options.H"
#include "x/locale.H"
#include "x/fd.H"
#include "x/fileattr.H"
#include "propertiescli.h"
#include "gettext_in.h"

static int main2(int argc, char **argv)
{
	auto locale=LIBCXX_NAMESPACE::locale::base::environment();
	auto msgs=LIBCXX_NAMESPACE::messages::create(LIBCXX_DOMAIN, locale);

	propertiescli args{msgs};
	std::list<std::string> files(args.parse(argc, argv, locale)->args);

	if (args.setvalue->is_set())
	{
		if (args.setvalue->value.substr(0, 1) != "/")
			args.setvalue->value = x::fd::base::cwd() + "/"
				+ args.setvalue->value;

		if (!args.nocheckset->value)
			LIBCXX_NAMESPACE::fileattr
				::create(args.setvalue->value)->stat();

	}

	for (std::list<std::string>::const_iterator b=files.begin(),
		     e=files.end(); b != e; ++b)
	{
		try {
			LIBCXX_NAMESPACE::fileattr
				attr(LIBCXX_NAMESPACE::fileattr::create(*b)
				     );

			if (args.clearvalue->is_set())
			{
				attr->removeattr("user.properties");
				std::cout << LIBCXX_NAMESPACE::gettextmsg
					(msgs->get
					 (_txt("%1%: properties removed")),
					 *b)
					  << std::endl;
				continue;
			}

			if (args.setvalue->is_set())
			{
				attr->setattr("user.properties",
					      args.setvalue->value);
				std::cout << LIBCXX_NAMESPACE::gettextmsg
					(msgs->get
					 (_txt("%1%: properties set to %2%")),
					 *b,args.setvalue->value)
					  << std::endl;
				continue;
			}

			std::set<std::string> attrs=attr->listattr();

			std::set<std::string>::const_iterator
				p=attrs.find("user.properties");

			if (p == attrs.end())
				std::cout << LIBCXX_NAMESPACE::gettextmsg
					(msgs->get
					 (_txt("%1%: no properties set")), *b)
					  << std::endl;
			else
				std::cout << *b << ": "
					  << attr->getattr("user.properties")
					  << std::endl;

		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cout << *b << ": " << e << std::endl;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	int rc;

	try {
		rc=main2(argc, argv);
	} catch (const x::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}

	exit(rc);
	return (0);
}
