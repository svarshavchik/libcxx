/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ftp_client_H
#define x_ftp_client_H

#include <x/namespace.h>
#include <x/ref.H>
#include <x/fd.H>
#include <x/ftp/clientfwd.H>
#include <x/ftp/clientobj.H>

namespace LIBCXX_NAMESPACE {

	//! FTP client namespace

	//! This namespace implements an \ref client "FTP client".

	namespace ftp {
#if 0
	}
}
#endif

//! Base class for \ref client "FTP clients".

class clientBase : public ptrrefBase {

public:

	//! Returned by \ref ftp_filestat "filestat() and dirstat()".

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::ftp::client::base::stat.
	//! See RFC 3659, section 7.5, for more information on the following.

	class stat {
	public:
		off64_t size=0; //!< File size

		ymdhms modify=ymdhms(0, tzfile::base::utc());
		//!< Modification timestamp

		//!< When the file was last modified.
		ymdhms create=ymdhms(0, tzfile::base::utc());
		//!< Creation timestamp.

		//!< The creation timestamp is usually not available on
		//!< POSIX systems.

		std::string type;
		//!< File type, converted to lowercase

		//!< Typically "file", "cdir" (current directory),
		//!< "pdir" (parent directory) or "dir" (subdirectory).
		
		std::string unique;
		//!< Unique ID

		//!< Typically dev+inode

		std::string perm; //!< Permissions.

		//!< See section 7.5.5 of RFC 3659

		std::string lang;
		//!< Language, lowercase

		std::string mime_type;
		//!< Mime type, lowercase

		std::string charset;
		//!< Character set, lowercase

		int mode=0; //!< UNIX.mode
		uid_t uid=0; //!< UNIX.uid
		gid_t gid=0; //!< UNIX.gid

		//! Default constructor
		stat();

		//! Construct from RFC3659 fact string
		stat(const std::string &str);

		//! Destructor
		~stat();
	};
};

//! filestat() and dirstat() internal callback

class clientObj::stat_callback {

public:
	//! Constructor
	stat_callback();

	//! Destructor

	~stat_callback();

	//! Callback

	virtual void operator()(const char *name,
				const client::base::stat &stat)=0;
};

//! Forward filestat() and dirstat() callbacks to a lambda

template<typename lambda_type>
class clientObj::stat_callback_impl : public stat_callback {

public:
	//! The lambda
	lambda_type lambda;

	//! Constructor
	stat_callback_impl(lambda_type &&lambdaArg)
		: lambda(std::move(lambdaArg))
	{
	}

	//! Destructor
	~stat_callback_impl()
	{
	}

	void operator()(const char *name,
			const client::base::stat &stat) override
	{
		lambda(name, stat);
	}
};

//! Return information about a file, or a directory

//! See \ref ftp::client "INSERT_LIBX_NAMESPACE::ftp::client".

template<typename lambda_type>
void clientObj::filestat(lambda_type &&lambda,
			 const std::string &filename) const
{
	stat_callback_impl<lambda_type> callback(std::move(lambda));

	do_filestat(callback, filename);
}

//! Return directory contents

//! See \ref ftp::client "INSERT_LIBX_NAMESPACE::ftp::client".

template<typename lambda_type>
void clientObj::dirstat(lambda_type &&lambda,
			const std::string &dirname) const
{
	stat_callback_impl<lambda_type> callback(std::move(lambda));

	do_dirstat(callback, dirname);
}

//! Return directory contents

//! See \ref ftp::client "INSERT_LIBX_NAMESPACE::ftp::client".
template<typename lambda_type>
void clientObj::dirstat(lambda_type &&lambda,
			const std::string &dirname,
			const fdtimeoutconfig &config) const
{
	stat_callback_impl<lambda_type> callback(std::move(lambda));

	do_dirstat(callback, dirname, config);
}

//! Return directory contents

//! See \ref ftp::client "INSERT_LIBX_NAMESPACE::ftp::client".

template<typename lambda_type>
void clientObj::dirstat(lambda_type &&lambda,
			const fdtimeoutconfig &config) const
{
	dirstat(std::move(lambda), "", config);
}

template<typename ...Args>
void clientObj::get_file(const std::string &filename,
			 const std::string &file,
			 Args && ...args)
{
	get_file(filename, 0666, file, std::forward<Args>(args)...);
}

template<typename ...Args>
void clientObj::get_file(const std::string &filename,
			 mode_t mode,
			 const std::string &file,
			 Args && ...args)
{
	auto f=fd::create(filename, mode);

	get(fdoutputiter(f), file, std::forward<Args>(args)...).flush();
	f->close();
}

#if 0
{
	{
#endif
	}
}
#endif
