/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/property_save.H"
#include "x/property_value.H"
#include "x/property_list.H"
#include "x/sysexception.H"
#include "x/fileattr.H"
#include "x/eventfactoryobj.H"
#include "x/singleton.H"
#include "x/messages.H"
#include "x/logger.H"
#include "x/imbue.H"
#include "x/fd.H"
#include "x/pidinfo.H"
#include "x/messages.H"
#include "x/strtok.H"
#include "gettext_in.h"
#include <fstream>
#include <list>
#include <cerrno>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <sstream>
#include <algorithm>
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
	namespace property {
#if 0
	};
};
#endif

void errhandler::operator()(const std::string &location,
			    size_t linenum,
			    const std::string &errtxt) const
{
	std::ostringstream o;

	o << location << "(" << linenum << ")";

	operator()(o.str(), errtxt);
}

errhandler::errhandler() noexcept
{
}

errhandler::~errhandler()
{
}

errhandler::errthrow::errthrow() noexcept
{
}

errhandler::errthrow::~errthrow()
{
}

void errhandler::errthrow::operator()(const std::string &location,
				       const std::string &errtxt)
	const
{
	std::ostringstream o;

	o << location << ": " << errtxt;

	throw EXCEPTION(o.str());
}

errhandler::errstream::errstream(std::ostream &oArg) noexcept : o(oArg)
{
}

errhandler::errstream::~errstream()
{
}

void errhandler::errstream::operator()(const std::string &location,
				       const std::string &errtxt)
	const
{
	o << location << ": " << errtxt << std::endl;
}

errhandler::errlog::errlog() noexcept
{
}

errhandler::errlog::~errlog()
{
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::property, errlog_handler);

void errhandler::errlog::operator()(const std::string &location,
				    const std::string &errtxt)
	const
{
	LOG_FUNC_SCOPE(errlog_handler);
	LOG_ERROR(location << ": " << errtxt);
}

const char propname_delim[3]={':', ':', 0};

std::string combinepropname(const std::list<std::string> &prophier)
{
	std::string n;

	const auto *p=propname_delim+
		sizeof(propname_delim)/sizeof(propname_delim[0])-1;

	for (std::list<std::string>::const_iterator b=prophier.begin(),
		     e=prophier.end(); b != e; ++b)
	{
		n += p;
		p=propname_delim;
		n += *b;
	}

	return n;
}

bool islibraryprop(const std::string &value)
{
	std::list<std::string> hier;

	parsepropname(value.begin(), value.end(), hier);

	if (hier.empty())
		return false;

	if (hier.front() == LIBCXX_NAMESPACE_STR)
		return true;

	for (std::list<std::string>::iterator b=hier.begin(), e=hier.end();
	     b != e; ++b)
		if (*b == "@log")
			return true;
	return false;
}

nodeObj::nodeObj(const std::string &valArg,
		 const ref<nodehierObj> hierpArg)
	: val(valArg), hierp(hierpArg), callbacks(callbacks_t::create())
{
}

nodeObj::~nodeObj()
{
}

nodehierObj::nodehierObj(const ptr<nodehierObj> &parentArg)
	: parent(parentArg), children(children_t::create())
{
}

nodehierObj::~nodehierObj()
{
}

listObj::listObj()
	: rootnode(ref<nodehierObj>::create(ptr<nodehierObj>())),
	  prophier(prophier_t::create())
{
}

listObj::~listObj()
{
}

void listObj::load(const std::string &properties,
		   bool update,
		   bool create,
		   const errhandler &errh,
		   const const_locale &l)
{
	parse(properties.begin(), properties.end(), "properties",
	      update, create, errh, l);
}

void listObj::load(std::istream &i,
		   const std::string &location,
		   bool update,
		   bool create,
		   const errhandler &errh,
		   const const_locale &l
		   )
{
	typedef std::istreambuf_iterator<char> iter_t;

	auto s=std::string(iter_t(i), iter_t());

	parse(s.begin(), s.end(), location, update, create, errh, l);
}

void listObj::load(const std::string &n,
		   const std::string &v,
		   bool do_update,
		   bool do_create,
		   const errhandler &errh,
		   const const_locale &l)
{
	std::list<std::string> hier;

	parsepropname(n.begin(), n.end(), hier, l);

	if (hier.empty())
		throw EXCEPTION("Invalid property name");

	loaded_t parsed;
	{
		std::lock_guard<std::mutex> lock(objmutex);

		update_locked(hier, v, parsed, do_update, do_create);
	}
	update(parsed, errh, l);
}

void listObj::load_file(const std::string &filename,
			bool update,
			bool create,
			const errhandler &errh,
			const const_locale &l
			)
{
	std::ifstream i;

	i.open(filename.c_str());

	if (!i.is_open())
		throw SYSEXCEPTION(filename);

	load_file(i, filename, update, create, errh, l);
}

void listObj::load_file(std::ifstream &i,
			const std::string &filename,
			bool update,
			bool create,
			const errhandler &errh,
			const const_locale &l
			)
{
	const_locale parse_locale=l;

	{
		std::string line;

		while (!std::getline(i, line).eof())
		{
			std::list<std::string> words;

			strtok_str(line, " \t\r\n", words);

			if (words.empty())
				continue;

			if (*words.front().c_str() != '#')
				break;

			if (words.front() == "#LOCALE")
			{
				words.pop_front();

				if (!words.empty())
					parse_locale=locale
						::create(to_string(words.front()
								  ));
			}
		}
	}
	i.seekg(0);
	i.clear();
	load(i, filename, update, create, errh, parse_locale);
}

ref<nodehierObj> listObj::findhier_locked(const std::list<std::string> &hier,
					  std::string &n)

{
	n.clear();

	ref<nodehierObj> p=rootnode;

	const auto *sep=propname_delim+
		sizeof(propname_delim)/sizeof(propname_delim[0])-1;

	for (std::list<std::string>::const_iterator b=hier.begin(),
		     e=hier.end(); b != e; ++b)
	{
		n += sep;
		n += *b;
		sep=propname_delim;

		ptr<nodehierObj> c;

		for (auto r=p->children->equal_range(*b);
		     r.first != r.second; ++r.first)
		{
			if (!(c=r.first->second.getptr()).null())
				break;
		}

		if (c.null())
		{
			c=ptr<nodehierObj>::create(p);
			(*p->children)[*b]=c;
		}
		p=c;
	}
	return p;
}

void listObj::update_locked(const std::list<std::string> &hier,
			    const std::string &v,
			    loaded_t &parsed,
			    bool update,
			    bool create)
{
	std::string n;

	ptr<nodehierObj> p=findhier_locked(hier, n);

	ptr<nodeObj> value=p->value.getptr();

	if (value.null())
	{
		if (!create)
			return;
		value=ptr<nodeObj>::create(v, p);
		p->value=value;
	}
	else
	{
		if (!update)
			return;

		value->val=v;
	}

	loaded.insert(std::make_pair(n, value));
	parsed.insert(std::make_pair(n, value));
}

void listObj::update(const loaded_t &loaded,
		     const errhandler &errh,
		     const const_locale &localeArg)
{
	for (auto loaded_node:loaded)
	{
		nodeObj &n=*loaded_node.second;

		std::lock_guard<std::mutex> lock(n.objmutex);

		std::list< ptr<eventhandlerObj<propvalueset_t> > > l;

		for (auto cb: *n.callbacks)
		{
			auto callback=cb.getptr();

			if (!callback.null())
				l.push_back(callback);
		}

		propvalueset_t setarg(&n.val, localeArg);

		while (!l.empty())
		{
			try {
				l.front()->event(setarg);
			} catch (const exception &e)
			{
				errh(to_string(loaded_node.first, localeArg),
				     (std::string)*e);
			}
			l.pop_front();
		}
	}
}

ref<nodeObj>
listObj::install(const ref<eventhandlerObj<propvalueset_t> > &callback,
		 const std::string &propname,
		 const std::string &initialvalue,
		 const const_locale &l)
{
	std::list<std::string> hier;

	parsepropname(propname.begin(), propname.end(), hier, l);

	ptr<nodeObj> value;

	std::string canonname=combinepropname(hier);

	{
		std::lock_guard<std::mutex> lock(objmutex);
		std::string n;
		ptr<nodehierObj> node;

		if (!hier.empty()) // Empty means it's a private value, this shouldn't happen
		{
			node=findhier_locked(hier, n);
			value=node->value.getptr();
		}
		else
			node=ptr<nodehierObj>::create(ptr<nodehierObj>());

		if (value.null())
		{
			value=ptr<nodeObj>::create(initialvalue, node);
			value->callbacks->push_front(callback);
			node->value=value;
			return value;
		}
	}

	std::lock_guard<std::mutex> lock(value->objmutex);

	value->callbacks->push_front(callback);

	try {
		callback->event(propvalueset_t(&value->val, l));
	} catch (const exception &e)
	{
		try {
			std::cerr << to_string(canonname, l)
				  << ": " << e << std::endl;
		} catch (const exception &e)
		{
			std::cerr << e << std::endl;
		}
	}
	return value;
}

listObj::iteratorObj::iteratorObj(const list &lArg,
				  const std::string &nodenameArg,
				  const ptr<nodehierObj> &pArg)
	: l(lArg), nodename(nodenameArg), p(pArg)
{
}

listObj::iteratorObj::~iteratorObj()
{
}

listObj::iterator::iterator(const ref<iteratorObj> &iterArg)
	: iter(iterArg)
{
}

listObj::iterator listObj::root()
{
	std::lock_guard<std::mutex> lock(objmutex);

	return iterator(ref<iteratorObj>::create(list(this),
						 "",
						 rootnode));
}

listObj::iterator listObj::find_internal(const std::string &propname,
					 const const_locale &l)
{
	std::list<std::string> hier;

	parsepropname(propname.begin(), propname.end(), hier, l);

	iterator p=root();

	while (!hier.empty())
	{
		p=p.child(hier.front(), l);
		hier.pop_front();

		if (p.propname().empty())
			break;
	}

	return p;
}

std::string listObj::iterator::propname() const
{
	return iter->nodename;
}

std::pair<std::string, bool> listObj::iterator::value() const
{
	ptr<nodeObj> node=iter->p->value.getptr();

	if (node.null())
		return std::make_pair(std::string(), true);

	std::lock_guard<std::mutex> lock(node->objmutex);

	return std::make_pair(node->val, false);
}

void listObj::iterator::children(children_t &childrenArg) const
{
	std::string n=propname();

	if (n.size() > 0)
		n += propname_delim;

	std::lock_guard<std::mutex> lock(iter->l->objmutex);

	for (auto child: *iter->p->children)
	{
		ptr<nodehierObj> getptr(child.second.getptr());

		if (getptr.null())
			continue;

		childrenArg.insert(std::make_pair(child.first,
						  iterator(ref<iteratorObj>
							   ::create(iter->l,
								    n+child
								    .first,
								    getptr))
						  ));
	}
}

listObj::iterator listObj::iterator::get(const std::string &childname)
	const
{
	std::string n=propname();

	if (n.size() > 0)
		n += propname_delim;

	std::lock_guard<std::mutex> lock(iter->l->objmutex);

	for (auto iters=iter->p->children->equal_range(childname);
	     iters.first != iters.second; ++iters.first)
	{
		ptr<nodehierObj> getptr(iters.first->second.getptr());

		if (!getptr.null())
			return iterator(ref<iteratorObj>
					::create(iter->l,
						 n+iters.first->first, getptr));
	}

	return iterator(ref<iteratorObj>::create(iter->l,
						 "",
						 ptr<nodehierObj>()));
}

void listObj::enumerate(std::map<std::string, std::string> &properties)

{
	enumerate(properties, root());
}

void listObj::enumerate(std::map<std::string, std::string> &properties,
			const iterator &iter)

{
	std::pair<std::string, bool> value(iter.value());

	if (!value.second)
		properties.insert(std::make_pair(iter.propname(), value.first));

	iterator::children_t children;

	iter.children(children);

	for (iterator::children_t::iterator
		     b=children.begin(), e=children.end(); b != e; ++b)
		enumerate(properties, b->second);
}

class globalListObj : public listObj {

public:
	globalListObj() LIBCXX_INTERNAL;
	~globalListObj();
};

globalListObj::globalListObj()
{
	const char *c=getenv("PROPERTIES");
	bool explicitprops=false;
	std::string exename;

	if (c && getuid() == geteuid() && getgid() == getegid())
	{
		exename=c;
		explicitprops=true;
	}
	else
	{
		try {
			exename=LIBCXX_NAMESPACE::exename();

			LIBCXX_NAMESPACE::fileattr attrs=
				LIBCXX_NAMESPACE::fileattr::create(exename);

			std::set<std::string> attrlist=attrs->listattr();

			std::set<std::string>::const_iterator p=
				attrlist.find("user.properties");

			if (p == attrlist.end())
				exename += ".properties";
			else
				exename=attrs->getattr("user.properties");

		} catch (const exception &e)
		{
			std::cerr << e << std::endl;
		}
	}

	if (exename.size() > 0)
	{
		std::ifstream i;

		i.open(exename.c_str());

		if (i.is_open())
			this->load_file(i, exename, true, true,
					errhandler::errstream(),
					locale::base::environment());
		else if (explicitprops)
		{
			std::cerr <<
				gettextmsg(libmsg(_txt("Cannot open %1%: %2%")),
					   exename,
					   strerror(errno)) << std::endl;
			abort();
		}
	}
}

globalListObj::~globalListObj()
{
}

singleton<globalListObj> glob;

listptr listObj::global()
{
	listptr p=glob.get();

	if (p.null())
		p=listptr::create();

	return p;
}

notifyObj::notifyObj()
{
}

notifyObj::~notifyObj()
{
}

void notifyObj::event(const propvalueset_t &newvalue)

{
	event();
}

void notifyObj::event(void)
{
}

class LIBCXX_HIDDEN propvaluebaseObj;

//! Property %value callback

//! \c property::value registers this object as a callback for property
//! %value change notifications.
//!
//! \c property::value maintains a reference on this handler, and invokes
//! setValueBase() so that it receives property %value change notifications.
//! Its destructor invokes setValueBase() with a NULL pointer, to stop
//! receiving updates. If the property update code invokes event() and
//! the pointer to propvaluesetbase is NULL, this is a race condition where
//! the \c property::value container has been removed, but this event handler
//! callback has not yet been destroyed, so event() becomes a no-op().

class propvaluebaseObj : public eventhandlerObj<propvalueset_t> {

	//! The canonical property value object

	ptr<nodeObj> node;

	//! Pointer to the container for the property

	propvaluesetbase *p;

public:
	//! Constructor
	propvaluebaseObj();

	//! Destructor
	~propvaluebaseObj();

	//! Register the property container.
	void setValueBase(//! The container for this %property %value

			  //! event() invokes pArg->set()
			  //! if pArg is not NULL.
			  //! pArg's destructor must invoke setValueBase(NULL).
			  propvaluesetbase *pArg);

	//! Update the property
	void event(const propvalueset_t &newvalue) override;

	//! Register this handler with a property list

	void install(//! Property list
		     const list &props,

		     //! This property's name
		     const std::string &propname,

		     //! The initial %value of this property
		     const std::string &initialvalue,

		     //! Value's locale
		     const const_locale &l);

	//! Install a notification callback
	void install(const notify &notifyRef);
};

propvaluebaseObj::propvaluebaseObj() : p(NULL)
{
}

void propvaluebaseObj::install(const list &props,
			       const std::string &propname,
			       const std::string &initialvalue,
			       const const_locale &l)
{
	node=props->install( ref<eventhandlerObj<propvalueset_t> >(this),
			     propname, initialvalue, l);
}

propvaluebaseObj::~propvaluebaseObj()
{
}

void propvaluebaseObj::setValueBase(propvaluesetbase *pArg)
{
	std::lock_guard<std::mutex> lock(objmutex);

	p=pArg;
}

void propvaluebaseObj::event(const propvalueset_t &newvalue)

{
	std::lock_guard<std::mutex> lock(objmutex);

	if (p)
		p->update(newvalue);
}

void propvaluebaseObj::install(const notify &notifyRef)
{
	node->callbacks->push_back(notifyRef);
}

propvaluesetbase::propvaluesetbase()
	: handler(ptr<propvaluebaseObj>::create())
{
}

propvaluesetbase::~propvaluesetbase()
{
}

void propvaluesetbase::uninstall()
{
	handler->setValueBase(0);
}
void propvaluesetbase::install(const char *namestr,
			       const std::string &initvalue,
			       const const_locale &localeArg)

{
	install(std::string(namestr), initvalue, localeArg);
}

void propvaluesetbase::install(const listptr &props,
			       const char *namestr,
			       const std::string &initvalue,
			       const const_locale &localeArg)

{
	install(props, std::string(namestr), initvalue, localeArg);
}

void propvaluesetbase::install(const std::string &namestr,
			       const std::string &initvalue,
			       const const_locale &localeArg)

{
	install(listObj::global(), namestr, initvalue, localeArg);
}

void propvaluesetbase::install(const list &props,
			       const std::string &namestr,
			       const std::string &initvalue,
			       const const_locale &localeArg)

{
	handler->setValueBase(this);

	try {
		handler->install(props, namestr, initvalue, localeArg);
	} catch (...) {
		handler->setValueBase(NULL);
		throw;
	}
}

void propvaluesetbase::installNotify(const notify &notifyRef)
{
	handler->install(notifyRef);
}

void load_properties(const std::string &default_properties,
		     bool update,
		     bool create,
		     const errhandler &errh,
		     const const_locale &localeRef)

{
	listObj::global()->load(default_properties, update, create,
				errh, localeRef);

}

void load_properties(std::istream &default_properties,
		     const std::string &filename,
		     bool update,
		     bool create,
		     const errhandler &errh,
		     const const_locale &localeRef)

{
	listObj::global()->load(default_properties, filename,
				update, create, errh, localeRef);
}

void enumerate_properties(std::map<std::string, std::string> &properties)

{
	listObj::global()->enumerate(properties);
}

void load_property(const std::string &propnamestr,
		   const std::string &propvaluestr,
		   bool update,
		   bool create,
		   const errhandler &errh,
		   const const_locale &localeRef)

{
	listObj::global()->load(propnamestr, propvaluestr, true, true,
				errh, localeRef);
}

void load_file(const std::string &filename,
	       bool update,
	       bool create,
	       const errhandler &errh,
	       const const_locale &l
	       )
{
	listObj::global()->load_file(filename, update, create, errh, l);
}

#define LIBCXX_TEMPLATE_DECL
#include "x/property_save_t.H"
#include "x/property_properties_t.H"
#include "x/property_list_t.H"

#if 0
{
	{
#endif
	}
}
