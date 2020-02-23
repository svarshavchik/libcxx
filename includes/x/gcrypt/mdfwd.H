/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_gcrypt_mdfwd_H
#define x_gcrypt_mdfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
	namespace gcrypt {
#if 0
	}
}
#endif

class mdObj;
class mdBase;

//! Message digests, a.k.a. hashes

//! \code
//! auto md=INSERT_LIBX_NAMESPACE::gcrypt::md::create("md5");
//!
//! md->write(&buffer[0], buffer.size());
//!
//! std::vector<unsigned char> digest;
//!
//! md->digest(digest);
//!
//! std::string hexdigest;
//!
//! std::string str=md->hexdigest();
//!
//! \endcode
//!
//! This object calculates a message digest, a.k.a. hash. The constructor
//! takes the hash's name (it can also be given by its GCRY_MD identifier,
//! instead). Pass the contents to write() (multiple calls to write are
//! allowed), then call digest() to calculate the digest. The value that
//! digest() places into the buffer is the raw, binary, digest/hash.
//! hexdigest() returns a hexadecimal string, using lowercase letters.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::vector<unsigned char> v=md->digest();
//! \endcode
//!
//! This overloaded version of digest() returns a new reference-counted
//! \ref vector "INSERT_LIBX_NAMESPACE::vector".
//!
//! \code
//! INSERT_LIBX_NAMESPACE::vector<unsigned char> v=std::copy(message_digest.begin(), message_digest.end(),
//!     INSERT_LIBX_NAMESPACE::gcrypt::md::base::iterator("md5")).digest();
//! \endcode
//!
//! \ref mdBase::iterator "INSERT_LIBX_NAMESPACE::gcrypt::md::iterator"
//! constructs an output iterator. The constructor takes the name of a hash
//! function. Iterate over the contents of the message, and invoke either
//! version of digest() to calculate the hash/digest.
//!
//! \code
//! std::string h=std::copy(message_digest.begin(), message_digest.end(),
//!     INSERT_LIBX_NAMESPACE::gcrypt::md::base::iterator("md5")).hexdigest();
//! \endcode
//!
//! ... or hexdigest().
//!
typedef ref<mdObj, mdBase> md;

//! A nullable reference pointer to a message digest

//! \see md

typedef ptr<mdObj, mdBase> mdptr;

//! A reference to a constant message digest object

//! \see md

typedef const_ref<mdObj, mdBase> const_md;

//! A nullable reference pointer to a constant message digest object

//! \see md

typedef const_ptr<mdObj, mdBase> const_mdptr;

#if 0
{
	{
#endif
	}
}
#endif
