/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <iostream>

#include "libcxx_config.h"

int main()
{
	static const char * const signalnames[]={
		"SIGABRT",
		"SIGALRM",
		"SIGBUS",
		"SIGCHLD",
		"SIGCONT",
		"SIGFPE",
		"SIGHUP",
		"SIGILL",
		"SIGINT",
		"SIGKILL",
		"SIGPIPE",
		"SIGQUIT",
		"SIGSEGV",
		"SIGSTOP",
		"SIGTERM",
		"SIGTSTP",
		"SIGTTIN",
		"SIGTTOU",
		"SIGUSR1",
		"SIGUSR2",
		"SIGPOLL",
		"SIGPROF",
		"SIGSYS",
		"SIGTRAP",
		"SIGURG",
		"SIGVTALRM",
		"SIGXCPU",
		"SIGXFSZ",

		"SIGIOT",
		"SIGEMT",
		"SIGSTKFLT",
		"SIGIO",
		"SIGCLD",
		"SIGPWR",
		"SIGINFO",
		"SIGLOST",
		"SIGWINCH",
		"SIGUNUSED"
	};

	for (size_t i=0; i<sizeof(signalnames)/sizeof(signalnames[0]); ++i)
	{
		std::cout << "#ifdef " << signalnames[i] << std::endl
			  << "  make(" << signalnames[i] << ", \""
			  << signalnames[i] << "\");" << std::endl
			  << "#endif" << std::endl;
	}
	return 0;
}
