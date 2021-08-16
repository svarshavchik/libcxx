/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fileattrobj_H
#define x_fileattrobj_H

#include <x/exceptionfwd.H>
#include <x/obj.H>
#include <x/basicattr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! File status and extended file attributes.

//! This class implements file status extended attribute access for discrete
//! files.
//! The constructor saves the filename, and the file status, and
//! extended attributes, are pulled from the given file.

class fileattrObj : public basic_attr {

	//! Access this file's extended attribute.

	std::string filename;

	//! Read extended attribute's of the symbolic link itself, and not the file it points to.
	bool readsymlinks;

public:
	//! The constructor.

	fileattrObj(//! Access extended attributes of this file.
		    const char *filenameArg,

		    //! Read extended attributes from the symbolic link.

		    //! If this parameter is set to \c true and filenameArg
		    //! is a symbolic link, the symbolic link's extended
		    //! attributes get accessed, otherwise the extended
		    //! attributes are accessed from the symbolic link's
		    //! target (the "l" %attr(5) functions).
		    //!
		    //! This parameter is ignored when filenameArg is not a
		    //! symbolic link.

		    bool readsymlinksArg=true) noexcept;

	//! The constructor.

	//! \overload
	//!
	fileattrObj(const std::string &filenameArg,
		    bool readsymlinksArg=true) noexcept;

	~fileattrObj();

private:
	//! Invoke the listxattr() or llistxattr() function.

	//! \see basic_attr::listattr_internal().
	//! \internal
	ssize_t listattr_internal(const listattr_internal_args &args)
		noexcept override LIBCXX_HIDDEN;

	//! Invoke the getxattr() or lgetxattr() function.

	//! \see basic_attr::getattr_internal().
	//! \internal
	ssize_t getattr_internal(const char *name,
				 void *value,
				 size_t size)
		noexcept override LIBCXX_HIDDEN;

	//! Invoke the setxattr() or lsetxattr() function.

	//! \see basic_attr::setattr_internal().
	//! \internal
	int setattr_internal(const char *name,
			     const void *value,
			     size_t size,
			     int flags)
		noexcept override LIBCXX_HIDDEN;

	//! Invoke the removexattr() or lremovexattr() function.

	//! \see basic_attr::removeattr_internal().
	//! \internal
	int removeattr_internal(const char *name)
		noexcept override LIBCXX_HIDDEN;


	//! Invoke the stat() or the lstat() function.

	//! \see basic_attr::stat_internal().
	//! \internal
	int stat_internal(//! The buffer where file status is placed, on success.
			  struct ::stat *buf) noexcept override LIBCXX_HIDDEN;

	//! Invoke the readlink() function.

	//! \see basic_attr::readlink_internal().
	//! \internal

	ssize_t readlink_internal(char *buf,
				  size_t bufsize) noexcept override
		LIBCXX_HIDDEN;

	//! Set the file permission bits

	//! \internal
	//! \see basic_attr::chmod_internal().
	int chmod_internal(mode_t mode) noexcept override LIBCXX_HIDDEN;

	//! Set the file ownership and permissions

	//! \internal
	//! \see basic_attr::chown_internal().
	int chown_internal(uid_t uid, gid_t group) noexcept override
		LIBCXX_HIDDEN;

	//! I am a file.

	//! \internal
	std::string whoami() override LIBCXX_HIDDEN;
};
#if 0
{
#endif
}
#endif
