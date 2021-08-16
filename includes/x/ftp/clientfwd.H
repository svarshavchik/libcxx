/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ftp_clientfwd_H
#define x_ftp_clientfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::ftp {
#if 0
}
#endif

class clientObj;
class clientBase;

//! A connection to an FTP server

//! \code
//! auto fd=INSERT_LIBX_NAMESPACE::netaddr("ftp.example.com", "ftp")->connect();
//!
//! INSERT_LIBX_NAMESPACE::ftp::client conn=INSERT_LIBX_NAMESPACE::ftp::client::create(netaddr);
//!
//! conn->login("anonymous", "user@example.com");
//! \endcode
//!
//! create() takes a connected file descriptor, and returns a reference to
//! \ref ref "reference counted object" for the connection to the FTP server.
//! create() waits for the server's greeting, before returning the handle.
//!
//! create() takes one or two optional parameters. The first optional parameter
//! is a custom \ref fdtimeout "INSERT_LIBX_NAMESPACE::fdtimeout" for the
//! initial greeting:
//!
//! \code
//! auto fd=INSERT_LIBX_NAMESPACE::netaddr("ftp.example.com", "ftp")->connect();
//!
//! auto timeout=INSERT_LIBX_NAMESPACE::fdtimeout::create(fd);
//!
//! timeout->set_read_timeout(8192, 60);
//! timeout->set_write_timeout(8192, 30);
//!
//! INSERT_LIBX_NAMESPACE::ftp::client conn=INSERT_LIBX_NAMESPACE::ftp::client::create(netaddr, timeout);
//!
//! conn->timeout(timeout);
//! \endcode
//!
//! If not specified, the default timeout settings come from
//! \c INSERT_LIBX_NAMESPACE::ftp::client::timeout::read_bytes and
//! \c INSERT_LIBX_NAMESPACE::ftp::client::timeout::read
//! \ref explicit_property_namespace "application properties".
//!
//! The custom timeout setting is used only for the initial greeting, and
//! then reverts to the default timeout settings, which also include
//! \c INSERT_LIBX_NAMESPACE::ftp::client::timeout::write_bytes and
//! \c INSERT_LIBX_NAMESPACE::ftp::client::timeout::write.
//!
//! Use timeout() to install a custom timeout handler, for the connection.
//!
//! The second optional parameter to create() is a \c bool indicating whether
//! FTP will use passive data connection, this defaults to \c true.
//!
//! create() waits for a greeting, but does not log in. Use login() to
//! log in to the FTP server. login() throws an exception if the login
//! request fails.
//!
//! \anchor ftp_timeout_config
//!
//! \par Data transfer timeouts
//!
//! Many FTP operations involve a data transfer channel. Those operations
//! take an optional \ref fdtimeoutconfig "INSERT_LIBX_NAMESPACE::fdtimeoutconfig"
//! parameter. If not specified, a stock one gets created that uses a
//! \ref fdtimeout "fdtimeout" that uses the
//! \c INSERT_LIBX_NAMESPACE::ftp::client::timeout::read_bytes,
//! \c INSERT_LIBX_NAMESPACE::ftp::client::timeout::read,
//! \c INSERT_LIBX_NAMESPACE::ftp::client::timeout::write_bytes and
//! \c INSERT_LIBX_NAMESPACE::ftp::client::timeout::write
//! \ref explicit_property_namespace "application properties".
//!
//! \code
//! conn->logout();
//! \endcode
//!
//! logout() formally logs out. Of course, one can always be rude, and simply
//! disconnect from the server (which happens automatically when
//! the last reference to the \c INSERT_LIBX_NAMESPACE::fd::client goes out
//! of scope, and the object gets destroyed.
//!
//! \par Errors
//!
//! \code
//! try {
//!    conn->chdir("uploads");
//! } catch (const INSERT_LIBX_NAMESPACE::ftp::exception &e)
//! {
//!    int status_code = e->status_code;
//!
//!    std::cerr << e << std::endl;
//! };
//! \endcode
//!
//! An \ref ftp::exception "exception"
//! gets thrown when an error occurs while executing the
//! following commands.
//!
//! \par Changing directories
//!
//! \code
//!     std::string directory=conn->pwd();
//!     std::string new_directory=conn->chdir("uploads");
//!     conn->cdup();
//!     conn->smnt("apps");
//! \endcode
//!
//! pwd() returns the name of the connection's current server directory.
//! chdir() changes the server directory, and returns the new directory name.
//! cdup() changes
//! the connection's server directory to the parent server directory.
//! smnt() executes the FTP SMNT command, which has server-specific
//! meaning, usually some kind of a filespace mounting operation.
//!
//! \par Directory listings
//!
//! \code
//! std::set<std::string> files;
//! std::vector<std::string> listing;
//!
//! conn->nlst(std::insert_iterator<std::set<std::string>>(files, files.end()),
//!     "pub");
//!
//! conn->list(std::insert_iterator<std::vector<std::string>>(files, files.end()),
//!     "pub");
//! \endcode
//!
//! nlst() returns a list of files in a directory. list() returns a
//! server-specific directory listing. The first parameter is an output
//! iterator. nlst() writes the name of each file in the server directory
//! to the output iterator. list() writes each line in the directory
//! listing to the output iterator. Both functions take two optional parameters:
//!
//! - the name of the directory, defaulting to an empty string, which
//! specifies the current server directory
//!
//! - A \ref ftp_timeout_config "data transfer timeout configuration" factory.
//!
//! \par Miscelaneous file and directory operations.
//!
//! \code
//! std::string new_directory=conn->mkdir("storage");
//!
//! conn->rmdir("storage");
//!
//! conn->rename("oldname", "newname");
//!
//! conn->unlink("filename");
//!
//! conn->site("chmod 755 filename");
//! \endcode
//!
//! mkdir() creates a new directory, if permitted, and returns the new
//! directory's name. rmdir() removes it. rename() renames a file, and
//! unlink() removes a file.
//!
//! site() executes a server-specific command specified by its parameter.
//!
//! \par Downloading files
//!
//! \code
//! std::string file;
//!
//! typedef std::back_insert_iterator<std::string> ins_iter;
//!
//! ins_iter iter=conn->get(ins_iter(file), "filename");
//!
//! \endcode
//!
//! The first of two required parameters to get() is an output iterator.
//! The second parameter specifies the name of the file. The output iterator
//! iterates over the contents of the downloaded file, and get() returns the
//! final value of the output iterator.
//!
//! get() also takes two optional parameters:
//!
//! - A \ref ftp_timeout_config "data transfer timeout configuration" factory.
//!
//! - A \c bool flag, defaulting to \c true which specifies a binary download.
//! Set this parameter to \c false for a text download (with CRLF newline
//! sequences).
//!
//! \code
//! conn->get_file("localfile", "remotefile");
//! \endcode
//!
//! get_file() uses get() to download a file. get_file() creates "localfile",
//! calls get() with an output iterator that writes to the file, then closes
//! it. Any additional parameter to get_file() get forwarded to get().
//!
//! get_file() itself takes an optional \c mode_t parameter after "localfile",
//! and before "remotefile", that specifies the local file's creation mode.
//! get_file() uses create() to create "localfile" under a temporary name,
//! then rename it to "localfile" after it gets downloaded. The temporary
//! file gets removed if the file download fails and an exception gets thrown.
//!
//! \par Uploading files
//!
//! \code
//! std::string file;
//!
//! conn->put("file", file.begin(), file.end());
//!
//! conn->append("file", file.begin(), file.end());
//!
//! std::string filename=conn->put_unique(file.begin(), file.end());
//!
//! \endcode
//!
//! put() uploads the file to the FTP server. Some servers can also append
//! to an existing file, using append(). The first parameter specifies the
//! name of the file on the FTP server. put_unique() has the server create
//! a new file, and returns the uploaded file's name.
//!
//! The contents of the file are specified by a beginning and an ending
//! input iterator the define the sequence with the file's contents.
//! Although random access iterators are preferred, even input iterators
//! will work, however the FTP ALLO command does not get sent in that case,
//! specifying the size of the file. Some servers may require the ALLO
//! command; with those servers at least a forward iterator is required.
//!
//! These three functions also take two optional parameters:
//!
//! - A \ref ftp_timeout_config "data transfer timeout configuration" factory.
//!
//! - A \c bool flag, defaulting to \c true which specifies a binary download.
//! Set this parameter to \c false for a text download (with CRLF newline
//! sequences).
//!
//! \par File information
//!
//! Most, but not all, FTP servers will also implement the following:
//!
//! \code
//! off64_t size=conn->size("filename");
//! INSERT_LIBX_NAMESPACE::ymdhms timestamp=conn->timestamp("filename");
//! \endcode
//!
//! size() returns the file's size, in bytes.
//! timestamp() returns the file's \ref ymdhms "modification time".
//!
//! \anchor ftp_filestat Extended file and directory listing
//!
//! Most FTP servers implement the following commands
//! which return file information and/or
//! directory listing in a structured format:
//!
//! \code
//! conn->filestat([]
//!                (const char *filename, const LIBCXX_NAMESPACE::ftp::client::base::stat &info)
//!                {
//!                    // ...
//!                });
//!
//! conn->dirstat([]
//!                (const char *filename, const LIBCXX_NAMESPACE::ftp::client::base::stat &info)
//!                {
//!                    // ...
//!                });
//! \endcode
//!
//! %filestat()'s and dirstat()'s first parameter
//! is a lambda callback. %filestat() returns
//! file information about a file or a directory specified by the optional
//! second parameter, defaulting to the current directory. The lambda callback
//! gets typically called only once, but some FTP servers may return multiple
//! records about the requested file or a directory (if it's available via
//! different names or links), in which case the lambda
//! gets invoked several times.
//!
//! dirstat() returns information about all files in a directory. The second
//! optional parameter specifies the directory's name, and this is parameter
//! also defaults to the current directory. The lambda gets called several
//! times, once for each record for each file in the specified directory.
//!
//! dirstat() also takes a third optional parameter. dirstat() uses a
//! separate data transfer channel, and the third optional parameter
//! specifies a \ref ftp_timeout_config "data time timeout" configuration.
//!
//! The first parameter to the lambda is a name of the file or a directory,
//! the second parameter is a \ref clientBase::stat "INSERT_LIBX_NAMESPACE::ftp::client::base::stat".

typedef ref<clientObj, clientBase> client;

//! A nullable pointer reference to an \ref client "FTP server connection".
typedef ptr<clientObj, clientBase> clientptr;

//! A reference to a constant \ref client "FTP server connection" object.
typedef const_ref<clientObj, clientBase> const_client;

//! A nullable pointer reference to a constant \ref client "FTP server connection" object.
typedef const_ptr<clientObj, clientBase> const_clientptr;

#if 0
{
#endif
}
#endif
