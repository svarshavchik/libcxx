/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_pkparamsobj_H
#define x_tls_pkparamsobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/ptr.H>
#include <x/vector.H>
#include <x/exceptionfwd.H>
#include <x/gnutls/datumwrapper.H>
#include <x/gnutls/pkparamsfwd.H>

#include <gnutls/gnutls.h>

namespace LIBCXX_NAMESPACE::gnutls {
#if 0
}
#endif

//! A temporary key exchange parameter

//! This is a generic superclass, currently the only subclass is dhparamsObj.
//!

class pkparamsObj : virtual public obj {

public:
	//! Default constructor

	pkparamsObj();

	//! Default destructor

	~pkparamsObj();

	//! Generate new key exchange parameters

	virtual void generate(//! Number of bits
			      unsigned int bits)=0;

	//! Export key exchange parameters

	virtual datum_t export_pk(//! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
				  gnutls_x509_crt_fmt_t format) const=0;

	//! Import key exchange parameters

	virtual void import_pk(//! The RSA parameters
			       datum_t pkparams,
			       //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
			       gnutls_x509_crt_fmt_t format)=0;

	//! Export raw key exchange parameters

	//! For dsa keys, this function resizes \c raw_datum to a two-element
	//! array, then places \c prime and \c generator into the array,
	//! and sets \c bits to the number of bits in the key.

	virtual void export_raw(//! Raw public key parameter
				std::vector<datum_t> &raw_datum,

				//! Number of bits in the public key parameter
				unsigned int &bits) const=0;

	//! Import raw key exchange parameters

	virtual void import_raw(//! Raw public key parameter

				//! This is a two-element array for DH
				//! keys (\c prime and \c generator values),

				const std::vector<datum_t> &raw_datum)=0;

	//! Import the default public key parameter file

	virtual void import()=0;

	//! Return the public key type

	//! \return \c GNUTLS_PK_RSA or \c GNUTLS_PK_DSA
	//!

	virtual gnutls_pk_algorithm_t get_pk_algorithm() const=0;

	//! Retrieve the number of bits in the public key parameters

	//! This function calls export_raw(), and throws away the resulting
	//! raw vectors, giving back just the bit count.

	unsigned int get_bits() const;

protected:
	//! Import a pregenerated public key parameter file

	//! Search the directory specified by the
	//! INSERT_LIBX_NAMESPACE::gnutls::tlsparamsdir
	//! \ref property "application property"
	//! for a public
	//! key parameter file. The following files are searched, and the first
	//! one found gets imported. An %exception gets thrown if no files were
	//! found:
	//!
	//! - user.{USERNAME}.suffix, where {USERNAME} is returned by geteuid().
	//!
	//! - group.{GROUPNAME}.suffix, where {GROUPNMAME} is returned by
	//! getegid(), and any supplementary groups this process belongs to.
	//!
	//! - system.suffix

	static datum_t open_default(const std::string &suffix);
};

//! Base class for ::pkparams

//! The base class provides the custom create() function
//!
class pkparamsBase {

public:

	//! Use a single implementation to construct pkparams and pkparamsptr.

	//! The implementation returns a pkparams.

	class newobj {

	public:
		//! Create a new temporary public key parameter

		static pkparams create(//! Public key ID
				       gnutls_pk_algorithm_t pkid);

		//! Create a new temporary public key parameter

		//! \overload
		//!
		static pkparams create(const std::string &pkidname);
	};

	//! Implement pkparams::create() and pkparamsptr::create()

	template<typename ptrrefType> class objfactory : public newobj {
	};
};

#if 0
{
#endif
}

#endif
