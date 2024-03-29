/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/logger.H"
#include "x/fileattr.H"
#include "x/glob.H"
#include "x/sysexception.H"
#include "x/locale.H"
#include "x/imbue.H"
#include "x/property_properties.H"
#include "x/property_list.H"
#include "x/singleton.H"
#include "x/threads/runfwd.H"
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <charconv>
#include <unordered_map>
#include <courier-unicode.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

#if HAVE_THR_SELF

extern "C" {
#include <sys/thr.h>
};

#endif
namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#if HAVE_THR_SELF
tid_t gettid() noexcept
{
	long y;
	thr_self(&y);

	return y;
}
#else
tid_t gettid() noexcept
{
	return syscall(SYS_gettid);
}
#endif

// std::cerr may not be initialized, yet!

#define LOGGING_FAILURE(s) do {					\
		std::ostringstream oos;				\
								\
		oos << s << std::endl;				\
								\
		std::string ss(oos.str());			\
								\
		if (write(2, ss.c_str(), ss.size()) < 0)	\
			;					\
	} while(0)


log_stringstream::log_stringstream()
{
	try {
		imbue(std::locale(std::locale(), "C", std::locale::numeric));
	} catch (const std::exception &e)
	{
	}
}

log_stringstream::~log_stringstream()
{
}

namespace {
#if 0
}
#endif

//! Superclass for log handlers

//! \internal
//! This is a superclass that defines an interface for a log handler.

class handlerObj : virtual public obj {

public:
	class fd;
	class syslogger;

	//! The default constructor
	handlerObj() noexcept=default;

	//! The default destructor
	~handlerObj()=default;

	//! Report a log message

	//! The preformatted log message gets logged by the handler.
	//!
	virtual void operator()(//! Preformatted log message
				const std::string &message,

				//! The log level
				short loglevel,

				//! Current time
				struct tm &tmbuf,

				//! Time formatter
				const std::time_put<char> &timecvt)=0;
};

//! A reference to a log handler

//! \internal
//! Log handlers are reference-counted objects.
typedef ref<handlerObj> handler;

//! A pointer to a log handler

//! \internal
//! Log handlers are reference-counted objects.
typedef ptr<handlerObj> handlerptr;

#if 0
{
#endif
}

//! Log messages to a file descriptor

//! \internal
//! This log handler implements logging messages to a file descriptor.

class handlerObj::fd : public handlerObj {

	//! The file descriptor
	int n;

	//! It's device and inode, cached
	struct stat n_stat;

	//! When the previous message was logge
	struct tm lastlogtime;

	//! The log filename pattern
	std::string logfilenamepattern;

	//! Current log filename
	std::string currentlogfilename;

	//! Rotate filename pattern
	std::string rotate;

	//! How many rotated filenames to keep
	size_t keep;

	//! Whether the destructor should close the file descriptor
	bool closeit;

	//! Mutex held while message is getting logged

	std::mutex mutex;
public:
	class filetime;
	class filetime_sort;

	//! Log to an existing file descriptor

	fd(//! The opened file descriptor
	   int nArg) noexcept;

	//! Create a log file

	fd(//! Log filename
	   const std::string &logfilenamepatternArg,

	   //! Rotate filename pattern
	   const std::string &rotateArg,

	   //! How many rotated files to keep
	   size_t keepArg) noexcept;

	//! The destructor
	~fd();

	//! Log the message to the file descriptor

	//! The preformatted message is written to the file. Write errors get
	//! ignored.
	void operator()(//! Preformatted log message
			const std::string &message,
			//! Ignored
			short loglevel,

			//! Current time
			struct tm &tmbuf,

			//! Time formatter
			const std::time_put<char> &timecvt) override;
};

handlerObj::fd::fd(int nArg) noexcept
	: n(nArg), keep(0), closeit(false)
{
}

handlerObj::fd::fd(const std::string &logfilenamepatternArg,
			   const std::string &rotateArg,
			   size_t keepArg) noexcept
	: n(-1), logfilenamepattern(logfilenamepatternArg),
	  rotate(rotateArg),
	  keep(keepArg),
	  closeit(true)
{
}

handlerObj::fd::~fd()
{
	if (closeit)
		close(n);
}

class handlerObj::fd::filetime {

public:
	std::string filename;
	time_t mtime;

	filetime(const char *filename) noexcept;
	~filetime();
};

handlerObj::fd::filetime::filetime(const char *filenameArg) noexcept
	: filename(filenameArg), mtime(0)
{
	auto st=fileattr::create(filename)->try_stat();

	if (st)
		mtime=st->st_mtime;
}

handlerObj::fd::filetime::~filetime()
{
}

class handlerObj::fd::filetime_sort {

public:
	filetime_sort()
	{
	}

	~filetime_sort()
	{
	}

	bool operator()(const filetime &a,
			const filetime &b) const noexcept
	{
		return a.mtime < b.mtime;
	}
};

void handlerObj::fd::operator()(const std::string &message,
					short loglevel,
					struct tm &tmbuf,
					const std::time_put<char> &timecvt)
{
	std::lock_guard<std::mutex> lock(mutex);

	if (n < 0 || (closeit && (tmbuf.tm_min != lastlogtime.tm_min
				  || tmbuf.tm_hour != lastlogtime.tm_hour
				  || tmbuf.tm_mday != lastlogtime.tm_mday
				  || tmbuf.tm_mon != lastlogtime.tm_mon
				  || tmbuf.tm_year != lastlogtime.tm_year)))
	{
		std::locale current_locale;

		std::ostringstream filenamebuf;

		const char *vcstr=logfilenamepattern.c_str();
		timecvt.put(std::ostreambuf_iterator<char>(filenamebuf),
			    filenamebuf, ' ', &tmbuf,
			    vcstr, vcstr+strlen(vcstr));

		std::string v=filenamebuf.str();

		struct stat stat_buf;

		if (n < 0 ||
		    v != currentlogfilename || // New log filename

		    // Log file was rotated:

		    stat(v.c_str(), &stat_buf) < 0 ||
		    stat_buf.st_dev < n_stat.st_dev ||
		    stat_buf.st_ino < n_stat.st_ino)
		{
			if (n >= 0)
			{
				close(n);
				n= -1;
			}

			if (keep > 0 && rotate.size() > 0)
			{
				typedef std::set<filetime,
						 filetime_sort> logfileset_t;

				filetime_sort sortobj;

				logfileset_t logfileset(sortobj);

				try {
					glob::create()
						->expand(rotate,
							 globObj::default_flags
							 | GLOB_NOCHECK)->
						get(std::insert_iterator
						    <logfileset_t>(logfileset,
								   logfileset
								   .end()));
				} catch (...)
				{
				}

				size_t s=logfileset.size();

				while (s > 0)
				{
					logfileset_t::iterator
						p(logfileset.begin());

					if (p->mtime == 0)
					{
						// May be caused by GLOB_NOCHECK
						logfileset.erase(p);
						--s;
						continue;
					}

					if (s <= keep)
						break;

					std::string n(p->filename);

					unlink(n.c_str());
					logfileset.erase(p);
					--s;

					size_t slash;

					while ((slash=n.rfind('/')) !=
					       std::string::npos)
					{
						n=n.substr(0, slash);
						if (rmdir(n.c_str()) < 0)
							break;
					}
				}
			}
			currentlogfilename=v;

			for (std::string::iterator b=v.begin(),
				     e=v.end(), p; (p=std::find(b, e, '/'))
				     != e; b=++p)
			{
				::mkdir(std::string(v.begin(), p).c_str(),
					0777);
			}

			n=open(v.c_str(),
			       O_WRONLY|O_APPEND|O_CREAT, 0666);

			if (n >= 0 && fstat(n, &n_stat) < 0)
			{
				close(n);
				n= -1;
			}

			if (n < 0)
			{
				LOGGING_FAILURE("Cannot create log file "
						<< v
						<< ": "
						<< strerror(errno));
				n=2;
				closeit=false;
			}
		}
	}

	lastlogtime=tmbuf;

	std::string msg=message + "\n";

	const char *p=msg.c_str();
	size_t cnt=msg.size();

	while (cnt > 0)
	{
		ssize_t rc=write(n, p, cnt);

		if (rc <= 0)
			break;

		p += rc;
		cnt -= rc;
	}
}

typedef std::unordered_map<short, int> syslog_map_t;

//! Log messages to syslog

//! \internal
//! This log handler implements logging messages to syslog.

class handlerObj::syslogger : public handlerObj {

	//! Map logging levels to syslog levels

	syslog_map_t syslog_map;

	static std::once_flag syslog_init_once;

	//! Initialize syslog by invoking openlog().

	static void syslog_init() noexcept;
public:

	//! \c syslog facility.

	static int facility;

	//! The constructor

	syslogger(//! The syslog level mapping
		  const syslog_map_t &syslog_mapArg) noexcept;

	~syslogger();

	//! Log the message to syslog

	//! The preformatted message is written to syslog.
	//!
	void operator()(//! Preformatted log message
			const std::string &message,
			//! The log level
			short loglevel,

			//! Current time
			struct tm &tmbuf,

			//! Time formatter
			const std::time_put<char> &timecvt) override;
};

std::once_flag handlerObj::syslogger::syslog_init_once;

int handlerObj::syslogger::facility=LOG_USER;

void handlerObj::syslogger::syslog_init() noexcept
{
	openlog(NULL, LOG_NDELAY, facility);
}

handlerObj::syslogger::syslogger(const syslog_map_t &syslog_mapArg) noexcept
	: syslog_map{syslog_mapArg}
{
}

handlerObj::syslogger::~syslogger()
{
}

void handlerObj::syslogger::operator()(const std::string &message,
					       short loglevel,
					       struct tm &tmbuf,
					       const std::time_put<char>
					       &timecvt)
{
	std::call_once(syslog_init_once, syslog_init);

	int sysloglevel=LOG_NOTICE;

	auto b=syslog_map.begin(),
		e=syslog_map.end(), p=e;

	while (b != e)
	{
		if (b->first <= loglevel)
		{
			if (p == e || p->first < b->first)
				p=b;
		}
		++b;
	}

	if (p != e)
		sysloglevel=p->second;

	syslog(sysloglevel, "%s", message.c_str());
}

namespace {
#if 0
}
#endif

class configmetaObj : virtual public obj {
public:
	//! Mapping of names of log levels to their numerical values
	std::unordered_map<std::string, short> loglevels;

	//! Reverse mapping
	std::unordered_map<short, std::string> loglevelsrev;

	//! Known log formats

	std::unordered_map<std::string, std::string> logformats;

	//! Known handlers

	std::unordered_map<std::string, handler> loghandlers;

	//! Default locale
	const_locale default_locale;

	configmetaObj() noexcept;
	~configmetaObj();
};
#if 0
{
#endif
}

configmetaObj::configmetaObj() noexcept
	: default_locale{locale::base::environment()}
{
}

configmetaObj::~configmetaObj()=default;

static singleton<configmetaObj> logconfig;

//! The singleton that initializes the logging subsystem

//! \internal
//! This singleton initializes the static data.
//! First, a default log configuration gets loaded. Then
//! if the environment variable LOGCONFIG is set, and the
//! given file exists, the configuration file is read from
//! LOGCONFIG.

namespace {
	class logconfig_init : virtual public obj {

	public:
		logconfig_init() noexcept;
		~logconfig_init();
	};

	singleton<logconfig_init> logger_init;
}

static std::mutex logconfig_mutex;

namespace {
#if 0
}
#endif

class logconfig_lock : std::lock_guard<std::mutex> {

public:
	const char *p;

	logconfig_lock(const char *pArg=0);
	~logconfig_lock();

	operator const char *() const { return p; }
};

logconfig_lock::logconfig_lock(const char *pArg)
	: std::lock_guard<std::mutex>(logconfig_mutex), p(pArg)
{
}

logconfig_lock::~logconfig_lock()=default;
#if 0
{
#endif
}

static bool logger_subsystem_initialized_flag=false;

bool logger::logger_subsystem_initialized() noexcept
{
	logconfig_lock llock;

	return logger_subsystem_initialized_flag;
}

std::string logger::debuglevelpropstr::tostr(short n, const const_locale &l)
{
	auto LOGCONFIG=logconfig.get();

	{
		logconfig_lock lock;

		auto iter=LOGCONFIG->loglevelsrev.find(n);

		if (iter != LOGCONFIG->loglevelsrev.end())
			return unicode::tolower(iter->second, l->charset());
	}

	std::ostringstream o;

	o << n;

	return o.str();
}

short logger::debuglevelpropstr::fromstr(const std::string &s,
					 const const_locale &l)
{
	auto LOGCONFIG=logconfig.get();

	{
		logconfig_lock lock;

		auto iter=
			LOGCONFIG->loglevels
			.find(unicode::toupper(s, l->charset()));

		if (iter != LOGCONFIG->loglevels.end())
			return iter->second;
	}

	short n=0;

	std::istringstream i(s);

	i >> n;

	if (!i.fail())
		return n;

	throw EXCEPTION("Undefined log level: " + to_string(s));
}

class logger::handlername {

public:
	std::string n;
	handlerptr h;

	handlername();
	~handlername();

	handlername(const std::string &,
		    const const_locale &);

	template<typename iter_type>
	iter_type to_string(iter_type iter, const const_locale &l)
		const
	{
		return std::copy(n.begin(), n.end(), iter);
	}

	template<typename iter_type>
	static handlername from_string(iter_type beg_iter,
				      iter_type end_iter,
				      const const_locale &localeRef)
	{
		return handlername(std::string(beg_iter, end_iter),
				   localeRef);
	}
};

logger::handlername::handlername()
{
}

logger::handlername::~handlername()
{
}

logger::handlername::handlername(const std::string &name,
				 const const_locale &localeRef)
{
	auto LOGCONFIG=logconfig.get();

	{
		logconfig_lock lock;

		auto iter=LOGCONFIG->loghandlers
			.find(unicode::toupper(name, localeRef->charset()));

		if (iter != LOGCONFIG->loghandlers.end())
		{
			n=name;
			h=iter->second;
			return;
		}
	}

	throw EXCEPTION("Undefined log level: " +
			LIBCXX_NAMESPACE::to_string(name, localeRef));
}

class logger::handlerfmt {

public:
	std::string n;
	std::string fmt;

	handlerfmt();
	~handlerfmt();

	handlerfmt(const std::string &, const const_locale &);

	template<typename iter_type>
	iter_type to_string(iter_type iter, const const_locale &l)
		const
	{
		return std::copy(n.begin(), n.end(), iter);
	}

	template<typename iter_type>
	static handlerfmt from_string(iter_type beg_iter,
				     iter_type end_iter,
				     const const_locale &localeRef)

	{
		return handlerfmt(std::string(beg_iter, end_iter),
				  localeRef);
	}

};

logger::handlerfmt::handlerfmt()
{
}

logger::handlerfmt::~handlerfmt()
{
}

logger::handlerfmt::handlerfmt(const std::string &name,
			       const const_locale &localeRef)
{
	auto LOGCONFIG=logconfig.get();

	{
		logconfig_lock lock;

		auto iter=LOGCONFIG->logformats
			.find(unicode::toupper(name, localeRef->charset()));

		if (iter != LOGCONFIG->logformats.end())
		{
			n=name;
			fmt=iter->second;
			return;
		}
	}
	throw EXCEPTION("Undefined log format: " +
			LIBCXX_NAMESPACE::to_string(name));

}

class logger::scopedestObj : virtual public obj {

public:
	property::value<handlername> name;
	property::value<handlerfmt> fmt;

	scopedestObj(const std::string &handlerpropname,
		     const std::string &handlernamestr,
		     const std::string &fmtpropname,
		     const std::string &fmtnamestr,

		     const const_locale &localeRef);
	~scopedestObj();
};

logger::scopedestObj::scopedestObj(const std::string &handlerpropname,
				   const std::string &handlernamestr,
				   const std::string &fmtpropname,
				   const std::string &fmtnamestr,
				   const const_locale &localeRef)
 : name(handlerpropname,
				handlername(handlernamestr, localeRef)),
			   fmt(fmtpropname,
			       handlerfmt(fmtnamestr, localeRef))
{
}

logger::scopedestObj::~scopedestObj()
{
}


class logger::inheritObj : virtual public obj {

public:
	inheritObj() LIBCXX_INTERNAL ;
	~inheritObj() LIBCXX_INTERNAL ;

	std::list<property::listObj::iterator> scope;

	// Key: log destination name

	// Value: pair<handler name,format name>

	typedef std::unordered_map<std::string,
				   std::pair<std::string,
					     std::string> > handlers_t;

	handlers_t handlers;

	std::list<std::string> hier;
};

logger::inheritObj::inheritObj()
{
}

logger::inheritObj::~inheritObj()
{
}

logger::scopebase::scopebase(inheritObj &inherit)
	: debuglevel(get_debuglevel_propname(inherit),
		     get_debuglevel_propvalue(inherit))
{
	for (inheritObj::handlers_t::iterator
		     b=inherit.handlers.begin(),
		     e=inherit.handlers.end();
	     b != e; ++b)
	{
		inherit.hier.push_back(b->first);

		std::string handlerpropname=
			property::combinepropname(inherit.hier);

		inherit.hier.push_back("format");

		std::string fmtpropname=
			property::combinepropname(inherit.hier);
		inherit.hier.pop_back();
		inherit.hier.pop_back();

		try {
			auto scopedest=ref<scopedestObj>
				::create(handlerpropname,
					 b->second.first,
					 fmtpropname,
					 b->second.second,
					 locale::base::environment());

			handlers.insert(std::make_pair(b->first, scopedest));

		} catch (const exception &e)
		{
			e->prepend(to_string(property::combinepropname
					    (inherit.hier)));

			LOGGING_FAILURE(e);
		}
	}
}

logger::scopebase::~scopebase()
{
}

std::string
logger::scopebase::get_debuglevel_propname (inheritObj &inherit)
{
	std::list<std::string> hier=inherit.hier;

	if (!hier.empty()) // Should end in @log::handler

		hier.back()="level";

	return property::combinepropname(hier);
}

short logger::scopebase::get_debuglevel_propvalue(inheritObj &inherit)

{
	short level=0;

	for (std::list<property::listObj::iterator>::iterator
		     b=inherit.scope.begin(), e=inherit.scope.end(); b != e; )
	{
		--e;

		property::listObj::iterator child=(*e).child("level");

		std::string name=child.propname();

		if (name.size() > 0)
		{
			auto v=child.value();

			level=logger::debuglevelpropstr
				::fromstr(v ? *v:std::string{},
					  logconfig.get()->default_locale);
			break;
		}
	}

	return level;
}

__thread logger::context *logger::context_stack=0;

logger::logger(const char *module_name) noexcept
	: scope(*scopebase::getscope(module_name)),
	  name(logconfig_lock(module_name)) // name must be updated under a lock!
{
}

logger::~logger()
{
	logconfig_lock lock;

	name=NULL;
}

#define GET_LOG_LEVEL(name) GET_LOG_LEVEL2(LOGLEVEL_ ## name)
#define GET_LOG_LEVEL2(n) GET_LOG_LEVEL3(n)
#define GET_LOG_LEVEL3(n) # n

#define LOG_NAMESPACE	LOG_NAMESPACE1(LIBCXX_NAMESPACE)
#define LOG_NAMESPACE1(n) LOG_NAMESPACE2(n)
#define LOG_NAMESPACE2(n) # n

#define DEFINE_LOG_LEVEL(name) LOG_NAMESPACE "::logger::level::" # name "=" GET_LOG_LEVEL(name) "\n"

logconfig_init::logconfig_init() noexcept
{
	ptr<property::listObj> globprops=property::listObj::global();

	const_locale global_locale{locale::base::environment()};

	auto chset=global_locale->charset();

	auto LOGCONFIG=logconfig.get();

	{
		std::string default_log_props=
			DEFINE_LOG_LEVEL(TRACE)
			DEFINE_LOG_LEVEL(DEBUG)
			DEFINE_LOG_LEVEL(INFO)
			DEFINE_LOG_LEVEL(WARNING)
			DEFINE_LOG_LEVEL(ERROR)
			DEFINE_LOG_LEVEL(FATAL)
			LOG_NAMESPACE "::logger::format::brief=%a %H:%M:%S @{class}: @{msg}\n"
			LOG_NAMESPACE "::logger::format::full=%a %H:%M:%S @@@{pid} @{thread} @{class}: @{msg}\n"
			LOG_NAMESPACE "::logger::format::interactive=@{class}: @{msg}\n"
			LOG_NAMESPACE "::logger::format::syslog=@{class}: @{msg}\n"
			LOG_NAMESPACE "::logger::handler::stderr=@2\n"
			LOG_NAMESPACE "::logger::handler::syslog=@syslog DEBUG=DEBUG INFO=INFO WARNING=WARNING ERR=ERROR CRIT=FATAL\n"
			LOG_NAMESPACE "::logger::syslog::facility=USER\n"

			LOG_NAMESPACE "::logger::log::level=error\n"
			LOG_NAMESPACE "::logger::log::handler::default=stderr\n"
			;

		if (isatty(2))
			default_log_props +=
				LOG_NAMESPACE "::logger::log::handler::default::format=interactive\n";
		else
			default_log_props +=
				LOG_NAMESPACE "::logger::log::handler::default::format=brief\n";

		globprops->load(default_log_props, false, true,
				property::errhandler::errstream(),
				global_locale);
	}

	{
		// Retrieve logger::level::NAME=LEVEL pairs

		std::unordered_map<std::string, std::string>
			xlogger_level_map;

		{
			property::listObj::iterator::children_t children;

			globprops->find(LOG_NAMESPACE "::logger::level")
				.children(children);

			for (property::listObj::iterator::children_t
				     ::iterator
				     b=children.begin(),
				     e=children.end(); b != e;
			     ++b)
			{
				std::string n=
					unicode::toupper(b->first,
							 chset);

				std::string v;

				auto propv=b->second.value();

				if (propv) v=*propv;

				std::basic_istringstream<std::string
							 ::value_type>
					parsen(v);

				short vint;

				parsen >> vint;

				if (parsen.fail())
				{
					xlogger_level_map[n]=
						unicode::toupper(v,
								 chset);
				}

				LOGCONFIG->loglevels[b->first]=vint;
				LOGCONFIG->loglevelsrev[vint]=b->first;
			}
		}

		// Perform passes to grab all aliases

		while (!xlogger_level_map.empty())
		{
			bool parsed=false;

			for (auto b=xlogger_level_map.begin(),
				     e=xlogger_level_map.end(), p=b;
			     b != e; b=p)
			{
				p=b; ++p;

				auto l=LOGCONFIG->loglevels.find(b->second);

				if (l == LOGCONFIG->loglevels.end())
					continue;

				LOGCONFIG->loglevels[b->first]=
					l->second;
				xlogger_level_map.erase(b);
				parsed=true;
			}

			if (!parsed)
				break;
		}

		for (auto b=xlogger_level_map.begin(),
			     e=xlogger_level_map.end(); b != e; ++b)
		{
			LOGGING_FAILURE("Undefined log level: "
					<< to_string(b->first));
		}
	}

	{
		// Retrieve logger::format::NAME=string pairs

		std::unordered_map<std::string, std::string>
			xlogger_format_map;

		{
			property::listObj::iterator::children_t children;

			globprops->find(LOG_NAMESPACE "::logger::format")
				.children(children);

			for (property::listObj::iterator::children_t
				     ::iterator
				     b=children.begin(),
				     e=children.end(); b != e;
			     ++b)
			{
				std::string n=
					unicode::toupper(b->first,
							 chset);

				std::string val;

				auto propval=b->second.value();

				if (propval)
					val=*propval;

				if (*val.c_str() == '$')
				{
					xlogger_format_map[n]=
						unicode::toupper(val.substr(1),
								 chset);
					continue;
				}

				LOGCONFIG->logformats[n]=val;
			}
		}

		// Perform passes to grab all aliases

		while (!xlogger_format_map.empty())
		{
			bool parsed=false;

			for (auto b=xlogger_format_map.begin(),
				     e=xlogger_format_map.end(), p=b;
			     b != e; b=p)
			{
				p=b; ++p;

				auto l=LOGCONFIG->logformats.find(b->second);

				if (l == LOGCONFIG->logformats.end())
					continue;

				LOGCONFIG->logformats[b->first]=l->second;
				xlogger_format_map.erase(b);
				parsed=true;
			}

			if (!parsed)
				break;
		}

		for (const auto &[key, value] : xlogger_format_map)
		{
			LOGGING_FAILURE("Undefined log format: "
					<< to_string(key));
		}
	}

	{
		// Retrieve logger::handler::NAME=string pairs

		std::unordered_map<std::string, std::string>
			xlogger_handler_map;

		{
			property::listObj::iterator::children_t children;

			globprops->find(LOG_NAMESPACE "::logger::handler")
				.children(children);

			for ( auto &[key, childnode] : children)
			{
				std::string n=unicode::toupper(key,
							       chset);

				std::string v;

				auto propval=childnode.value();

				if (propval)
					v=*propval;

				if (*v.c_str() == '$')
				{
					xlogger_handler_map[n]=
						unicode::toupper(v.substr(1),
								 chset);
					continue;
				}

				if (v.substr(0, 7) == "@syslog")
				{
					v=v.substr(7);

					syslog_map_t syslog_map;

					std::string::iterator
						b=v.begin(), e=v.end();

					while (b != e)
					{
						if (strchr(" \t\r\n", *b))
						{
							++b;
							continue;
						}

						std::string::iterator
							p=b;

						while (b != e)
						{
							if (strchr(" \t\r\n",
								   *b))
								break;
							++b;
						}

						std::string w(p, b);

						size_t eq=w.find('=');

						if (eq == w.npos)
							continue;

						std::string
							sysn=unicode::toupper
							(w.substr(0,
								  eq),
							 chset);

						std::string
							l=unicode::toupper
							(w.substr(++eq),
							 chset);

						auto i=LOGCONFIG->loglevels
							.find(l);

						if (i == LOGCONFIG->loglevels
						    .end())
						{
							LOGGING_FAILURE(to_string(n)
									<< ": Undefined log level: "
									<< to_string(l));
							continue;
						}

						int loglevel;

						if (sysn == "EMERG")
							loglevel=LOG_EMERG;
						else if (sysn == "ALERT")
							loglevel=LOG_ALERT;
						else if (sysn == "CRIT")
							loglevel=LOG_CRIT;
						else if (sysn == "ERR")
							loglevel=LOG_ERR;
						else if (sysn == "WARNING")
							loglevel=LOG_WARNING;
						else if (sysn == "NOTICE")
							loglevel=LOG_NOTICE;
						else if (sysn == "INFO")
							loglevel=LOG_INFO;
						else if (sysn == "DEBUG")
							loglevel=LOG_DEBUG;
						else
						{
							LOGGING_FAILURE(to_string(n)
									<< ": Undefined syslog level: "
									<< to_string(sysn));
							continue;
						}
						syslog_map[i->second]=loglevel;
					}

					auto h=ref<handlerObj::syslogger>
						::create(syslog_map);

					LOGCONFIG->loghandlers
						.insert(std::make_pair(n, h));
					continue;
				}

				if (*v.c_str() == '@')
				{
					int fd;

					std::istringstream
						ifd(v.substr(1));

					ifd >> fd;

					if (ifd.fail())
					{
						LOGGING_FAILURE(to_string(key)
								<< ": Bad log handler: "
								<< to_string(v));
						continue;
					}

					auto h=ref<handlerObj::fd>::create(fd);

					LOGCONFIG->loghandlers
						.insert(std::make_pair(n, h));
					continue;
				}

				size_t keep=0;

				std::optional<std::string>
					keepValue, rotateValue;

				property::listObj::iterator keepProp
					=childnode.child("keep");

				if (keepProp.propname().size() != 0 &&
				    (keepValue=keepProp.value()))
				{
					property::listObj::iterator rotateProp
						=childnode.child("rotate");

					if (rotateProp.propname().size() == 0 ||
					    !(rotateValue=rotateProp.value()))
					{
						LOGGING_FAILURE(to_string(key)
								<< ": missing rotate setting");
					}
					else
					{
						auto keepValue_cstr=
							keepValue->c_str();

						auto res=std::from_chars
							(keepValue_cstr,
							 keepValue_cstr
							 +keepValue->size(),
							 keep);

						if (res.ec != std::errc{} ||
						    keep == 0)
						{
							LOGGING_FAILURE(to_string(key) <<
									": invalid keep setting");
							keep=0;
						}
					}
				}

				auto h=ref<handlerObj::fd>
					::create(to_string(v, global_locale),
						 to_string(rotateValue ?
							   *rotateValue
							   : std::string{},
							   global_locale),
						 keep);

				LOGCONFIG->loghandlers
					.insert(std::make_pair(n, h));
			}
		}

		// Perform passes to grab all aliases

		while (!xlogger_handler_map.empty())
		{
			bool parsed=false;

			for (auto b=xlogger_handler_map.begin(),
				     e=xlogger_handler_map.end(), p=b;
			     b != e; b=p)
			{
				p=b; ++p;

				auto l=LOGCONFIG->loghandlers
					.find(b->second);

				if (l == LOGCONFIG->loghandlers.end())
					continue;

				LOGCONFIG->loghandlers
					.insert(std::make_pair(b->first,
							       l->second));
				xlogger_handler_map.erase(b);
				parsed=true;
			}

			if (!parsed)
				break;
		}

		for (auto &n:xlogger_handler_map)
		{
			LOGGING_FAILURE("Undefined log handler: "
					<< to_string(n.first));
		}
	}

	{
		auto facility=globprops->find(LOG_NAMESPACE
					      "::logger::syslog::facility");

		if (facility.propname().size() > 0)
		{
			auto propval=facility.value();

			std::string v=
				unicode::toupper(propval ? *propval
						 : std::string{},
						 chset);


			static const char * const facilities[]={
				"AUTHPRIV",
				"CRON",
				"DAEMON",
				"FTP",
				"KERN",
				"LOCAL0",
				"LOCAL1",
				"LOCAL2",
				"LOCAL3",
				"LOCAL4",
				"LOCAL5",
				"LOCAL6",
				"LOCAL7",
				"MAIL",
				"NEWS",
				"USER",
				"UUCP"};

			static const int facilitiesn[]={
				LOG_AUTHPRIV,
				LOG_CRON,
				LOG_DAEMON,
				LOG_FTP,
				LOG_KERN,
				LOG_LOCAL0,
				LOG_LOCAL1,
				LOG_LOCAL2,
				LOG_LOCAL3,
				LOG_LOCAL4,
				LOG_LOCAL5,
				LOG_LOCAL6,
				LOG_LOCAL7,
				LOG_MAIL,
				LOG_NEWS,
				LOG_USER,
				LOG_UUCP};

			size_t n;

			for (n=0; n<sizeof(facilities)/sizeof(facilities[0]);
			     n++)
				if (v == facilities[n])
					break;

			if (n >= sizeof(facilities)/sizeof(facilities[0]))
				LOGGING_FAILURE("Unknown syslog facility: "
						<< to_string(v));
			else
				handlerObj::syslogger::facility=
					facilitiesn[n];
		}
	}

	{
		logconfig_lock llock;

		logger_subsystem_initialized_flag=true;
	}
}

logconfig_init::~logconfig_init()
{
	logconfig_lock llock;

	logger_subsystem_initialized_flag=false;
}

ptr<logger::inheritObj> logger::scopebase::getscope(const std::string &name)

{
	logger_init.get();

	ptr<inheritObj> h(ptr<inheritObj>::create());

	ptr<property::listObj> globprops=property::listObj::global();

	const_locale global{locale::base::environment()};

	property::listObj::iterator iter=
		globprops->find(LOG_NAMESPACE "::logger::log", global);

	if (iter.propname().size() > 0)
		h->scope.push_back(iter);

	property::parsepropname(name.begin(), name.end(), h->hier, global);

	iter=globprops->root();

	for (std::list<std::string>::iterator b=h->hier.begin(),
		     e=h->hier.end(); b != e; ++b)
	{
		iter=iter.child(*b, global);

		if (iter.propname().size() == 0)
			break; // No more properties at this hierarchy

		property::listObj::iterator atlog=iter.child("@log", global);

		if (atlog.propname().size() > 0)
			h->scope.push_back(atlog);
	}

	for (auto &i:h->scope)
	{
		{
			std::string inherit=i.child("inherit",
						       global)
				.propname();

			if (inherit.size() > 0)
			{
				property::value<bool> inheritflag(inherit,
								  true);

				if (!inheritflag.get())
					h->handlers.clear();
			}
		}

		property::listObj::iterator::children_t children;

		{
			property::listObj::iterator
				handlers=i.child("handler", global);

			if (handlers.propname().size() == 0)
				continue;

			handlers.children(children);
		}

		for (auto &[name, child]:children)
		{
			auto v=child.value();

			if (v)
				h->handlers[name].first=*v;

			property::listObj::iterator format=
				child.child("format", global);

			if (format.propname().size() == 0)
				continue;

			v=format.value();

			if (v)
				h->handlers[name].second=*v;
		}

	}

	h->hier.push_back("@log");
	h->hier.push_back("handler");

	std::string propbase=property::combinepropname(h->hier);

	for (inheritObj::handlers_t::iterator
		     b=h->handlers.begin(),
		     e=h->handlers.end(), p; (p=b) != e; )
	{
		++b;

		if (p->second.first.size() == 0)
		{
			LOGGING_FAILURE("Missing definition of "
					<< to_string(propbase, global)
					<< "::" << to_string(p->first, global));
			h->handlers.erase(p);
			continue;
		}

		if (p->second.second.size() == 0)
		{
			LOGGING_FAILURE("Missing definition of "
					<< to_string(propbase, global)
					<< "::"
					<< to_string(p->first, global)
					<< "::format");
			h->handlers.erase(p);
			continue;
		}
	}

	return h;
}

void logger::operator()(const std::string &s, short loglevel) const noexcept
{
	{
		logconfig_lock lock;

		if (!name)
		{
			// An exception occured during app startup before this
			// logger was initialized

			std::string m=s+"\n";
			if (write(2, m.c_str(), m.size()) < 0)
				; // Ignored, too bad.
			return;
		}
	}

	time_t t=time(NULL);
	struct tm tmbuf;

	localtime_r(&t, &tmbuf);

	std::locale current_locale;
	const std::time_put<char> &timecvt=
		std::use_facet<std::time_put<char> >(current_locale);

	std::unordered_map<std::string, std::string> vars;

	vars["class"]=name ? name:"(unknown)";

	vars["thread"]=run_async::thread_name;

	{
		std::ostringstream pidid;

		pidid << gettid();

		if (context_stack)
			pidid << ":" << context_stack->getContext();

		vars["pid"]=pidid.str();
	}

	auto LOGCONFIG=logconfig.get();

	vars["level"]=
		to_string(unicode::tolower
			 (logger::debuglevelpropstr::tostr(loglevel,
							   LOGCONFIG
							   ->default_locale)
			  ), LOGCONFIG->default_locale);

	std::string::const_iterator b=s.begin(), e=s.end();

	while (b != e)
	{
		std::string::const_iterator p=b;

		b=std::find(b, e, '\n');

		if (p != b)
		{
			vars["msg"]=std::string(p, b);
			domsg(vars, timecvt, tmbuf, loglevel);
		}

		if (b != e)
			++b;
	}
}

void logger::domsg(std::unordered_map<std::string, std::string> &vars,
		   const std::time_put<char> &timecvt,
		   struct tm &tmbuf,
		   short loglevel) const noexcept
{
	std::list<std::pair<handler, std::string> > msg_list;

	const_locale global=locale::base::environment();

	for (scopebase::handlers_t::const_iterator hb=scope.handlers.begin(),
		     he=scope.handlers.end(); hb != he; ++hb)
	{
		scopedestObj &hep=*hb->second;

		std::string fmtstr=to_string(hep.fmt.get().fmt, global);

		const char *fmt=fmtstr.c_str();

		std::ostringstream o;

		const char *fmtstart=fmt;

		while (*fmt)
		{
			if (*fmt == '%')
			{
				o.write(fmtstart, fmt-fmtstart);

				if (!*++fmt)
				{
					fmtstart=fmt;
					break;
				}

				if (*fmt == '%')
				{
					o << *fmt++;
					fmtstart=fmt;
					continue;
				}

				char pattern[2];

				pattern[0]='%';
				pattern[1]=*fmt++;
				timecvt.put(std::ostreambuf_iterator<char>(o),
					    o, ' ', &tmbuf, pattern, pattern+2);
				fmtstart=fmt;
				continue;
			}

			if (*fmt == '@')
			{
				o.write(fmtstart, fmt-fmtstart);

				if (!*++fmt)
				{
					fmtstart=fmt;
					break;
				}

				if (*fmt == '@')
				{
					o << *fmt++;
					fmtstart=fmt;
					continue;
				}

				if (*fmt != '{')
				{
					fmtstart=fmt;
					continue;
				}

				const char *p=++fmt;

				while (*fmt)
				{
					if (*fmt == '}')
						break;
					++fmt;
				}

				std::string n(p, fmt);

				if (*fmt)
					++fmt;

				fmtstart=fmt;

				auto sp=vars.find(n);

				if (sp != vars.end())
					o << sp->second;
				continue;
			}
			++fmt;
		}

		o.write(fmtstart, fmt-fmtstart);

		msg_list.push_back(std::make_pair(hep.name.get().h,
						  o.str()));
	}

	while (!msg_list.empty())
	{
		(*msg_list.front().first)(msg_list.front().second, loglevel,
					  tmbuf, timecvt);

		msg_list.pop_front();
	}
}

logger::context::context(const std::string &nameArg) noexcept : name(nameArg)
{
	logconfig_lock llock;

	next_context=context_stack;

	context_stack=this;
}

logger::context::~context()
{
	logconfig_lock llock;

	context_stack=next_context;
}

std::string logger::context::getContext() noexcept
{
	if (next_context)
		return next_context->getContext() + "/" + name;

	return name;
}

#if 0
{
#endif
}
