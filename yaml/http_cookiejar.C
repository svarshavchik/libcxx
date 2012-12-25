/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fd.H"
#include "x/fditer.H"
#include "x/yaml/writer.H"
#include "x/yaml/newsequencenode.H"
#include "x/yaml/newmappingnode.H"
#include "x/yaml/newscalarnode.H"
#include "x/yaml/document.H"
#include "x/yaml/parser.H"
#include "x/yaml/scalarnode.H"
#include "x/yaml/mappingnode.H"
#include "x/yaml/sequencenode.H"
#include "x/http/cookiejar.H"
#include "x/http/cookie.H"
#include "x/exception.H"
#include "x/sysexception.H"

#include <iterator>
#include <fstream>

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	}
};
#endif

#pragma GCC visibility push(hidden)

// Save a cookie as a YAML mapping

static inline yaml::newnode save_cookie(const cookie &c)
{
	return yaml::newmapping::create
		([c]
		 (yaml::newmapping &map)
		 {
			 std::list<std::pair<yaml::newscalarnode,
					     yaml::newscalarnode> > m;

			 m.push_back(std::make_pair(yaml::newscalarnode
						    ::create("name"),
						    yaml::newscalarnode
						    ::create(c.name)));
			 m.push_back(std::make_pair(yaml::newscalarnode
						    ::create("value"),
						    yaml::newscalarnode
						    ::create(c.value)));
			 m.push_back(std::make_pair(yaml::newscalarnode
						    ::create("expires"),
						    yaml::newscalarnode
						    ::create
						    (c.expires_as_string())));
			 m.push_back(std::make_pair(yaml::newscalarnode
						    ::create("domain"),
						    yaml::newscalarnode
						    ::create(c.domain)));
			 m.push_back(std::make_pair(yaml::newscalarnode
						    ::create("path"),
						    yaml::newscalarnode
						    ::create(c.path)));
			 if (c.secure)
				 m.push_back(std::make_pair(yaml::newscalarnode
							    ::create("secure"),
							    yaml::newscalarnode
							    ::create("1")));
			 if (c.httponly)
				 m.push_back(std::make_pair(yaml::newscalarnode
							    ::create("httponly")
							    ,
							    yaml::newscalarnode
							    ::create("1")));
			 map(m.begin(), m.end());
		 });
}

// Iterate over cookies in a jar, to define a YAML sequence.

class save_iterator : public std::iterator<std::input_iterator_tag,
					   yaml::newnode> {

public:
	time_t now;

	cookiejar::base::iterator cur_iter, end_iter;

	yaml::newnodeptr cur_node;

	save_iterator(cookiejar::base::iterator cur_iterArg,
		      cookiejar::base::iterator end_iterArg)
		: now(time(NULL)),
		  cur_iter(cur_iterArg), end_iter(end_iterArg)
	{
		nextcookie();
	}

	void nextcookie()
	{
		// Ignore session cookies, and expired cookies.

		while (cur_iter != end_iter)
		{
			auto cur_cookie= *cur_iter;

			if (cur_cookie.has_expiration() &&
			    cur_cookie.expiration > now)
			{
				cur_node=save_cookie(cur_cookie);
				break;
			}
			++cur_iter;
		}
	}

	save_iterator &operator++()
	{
		++cur_iter;
		nextcookie();
		return *this;
	}

	save_iterator operator++(int)
	{
		save_iterator copy(end_iter, end_iter);

		copy.cur_node=cur_node;
		operator++();
		return copy;
	}

	yaml::newnode operator*() const
	{
		return cur_node;
	}

	bool operator==(const save_iterator &o) const
	{
		return cur_iter == end_iter && o.cur_iter == o.end_iter;
	}

	bool operator!=(const save_iterator &o) const
	{
		return !operator==(o);
	}
};

#pragma GCC visibility pop

void cookiejarObj::save(const std::string &filename)
{
	auto fd=fd::create(filename);

#pragma GCC visibility push(hidden)

	ref<cookiejarObj> jar(this);

	yaml::writer::write_one_document
		(yaml::make_newdocumentnode
		 ([jar]
		  {
			  return yaml::newsequence::create
				  ([jar]
				   (yaml::newsequence &seq)
				  {
					  auto b=jar->begin(), e=jar->end();

					  seq(save_iterator(b, e),
					      save_iterator(e, e));
				  });
		  }), fdoutputiter(fd)).flush();

#pragma GCC visibility pop

	fd->close();
}

void cookiejarObj::load(const std::string &filename)
{
	yaml::parser p=({

			std::ifstream i(filename);

			if (!i.is_open())
				throw SYSEXCEPTION(filename);

			yaml::parser::create(std::istreambuf_iterator<char>(i),
					     std::istreambuf_iterator<char>());
		});

	if (p->documents.empty())
		return;

	yaml::sequencenode cookies=p->documents.front()->root();

#pragma GCC visibility push(hidden)

	ref<cookiejarObj> jar(this);

	cookies->for_each
		([jar]
		 (yaml::mappingnode &&n)
		 {
			 cookie c;

			 n->for_each([&c]
				     (yaml::scalarnode &&name,
				      yaml::scalarnode &&value)
				     {
					     std::string n=name->value;
					     std::string v=value->value;

					     if (n == "name")
						     c.name=v;
					     else if (n == "value")
						     c.value=v;
					     else if (n == "expires")
					     {
						     c.expiration=
							     c.expires_from_str
							     (v);
					     }
					     else if (n == "domain")
						     c.domain=v;
					     else if (n == "path")
						     c.path=v;
					     else if (n == "secure")
						     c.secure=true;
					     else if (n == "httponly")
						     c.httponly=true;
				     });
			 if (!c.name.empty() &&
			     c.has_expiration() &&
			     !c.domain.empty() &&
			     !c.path.empty())
			 {
				 jar->store(c);
			 }
		 });
#pragma GCC visibility pop
}

#if 0
{
	{
#endif
	};
};
