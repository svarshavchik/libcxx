/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/idn.H"
#include "x/property_value.H"
#include "x/hier.H"
#include "x/strtok.H"
#include "x/join.H"
#include "x/chrcasecmp.H"
#include "x/singleton.H"
#include "x/http/cookiejar.H"
#include <fstream>
#include <cstring>
#include <algorithm>
#include "sysconfdir.h"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

static property::value<std::string>
effective_tld_names_filename(LIBCXX_NAMESPACE_WSTR
			     L"::http::effective_tld_names",
			     SYSCONFDIR "/effective_tld_names.dat");

class LIBCXX_HIDDEN tldnamesObj : virtual public obj {

public:
	class valueObj : virtual public obj {

	public:
		bool exceptionrule;

		valueObj(bool exceptionruleArg)
			: exceptionrule(exceptionruleArg)
		{
		}

		~valueObj() noexcept {}
	};

	typedef ref<valueObj> value;

	typedef hier<std::string, value> tld_t;

	tld_t tld;

	~tldnamesObj() noexcept {}

	tldnamesObj() : tld(load_tlds(locale::base::utf8()))
	{
	}

	static tld_t load_tlds(const locale &utf8)
	{
		try {
			return init(utf8);
		} catch (const exception &e)
		{
			return tld_t::create();
		}
	}

	static tld_t init(const locale &utf8)
	{
		tld_t tld=tld_t::create();

		std::ifstream i(effective_tld_names_filename.getValue()
				.c_str());

		while (i.is_open() && !i.eof())
		{
			std::string s;

			// Each line is only read up to the first whitespace;
			// entire lines can also be commented using //. 
			// Each line which is not entirely whitespace or begins
			// with a comment contains a rule. 

			std::getline(i, s);

			s=s.substr(0, s.find("//"));

			s=std::string(s.begin(),
				      std::find_if(s.begin(), s.end(),
						   []
						   (char c)
				{
					return strchr(" \t\r\n", c) != NULL;
				}));

			if (s.size() == 0)
				continue;

			bool exceptionrule=false;

			if (*s.begin() == '!')
			{
				exceptionrule=true;
				s=s.substr(1);
			}

			std::transform(s.begin(),
				       s.end(),
				       s.begin(),
				       chrcasecmp::tolower);

			std::list<std::string> labels_rev;

			{
				std::list<std::string> labels;

				strtok_str(s, ".", labels);

				// Reverse them

				std::copy(labels.begin(), labels.end(),
					  std::front_insert_iterator
					  <std::list<std::string>>(labels_rev)
					  );
			}
			insert(tld, labels_rev, exceptionrule);
		}

		std::list<std::string> wildcard;

		wildcard.push_back("*");
		insert(tld, wildcard, false);
		return tld;
	}

	static void insert(const tld_t &tld,
			   const std::list<std::string> &labels_rev,
			   bool exceptionrule)
	{
		tld->insert([exceptionrule]
			    {
				    return value::create(exceptionrule);
			    },
			    labels_rev,
			    []
			    (const value &dummy)
			    {
				    return false;
			    });
	}

	std::string public_suffix(const std::string &domain)
	{
		std::list<std::string> labels;

		try {
			std::string s;

			if (domain.substr(0, 1) == ".")
				return s;

			s.reserve(domain.size());

			std::transform(domain.begin(),
				       domain.end(),
				       std::back_insert_iterator<std::string>
				       (s),
				       chrcasecmp::tolower);

			strtok_str(s, ".", labels);
		} catch (...)
		{
			return "";
		}

		std::list<std::string>::const_iterator b=labels.begin(),
			e=labels.end(), p=e, q=e;

		auto lock=tld->create_readlock();

		while (b != p)
		{
			--p;

			if (!lock->to_child(*p) && !lock->to_child("*"))
				break;

			auto entry=lock->entry();

			if (!entry.null())
			{
				if (entry->exceptionrule)
				{
					q=p;
				}
				else
				{
					if (b != p)
					{
						q=p;
						--q;
					}
					else
					{
						q=e;
					}
				}
			}
		}

		std::string s;

		join(q, e, ".",
		     std::back_insert_iterator<std::string>(s));
		return s;
	}
};

static singleton<tldnamesObj> tld_singleton;

std::string cookiejarBase::public_suffix(const std::string &domain)
{
	return tld_singleton.get()->public_suffix(domain);
}

#if 0
{
	{
#endif
	}
}
