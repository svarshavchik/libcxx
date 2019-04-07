/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_list.H"
#include "x/option_valuebaseobj.H"

#include <iomanip>
#include <sstream>
#include <vector>

#include "x/option_definitionbaseobj.H"
#include "x/option_definitionfunctionobj.H"

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
	};
};
#endif

definitionbaseObj::definitionbaseObj(char32_t shortnameArg,
				     const std::string &longnameArg,
				     int flagsArg,
				     const std::string &descriptionArg,
				     const std::string &argDescriptionArg)
	noexcept
	: shortname(shortnameArg),
	  longname(longnameArg),
	  flags(flagsArg),
	  description(descriptionArg),
	  argDescription(argDescriptionArg)
{
}

//! Default destructor

definitionbaseObj::~definitionbaseObj()
{
}

bool definitionbaseObj::multiple() const
{
	return false;
}

bool definitionbaseObj::usage(std::ostream &o,
			      size_t indentlevel,
			      size_t width) const
{
	if (description.size() == 0)
		return false;

	if (!(flags & option::list::base::required))
		o << '[';

	std::u32string shortname_v;

	shortname_v.push_back('-');
	shortname_v.push_back(shortname);

	if (shortname)
		o << unicode::iconvert::fromu::convert(shortname_v,
						       unicode::utf_8)
			.first;

	if (shortname && longname.size() > 0)
		o << '|';

	if (longname.size() > 0)
		o << "--" << longname;

	if (flags & option::list::base::hasvalue)
		o << "=" << (argDescription.size() > 0
			     ? argDescription:"value");

	if (!(flags & option::list::base::required))
		o << ']';
	if (multiple())
		o << "...";
	return false;
}

void definitionbaseObj::help(std::ostream &o,
			     size_t indentlevel,
			     size_t width) const
{
	if (description.size() == 0)
		return;

	std::string optname;

	{
		std::ostringstream optname_s;

		optname_s << std::setw(indentlevel) << "";

		std::u32string shortname_v;

		shortname_v.push_back('-');
		shortname_v.push_back(shortname);

		if (shortname)
			optname_s << unicode::iconvert::fromu
				::convert(shortname_v,
					  unicode::utf_8)
				.first;

		if (shortname && longname.size() > 0)
			optname_s << ", ";

		if (longname.size() > 0)
			optname_s << "--" << longname;

		if (flags & option::list::base::hasvalue)
			optname_s << "="
				  << (argDescription.size() > 0
				      ? argDescription:"value");
		optname=optname_s.str();
	}

	size_t descrColumn=width/2;

	// Convert to unicode.

	auto descriptionBuf=
		unicode::iconvert::tou::convert(description,
						unicode::utf_8).first;

	std::u32string::iterator b=descriptionBuf.begin(),
		e=descriptionBuf.end(), p, q;

	bool shown_option=false;

	auto optname_u=
		unicode::iconvert::tou::convert(optname, unicode::utf_8).first;

	while (b != e)
	{
		if (unicode_isspace(*b))
		{
			++b;
			continue;
		}

		p=q=b;

		while (1)
		{
			if (b == e || unicode_isspace(*b))
			{
				if (p == q)
					q=b; // At least one word.

				size_t w=0;

				for (auto ptr=p; ptr != b; ++ptr)
					w += unicode_wcwidth(*ptr);

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
			size_t optW=0;

			for (auto c:optname_u)
				optW += unicode_wcwidth(c);

			o << unicode::iconvert::fromu
				::convert(optname_u,
					  unicode::utf_8)
				.first;

			if (optW >= descrColumn)
			{
				o << std::endl
				  << std::setw(descrColumn) << ""
				  << std::setw(0);
			}
			else
			{
				o << std::setw(descrColumn-optW) << ""
				  << std::setw(0);
			}
			shown_option=true;
		}
		else
		{
			o << std::setw(descrColumn) << ""
			  << std::setw(0);
		}
		o << std::string(p, q) << std::endl;
		b=q;
	}

	if (!shown_option)
		o << unicode::iconvert::fromu
			::convert(optname_u, unicode::utf_8).first
		  << std::endl;
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
::definitionfuncvoidObj(char32_t shortnameArg,
			const std::string &longnameArg,
			int flagsArg,
			const std::string &descriptionArg,
			const std::string &argDescriptionArg,
			int (*optfuncvalArg)(void)) noexcept
	: definitionfuncbaseObj<int (*)(void), bool> (shortnameArg,
						      longnameArg,
						      flagsArg,
						      descriptionArg,
						      argDescriptionArg,
						      optfuncvalArg)
{
}

definitionfuncvoidObj::~definitionfuncvoidObj()
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
