/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_vectorobj_H
#define x_vectorobj_H

#include <vector>
#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/fd.H>
#include <x/interface.H>
#include <iostream>
#include <cstdio>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Error reporting functions for the vector template class

//! \internal
//!
class vectorObjBase {

public:
	//! Throw a read error exception

	static void read_error() __attribute__((noreturn));

	//! Throw a write error exception
	static void write_error() __attribute__((noreturn));
};

//! A reference-counted %vector

//! This template defines a reference-counted wrapper for a \c std::vector.
//!

template<class T, class Allocator>
class vectorObj : virtual public obj, private vectorObjBase,
		  public std::vector<T, Allocator> {

public:
	//! The container type

	typedef std::vector<T, Allocator> container;

	//! The constructor
	template<typename ...Args>
	vectorObj(Args && ...args)
		: std::vector<T, Allocator>(std::forward<Args>(args)...)
	{
	}

	~vectorObj() {}


	//! Dump the contents of this vector to a stream

	void write(//! The output stream
		   const ref<obj> &writeStream) const
	{
		interface<std::ostream> oInt(writeStream);

		std::ostream *o=oInt.getptr();

		try {
			o->write( (char *)&(*this)[0],
				  this->size() * sizeof(T)).flush();

		} catch (...)
		{
			write_error();
		}

		if (o->bad() || o->fail())
			write_error();
	}

	//! Read this vector's contents from an input stream

	//! Existing contents of this vector objects are removed.
	//!
	void read(//! Input stream
		  const ref<obj> &readStream)
	{
		interface<std::istream> iInt(readStream);

		std::istream *i=iInt.getptr();

		std::streampos origPos(-1);
		std::streampos eofPos(-1);

		try {
			origPos=i->tellg();

			if (i->seekg(0, std::ios_base::end).good())
				eofPos=i->tellg();
		} catch (...)
		{
			i->clear();
		}

		if (origPos != -1 && eofPos != -1)
		{
			i->seekg(origPos);

			if (i->bad() || i->fail())
				read_error();

			std::streamsize seqSize=(eofPos-origPos)/sizeof(T);

			this->reserve(seqSize);
			this->resize(seqSize);

			i->read( (char *)&(*this)[0], seqSize * sizeof(T));

			if (i->bad() || i->fail())
				read_error();
			return;
		}

		this->clear();

		size_t n=0;
		std::streamsize cnt;

		do
		{
			this->resize(n+BUFSIZ);

			char *p= (char *)&(*this)[n];
			size_t left=BUFSIZ*sizeof(T);

			std::streamsize done=0;

			do
			{
				try {
					cnt=i->readsome(p, left);
				} catch (...)
				{
					cnt=0;
				}

				p += cnt;
				left -= cnt;
				done += cnt;
			} while (cnt);

			n += done/sizeof(T);
		} while (cnt);
		this->resize(n);
		if (!i->eof())
			read_error();
	}

	//! Save this vector to a file descriptor

	void save(//! The file descriptor
		  const fd &fdRef) const
	{
		ostream o(fdRef->getostream());

		write(o);

		try {
			o->flush();
		} catch (...)
		{
			write_error();
		}
		if (o->fail())
			write_error();
	}

	//! Load this vector from a file descriptor

	void load(//! The file descriptor
		  const fd &fdRef)
	{
		istream i(fdRef->getistream());
		read(i);
	}

	//! Save this vector to a file

	void save(//! The filename
		  const char *filename,

		  //! Creation mode
		  mode_t mode=0666) const
	{
		save(std::string(filename), mode);
	}

	//! Save this vector to a file

	//! \overload
	//!
	void save(//! The filename
		  const std::string &filename,

		  //! Creation mode
		  mode_t mode=0666) const
	{
		fd file(fd::create(filename, mode));
		save(file);
		file->close();
	}

	//! Load this vector from a file

	void load(//! The filename
		  const char *filename)
	{
		load(std::string(filename));
	}

	//! Save this vector from a file

	//! \overload
	//!
	void load(//! The filename
		  const std::string &filename)
	{
		fd file(fd::base::open(filename, O_RDONLY));

		load(file);
	}

};

#if 0
{
#endif
}
#endif
