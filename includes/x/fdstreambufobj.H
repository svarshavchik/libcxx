/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdstreambufobj_H
#define x_fdstreambufobj_H

#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/ref.H>
#include <x/fdbasefwd.H>
#include <x/basicstreambufobj.H>

#include <streambuf>
#include <iosfwd>
#include <vector>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A \c std::streambuf implementation for fd objects

//! \internal
//! This reference-counted \c std::streambuf subclass implements
//! \c std::streambuf semantics for fd objects.

class fdstreambufObj : public basic_streambufObj {

	//! The underlying file descriptor reference
	fdbase fdref;

	//! Internally-allocated buffer

	std::vector<char_type> readbuffer;

	//! Internally-allocated buffer

	std::vector<char_type> writebuffer;

	//! Suggested buffer size

	size_t defaultBufSize;

	//! Current buffer
	char_type *read_buf_ptr;

	//! Buffer's size
	std::streamsize read_buf_size;

	//! Current buffer
	char_type *write_buf_ptr;

	//! Buffer's size
	std::streamsize write_buf_size;

	//! Where the read and the write buffer is shared

	bool syncflag;

public:
	//! The constructor

	//! The constructor takes a reference to an fd object.

	fdstreambufObj(//! The fd object.
		       const fdbase &fdrefArg,

		       //! Default buffer size
		       size_t defaultBufSizeArg,

		       //! Synchronize input and output. Before writing, if
		       //! there was buffered read data, seek the underlying
		       //! file descriptor to back up over the buffered read
		       //! data.

		       bool syncflagArg
		       ) noexcept;

	//! Default destructor
	~fdstreambufObj();

	//! Implement \c std::streambuf::setbuf()

	//! \return a \c NULL if there was an error, or the pointer
	//! to this \c std::streambuf

	std::streambuf *setbuf(char_type *s, std::streamsize n) override;

	//! Implement \c std::streambuf::underflow()

	//! See the description of \c std::streambuf::underflow() for more
	//! information.
	int_type underflow() override;

	//! Implement \c std::streambuf::sync()

	//! See the description of \c std::streambuf::sync() for more
	//! information.

	int sync() override;

	//! Implement the \c std::basic_streambuf::showmanyc() method

	std::streamsize showmanyc() override;

	//! Implement \c std::streambuf::overflow()

	//! See the description of \c std::streambuf::overflow() for more
	//! information.

	int_type overflow(int_type c) override;

	//! Implement \c std::streambuf::seekoff()

	//! See the description of \c std::streambuf::seekoff() for more
	//! information.

	pos_type seekoff(off_type off, std::ios_base::seekdir way,
			 std::ios_base::openmode which) override;

	//! Implement \c std::streambuf::seekpos()

	//! See the description of \c std::streambuf::seekpos() for more
	//! information.

	pos_type seekpos(pos_type sp,
			 std::ios_base::openmode which) override;

};

#if 0
{
#endif
}
#endif
