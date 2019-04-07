/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_list.H"
#include "x/fileattr.H"
#include "x/fd.H"
#include "x/option_definitionhelpobj.H"
#include "x/option_definitionusageobj.H"
#include "x/option_definitionpropertiesobj.H"
#include "x/option_definitionsetpropertyobj.H"
#include "x/option_groupdefinitionobj.H"
#include "x/tostring.H"
#include "x/pidinfo.H"
#include "gettext_in.h"
#include <sys/ioctl.h>
#include <cwchar>
#include <iomanip>

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
	};
};
#endif

listObj::listObj()
{
	first_group_option=options.end();

	std::string appName=exename();

	std::string::iterator b=appName.begin(), e=appName.end(), p=b;

	while (b != e)
	{
		if (*b++ == '/')
			p=b;
	}

	setAppName(std::string(p, e));
}

listObj::~listObj()
{
}

void listObj::reset()
{
	std::list<ref<definitionbaseObj> >::iterator
		b=options.begin(), e=options.end();

	while (b != e)
	{
		(*b)->reset();
		++b;
	}
}

void listObj::setAppName(const std::string &applicationNameArg)
{
	applicationName=unicode::iconvert::convert(applicationNameArg,
						   unicode_default_chset(),
						   unicode::utf_8);
}

listObj &listObj::add (const ref<valuebaseObj> &valueArg,
		       char32_t shortnameArg,
		       const std::string &longnameArg,
		       int flagsArg,
		       const std::string &descriptionArg,
		       const std::string &argDescriptionArg)
{
	auto newDef(ref<definitionObj>::create(valueArg, shortnameArg,
					       longnameArg, flagsArg,
					       descriptionArg,
					       argDescriptionArg));

	options.insert(first_group_option, newDef);
	return *this;
}

listObj &listObj::add(int (*optfunc)(),
		      char32_t shortnameArg,
		      const std::string &longnameArg,
		      int flagsArg,
		      const std::string &descriptionArg,
		      const std::string &argDescriptionArg)
{
	options.insert(first_group_option,
		       ref<definitionfuncvoidObj>
		       ::create(shortnameArg, longnameArg, flagsArg,
				descriptionArg, argDescriptionArg,
				optfunc));
	return *this;
}

listObj &listObj::add(const ref<valuebaseObj> &valueArg,
		      char32_t shortnameArg,
		      const std::string &longnameArg,
		      const list &groupOptionsArg,
		      const std::string &descriptionArg,
		      const std::string &argDescriptionArg)
{
	options.push_back(ref<groupdefinitionObj>
			  ::create(valueArg, shortnameArg,
				   longnameArg, groupOptionsArg,
				   descriptionArg,
				   argDescriptionArg));

	if (first_group_option == options.end())
		--first_group_option;

	return *this;
}

listObj &listObj::addDefaultOptions(std::ostream &o)
{
	options.insert(first_group_option,
		       ref<definitionPropertiesObj>::create(o));
	options.insert(first_group_option,
		       ref<definitionSetPropertyObj>::create());

	options.insert(first_group_option,
		       ref<definitionHelpObj>::create(o));

	options.insert(first_group_option,
		       ref<definitionUsageObj>::create(o));
	return *this;
}

listObj &listObj::addArgument(const std::string &nameArg,
			      int flags)
{
	auto name=nameArg;

	if (flags & option::list::base::repeated)
		name += "...";

	if (!(flags & option::list::base::required))
		name = "[" + name + "]";

	arguments.push_back(name);
	return *this;
}

char32_t listObj::option_unicodechar(const std::string &shortName)
{
	auto ubuf=unicode::iconvert::tou
		::convert(shortName, unicode::utf_8).first;
	ubuf.push_back(0);

	return *ubuf.begin();
}

// Generate a usage message

void listObj::usage(std::ostream &o, size_t width)
{
	if (width == 0)
		width=getTtyWidth();

	auto p=_("Usage: ") + applicationName;

	usage_internal(o, p, width, 16);
}

void listObj::usage_internal(std::ostream &output,
			     const std::string &pArg,
			     size_t width, size_t indent)
{
	std::ostringstream o; // UTF-8 collected_here.

	size_t w=0;

	auto p_name=unicode::iconvert::tou::convert(pArg,
						    unicode::utf_8).first;

	for (auto c:p_name)
		w += unicode_wcwidth(c);

	o << pArg;

	std::list<ref<definitionbaseObj> >::const_iterator
		b=options.begin(),
		e=options.end();

	for (; b != e; ++b)
	{
		std::stringstream descr;
		bool flag=(*b)->usage(descr, indent, width);

		if (!flag)
		{
			bool nospc=false;

			auto p=descr.str();

			if (p.size() == 0)
				continue;

			if (w == 0)
			{
				o << std::setw(indent) << ""
				  << std::setw(0);
				w += indent;
				nospc=true;
			}

			size_t nw=0;

			{
				auto descr_u=unicode::iconvert::tou
					::convert(p, unicode::utf_8).first;

				for (auto c:descr_u)
					nw += unicode_wcwidth(c);
			}

			if (w + nw + (w?1:0) >= width)
			{
				o << std::endl
				  << std::setw(indent) << ""
				  << std::setw(0) << p;
				w=nw + indent;
				continue;
			}

			if (!nospc)
			{
				o << ' ';
				++w;
			}

			o << p;
			w += nw;
			continue;
		}

		if (w > 0)
			o << std::endl;

		o << std::setw(indent) << ""
		  << std::setw(0);

		o << descr.rdbuf();
		w=0;
	}

	auto argb=arguments.begin(), arge=arguments.end();

	while (argb != arge)
	{
		size_t nw=0;

		std::string &p= *argb++;

		auto p_u=unicode::iconvert::tou::convert(p,
							 unicode::utf_8).first;

		for (auto c:p_u)
			nw += unicode_wcwidth(c);

		if (w == 0)
		{
			o << std::setw(indent) << ""
			  << std::setw(0) << p;
			w += indent;
			w += nw;
			continue;
		}

		if (w)
			++nw;

		if (w + nw >= width && w)
		{
			--nw;
			o << std::endl
			  << std::setw(indent) << ""
			  << std::setw(0);
			w=indent + nw;
		}
		else
		{
			if (w) o << ' ';
			w += nw;
		}
		o << p;
	}

	if (w)
		o << std::endl;

	output << unicode::iconvert::convert(o.str(),
					     unicode::utf_8,
					     unicode_default_chset());
}

//! Generate a help message

void listObj::help(std::ostream &o, size_t width)
{
	if (width == 0)
		width=getTtyWidth();

	o << _("Usage: ") << applicationName
	  << _(" [OPTION]...")
	  << std::endl;

	help_internal(o, width, 2);
}

void listObj::help_internal(std::ostream &output,
			    size_t width,
			    size_t indent)
{
	std::ostringstream o;

	std::list<ref<definitionbaseObj> >::iterator
		b=options.begin(),
		e=options.end();

	for (; b != e; ++b)
	{
		(*b)->help(o, indent, width);
	}

	output << unicode::iconvert::convert(o.str(),
					     unicode::utf_8,
					     unicode_default_chset());
}


size_t listObj::getTtyWidth()
{
	try {
		struct winsize ws;
		fd ttyfd(fd::base::open("/dev/tty", O_RDWR));

		if (ioctl(ttyfd->get_fd(), TIOCGWINSZ, &ws) == 0)
			return ws.ws_col;
	} catch (...)
	{
	}
	return 80;
}

#if 0
{
	{
#endif
	}
}
