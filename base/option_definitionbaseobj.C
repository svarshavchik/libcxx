/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "option_list.H"
#include "option_valuebaseobj.H"

#include <iomanip>
#include <sstream>
#include <vector>

#include "option_definitionbaseobj.H"
#include "option_definitionfunctionobj.H"

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
	};
};
#endif

definitionbaseObj::definitionbaseObj(wchar_t shortnameArg,
				     const std::wstring &longnameArg,
				     int flagsArg,
				     const std::wstring &descriptionArg,
				     const std::wstring &argDescriptionArg)
	noexcept
	: shortname(shortnameArg),
	  longname(longnameArg),
	  flags(flagsArg),
	  description(descriptionArg),
	  argDescription(argDescriptionArg)
{
}

//! Default destructor

definitionbaseObj::~definitionbaseObj() noexcept
{
}

bool definitionbaseObj::multiple() const
{
	return false;
}

bool definitionbaseObj::usage(std::wostream &o,
			      size_t indentlevel,
			      size_t width) const
{
	if (description.size() == 0)
		return false;

	if (!(flags & option::list::base::required))
		o << L'[';

	if (shortname)
		o << L'-' << shortname;

	if (shortname && longname.size() > 0)
		o << L'|';

	if (longname.size() > 0)
		o << L"--" << longname;

	if (flags & option::list::base::hasvalue)
		o << L'='
		  << (argDescription.size() > 0 ? argDescription:L"value");

	if (!(flags & option::list::base::required))
		o << L']';
	if (multiple())
		o << L"...";
	return false;
}

void definitionbaseObj::help(std::wostream &o,
			     size_t indentlevel,
			     size_t width) const
{
	if (description.size() == 0)
		return;

	std::wstring optname;

	{
		std::wostringstream optname_s;

		optname_s << std::setw(indentlevel) << L"";

		if (shortname)
			optname_s << L'-' << shortname;

		if (shortname && longname.size() > 0)
			optname_s << L", ";

		if (longname.size() > 0)
			optname_s << L"--" << longname;

		if (flags & option::list::base::hasvalue)
			optname_s << L'='
				  << (argDescription.size() > 0
				      ? argDescription:L"value");
		optname=optname_s.str();
	}

	size_t descrColumn=width/2;

	// Technically, we can't pass a ptr to somewhere in a wstring to
	// wcwidth, since wstring may not necessary hold its contents in a
	// single buffer.

	std::vector<wchar_t> descriptionBuf;

	descriptionBuf.resize(description.size());
	std::copy(description.begin(), description.end(),
		  descriptionBuf.begin());

	std::vector<wchar_t>::iterator b=descriptionBuf.begin(),
		e=descriptionBuf.end(), p, q;

	bool shown_option=false;

	while (b != e)
	{
		if (iswspace(*b))
		{
			++b;
			continue;
		}

		p=q=b;

		while (1)
		{
			if (b == e || iswspace(*b))
			{
				if (p == q)
					q=b;

				size_t w=b-p;

				int wint=wcswidth(&*p, b-p);

				if (wint > 0)
					w=(size_t)wint;

				if (w >= width-descrColumn)
					break;
				q=b;
			}

			if (b == e)
				break;
			++b;
		}

		if (!shown_option)
		{
			size_t optW=optname.size();
			int optWn=wcswidth(optname.c_str(), optname.size());

			if (optWn > 0)
				optW=(size_t)optWn;


			if (optW >= descrColumn)
			{
				o << optname << std::endl
				  << std::setw(descrColumn) << L""
				  << std::setw(0);
			}
			else
			{
				o << optname
				  << std::setw(descrColumn-optW) << L""
				  << std::setw(0);
			}
			shown_option=true;
		}
		else
		{
			o << std::setw(descrColumn) << L""
			  << std::setw(0);
		}
		o << std::wstring(p, q) << std::endl;
		b=q;
	}

	if (!shown_option)
		o << optname << std::endl;
}

template<>
int definitionfuncbaseObj<int (*)(bool), bool>::set(parserObj &obj)
	const
{
	int rc=set_explicit(true);

	if (rc == 0)
		was_set=true;

	return rc;
}

template<>
int definitionfuncbaseObj<int (*)(const bool &), bool>::set(parserObj &obj)
	const
{
	int rc=set_explicit(true);

	if (rc == 0)
		was_set=true;

	return rc;
}

definitionfuncvoidObj
::definitionfuncvoidObj(wchar_t shortnameArg,
			const std::wstring &longnameArg,
			int flagsArg,
			const std::wstring &descriptionArg,
			const std::wstring &argDescriptionArg,
			int (*optfuncvalArg)(void)) noexcept
	: definitionfuncbaseObj<int (*)(void), bool> (shortnameArg,
						      longnameArg,
						      flagsArg,
						      descriptionArg,
						      argDescriptionArg,
						      optfuncvalArg)
{
}

definitionfuncvoidObj::~definitionfuncvoidObj() noexcept
{
}

int definitionfuncvoidObj::set_default() const
{
	return (*optfunc)();
}

int definitionfuncvoidObj::set_explicit(const bool &arg)
	const
{
	if (arg)
		return (*optfunc)();

	return option::parser::base::err_invalidoption;
}

#if 0
{
	{
#endif
	}
}
