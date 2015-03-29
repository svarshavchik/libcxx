/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basicattr_H
#define x_basicattr_H

#include <string>
#include <set>
#include <vector>
#include <errno.h>
#include <sys/types.h>
#include <x/exceptionfwd.H>
#include <x/filestat.H>
#include <x/namespace.h>
#include <x/sys/xattrsys.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! basic_attr::setattr() flag - create attribute only, fail if attribute exists.
static const int setattr_create=1;

//! basic_attr::setattr() flag - replace attribute only, fail if attribute does not already exist.
static const int setattr_replace=2;

//! An interface for retrieving file status %stat() and extended attributes.

//! See the %stat(2) man page for a more in-depth discussion of
//! basic file attributes.
//! See the %attr(5) man page for a more in-depth discussion of
//! extended file attributes.
//! See the %readlink(2) man page for the description of this system call.
//!
//! The subclass must implement stat_internal() using the appropriate %stat()
//! or %fstat() functiion.
//! The subclass must implement listattr_internal(), getattr_internal(),
//! setattr_internal(), and removeattr_internal() using the appropriate
//! listxattr()
//! getxattr(), setxattr(), and removexattr() function.
//! The subclass must implement readlink_internal() using
//! %readlink(2), if possible, or return a failure code.
//! The subclass must implement chmod_internal() and chown_internal() using
//! chmod() and chown(), and return the error code from the syscall.
//!
//! The public list(), get(), set(), and remove() methods encapsulate the
//! system calls into C++ methods.
//!
//! This class defines an interface that retrieves and/or updates
//! standard file attributes (the %stat(2) system call), and extended
//! file attributes.
//! This interface
//! is subclassed by fdObj, which implements the internal functions using
//! fstat() and the
//! "f" versions of %attr(5) functions. This interface is also subclassed by
//! fileattrObj - a reference counted object with a corresponding reference
//! type, and \link ::attr attr \endlink - which implements the internal
//! functions using stat(2) or lstat(2), and the regular
//! and "l" versions of %attr(5) functions, which operate on filenames.
//! \link ::attr attr \endlink also implements readlink_internal().

class basic_attr : virtual public obj {

protected:

	class listattr_internal_args;

	//! Throw an exception after an error

	//! \internal
	void fatal(const char *syscall) LIBCXX_INTERNAL __attribute__((noreturn));

	//! What exactly am I?

	//! The subclass should return a string that identifies this
	//! attribute object. When an error occurs, this goes into the
	//! thrown %exception.
	virtual std::string whoami() LIBCXX_HIDDEN=0;

	//! Retrieve standard file status attributes.

	//! \internal The subclass must define this function to invoke one
	//! of the %stat(2) system calls.
	//! \return The return code from the %stat(2) system call.

	virtual int stat_internal(//! The buffer where file status is placed, on success.
				  struct ::stat *buf) noexcept LIBCXX_HIDDEN;

	//! Invoke the listxattr() function.

	//! \internal The subclass must define this function to invoke one
	//! of the listxattr() functions.
	//! \return the number of bytes placed into the attribute buffer,
	//! or (ssize_t)-1 and sets errno if an error
	//! occurs.
	//! See the listxattr(2) manual page for more information on the
	//! return value and the meanings of various errors.

	virtual ssize_t listattr_internal(const listattr_internal_args &args)
		noexcept LIBCXX_HIDDEN;

	//! Invoke the getxattr() function.

	//! \internal The subclass must define this function to invoke one
	//! of the getxattr() functions.
	//! \return the number of bytes placed into the attribute value buffer,
	//! or (ssize_t)-1 and sets errno if an error
	//! occurs.
	//! See the getxattr(2) manual page for more information on the
	//! return value and the meanings of various errors.
	virtual ssize_t getattr_internal(//! Attribute's name.
					 const char *name,
					 //! Buffer for the attribute's value.
					 void *value,
					 //! Size of the value buffer.
					 size_t size)
		noexcept LIBCXX_HIDDEN;

	//! Invoke the setxattr() function

	//! \internal The subclass must define this function to invoke one
	//! of the setxattr() functions.
	//! \return 0 if the function succeeds,
	//! or (ssize_t)-1 and sets errno if an error
	//! occurs.
	//! See the setxattr(2) manual page for more information on the
	//! return value and the meanings of various errors.
	virtual int setattr_internal(//! Attribute's name.
				     const char *name,
				     //! The attribute's value.
				     const void *value,
				     //! Size of the attribute's value.
				     size_t size,
				     //! Optional flags: \c setattr_create or \c setattr_replace.
				     int flags)
		noexcept LIBCXX_HIDDEN;

	//! Invoke the removexattr() function

	//! \internal The subclass must define this function to invoke one
	//! of the removexattr() functions.
	//! \return 0 if the function succeeds,
	//! or (ssize_t)-1 and sets errno if an error
	//! occurs.
	//! See the removexattr(2) manual page for more information on the
	//! return value and the meanings of various errors.
	virtual int removeattr_internal(//! Attribute's name
					const char *name)
		noexcept LIBCXX_HIDDEN;

	//! Retrieve the symbolic link target

	//! \internal The subclass must define this function to invoke the
	//! %readlink22) system call.
	//! \return the return code from %readlink(2).

	virtual ssize_t readlink_internal(//! The output buffer
					  char *buf,

					  //! Output buffer's size
					  size_t bufsize) noexcept LIBCXX_HIDDEN;

	//! Set the file permission bits

	//! \internal
	virtual int chmod_internal(mode_t mode) noexcept LIBCXX_HIDDEN;

	//! Set the file ownership and permissions

	//! \internal
	virtual int chown_internal(uid_t uid, gid_t group) noexcept LIBCXX_HIDDEN;

private:
	//! Check the return value of getattr_internal()

	//! \internal This helper function throws an error if
	//! getattr_internal() failed.

	void get_chk(ssize_t l);
public:
	//! Return file status

	//! This function retrieves the file status. See the %stat(2)
	//! man page for more information.
	//! \return a ::filestat reference. An %exception gets thrown if the
	//! underlying system call fails. This is a reference to an object
	//! that contains all the fields described in the %stat(2) man page.

	filestat stat();

	//! Return the names of all defined attributes

	//! This function retrieves a list of all attributes that are set
	//! for this object.
	//! \return a set of attribute names. An attribute name is given
	//! as a \c std::string.
	std::set<std::string> listattr();

	//! Return an attribute's value

	//! The value of the given attribute is retrieved and placed into
	//! the supplied buffer.

	template<typename t>
	void getattr(//! The name of the attribute.
		     const char *name,

		     //! A buffer where the attribute's value is placed.
		     //! The buffer gets resized according to the attribute's
		     //! value.
		     std::vector<t> &vec)
	{
		ssize_t l;
		vec.resize(1024);

		while ((l=getattr_internal(name,
					   static_cast<void *>(&vec[0]),
					   vec.size() * sizeof(t))) < 0 &&
		       errno == ERANGE)
		{
			vec.resize((getattr_internal(name, NULL, 0)
				    + (sizeof(t)-1)) / sizeof(t));
		}
		get_chk(l);
		vec.resize((l+(sizeof(t)-1))/sizeof(t));
	}

	//! Return an attribute's value

	//! \overload
	//!
	template<typename t>
	void getattr(const std::string &name,
		     std::vector<t> &vec)
	{
		getattr(name.c_str(), vec);
	}

	//! Return an attribute's value, as a \c std::string

	//! \overload
	//! \return The attribute's value.

	std::string getattr(//! The attribute's name
			    const char *name)
	{
		std::vector<char> buf;

		getattr(name, buf);
		return std::string(buf.begin(), buf.end());
	}

	//! Return an attribute's value, as a \c std::string

	//! \overload
	//!
	std::string getattr(const std::string &name)
	{
		return getattr(name.c_str());
	}

private:
	//! Set an attribute.

	//! \internal
	//! This function implements the attribute setting object.
	//! The other %setattr() functions invoke this function after
	//! converting their arguments into the form expected by this
	//! function.
	void setattr(//! The attribute's name.
		     const char *name,
		     //! The attribute's value.
		     const void *value,

		     //! The attribute's value's size, in bytes.
		     size_t size,

		     //! Optional flags: \c setattr_create or \c setattr_replace.
		     int flags);
public:
	basic_attr() noexcept;

	virtual ~basic_attr() noexcept;

	//! Set an attribute's value.

	//! This function sets the value of the
	//! given attribute.
	template<typename t>
	void setattr(//! The attribute's name.
		     const char *name,

		     //! The attribute's value.
		     const std::vector<t> &vec,

		     //! Optional flags.

		     //! The default flag of 0 either creates or updates the
		     //! attribute's value. If the attribute does not exist,
		     //! it gets created. If the attribute exists, it gets
		     //! update. Setting flags to \c setattr_create creates an
		     //! the attribute, an %exception gets thrown if the
		     //! attribute already exist. Setting flags to
		     //! \c setattr_replace updates an existing attribute, or
		     //! throws an %exception if the attribute does not exist.
		     int flags=0)
	{
		setattr(name, static_cast<const void *>(&vec[0]),
		    vec.size() * sizeof(t), flags);
	}

	//! Set an attribute's value.

	//! \overload
	//!
	template<typename t>
	void setattr(const std::string &name, const std::vector<t> &vec,
		     int flags=0)
	{
		return setattr(name.c_str(), vec, flags);
	}

	//! Set an attribute's value.

	//! \overload
	//!
	void setattr(const char *name, const std::string &value,
		 int flags=0)
	{
		setattr(name, &value[0], value.size(), flags);
	}

	//! Set an attribute's value.

	//! \overload
	//!
	void setattr(const std::string &name, const std::string &value,
		 int flags=0)
	{
		setattr(name.c_str(), value, flags);
	}

	//! Remove an attribute.

	//! This function removes the given attribute.
	//!
	void removeattr(//! The attribute's name
			const char *name);

	//! Remove an attribute.

	//! \overload
	//!
	void removeattr(const std::string &name)
	{
		removeattr(name.c_str());
	}

	//! Read the symbolic link.

	//! This function returns the value of the symbolic link.
	//!
	std::string readlink();

	//! Set the file's permissions
	void chmod(mode_t mode);

	//! Set the file's ownership
	void chown(uid_t userid,
		   gid_t groupid);
};


#if 0
{
#endif
}
#endif
