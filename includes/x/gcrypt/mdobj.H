/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_gcrypt_mdobj_H
#define x_gcrypt_mdobj_H

#include <x/namespace.h>
#include <x/gcrypt/mdfwd.H>
#include <x/gcrypt/errors.H>
#include <x/obj.H>
#include <x/vectorfwd.H>
#include <gcrypt.h>

#include <vector>

namespace LIBCXX_NAMESPACE::gcrypt {
#if 0
}
#endif

//! An interface to libgcrypt's message digest API

class mdObj : virtual public obj {

	//! The internal gcrypt handle

	gcry_md_hd_t h;

	//! Main algorithm
	gcry_md_algos algorithm;
public:
	//! Constructor

	mdObj(//! Digest algorithm, or use enable()
	      gcry_md_algos algorithm=GCRY_MD_NONE,
	      //! GCRY_MD_FLAG_SECURE, GCRY_MD_FLAG_HMAC
	      unsigned int flags=0);

	//! Constructor

	//! \overload
	mdObj(//! Specify an algorithm by name
	      const std::string &algorithm,

	      //! GCRY_MD_FLAG_SECURE, GCRY_MD_FLAG_HMAC
	      unsigned int flags=0);

	//! Constructor

	//! Clone another message digest object.
	//! \overload

	mdObj(const const_md &src);

	//! Destructor
	~mdObj();

	//! Enable an algorithm

	void enable(//! Digest algorithm
		    gcry_md_algos algorithm);

	//! Enable an algorithm

	//! \overload
	void enable(//! Digest algorithm
		    const std::string &algorithm);

	//! Set HMAC key

	void setkey(const void *key, size_t keylen);

	//! Set the key

	template<typename char_type, typename Allocator>
	void setkey(const std::vector<char_type, Allocator> &key)
	{
		setkey(&*key.begin(), key.size() * sizeof(char_type));
	}

	//! Calculate message digest

	void write(const void *key, size_t keylen);

	//! Calculate message digest

	template<typename char_type, typename Allocator>
	void write(const std::vector<char_type, Allocator> &key)
	{
		write(&*key.begin(), key.size() * sizeof(char_type));
	}

	//! Calculate message digest

	inline void write(int c)
	{
		gcry_md_putc(h, c);
	}

	//! Iterator operator

	inline void operator=(int c)
	{
		write(c);
	}

	//! Return the calculated message digest

	void digest(//! Output buffer
		    std::vector<unsigned char> &buffer,

		    //! Digest algorithm, or use default one
		    gcry_md_algos algorithm=GCRY_MD_NONE);

	//! Return the calculated message digest

	vector<unsigned char> digest(//! Digest algorithm, or use default one
				     gcry_md_algos algorithm=GCRY_MD_NONE);

	//! Return the calculated message digest

	std::string hexdigest(//! Digest algorithm, or use default one
			      gcry_md_algos algorithm=GCRY_MD_NONE);
};

#if 0
{
#endif
}
#endif
