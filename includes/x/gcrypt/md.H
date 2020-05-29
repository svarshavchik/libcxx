/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_gcrypt_md_H
#define x_gcrypt_md_H

#include <x/namespace.h>
#include <x/gcrypt/mdfwd.H>
#include <x/gcrypt/mdobj.H>
#include <x/ref.H>
#include <set>
#include <iterator>

namespace LIBCXX_NAMESPACE {
	namespace gcrypt {
#if 0
	}
}
#endif

//! Base class for message digest objects

//! Refer to this class as \ref md "INSERT_LIBX_NAMESPACE::gcrypt::md::base".
//!

class mdBase : public ptrref_base {

public:
	//! Fill algos with a list of available message digests

	static void enumerate(std::set<std::string> &algos);

	//! Return the name of a message digest number

	static std::string name(gcry_md_algos algo);

	//! Return the number of a message digest

	static gcry_md_algos number(const std::string &name);

	class iterator;

	//! Calculate a digest in one shot.

	//! Calculate a digest from an input sequence.

	template<typename iter_type, typename ...DigestArgs>
	static vector<unsigned char> digest(//! Beginning input iterator
					    iter_type beg_iter,

					    //! Ending input iterator
					    iter_type end_iter,

					    //! Digest arguments, forwarded to the digest constructor
					    DigestArgs && ...args)
	{
		return std::copy(beg_iter, end_iter,
				 iterator(std::forward<DigestArgs>(args)...))
			.digest();
	}

	//! Calculate a digest in one shot, in hexadecimal.

	//! Calculate a digest from an input sequence.

	template<typename iter_type, typename ...DigestArgs>
	static std::string hexdigest(//! Beginning input iterator
				     iter_type beg_iter,
				     //! Ending input iterator
				     iter_type end_iter,
				     //! Digest algorithm, required.
				     gcry_md_algos algorithm,
				     //! GCRY_MD_FLAG_SECURE, GCRY_MD_FLAG_HMAC
				     unsigned int flags=0)
	{
		return std::copy(beg_iter, end_iter, iterator(algorithm, flags))
			.hexdigest();
	}

};

//! Output iterator that calculates a digest

//! Refer to this class as \ref md "INSERT_LIBX_NAMESPACE::gcrypt::md::base::iterator".

class mdBase::iterator : public md {

public:
	//! Iterator trait
	typedef std::output_iterator_tag iterator_category;

	//! Iterator trait
	typedef void value_type;

	//! Iterator trait
	typedef void difference_type;

	//! Iterator trait
	typedef void pointer;

	//! Iterator trait
	typedef void reference;

	//! Constructor passes through
	template<typename ...Args>
	iterator(Args && ...args)
		: md(md::create(std::forward<Args>(args)...))
	{
	}

	//! Destructor
	~iterator();

	using md::operator*;
	using md::operator->;

	//! iterator operator

	iterator &operator++()
	{
		return *this;
	}

	//! iterator operator

	iterator &operator++(int)
	{
		return *this;
	}

	//! Return the calculated digest

	//! After iterating over the message, its hash gets calculated and
	//! returned.

	void digest(std::vector<unsigned char> &buffer,
		    gcry_md_algos algorithm=GCRY_MD_NONE)
	{
		return (*this)->digest(buffer, algorithm);
	}

	//! Return the calculated digest

	//! \overload
	vector<unsigned char> digest(//! Digest algorithm, or use default one
				     gcry_md_algos algorithm=GCRY_MD_NONE);

	//! Return the calculated digest in hexadecimal

	std::string hexdigest(//! Digest algorithm, or use default one
			      gcry_md_algos algorithm=GCRY_MD_NONE);

};
#if 0
{
	{
#endif
	}
}
#endif
