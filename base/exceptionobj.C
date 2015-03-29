/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/exceptionobj.H"
#include "x/logger.H"

#if HAVE_BACKTRACE
#include <execinfo.h>
#endif

#if HAVE_LIBUNWIND
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <dlfcn.h>
#endif

#include <cxxabi.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

static property::value<bool> show_filename(LIBCXX_NAMESPACE_WSTR
					   L"::exception::fileline", false);

exceptionObj::exceptionObj() noexcept
{
#if HAVE_BACKTRACE
	int n=32;

	while (1)
	{
		void *funcaddrs[n];

		int nn=::backtrace(funcaddrs, n);

		if (nn == n)
		{
			n *= 2;
			continue;
		}

		std::ostringstream o;

		char **syms=backtrace_symbols(funcaddrs, nn);

		if (!syms)
			break;

		std::string syms_s[nn];

		for (n=0; n<nn; n++)
		{
			try {
				syms_s[n]=syms[n];
			} catch (...)
			{
				free(syms);
				return;
			}
		}

		free(syms);

		for (n=0; n<nn; n++)
		{
			std::string::iterator b=syms_s[n].begin();
			std::string::iterator e=syms_s[n].end();

			std::string::iterator p=std::find(b, e, ' ');
			std::string::iterator q;

			// glibc: {exename}({symbol}+{0xHEXOFFSET}){space}...

			if (p == e)
				continue;

			if ((q=std::find(b, p, '(')) != p)
			{
				if (*--p != ')')
					continue;
				++q;
			}

			p=std::find(q, p, '+');

			std::string mangled_name(q, p);

			int status;
			char *t=abi::__cxa_demangle(mangled_name.c_str(),
						    0, 0, &status);

			if (status == 0)
			{
				try {
					syms_s[n]=std::string(syms_s[n].begin(),
							      q) + t +
						std::string(p, syms_s[n].end());
					free(t);
				} catch (...)
				{
					free(t);
				}
			}
		}

		size_t buf_s=1;

		for (n=0; n<nn; n++)
		{
			buf_s += syms_s[n].size()+1;
		}

		char buf[buf_s];

		buf[0]=0;

		// Toss away the last stack frame -- this function.

		for (n=1; n<nn; n++)
		{
			strcat(strcat(buf, syms_s[n].c_str()), "\n");
		}

		backtrace=buf;
		break;
	}
#endif

#if HAVE_LIBUNWIND

	std::ostringstream o;
	o << std::hex << std::setfill('0');

	unw_cursor_t cursor;
	unw_context_t uc;
	unw_word_t ip;

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);
	while (unw_step(&cursor) > 0)
	{
		Dl_info info;

		unw_get_reg(&cursor, UNW_REG_IP, &ip);

		if (dladdr((void *)ip, &info))
		{
			unw_word_t diff = ip - (unw_word_t)info.dli_saddr;

			o << info.dli_fname << "(";

			int status;
			char *t=abi::__cxa_demangle(info.dli_sname,
						    0, 0, &status);

			if (t)
			{
				o << t;
				free(t);
			}
			else
			{
				o << info.dli_sname;
			}

			if (diff)
			{
				o << "+0x"
				  << std::setw(0)
				  << diff;
			}
			o << ")";
		}
		else
		{
			o << "<unknown>";
		}

		o << " [0x" << std::setw(sizeof(ip)*2) << std::hex
		  << ip << "]" << std::endl;
	}

	backtrace=o.str();
#endif
}

exceptionObj::~exceptionObj() noexcept
{
}

void exceptionObj::fileline(const char *file, int line) noexcept
{
	if (show_filename.getValue())
		(std::ostringstream &)*this << file << "(" << line << "): ";
}

void exceptionObj::save() noexcept
{
	what_buf=str() + "\n\n" + backtrace;
}

const char *exceptionObj::what() noexcept
{
	return what_buf.c_str();
}

void exceptionObj::prepend(const std::string &s)
{
	str(s + str());
	save();
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::exception, log_exception_log);

void exceptionObj::log() noexcept
{
	save();

	if (!logger::logger_subsystem_initialized())
	{
		// Exception at startup/shutdown. Bleah.

		std::cerr << operator std::string() << std::endl;
		return;
	}

	LOG_FUNC_SCOPE(log_exception_log);

	LOG_DEBUG( operator std::string());
	LOG_TRACE( backtrace );
}

void exceptionObj::caught() noexcept
{
	if (!logger::logger_subsystem_initialized())
	{
		// Exception at startup/shutdown. Bleah.

		std::cerr << operator std::string() << std::endl;
		return;
	}

	LOG_FUNC_SCOPE(log_exception_log);

	LOG_ERROR( operator std::string());
}

void exceptionObj::rethrow()
{
	throw exception(this);
}

#if 0
{
#endif
}
