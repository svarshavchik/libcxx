/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/logger.H"
#include "x/fileattr.H"
#include "x/dir.H"
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
#include <algorithm>
#include <iterator>
#include <cstring>
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

//! Superclass for log handlers

//! \internal
//! This is a superclass that defines an interface for a log handler.

class logger::handlerObj : virtual public obj {

public:
	class LIBCXX_INTERNAL fd;
	class LIBCXX_INTERNAL syslogger;

	//! The default constructor
	handlerObj() noexcept;

	//! The default destructor
	~handlerObj();

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

logger::handlerObj::handlerObj() noexcept
{
}

logger::handlerObj::~handlerObj()
{
}

//! Log messages to a file descriptor

//! \internal
//! This log handler implements logging messages to a file descriptor.

class logger::handlerObj::fd : public logger::handlerObj {

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
			const std::time_put<char> &timecvt);
};

logger::handlerObj::fd::fd(int nArg) noexcept
	: n(nArg), keep(0), closeit(false)
{
}

logger::handlerObj::fd::fd(const std::string &logfilenamepatternArg,
			   const std::string &rotateArg,
			   size_t keepArg) noexcept
	: n(-1), logfilenamepattern(logfilenamepatternArg),
	  rotate(rotateArg),
	  keep(keepArg),
	  closeit(true)
{
}

logger::handlerObj::fd::~fd()
{
	if (closeit)
		close(n);
}

class logger::handlerObj::fd::filetime {

public:
	std::string filename;
	time_t mtime;

	filetime(const char *filename) noexcept;
	~filetime();
};

logger::handlerObj::fd::filetime::filetime(const char *filenameArg) noexcept
	: filename(filenameArg), mtime(0)
{
	auto st=fileattr::create(filename)->try_stat();

	if (st)
		mtime=st->st_mtime;
}

logger::handlerObj::fd::filetime::~filetime()
{
}

class logger::handlerObj::fd::filetime_sort {

public:
	filetime_sort()
	{
	}

	~filetime_sort()
	{
	}

	bool operator()(const filetime &a,
			const filetime &b) noexcept
	{
		return a.mtime < b.mtime;
	}
};

void logger::handlerObj::fd::operator()(const std::string &message,
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

//! Log messages to syslog

//! \internal
//! This log handler implements logging messages to syslog.

class logger::handlerObj::syslogger : public logger::handlerObj {

	//! Map logging levels to syslog levels

	std::map<short, int> syslog_map;

	static std::once_flag syslog_init_once;

	//! Initialize syslog by invoking openlog().

	static void syslog_init() noexcept;
public:

	//! \c syslog facility.

	static int facility;

	//! The constructor

	syslogger(//! The syslog level mapping
		  const std::map<short, int> &syslog_mapArg) noexcept;

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
			const std::time_put<char> &timecvt);
};

std::once_flag logger::handlerObj::syslogger::syslog_init_once;

int logger::handlerObj::syslogger::facility=LOG_USER;

void logger::handlerObj::syslogger::syslog_init() noexcept
{
	openlog(NULL, LOG_NDELAY, facility);
}

logger::handlerObj::syslogger::syslogger(const std::map<short, int> &syslog_mapArg) noexcept
	: syslog_map(syslog_mapArg)
{
}

logger::handlerObj::syslogger::~syslogger()
{
}

void logger::handlerObj::syslogger::operator()(const std::string &message,
					       short loglevel,
					       struct tm &tmbuf,
					       const std::time_put<char>
					       &timecvt)
{
	std::call_once(syslog_init_once, syslog_init);

	int sysloglevel=LOG_NOTICE;

	std::map<short, int>::iterator b=syslog_map.begin(),
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


//! Temporary state kept while reading configuration data

//! \internal
//! This objects holds some internal data when the configuration
//! file is getting parsed.

class logger::initstate {

public:
	initstate();

	//! Startup time
	struct tm tmbuf;
};

logger::initstate::initstate()
{
}

class logger::configmeta {
public:
	//! Mapping of names of log levels to their numerical values
	std::map<std::string, short> loglevels;

	//! Reverse mapping
	std::map<short, std::string> loglevelsrev;

	//! Known log formats

	std::map<std::string, std::string> logformats;

	//! Known handlers

	std::map<std::string, handler> loghandlers;

	//! Default locale
	locale default_locale;

	configmeta() noexcept;
	~configmeta();
};

logger::configmeta::configmeta() noexcept : default_locale(locale::base::environment())
{
}

logger::configmeta::~configmeta()
{
}

logger::configmeta *logger::logconfig=0;

class logger::logconfig_init : virtual public obj {

public:
	logconfig_init() noexcept;
	~logconfig_init();
};

singleton<logger::logconfig_init> logger::logger_init;

static std::mutex logconfig_mutex;

class logger::logconfig_lock : std::lock_guard<std::mutex> {

public:
	const char *p;

	logconfig_lock(const char *pArg=0);
	~logconfig_lock();

	operator const char *() const { return p; }
};

logger::logconfig_lock::logconfig_lock(const char *pArg)
	: std::lock_guard<std::mutex>(logconfig_mutex), p(pArg)
{
}

logger::logconfig_lock::~logconfig_lock()
{
}

static bool logger_subsystem_initialized_flag=false;

bool logger::logger_subsystem_initialized() noexcept
{
	logconfig_lock llock;

	return logger_subsystem_initialized_flag;
}

std::string logger::debuglevelpropstr::tostr(short n, const const_locale &l)
{
	{
		logger::logconfig_lock lock;

		std::map<short, std::string>::iterator iter=
			logger::logconfig->loglevelsrev.find(n);

		if (iter != logger::logconfig->loglevelsrev.end())
			return unicode::tolower(iter->second, l->charset());
	}

	std::ostringstream o;

	o << n;

	return o.str();
}

short logger::debuglevelpropstr::fromstr(const std::string &s,
					 const const_locale &l)
{
	{
		logger::logconfig_lock lock;

		std::map<std::string, short>::iterator iter=
			logger::logconfig->loglevels
			.find(unicode::toupper(s, l->charset()));

		if (iter != logger::logconfig->loglevels.end())
			return iter->second;
	}

	short n=0;

	std::istringstream i(s);

	i >> n;

	if (!i.fail())
		return n;

	throw EXCEPTION("Undefined log level: " + tostring(s));
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
	iter_type toString(iter_type iter, const const_locale &l)
		const
	{
		return std::copy(n.begin(), n.end(), iter);
	}

	template<typename iter_type>
	static handlername fromString(iter_type beg_iter,
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
	{
		logger::logconfig_lock lock;

		std::map<std::string, handler>::iterator
			iter=logconfig->loghandlers
			.find(unicode::toupper(name, localeRef->charset()));

		if (iter != logconfig->loghandlers.end())
		{
			n=name;
			h=iter->second;
			return;
		}
	}

	throw EXCEPTION("Undefined log level: " + tostring(name, localeRef));
}

class logger::handlerfmt {

public:
	std::string n;
	std::string fmt;

	handlerfmt();
	~handlerfmt();

	handlerfmt(const std::string &, const const_locale &);

	template<typename iter_type>
	iter_type toString(iter_type iter, const const_locale &l)
		const
	{
		return std::copy(n.begin(), n.end(), iter);
	}

	template<typename iter_type>
	static handlerfmt fromString(iter_type beg_iter,
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
	{
		logger::logconfig_lock lock;

		std::map<std::string, std::string>::iterator
			iter=logconfig->logformats
			.find(unicode::toupper(name, localeRef->charset()));

		if (iter != logconfig->logformats.end())
		{
			n=name;
			fmt=iter->second;
			return;
		}
	}
	throw EXCEPTION("Undefined log format: " + tostring(name));

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

	typedef std::map<std::string,
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
			e->prepend(tostring(property::combinepropname
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
			level=logger::debuglevelpropstr
				::fromstr(child.value().first,
					  logconfig->default_locale);
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

logger::logconfig_init::logconfig_init() noexcept
{
	initstate s;

	ptr<property::listObj> globprops=property::listObj::global();

	locale global_locale(locale::base::environment());

	auto chset=global_locale->charset();

	if (logconfig == NULL)
		logconfig=new logger::configmeta;

	{
		time_t t=time(NULL);

		localtime_r(&t, &s.tmbuf);

		static const char default_log_props[]=
			DEFINE_LOG_LEVEL(TRACE)
			DEFINE_LOG_LEVEL(DEBUG)
			DEFINE_LOG_LEVEL(INFO)
			DEFINE_LOG_LEVEL(WARNING)
			DEFINE_LOG_LEVEL(ERROR)
			DEFINE_LOG_LEVEL(FATAL)
			LOG_NAMESPACE "::logger::format::brief=%a %H:%M:%S @{class}: @{msg}\n"
			LOG_NAMESPACE "::logger::format::full=%a %H:%M:%S @@@{pid} @{thread} @{class}: @{msg}\n"
			LOG_NAMESPACE "::logger::format::syslog=@{class}: @{msg}\n"
			LOG_NAMESPACE "::logger::handler::stderr=@2\n"
			LOG_NAMESPACE "::logger::handler::syslog=@syslog DEBUG=DEBUG INFO=INFO WARNING=WARNING ERR=ERROR CRIT=FATAL\n"
			LOG_NAMESPACE "::logger::syslog::facility=USER\n"

			LOG_NAMESPACE "::logger::log::level=error\n"
			LOG_NAMESPACE "::logger::log::handler::default=stderr\n"
			LOG_NAMESPACE "::logger::log::handler::default::format=brief\n";

		globprops->load(default_log_props, false, true,
				property::errhandler::errstream(),
				global_locale);
	}

	{
		// Retrieve logger::level::NAME=LEVEL pairs

		std::map<std::string, std::string>
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

				std::string v=b->second.value().first;

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

				logconfig->loglevels[b->first]=vint;
				logconfig->loglevelsrev[vint]=b->first;
			}
		}

		// Perform passes to grab all aliases

		while (!xlogger_level_map.empty())
		{
			bool parsed=false;

			for (std::map<std::string, std::string>
				     ::iterator
				     b=xlogger_level_map.begin(),
				     e=xlogger_level_map.end(), p;
			     b != e; b=p)
			{
				p=b; ++p;

				std::map<std::string, short>::iterator
					l=logconfig->loglevels
					.find(b->second);

				if (l == logconfig->loglevels.end())
					continue;

				logconfig->loglevels[b->first]=
					l->second;
				xlogger_level_map.erase(b);
				parsed=true;
			}

			if (!parsed)
				break;
		}

		for (std::map<std::string, std::string>
			     ::iterator
			     b=xlogger_level_map.begin(),
			     e=xlogger_level_map.end(); b != e; ++b)
		{
			LOGGING_FAILURE("Undefined log level: "
					<< tostring(b->first));
		}
	}

	{
		// Retrieve logger::format::NAME=string pairs

		std::map<std::string, std::string>
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

				std::string val=b->second.value().first;

				if (*val.c_str() == '$')
				{
					xlogger_format_map[n]=
						unicode::toupper(val.substr(1),
								 chset);
					continue;
				}

				logconfig->logformats[n]=val;
			}
		}

		// Perform passes to grab all aliases

		while (!xlogger_format_map.empty())
		{
			bool parsed=false;

			for (std::map<std::string, std::string>
				     ::iterator
				     b=xlogger_format_map.begin(),
				     e=xlogger_format_map.end(), p;
			     b != e; b=p)
			{
				p=b; ++p;

				std::map<std::string, std::string>
					::iterator
					l=logconfig->logformats
					.find(b->second);

				if (l == logconfig->logformats.end())
					continue;

				logconfig->logformats[b->first]=l->second;
				xlogger_format_map.erase(b);
				parsed=true;
			}

			if (!parsed)
				break;
		}

		for (std::map<std::string, std::string>
			     ::iterator
			     b=xlogger_format_map.begin(),
			     e=xlogger_format_map.end(); b != e; ++b)
		{
			LOGGING_FAILURE("Undefined log format: "
					<< tostring(b->first));
		}
	}

	{
		// Retrieve logger::handler::NAME=string pairs

		std::map<std::string, std::string>
			xlogger_handler_map;

		{
			property::listObj::iterator::children_t children;

			globprops->find(LOG_NAMESPACE "::logger::handler")
				.children(children);

			for (property::listObj::iterator::children_t
				     ::iterator
				     b=children.begin(),
				     e=children.end(); b != e;
			     ++b)
			{
				std::string n=unicode::toupper(b->first,
							       chset);

				std::string v=b->second.value().first;

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

					std::map<short, int> syslog_map;

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

						std::map<std::string,
							 short>::iterator i
							=logconfig->loglevels
							.find(l);

						if (i == logconfig->loglevels
						    .end())
						{
							LOGGING_FAILURE(tostring(n)
									<< ": Undefined log level: "
									<< tostring(l));
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
							LOGGING_FAILURE(tostring(n)
									<< ": Undefined syslog level: "
									<< tostring(sysn));
							continue;
						}
						syslog_map[i->second]=loglevel;
					}

					auto h=ref<handlerObj::syslogger>
						::create(syslog_map);

					logconfig->loghandlers
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
						LOGGING_FAILURE(tostring(b->first)
								<< ": Bad log handler: "
								<< tostring(v));
						continue;
					}

					auto h=ref<handlerObj::fd>::create(fd);

					logconfig->loghandlers
						.insert(std::make_pair(n, h));
					continue;
				}

				size_t keep=0;
				std::pair<std::string, bool>
					keepValue, rotateValue;

				property::listObj::iterator keepProp
					=b->second.child("keep");

				if (keepProp.propname().size() != 0 &&
				    !(keepValue=keepProp.value()).second)
				{
					property::listObj::iterator rotateProp
						=b->second.child("rotate");

					if (rotateProp.propname().size() == 0 ||
					    (rotateValue=rotateProp.value())
					    .second)
					{
						LOGGING_FAILURE(tostring(b->first)
								<< ": missing rotate setting");
					}
					else
					{
						std::istringstream
							i(keepValue.first);

						i >> keep;

						if (i.fail() || keep == 0)
						{
							LOGGING_FAILURE(tostring(b->first) <<
									": invalid keep setting");
							keep=0;
						}
					}
				}

				auto h=ref<handlerObj::fd>
					::create(tostring(v, global_locale),
						 tostring(rotateValue.first,
							  global_locale),
						 keep);

				logconfig->loghandlers
					.insert(std::make_pair(n, h));
			}
		}

		// Perform passes to grab all aliases

		while (!xlogger_handler_map.empty())
		{
			bool parsed=false;

			for (std::map<std::string, std::string>
				     ::iterator
				     b=xlogger_handler_map.begin(),
				     e=xlogger_handler_map.end(), p;
			     b != e; b=p)
			{
				p=b; ++p;

				std::map<std::string, handler>::iterator
					l=logconfig->loghandlers
					.find(b->second);

				if (l == logconfig->loghandlers.end())
					continue;

				logconfig->loghandlers
					.insert(std::make_pair(b->first,
							       l->second));
				xlogger_handler_map.erase(b);
				parsed=true;
			}

			if (!parsed)
				break;
		}

		for (std::map<std::string, std::string>
			     ::iterator
			     b=xlogger_handler_map.begin(),
			     e=xlogger_handler_map.end(); b != e; ++b)
		{
			LOGGING_FAILURE("Undefined log handler: "
					<< tostring(b->first));
		}
	}

	{
		property::listObj::iterator
			facility=globprops->find(LOG_NAMESPACE
						 "::logger::syslog::facility");

		if (facility.propname().size() > 0)
		{
			std::string v=
				unicode::toupper(facility.value().first,
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
						<< tostring(v));
			else
				logger::handlerObj::syslogger::facility=
					facilitiesn[n];
		}
	}

	{
		logconfig_lock llock;

		logger_subsystem_initialized_flag=true;
	}
}

logger::logconfig_init::~logconfig_init()
{
	logconfig_lock llock;

	if (logconfig)
	{
		delete logconfig;
		logconfig=0;
	}
	logger_subsystem_initialized_flag=false;
}

ptr<logger::inheritObj> logger::scopebase::getscope(const std::string &name)

{
	logger_init.get();

	ptr<inheritObj> h(ptr<inheritObj>::create());

	ptr<property::listObj> globprops=property::listObj::global();

	locale global(locale::base::environment());

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

	for (std::list<property::listObj::iterator>::iterator
		     b=h->scope.begin(), e=h->scope.end(); b != e; ++b)
	{
		{
			std::string inherit=(*b).child("inherit",
						       global)
				.propname();

			if (inherit.size() > 0)
			{
				property::value<bool> inheritflag(inherit,
								  true);

				if (!inheritflag.getValue())
					h->handlers.clear();
			}
		}

		property::listObj::iterator::children_t children;

		{
			property::listObj::iterator
				handlers=(*b).child("handler", global);

			if (handlers.propname().size() == 0)
				continue;

			handlers.children(children);
		}

		for (property::listObj::iterator::children_t::iterator
			     b=children.begin(), e=children.end();
		     b != e; ++b)
		{
			std::pair<std::string, bool>
				v=b->second.value();

			if (!v.second)
				h->handlers[b->first].first=v.first;

			property::listObj::iterator format=
				b->second.child("format", global);

			if (format.propname().size() == 0)
				continue;

			v=format.value();

			if (!v.second)
				h->handlers[b->first].second=v.first;
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
					<< tostring(propbase, global)
					<< "::" << tostring(p->first, global));
			h->handlers.erase(p);
			continue;
		}

		if (p->second.second.size() == 0)
		{
			LOGGING_FAILURE("Missing definition of "
					<< tostring(propbase, global)
					<< "::"
					<< tostring(p->first, global)
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

	std::map<std::string, std::string> vars;

	vars["class"]=name ? name:"(unknown)";

	vars["thread"]=run_async::thread_name;

	{
		std::ostringstream pidid;

		pidid << gettid();

		if (context_stack)
			pidid << ":" << context_stack->getContext();

		vars["pid"]=pidid.str();
	}

	vars["level"]=
		tostring(unicode::tolower
			 (logger::debuglevelpropstr::tostr(loglevel,
							   logger::logconfig
							   ->default_locale)
			  ), logger::logconfig->default_locale);

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

void logger::domsg(std::map<std::string, std::string> &vars,
		   const std::time_put<char> &timecvt,
		   struct tm &tmbuf,
		   short loglevel) const noexcept
{
	std::list<std::pair<logger::handler, std::string> > msg_list;

	locale global=locale::base::environment();

	for (scopebase::handlers_t::const_iterator hb=scope.handlers.begin(),
		     he=scope.handlers.end(); hb != he; ++hb)
	{
		scopedestObj &hep=*hb->second;

		std::string fmtstr=tostring(hep.fmt.getValue().fmt, global);

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

				std::map<std::string,
					 std::string>::iterator
					sp=vars.find(n);

				if (sp != vars.end())
					o << sp->second;
				continue;
			}
			++fmt;
		}

		o.write(fmtstart, fmt-fmtstart);

		msg_list.push_back(std::make_pair(hep.name.getValue().h,
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
