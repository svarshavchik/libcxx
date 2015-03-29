/*
** Copyright 2012 Double Precision, Inc.
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

errhandler::~errhandler() noexcept
{
}

errhandler::errthrow::errthrow() noexcept
{
}

errhandler::errthrow::~errthrow() noexcept
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

errhandler::errstream::~errstream() noexcept
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

errhandler::errlog::~errlog() noexcept
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

const propvalue::value_type propname_delim[3]={':', ':', 0};

propvalue combinepropname(const std::list<propvalue> &prophier)
{
	propvalue n;

	const propvalue::value_type *p=propname_delim+
		sizeof(propname_delim)/sizeof(propname_delim[0])-1;

	for (std::list<propvalue>::const_iterator b=prophier.begin(),
		     e=prophier.end(); b != e; ++b)
	{
		n += p;
		p=propname_delim;
		n += *b;
	}

	return n;
}

bool islibraryprop(const propvalue &value)
{
	std::list<propvalue> hier;

	parsepropname(value.begin(), value.end(), hier);

	if (hier.empty())
		return false;

	if (hier.front() == LIBCXX_NAMESPACE_WSTR)
		return true;

	for (std::list<propvalue>::iterator b=hier.begin(), e=hier.end();
	     b != e; ++b)
		if (*b == L"@log")
			return true;
	return false;
}

nodeObj::nodeObj(const propvalue &valArg,
		 const ref<nodehierObj> hierpArg)
	: val(valArg), hierp(hierpArg), callbacks(callbacks_t::create())
{
}

nodeObj::~nodeObj() noexcept
{
}

nodehierObj::nodehierObj(const ptr<nodehierObj> &parentArg)
	: parent(parentArg), children(children_t::create())
{
}

nodehierObj::~nodehierObj() noexcept
{
}

listObj::listObj()
	: rootnode(ref<nodehierObj>::create(ptr<nodehierObj>())),
	  prophier(prophier_t::create())
{
}

listObj::~listObj() noexcept
{
}

void listObj::load(const propvalue &properties,
		   bool update,
		   bool create,
		   const errhandler &errh,
		   const const_locale &l)
{
	parse(properties.begin(), properties.end(), "properties",
	      update, create, errh, l);
}

void listObj::load(propistream &i,
		   const std::string &location,
		   bool update,
		   bool create,
		   const errhandler &errh,
		   const const_locale &l
		   )
{
	std::basic_string<propvalue::value_type> s;

	typedef std::istreambuf_iterator<propvalue::value_type> iter_t;

	{
		imbue<propistream> im(l, i);

		s=std::basic_string<propvalue::value_type>
			(iter_t(i), iter_t());
	}

	parse(s.begin(), s.end(), location, update, create, errh, l);
}

void listObj::load(const propvalue &n,
		   const propvalue &v,
		   bool do_update,
		   bool do_create,
		   const errhandler &errh,
		   const const_locale &l)
{
	std::list<propvalue> hier;

	parsepropname(n.begin(), n.end(), hier, l);

	if (hier.empty())
		throw EXCEPTION("Invalid property propvalue");

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
	std::basic_ifstream<propvalue::value_type> i;

	i.open(filename.c_str());

	if (!i.is_open())
		throw SYSEXCEPTION(filename);

	load_file(i, filename, update, create, errh, l);
}

void listObj::load_file(std::basic_ifstream<propvalue::value_type> &i,
			const std::string &filename,
			bool update,
			bool create,
			const errhandler &errh,
			const const_locale &l
			)
{
	const_locale parse_locale=l;

	{
		std::basic_string<propvalue::value_type> line;

		basic_ctype<propvalue::value_type> ct(l);

		while (!std::getline(i, line).eof())
		{
			std::list<std::basic_string<propvalue::value_type> >
				words;

			ct.strtok_is(line, words);

			if (words.empty())
				continue;

			if (*words.front().c_str() != '#')
				break;

			if (words.front() == L"#LOCALE")
			{
				words.pop_front();

				if (!words.empty())
					parse_locale=locale
						::create(tostring(words.front()
								  ));
			}
		}
	}
	i.seekg(0);
	i.clear();
	load(i, filename, update, create, errh, parse_locale);
}

ref<nodehierObj> listObj::findhier_locked(const std::list<propvalue> &hier,
					  propvalue &n)

{
	n.clear();

	ref<nodehierObj> p=rootnode;

	const propvalue::value_type *sep=propname_delim+
		sizeof(propname_delim)/sizeof(propname_delim[0])-1;

	for (std::list<propvalue>::const_iterator b=hier.begin(),
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

void listObj::update_locked(const std::list<propvalue> &hier,
			    const propvalue &v,
			    loaded_t &parsed,
			    bool update,
			    bool create)
{
	propvalue n;

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
				errh(tostring(loaded_node.first, localeArg),
				     (std::string)*e);
			}
			l.pop_front();
		}
	}
}

ref<nodeObj>
listObj::install(const ref<eventhandlerObj<propvalueset_t> > &callback,
		 const propvalue &propname,
		 const propvalue &initialvalue,
		 const const_locale &l)
{
	std::list<propvalue> hier;

	parsepropname(propname.begin(), propname.end(), hier, l);

	ptr<nodeObj> value;

	propvalue canonname=combinepropname(hier);

	{
		std::lock_guard<std::mutex> lock(objmutex);
		propvalue n;
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
			std::cerr << tostring(canonname, l)
				  << ": " << e << std::endl;
		} catch (const exception &e)
		{
			std::cerr << e << std::endl;
		}
	}
	return value;
}

listObj::iteratorObj::iteratorObj(const list &lArg,
				  const propvalue &nodenameArg,
				  const ptr<nodehierObj> &pArg)
	: l(lArg), nodename(nodenameArg), p(pArg)
{
}

listObj::iteratorObj::~iteratorObj() noexcept
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
						 LIBCXX_PROPERTY_NAME(""),
						 rootnode));
}

listObj::iterator listObj::find_internal(const propvalue &propname,
					 const const_locale &l)
{
	std::list<propvalue> hier;

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

propvalue listObj::iterator::propname() const
{
	return iter->nodename;
}

std::pair<propvalue, bool> listObj::iterator::value() const
{
	ptr<nodeObj> node=iter->p->value.getptr();

	if (node.null())
		return std::make_pair(propvalue(), true);

	std::lock_guard<std::mutex> lock(node->objmutex);

	return std::make_pair(node->val, false);
}

void listObj::iterator::children(children_t &childrenArg) const
{
	propvalue n=propname();

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

listObj::iterator listObj::iterator::get(const propvalue &childname)
	const
{
	propvalue n=propname();

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
						 LIBCXX_PROPERTY_NAME(""),
						 ptr<nodehierObj>()));
}

void listObj::enumerate(std::map<propvalue, propvalue> &properties)

{
	enumerate(properties, root());
}

void listObj::enumerate(std::map<propvalue, propvalue> &properties,
			const iterator &iter)

{
	std::pair<propvalue, bool> value(iter.value());

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
	~globalListObj() noexcept;
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
		std::basic_ifstream<propvalue::value_type> i;

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

globalListObj::~globalListObj() noexcept
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

notifyObj::~notifyObj() noexcept
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
	~propvaluebaseObj() noexcept;

	//! Register the property container.
	void setValueBase(//! The container for this %property %value

			  //! event() invokes pArg->set()
			  //! if pArg is not NULL.
			  //! pArg's destructor must invoke setValueBase(NULL).
			  propvaluesetbase *pArg);

	//! Update the property
	void event(const propvalueset_t &newvalue)
;

	//! Register this handler with a property list

	void install(//! Property list
		     const list &props,

		     //! This property's name
		     const propvalue &propname,

		     //! The initial %value of this property
		     const propvalue &initialvalue,

		     //! Value's locale
		     const const_locale &l);

	//! Install a notification callback
	void install(const notify &notifyRef);
};

propvaluebaseObj::propvaluebaseObj() : p(NULL)
{
}

void propvaluebaseObj::install(const list &props,
			       const propvalue &propname,
			       const propvalue &initialvalue,
			       const const_locale &l)
{
	node=props->install( ref<eventhandlerObj<propvalueset_t> >(this),
			     propname, initialvalue, l);
}

propvaluebaseObj::~propvaluebaseObj() noexcept
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
		p->set(newvalue);
}

void propvaluebaseObj::install(const notify &notifyRef)
{
	node->callbacks->push_back(notifyRef);
}

propvaluesetbase::propvaluesetbase()
	: handler(ptr<propvaluebaseObj>::create())
{
}

propvaluesetbase::~propvaluesetbase() noexcept
{
}

void propvaluesetbase::uninstall()
{
	handler->setValueBase(0);
}
void propvaluesetbase::install(const propvalue::value_type *namestr,
			       const propvalue &initvalue,
			       const const_locale &localeArg)

{
	install(propvalue(namestr), initvalue, localeArg);
}

void propvaluesetbase::install(const listptr &props,
			       const propvalue::value_type *namestr,
			       const propvalue &initvalue,
			       const const_locale &localeArg)

{
	install(props, propvalue(namestr), initvalue, localeArg);
}

void propvaluesetbase::install(const propvalue &namestr,
			       const propvalue &initvalue,
			       const const_locale &localeArg)

{
	install(listObj::global(), namestr, initvalue, localeArg);
}

void propvaluesetbase::install(const list &props,
			       const propvalue &namestr,
			       const propvalue &initvalue,
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

void load_properties(const propvalue &default_properties,
		     bool update,
		     bool create,
		     const errhandler &errh,
		     const const_locale &localeRef)

{
	listObj::global()->load(default_properties, update, create,
				errh, localeRef);

}

void load_properties(propistream &default_properties,
		     const std::string &filename,
		     bool update,
		     bool create,
		     const errhandler &errh,
		     const const_locale &localeRef)

{
	listObj::global()->load(default_properties, filename,
				update, create, errh, localeRef);
}

void enumerate_properties(std::map<propvalue, propvalue> &properties)

{
	listObj::global()->enumerate(properties);
}

void load_property(const propvalue &propnamestr,
		   const propvalue &propvaluestr,
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
