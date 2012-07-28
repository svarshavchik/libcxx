/*
** Copyright 2012 Double Precision, Inc.
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

	std::string appName=pidinfo().exe;

	std::string::iterator b=appName.begin(), e=appName.end(), p=b;

	while (b != e)
	{
		if (*b++ == '/')
			p=b;
	}

	setAppName(std::string(p, e));
}

listObj::~listObj() noexcept
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

void listObj::setAppName(const std::wstring &applicationNameArg)
{
	applicationName=applicationNameArg;
}

void listObj::setAppName(const std::string &applicationNameArg,
			 const const_locale &l)
{
	applicationName=towstring(applicationNameArg, l);
}

listObj &listObj::add (const ref<valuebaseObj> &valueArg,
		       wchar_t shortnameArg,
		       const std::wstring &longnameArg,
		       int flagsArg,
		       const std::wstring &descriptionArg,
		       const std::wstring &argDescriptionArg)
{
	auto newDef(ref<definitionObj>::create(valueArg, shortnameArg,
					       longnameArg, flagsArg,
					       descriptionArg,
					       argDescriptionArg));

	options.insert(first_group_option, newDef);
	return *this;
}

listObj &listObj::add(int (*optfunc)(),
		      wchar_t shortnameArg,
		      const std::wstring &longnameArg,
		      int flagsArg,
		      const std::wstring &descriptionArg,
		      const std::wstring &argDescriptionArg)
{
	options.insert(first_group_option,
		       ref<definitionfuncvoidObj>
		       ::create(shortnameArg, longnameArg, flagsArg,
				descriptionArg, argDescriptionArg,
				optfunc));
	return *this;
}

listObj &listObj::add(const ref<valuebaseObj> &valueArg,
		      wchar_t shortnameArg,
		      const std::wstring &longnameArg,
		      const list &groupOptionsArg,
		      const std::wstring &descriptionArg,
		      const std::wstring &argDescriptionArg)
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

listObj &listObj::addDefaultOptions(std::wostream &o)
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

listObj &listObj::addArgument(const std::wstring &nameArg,
			      int flags)
{
	std::wstring name(nameArg);

	if (flags & option::list::base::repeated)
		name += L"...";

	if (!(flags & option::list::base::required))
		name = L"[" + name + L"]";

	arguments.push_back(name);
	return *this;
}

// Generate a usage message

void listObj::usage(std::wostream &o, size_t width)
{
	if (width == 0)
		width=getTtyWidth();

	std::wstring p=towstring(_("Usage: "),
				 locale::base::environment()) + applicationName;

	usage_internal(o, p, width, 16);
}

void listObj::usage_internal(std::wostream &o,
			     const std::wstring &pArg,
			     size_t width, size_t indent)
{
	std::wstring p(pArg);

	size_t w=p.size();

	int n=wcswidth(p.c_str(), p.size());

	if (n > 0)
		w=n;
	o << p;

	std::list<ref<definitionbaseObj> >::const_iterator
		b=options.begin(),
		e=options.end();

	for (; b != e; ++b)
	{
		std::wstringstream descr;
		bool flag=(*b)->usage(descr, indent, width);

		if (!flag)
		{
			bool nospc=false;

			p=descr.str();

			if (p.size() == 0)
				continue;

			if (w == 0)
			{
				o << std::setw(indent) << ""
				  << std::setw(0);
				w += indent;
				nospc=true;
			}

			size_t nw=p.size();
			int nn=wcswidth(p.c_str(), p.size());

			if (nn > 0)
				nw=nn;

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
				o << L' ';
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

	std::list<std::wstring>::iterator argb=arguments.begin(),
		arge=arguments.end();

	while (argb != arge)
	{
		std::wstring &p= *argb++;

		size_t nw=p.size();
		int nn=wcswidth(p.c_str(), p.size());

		if (nn > 0)
			nw=nn;

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
			if (w) o << L' ';
			w += nw;
		}
		o << p;
	}

	if (w)
		o << std::endl;
}

//! Generate a help message

void listObj::help(std::wostream &o, size_t width)
{
	if (width == 0)
		width=getTtyWidth();

	locale l(locale::base::environment());

	o << towstring(_("Usage: "), l) << applicationName
	  << towstring(_(" [OPTION]..."), l)
	  << std::endl;

	help_internal(o, width, 2);
}

void listObj::help_internal(std::wostream &o,
			    size_t width,
			    size_t indent)
{
	std::list<ref<definitionbaseObj> >::iterator
		b=options.begin(),
		e=options.end();

	for (; b != e; ++b)
	{
		(*b)->help(o, indent, width);
	}
}


size_t listObj::getTtyWidth()
{
	try {
		struct winsize ws;
		fd ttyfd(fd::base::open("/dev/tty", O_RDWR));

		if (ioctl(ttyfd->getFd(), TIOCGWINSZ, &ws) == 0)
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
