/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "httportmap_server.H"
#include "server.H"
#include "x/pwd.H"
#include "x/csv.H"
#include "x/joiniterator.H"
#include "x/xml/escape.H"
#include "x/http/cgiimpl.H"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

httpserver::httpserver(bool isinternalArg,
		       bool allowstopArg,
		       httportmap_server &portmapArg)

	: isinternal(isinternalArg),
	  allowstop(allowstopArg),
	  portmap(portmapArg)
{
}

httpserver::~httpserver()
{
}

void httpserver::run(const LIBCXX_NAMESPACE::fd &socket,
		     const LIBCXX_NAMESPACE::fd &termpipe)

{
	httpserverimpl impl(isinternal, allowstop, portmap);

	impl.run(socket, termpipe);
}

httpserverimpl::httpserverimpl(bool isinternalArg,
			       bool allowstopArg,
			       httportmap_server &portmapArg)

	: isinternal(isinternalArg), allowstop(allowstopArg),
	  portmap(portmapArg)
{
	media_type_list.push_back("text/csv; charset=utf-8");
	media_type_list.push_back("application/xml; charset=utf-8");
	media_type_list.push_back("text/xml; charset=utf-8");
}

httpserverimpl::~httpserverimpl()
{
}

class httpserverimpl::svclistiter : public std::iterator<std::input_iterator_tag,
							 std::string> {

	class implObj : virtual public LIBCXX_NAMESPACE::obj {

	public:
		httportmap_server &portmap;
		const LIBCXX_NAMESPACE::http::form::parameters &params;
		bool check_userids;
		std::set<uid_t> userids;
		bool check_pids;
		std::set<pid_t> pids;
		httportmap_server::services_obj_t::lock lock;
		httportmap_server::iterator beg_iter;
		httportmap_server::iterator end_iter;
		bool external_query;

		implObj(httportmap_server &portmapArg,
			const LIBCXX_NAMESPACE::http::form::parameters &paramsArg,
			bool external_queryArg);
		~implObj();

		void filter();
	};

	typedef LIBCXX_NAMESPACE::ptr<implObj> impl;

	impl implRef;

protected:
	std::string row;

private:
	std::string rowsave;

	bool firstrow;
	bool lastrow;
public:
	class csv;
	class xml;

	svclistiter(httportmap_server &portmap,
		    const LIBCXX_NAMESPACE::http::form::parameters &params,
		    bool external_query);
	svclistiter();
	~svclistiter();

	bool operator==(const svclistiter &iter) const;
	bool operator!=(const svclistiter &iter) const;

	const std::string &operator*() noexcept
	{
		return row;
	}

	const std::string *operator->() noexcept
	{
		return &row;
	}

	svclistiter &operator++();

	const std::string *operator++(int)
	{
		rowsave=row;
		operator++();
		return &rowsave;
	}
private:
	virtual void getnextrow(const httportmap_server::iterator &i)=0;
	virtual void getlastrow()=0;
};

httpserverimpl::svclistiter::implObj::implObj(httportmap_server &portmapArg,
					      const LIBCXX_NAMESPACE::http::form
					      ::parameters &paramsArg,
					      bool external_queryArg)

	: portmap(portmapArg), params(paramsArg),
	  check_userids(false),
	  check_pids(false),
	  lock(portmap.services),
	  beg_iter(portmap.begin(lock)),
	  end_iter(portmap.end(lock)),
	  external_query(external_queryArg)
{
	for (std::pair<LIBCXX_NAMESPACE::http::form::map_t::const_iterator,
		       LIBCXX_NAMESPACE::http::form::map_t::const_iterator>
		     arg=params->equal_range("user"); arg.first != arg.second;
	     ++arg.first)
	{
		check_userids=true;

		std::istringstream i(arg.first->second);

		uid_t u;

		i >> u;

		if (!i.fail())
			userids.insert(u);
	}

	for (std::pair<LIBCXX_NAMESPACE::http::form::map_t::const_iterator,
		       LIBCXX_NAMESPACE::http::form::map_t::const_iterator>
		     arg=params->equal_range("pid"); arg.first != arg.second;
	     ++arg.first)
	{
		check_pids=true;

		std::istringstream i(arg.first->second);

		pid_t p;

		i >> p;

		if (!i.fail())
			pids.insert(p);
	}

}

httpserverimpl::svclistiter::implObj::~implObj()
{
}

void httpserverimpl::svclistiter::implObj::filter()

{

	while (beg_iter != end_iter)
	{
		// Only show services that have a numerical port, to external
		// queries.

		if (external_query &&
		    (std::find_if(beg_iter->port.begin(),
				  beg_iter->port.end(),
				  std::not1(std::ptr_fun(isdigit)))
		     != beg_iter->port.end() ||
		     beg_iter->port.size() == 0))
		{
			++beg_iter;
			continue;
		}

		std::pair<LIBCXX_NAMESPACE::http::form::map_t::const_iterator,
			  LIBCXX_NAMESPACE::http::form::map_t::const_iterator>
			arg;

		arg=params->equal_range("service");

		if (arg.first != arg.second)
		{
			while (arg.first != arg.second)
			{
				if (beg_iter->service == arg.first->second)
					break;
				++arg.first;
			}

			if (arg.first == arg.second)
			{
				++beg_iter;
				continue;
			}
		}

		if (check_userids &&
		    userids.find(beg_iter->user) == userids.end())
		{
			++beg_iter;
			continue;
		}

		if (check_pids &&
		    pids.find(beg_iter->pid) == pids.end())
		{
			++beg_iter;
			continue;
		}
		break;
	}
}

class httpserverimpl::svclistiter::csv : public svclistiter {

public:
	csv(httportmap_server &portmap,
	    const LIBCXX_NAMESPACE::http::form::parameters &params,
	    bool external_query);
	csv();
	~csv();

	void getnextrow(const httportmap_server::iterator &i) override;
	void getlastrow() override;
};

httpserverimpl::svclistiter::svclistiter::csv
::csv(httportmap_server &portmap,
      const LIBCXX_NAMESPACE::http::form::parameters &params,
      bool external_query)

	: svclistiter(portmap, params, external_query)
{
	std::vector<std::string> cols;

	httportmap_server::entry
		::names(std::back_insert_iterator<std::vector<std::string> >
			(cols));

	LIBCXX_NAMESPACE::tocsv(cols.begin(), cols.end(),
			      std::back_insert_iterator<std::string>(row));
	row.push_back('\n');
}

httpserverimpl::svclistiter::svclistiter::csv
::csv()
{
}

httpserverimpl::svclistiter::svclistiter::csv::~csv()
{
}

void httpserverimpl::svclistiter::svclistiter::csv
::getnextrow(const httportmap_server::iterator &i)

{
	row.clear();

	std::vector<std::string> cols;

	i->values(std::back_insert_iterator<std::vector<std::string> >(cols));

	LIBCXX_NAMESPACE::tocsv(cols.begin(), cols.end(),
			      std::back_insert_iterator<std::string>(row));
	row.push_back('\n');
}

void httpserverimpl::svclistiter::svclistiter::csv::getlastrow()

{
	row="";
}

class httpserverimpl::svclistiter::xml : public svclistiter {

	std::vector<std::string> cols;

public:
	xml(httportmap_server &portmap,
	    const LIBCXX_NAMESPACE::http::form::parameters &params,
	    bool external_query);
	xml();
	~xml();

	void getnextrow(const httportmap_server::iterator &i) override;

	void getlastrow() override;
};

httpserverimpl::svclistiter::svclistiter::xml
::xml(httportmap_server &portmap,
      const LIBCXX_NAMESPACE::http::form::parameters &params,
      bool external_query)

	: svclistiter(portmap, params, external_query)
{
	httportmap_server::entry
		::names(std::back_insert_iterator<std::vector<std::string> >
			(cols));

	row="<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		"<?xml-stylesheet type=\"text/xml\" href=\"/portmap/portmap.xsl\"?>\r\n"

		"<portmap>";
}

httpserverimpl::svclistiter::svclistiter::xml
::xml()
{
}

httpserverimpl::svclistiter::svclistiter::xml::~xml()
{
}

void httpserverimpl::svclistiter::svclistiter::xml
::getnextrow(const httportmap_server::iterator &i)

{
	std::vector<std::string> fields;

	i->values(std::back_insert_iterator<std::vector<std::string> >(fields));

	std::ostringstream o;

	o << "<instance>";

	std::vector<std::string>::const_iterator p=cols.begin();

	for (std::vector<std::string>::const_iterator
		     b(fields.begin()), e(fields.end());
	     b != e && p != cols.end(); ++b, ++p)
	{
		o << "<" << *p << ">";

		LIBCXX_NAMESPACE::xml::escape(b->begin(), b->end(),
					    std::ostreambuf_iterator<char>(o));

		o << "</" << *p << ">";
	}
	o << "</instance>";
	row=o.str();
}

void httpserverimpl::svclistiter::svclistiter::xml::getlastrow()

{
	row="</portmap>";
}

httpserverimpl::svclistiter::svclistiter(httportmap_server &portmap,
					 const LIBCXX_NAMESPACE::http::form
					 ::parameters &params,
					 bool external_query)

	: implRef(impl::create(portmap, params, external_query)),
	  firstrow(true), lastrow(false)
{
	implRef->filter();
}

httpserverimpl::svclistiter::~svclistiter()
{
}

httpserverimpl::svclistiter::svclistiter()
{
}

bool httpserverimpl::svclistiter::operator==(const svclistiter &iter) const

{
	return (implRef.null() && iter.implRef.null());
}

bool httpserverimpl::svclistiter::operator!=(const svclistiter &iter) const

{
	return !operator==(iter);
}

httpserverimpl::svclistiter &httpserverimpl::svclistiter::operator++()

{
	implObj &i=*implRef;

	if (lastrow)
	{
		implRef=impl();
		return *this;
	}

	if (!firstrow)
	{
		++i.beg_iter;
		i.filter();
	}
	firstrow=false;

	if (i.beg_iter == i.end_iter)
	{
		lastrow=true;
		getlastrow();
		return *this;
	}

	getnextrow(i.beg_iter);
	return *this;
}

void httpserverimpl::received(const LIBCXX_NAMESPACE::http::requestimpl &req,
			      bool bodyflag)
{
	LIBCXX_NAMESPACE::sharedlock::base::shared
		lock=portmap.request_lock->create_shared();

	std::string lastcomponent;
	bool external=false;

	if (!isinternal)
	{
		if (req.get_URI().get_path().substr(0, 8) != "/portmap")
		{
			// Accept only /portmap URLs from external sources

			LIBCXX_NAMESPACE::http::fdserverimpl
				::received_unknown_request(req, bodyflag);
			return;
		}

		// When handling HTTP service directly, check peer's IP address.
		if (getconnection()->getpeername()->address() !=
		    getconnection()->getsockname()->address())
		{
			external=true;
		}
	}

	// Otherwise, this is being proxied via CGI, so check for the header.
	else if (req.find("X-External-Request") != req.end())
		external=true;

	{
		std::string path=req.get_URI().get_path();
		size_t lastslash=path.rfind('/');

		if (lastslash != std::string::npos)
			lastcomponent=path.substr(lastslash);
	}

	if (lastcomponent == "/stop" && allowstop)
	{
		LIBCXX_NAMESPACE::http::responseimpl resp(200, "Ok");

		resp.set_version(LIBCXX_NAMESPACE::http::httpver_t::http10);

		// Cheap way to get input iterators for an HTTP/1.0 response.

		std::stringstream o;

		o << "Good-bye!" << std::endl;

		send(resp, req,
		     std::istreambuf_iterator<char>(o),
		     std::istreambuf_iterator<char>());
		portmap.stop(getconnection());
		return;
	}

	const char *ct;

	if (req.get_method() == LIBCXX_NAMESPACE::http::GET &&
	    (((ct="text/xml"), lastcomponent) == "/portmap.xsl" ||
	     ((ct="text/css"), lastcomponent) == "/portmap.css"))
	{
		std::ifstream ifs((portmap.portmapdatadir + lastcomponent)
				  .c_str());

		if (ifs.is_open())
		{
			LIBCXX_NAMESPACE::http::responseimpl resp(200, "Ok");

			resp["Content-Type"]=ct;

			send(resp, req,
			     std::istreambuf_iterator<char>(ifs),
			     std::istreambuf_iterator<char>());
			return;
		}

		LIBCXX_NAMESPACE::http::responseimpl::throw_not_found();
	}

	auto form=getform(req, bodyflag);

	if (form.second)
		bodyflag=false;

	if (!form.second && req.get_method() != LIBCXX_NAMESPACE::http::GET)
	{
		LIBCXX_NAMESPACE::http::fdserverimpl
			::received_unknown_request(req, bodyflag);
		return;
	}

	LIBCXX_NAMESPACE::http::responseimpl resp(200, "Ok");

	std::vector<media_type_list_t::iterator> content_type;

	LIBCXX_NAMESPACE::http::accept_header(req)
		.find(media_type_list.begin(),
		      media_type_list.end(),
		      std::back_insert_iterator<std::vector<media_type_list_t::iterator> >(content_type));

	if (content_type.empty())
		LIBCXX_NAMESPACE::http::responseimpl::
			throw_unsupported_media_type();

	std::string content_type_str;

	LIBCXX_NAMESPACE::http::accept_header::media_type_t
		&content_media_type=**content_type.begin();

	content_media_type.to_string(std::back_insert_iterator
				    <std::string>(content_type_str));

	resp["Content-Type"]=content_type_str;
	resp.set_no_cache();

	typedef std::list<std::string> buf_t;

	buf_t buf;

	if (content_media_type.subtype == "csv")
		std::copy(svclistiter::csv(portmap, form.first, external),
			  svclistiter::csv(),
			  std::back_insert_iterator<buf_t>(buf));
	else
		std::copy(svclistiter::xml(portmap, form.first, external),
			  svclistiter::xml(),
			  std::back_insert_iterator<buf_t>(buf));

	LIBCXX_NAMESPACE::joiniterator<buf_t::iterator>
		buf_iter(buf.begin(), buf.end());

	send(resp, req, buf_iter, buf_iter.end());
}
