/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/hier.H"
#include "x/exception.H"
#include "x/join.H"
#include <iostream>

class myElemObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	std::string s;

	myElemObj(const std::string &sArg) : s(sArg)
	{
	}
};

typedef LIBCXX_NAMESPACE::ref<myElemObj> myElem;

template class LIBCXX_NAMESPACE::hierObj<std::string, myElem>;

typedef LIBCXX_NAMESPACE::hier<std::string, myElem> href;


class myHierObj : public LIBCXX_NAMESPACE::hierObj<std::string, myElem> {

public:
	myHierObj() {}
	~myHierObj() {}

	void validate()
	{
		validate(create_readlock());
	}

	// validate: all leaf nodes must have an element.

	void validate(const readlock &r)
	{
		auto children=r->children();

		if (children.empty())
		{
			if (r->name().empty())
				return; // ... except for the root node.

			if (r->entry().null())
				throw EXCEPTION("Empty leaf node");
			return;
		}

		for (auto &child:r->children())
		{
			r->to_child(child);
			validate(r);
			r->to_parent();
		}
	}

	static std::list<std::string> tohier(const std::string &name)
	{
		std::list<std::string> n;

		std::string::const_iterator p=name.begin();

		for (auto b=name.begin(), e=name.end(); b != e; )
		{
			if (*b == '/')
			{
				n.emplace_back(p, b);
				p= ++b;
			}
			else
				++b;
		}
		n.emplace_back(p, name.end());
		return n;
	}

	void add(const std::string &name, const std::string &value,
		 bool replace,
		 bool shouldve_created,
		 const std::string &expected_value)
	{
		bool flag=insert([&]
				 {
					 return LIBCXX_NAMESPACE
						 ::ref<myElemObj>
						 ::create(value);
				 },
				 tohier(name),
				 [replace]
				 (LIBCXX_NAMESPACE::ref<myElemObj> &&dummy)
				 {
					 return replace;
				 });

		if (flag != shouldve_created)
			throw EXCEPTION("Unexpected created status");

		auto lock=search(tohier(name));

		if (lock->entry()->s != expected_value)
			throw EXCEPTION("Unexpected value after create");

		validate();
	}

	void del(const std::string &name, bool shouldve_erased)
	{
		if (erase(tohier(name)) != shouldve_erased)
			throw EXCEPTION("Unexpected return value from erase()");
		validate();
	}

	void del(const std::string &name, bool shouldve_erased,
		 const std::list<std::string> &new_parent, bool parent_exists)
	{
		{
			bool flag;

			auto lock=create_writelock();

			if (!lock->to_child(tohier(name), false))
			{
				flag=false;
			}
			else
			{
				if (!lock->erase())
					throw EXCEPTION("Unexpected return value from erase() (2)");
				flag=true;

				if (lock->name() != new_parent)
					throw EXCEPTION("Unexpected return value from erase() (3)");

				if (lock->exists() != parent_exists)
				{
					throw EXCEPTION("Unexpected return value from erase() (4)");
				}
			}

			if (flag != shouldve_erased)
				throw EXCEPTION("Unexpected return value from erase() (5)");
		}

		validate();
	}

	void doesnotexist(const std::string &name) const
	{
		if (!search(tohier(name))->entry().null())
			throw EXCEPTION(name + " should not exist");
	}

	void shouldexist(const std::string &name,
			 const std::string &existname,
			 const std::string &value)
	{
		auto lock=search(tohier(name));

		if (lock->entry().null())
			throw EXCEPTION(name + " should exist");

		if (lock->name() != tohier(existname))
			throw EXCEPTION("Found node name is wrong");

		if (lock->entry()->s != value)
			throw EXCEPTION("Found node value is wrong");
	}

	void do_collect(std::set< std::list<std::string> > &nodeset)
	{
	}

	template<typename ...Args>
	void do_collect(std::set< std::list<std::string> > &nodeset,
			const std::string &n,
			Args &&...args)
	{
		nodeset.insert(tohier(n));
		do_collect(nodeset, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	void contents(Args &&...args)
	{
		std::set< std::list<std::string> > expected;

		do_collect(expected, std::forward<Args>(args)...);

		std::set< std::list<std::string> > contents;

		contents.insert(begin(), end());

		for (href::base::iterator b=begin(), e=end(); b != e;
		     ++b)
		{
			readlock l=b.clone();
		}

		if (expected != contents)
			throw EXCEPTION("Expected contents mismatch");
	}
};

typedef LIBCXX_NAMESPACE::ref<myHierObj> myHier;

void testhier()
{
	auto h=myHier::create();

	h->validate();
	h->doesnotexist("a/b/c");
	h->contents();
	h->add("a/b/c", "c1", false, true, "c1");
	h->add("a/b/c", "c", false, false, "c1");
	h->add("a/b/c", "c", true, true, "c");
	h->contents("a/b/c");

	h->shouldexist("a/b/c", "a/b/c", "c");
	h->shouldexist("a/b/c/d", "a/b/c", "c");
	h->doesnotexist("a/b");

	h->add("a/b/c", "d", false, false, "c");
	h->contents("a/b/c");
	h->shouldexist("a/b/c", "a/b/c", "c");
	h->shouldexist("a/b/c/d", "a/b/c", "c");
	h->doesnotexist("a/b");

	h->add("a", "a", false, true, "a");
	h->contents("a", "a/b/c");
	h->shouldexist("a", "a", "a");
	h->shouldexist("a/b", "a", "a");
	h->doesnotexist("b");

	h->del("a/b", false);
	h->contents("a", "a/b/c");
	h->shouldexist("a/b", "a", "a");
	h->shouldexist("a/b/c/d", "a/b/c", "c");
	h->del("a/b/c", true);
	h->contents("a");
	h->shouldexist("a/b/c/d", "a", "a");


	h->add("b/c/d/e", "e1", false, true, "e1");
	h->add("b/c/d2/e", "e2", false, true, "e2");
	h->contents("a", "b/c/d/e", "b/c/d2/e");

	h->shouldexist("b/c/d/e", "b/c/d/e", "e1");
	h->shouldexist("b/c/d/e/f", "b/c/d/e", "e1");

	h->shouldexist("b/c/d2/e", "b/c/d2/e", "e2");
	h->shouldexist("b/c/d2/e/f", "b/c/d2/e", "e2");

	h->del("b/c/d/e", true);
	h->doesnotexist("b/c/d/e");
	h->shouldexist("b/c/d2/e", "b/c/d2/e", "e2");

	h->contents("a", "b/c/d2/e");

	h->del("b/c/d2/e", true, std::list<std::string>(), false);
	h->add("a/b/c", "c", false, true, "c");
	{
		auto l=h->create_readlock();

		if (!l->child(h->tohier("a"), false))
			throw EXCEPTION("Read lock navigation failed (1)");

		if (l->child(h->tohier("a/b"), false))
			throw EXCEPTION("Read lock navigation failed (2)");

		if (!l->to_child(h->tohier("a/b/c"), false))
			throw EXCEPTION("Read lock navigation failed (3)");

		if (!l->parent() || !l->to_parent() || !l->to_parent()
		    || !l->to_parent()
		    || l->parent() || l->to_parent())
		{
			throw EXCEPTION("Read lock navigation failed (4)");
		}
	}

	h->del("a/b/c", true, h->tohier("a"), true);

	h->contents("a");

	h->add("a/b/c", "c", false, true, "c");
	h->contents("a", "a/b/c");
	h->prune(h->tohier("a"));
	h->contents("a");
	h->del("a", true, std::list<std::string>(), false);

	h->add("a/b/c", "c", false, true, "c");
	h->contents("a/b/c");

	h->prune(h->tohier("a/b"));
	h->contents();

	h->add("a", "a", false, true, "a");
	h->add("a/b/c", "c", false, true, "c");
	h->add("a/b/d", "d", false, true, "d");

	h->prune_if(h->tohier("a"), []
		    (const myElem &e, const std::list<std::string> &name)
		    {
			    return LIBCXX_NAMESPACE::join(name, "/")
				    == "a/b/d";
		    });

	h->contents("a", "a/b/c");

	{
		auto first=h->create_readlock();

		first->to_child(h->tohier("a"), false);

		auto second=first->clone();

		second->to_child(h->tohier("b/c"), false);

		if (first->entry()->s != "a" ||
		    first->name() != h->tohier("a"))
			throw EXCEPTION("readlock clone failed(1)");

		if (second->entry()->s != "c" ||
		    second->name() != h->tohier("a/b/c"))
			throw EXCEPTION("readlock clone failed(2)");
	}

	bool caught=false;
	try {
		(href::base::readlock)(h->create_writelock())->clone();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("How did I just clone a write lock?");
}

int main(int argc, char **argv)
{
	try {
		testhier();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testhier: "
			  << e << std::endl << e->backtrace;
		exit(1);
	}
	return (0);
}

