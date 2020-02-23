/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_forkexec_H
#define x_forkexec_H

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>

#include <x/fd.H>
#include <x/exception.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Execute another process via fork and exec.

//! Takes care of the dirty work in setting up fork/exec, with file descriptor
//! redirecting.
//!
//! Pass the argv array or a variadic list to the constructor. argv[0] is
//! the program to execute (or use program() to explicitly set it). PATH gets
//! searched unless it's specified (the execvp library call gets invoked).
//!
//! Use various methods to redirect stdin, stdout, stderr, or any other file
//! descriptor. Finally, call spawn(), spawn_detached(), system(), or even
//! exec() (no forks here). They all, eventually, wind up calling exec(2).
//! Noteworthy: all of these will throw an exception if the process cannot
//! be executed, for some reason.

class forkexec {

	//! Collected argument vector.
	std::vector<std::string> argv;

	//! Explicitly specified path to execute.
	std::string exe;

	//! Holding area for file descriptors to get redirected.
	std::map<int, fd> filedescs;

public:

	//! Constructor: specify the argument vector in, well, a vector.

	forkexec(//! An exception gets thrown if this vector is empty.
		 const std::vector<std::string> &argvArg);

	//! Constructor: specify thea rgument vector as variadic parameters.
	template<typename ...Args>
	forkexec(const std::string &arg, Args && ...args)
	{
		argv.reserve(1+sizeof...(Args));
		collect(arg, std::forward<Args>(args)...);
	}

	~forkexec();

private:

	//! Tail end of argument vector collector.
	void collect() {}

	//! Collect variadic argument vector parameters.
	template<typename ...Args>
	void collect(const std::string &arg, Args && ...args)
	{
		argv.push_back(arg);
		collect(std::forward<Args>(args)...);
	}

public:

	//! Specify the program to execute.

	//! If this does not get called, argv[0] gets executed.

	forkexec &program(const std::string &exeArg)
	{
		exe=exeArg;
		return *this;
	}

	//! Redirect the process's file descriptor.

	forkexec &set_fd(//! The process's file descriptor to redirect
			 int filedesc,

			 //! Redirect it to/from here.
			 const fd &fdArg);

	//! Redirect process's file descriptor to a pipe that we'll read from.

	//! Calls pipe(2). Passes the write side of the pipe to set_fd().
	//! Returns the read side.

	fd pipe_from(//! File descriptor to redirect
		     int filedesc=1);

	//! Redirect process's file descriptor from a pipe that we'll write to.

	//! Calls pipe(2). Passes the read side of the pipe to set_fd().
	//! Returns the write side of the pipe.

	fd pipe_to(//! File descriptor to redirect
		   int filedesc=0);

	//! Redirect process's file descriptor to a bidirectional pipe.

	//! Calls socketpair(). Passes one socket to set_fd(), returns the
	//! other one.
	fd socket_fd(//! File descriptor to redirect
		     int filedesc);

	//! Calls execvp().

	//! Success means we never return. The only other possibility is a
	//! thrown exception, if the process cannot be executed.
	void exec() __attribute__((noreturn));

	//! Calls fork() then exec().

	//! Returns the process ID of the started process. You can call wait4()
	//! to wait for it.

	pid_t spawn();

	//! Calls fork() twice, then exec().

	//! Don't end up with a child process. This calls fork(), the child
	//! process calls fork() again, the second child calls exec(), and the
	//! first fork() exits. The original process winds up with no direct
	//! child process. The started process's parent becomes init.
	//!
	//! Note that an exception still gets thrown here, if the exec() in the
	//! second child process fails, for some reason!

	void spawn_detached();

	//! Calls fork() and exec(), then waits for the process to terminate.

	//! This is just wait4(spawn()).

	int system();

private:

	//! Internal method.
	void after_fork(const fd &errArg) noexcept LIBCXX_INTERNAL;
	//! Internal method.
	void before_fork(const fd &errArg) LIBCXX_INTERNAL;
public:

	//! Wait for some process to terminate.

	//! Waits for the child to exit, returns the child process's exit
	//! code. A negative signal number gets returned if the child process
	//! got terminated by a signal, and 256 for all other reasons.
	//! An exception gets thrown if the pid does not exist.

	static int wait4(//! Child process
			 pid_t);
};

#if 0
{
#endif
}
#endif
