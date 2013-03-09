/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_x509_crtobj_H
#define x_tls_x509_crtobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/exception.H>
#include <x/vector.H>
#include <gnutls/x509.h>
#include <x/gnutls/x509_crtfwd.H>
#include <x/gnutls/x509_crtobjfwd.H>
#include <x/gnutls/init.H>
#include <x/gnutls/x509_crt.H>
#include <x/gnutls/x509_privkey.H>
#include <x/gnutls/datumwrapper.H>
#include <x/gnutls/pubkey.H>
#include <x/gnutls/digest_algorithm.H>

#include <ctime>
#include <stdint.h>
#include <list>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A wrapper for a \c gnutls_x509_crt_t

//! \see gnutls::x509::crt
//!

class gnutls::x509::crtObj : virtual public obj {

	//! The underlying GnuTLS handle

	gnutls_x509_crt_t crt;

public:
	//! Attach a crtObj to a native \c gnutls_x509_crt_t

	//! \internal
	//!
	crtObj(gnutls_x509_crt_t crtArg) LIBCXX_HIDDEN;

	friend class gnutls::credentials::certificateObj;
	friend class gnutls::x509::crtBase;

	//! The default constructor
	crtObj();

	//! The default destructor
	~crtObj() noexcept;

	//! Match the server/host name

	//! \return \c true if this certificate's host matches the given name,
	//! \c false otherwise

	bool check_hostname(//! DNS name
			    const std::string &name) const noexcept;

	//! Check this certificate's issuer

	//! \return \c true if the given certificate is this certificate's
	//! issuer.

	bool check_issuer(//! Potential issuer
			  const gnutls::x509::crt &issuer)
		const;

	//! Export this certificate

	//! \return the certificate in \c PEM o \c DER format
	//!
	datum_t export_cert(//! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
			    gnutls_x509_crt_fmt_t format) const
;

	//! Export this certificate to a file

	//! \overload
	//!
	void export_cert(//! Filename
			 const std::string &filename,
			 //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
			 gnutls_x509_crt_fmt_t format) const;

	//! Import a certificate

	void import_cert(//! The certificate
			 datum_t cert,

			 //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
			 gnutls_x509_crt_fmt_t format
			 );

	//! Import a list of certificates

	static void import_cert_list( //! Certificate objects placed here
				     std::list<gnutls::x509::crt> &certList,

				     //! The certificates

				     datum_t certs,

				     //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
				     gnutls_x509_crt_fmt_t format)
;

	//! Set the certificate's activation time

	void set_activation_time(//! Activation time
				 time_t   	act_time);

	//! Set this certificate's type

	void set_basic_constraints(//! If \c true - this is a CA
				   bool ca,

				   //! Maximum trusted path

				   //! If < 0, no limits on cross certs, else
				   //! limits cross-certs

				   int pathLenConstraint = -1)
;

	//! Set this certificate's subject

	void set_dn_by_oid( //! Some \c GNUTLS_OID defined constant

			   const std::string &oid,

			   //! Value, not raw.
			   const std::string &value);

	//! Set this certificate's issuer's subject

	void set_issuer_dn_by_oid( //! Some \c GNUTLS_OID defined constant

				  const std::string &oid,

				  //! Value, not raw.
				  const std::string &value);

	//! Set this certificate's expiration time

	void set_expiration_time(//! Expiration time
				 time_t   	exp_time);


	//! Set this certificate's public key

	void set_key(//! This certificate's private key
		     const gnutls::x509::privkey &key);

	//! Set the key's usage

	void set_key_usage(//! A bitmask of \c GNUTLS_KEY_* flags
			   unsigned int flags=GNUTLS_KEY_DIGITAL_SIGNATURE |
			   GNUTLS_KEY_NON_REPUDIATION |
			   GNUTLS_KEY_KEY_ENCIPHERMENT |
			   GNUTLS_KEY_DATA_ENCIPHERMENT |
			   GNUTLS_KEY_KEY_AGREEMENT );

	//! Set this certificate's allowed purpose

	void set_key_purpose_oid(//! Some \c GNUTLS_KP constant
				 const std::string &oid,

				 //! If this is a critical purpose
				 bool critical);

	//! Set this certificate's serial number

	//! \note Use this function when serial number are expected to fit
	//! within a natural integer type. Certificate serial numbers
	//! may exceed the size of the largest available integer type,
	//! use set_serial(datum_t) or set_serial(const unsigned char *, size_t);

	template<typename intval> void set_serial(//! Serial number
						  intval sernum)

	{
		unsigned char buf[sizeof(sernum)];

		size_t n=sizeof(sernum);

		while (n)
		{
			buf[--n]= (unsigned char)sernum;

			sernum >>= 8;
		}

		set_serial(buf, sizeof(buf));
	}

	//! Set this certificate's serial number

	//! Set an arbitrary large certificate's serial number
	//! that's expressed as a raw datum object.
	//!
	void set_serial(datum_t id)
	{
		set_serial(&(*id)[0], id->size());
	}

	//! Set this certificate's serial number

	//! Set an arbitrary large certificate's serial number
	//! that's expressed as a raw octet vector.

	void set_serial(const unsigned char *buf, size_t buf_size)
;

	//! Set the alternative subject name
	void set_subject_alternative_name( //! Type, one of \c GNUTLS_SAN_*
					  gnutls_x509_subject_alt_name_t type,

					  //! The name
					  const std::string &name)
;

	//! Set the subject key ID certificate extension

	void set_subject_key_id(//! The key ID
				datum_t id);

	//! Set the subject key ID certificate extension

	//! \overload
	//! The default key id is obtained by get_key_id().

	void set_subject_key_id()
	{
		set_subject_key_id(get_key_id());
	}

	//! Set this certificate's X.509 version

	void set_version(//! X.509 version of this certificate
			 unsigned intversion=3);

	//! Sign this certificate

	//! If this certificate's serial number is not set, a serial number
	//! is generated from the current system time. This is less than ideal,
	//! where possible set the serial number explicitly.

	void sign(//! The signer's certificate
		  const gnutls::x509::crt &issuer,

		  //! The authority's private key,
		  const gnutls::x509::privkey &issuer_key);


	//! Sign this certificate

	//! \overload
	void sign(//! The signer's certificate
		  const gnutls::x509::crt &issuer,

		  //! The authority's private key,
		  const gnutls::x509::privkey &issuer_key,

		  //! The signature algorithm
		  const digest_algorithm &algo);

	//! Verify this certificate

	//! \return 0 if this certificate is good, a bitmask of
	//! \c gnutls_certificate_status_t error flags otherwise.

	template<typename container_t>
	unsigned int crt_verify(//! Container of trusted certificates
				const container_t &trustedCerts,

				//! Verification flags

				//! Optional \c GNUTLS_VERIFY flags
				//!
				unsigned int flags=0) const
	{
		std::vector<gnutls_x509_crt_t> ca_list;

		ca_list.reserve(trustedCerts.size());

		typename container_t::const_iterator
			b=trustedCerts.begin(),
			e=trustedCerts.end();

		while (b != e)
		{
			ca_list.push_back( (*b)->crt );
			++b;
		}

		return crt_verify_internal(ca_list, flags);
	}
private:
	//! Internal verification function

	//! \internal
	//!
	unsigned int crt_verify_internal(//! A vector of trusted certificates
					 std::vector<gnutls_x509_crt_t> &ca,

					 //! Verification flags
					 unsigned int flags)
		const;
public:
	//! Retrieve this certificate's activation time
	time_t get_activation_time() const;

	//! Retrieve this certificate's basic constraints

	void get_basic_constraints(//! Return value - this is a CA

				   bool &ca,

				   //! Return value - critical

				   bool &critical,

				   //! Path length constraint
				   int &pathLenConstraint)
		const;
	//! Retrieve formatted DNs

	//! \return the name of this certificate in the form
	//! "C=xxxx,O=yyyy,CN=zzzz".

	std::string get_dn() const;

	//! Retrieve a single part of the DN as a string

	//! This is a convenience function that takes an OID and returns
	//! the given part of the certificate's name, as a string.
	//!
	//! \return If the OID occurs more than once in the DN, the values
	//! in the returned string are separated by newlines.
	//! \note The last occurence is not followed by a newline.
	std::string get_dn(//! OID of the DN's part
			   const std::string &oid) const;

	//! Retrieve a single part of the DN as a string

	//! \return The \c commonName, the hostname, as string.
	//!
	std::string get_dn_hostname() const
	{
		return get_dn(GNUTLS_OID_X520_COMMON_NAME);
	}

	//! Retrieve a specific part of this certificate's name

	void get_dn_by_oid(//! Some \c GNUTLS_OID defined constant
			   const std::string &oid,

			   //! The DN is placed here.

			   //! The list will have more than one entry if the
			   //! given part occurs more than once in this
			   //! certificate's name.
			   std::list<datum_t> &dnRet,

			   //! If \c true, retrieve the DER-encoded value
			   bool raw=false) const;

	//! Retrieve the list of DNs in the certificate's subject

	void get_all_dns(//! All DNs are placed here
			 std::list<std::string> &dnRet) const;

	//! Retrieve the certificate's expiration time

	time_t get_expiration_time() const;

	//! Retrieve a certificate extension given its \c OID

	void get_extension_by_oid(//! Some \c GNUTLS_OID defined constant
				  const std::string &oid,

				  //! The extension data is placed here.

				  //! This list will have multiple entries
				  //! if the certificate has more than one
				  //! extension record for the given \c OID.
				  std::list<datum_t> &dnRet,

				  //! Whether the extension is marked critical
				  bool &critical) const;

	//! Retrieve a list of all certificate extensions

	void get_all_extensions_list(//! List of <oid,critical> tuples returned
				     std::list< std::pair<std::string, bool> >
				     &extensionListRet) const;

	//! Compute this certificate's fingerprint

	datum_t get_fingerprint() const;

	//! Compute this certificate's fingerprint

	datum_t get_fingerprint(//! The fingerprint algorithm
				const digest_algorithm &algo) const;

	//! Retrieve this certificate's \c DN.

	//! \return the name of this ertificate's issuer, in the form
	//! "C=xxxx,O=yyyy,CN=zzzz".

	std::string get_issuer_dn() const;

	//! Retrieve a single part of the issuer's DN as a string

	//! This is a convenience function that takes an OID and returns
	//! the given part of the certificate's issuer's name, as a string.
	//!
	//! \return If the OID occurs more than once in the DN, the values
	//! in the returned string are separated by newlines.
	//! \note The last occurence is not followed by a newline.

	std::string get_issuer_dn(const std::string &oid)
		const;

	//! Retrieve a specific part of the issuer's name.

	void get_issuer_dn_by_oid(//! Some \c GNUTLS_OID defined constant
				  const std::string &oid,

				  //! The DN is placed here.

				  //! The list will have more than one entry
				  //! if the given part occurs more than once
				  //! in this certificate's issuer's name.

				  std::list<datum_t> &dnRet,

				  //! If \c true, retrieve the DER-encoded value
				  bool raw=false) const;

	// Retrieve this certificate's key's identifier

	//! \return a hash (normally \c SHA1) of this certificate's
	//! public key.
	datum_t get_key_id() const;

	//! Retrieve this certificate's purpose extension

	void get_key_purpose_oid(//! Returned list of <oid,critical> tuples
				 std::list<std::pair<std::string, bool> >
				 &retList) const;

	//! Retrieve this certificate's usage indicator

	//! \return a bitmask containing various \c GNUTLS_KEY_* constants.
	//!
	unsigned int get_key_usage(//! Critical flag returned
				   bool &critical) const;

	//! Retrieve this certificate's public key algorithm and number of bits

	//! \return \c GNUTLS_PK_RSA or \c GNUTLS_PK_DSA

	gnutls_pk_algorithm_t get_pk_algorithm(//! Number of bits
					       unsigned int &bitsArg)
		const;

	//! Retrieve this certificate's public key algorithm and number of bits

	//! \overload

	gnutls_pk_algorithm_t get_pk_algorithm()
		const
	{
		unsigned dummy;

		return get_pk_algorithm(dummy);
	}

	//! Retrieve this certificate's raw public key

	//! A reference to this certificate's raw public key.
	//!
	pubkey get_pk() const;

	//! Retrieve this certificate's raw serial number
	datum_t get_serial_raw() const;

	//! Retrieve this certificate's serial number
	uint64_t get_serial() const;

	//! Retrieve this certificate's signature

	datum_t get_signature() const;

	//! Retrieve this certificate's signature algorithm

	gnutls_sign_algorithm_t get_signature_algorithm()
		const;

	//! Retrieve this certificate's alternative names extension data

	void get_subject_alternative_names(//! Returned <type,contents> tuple list
					   std::list<std::pair<
					   gnutls_x509_subject_alt_name_t,
					   datum_t> > &namesRet,
					   //! Returns critical extension flag
					   bool &criticalRet)
		const;

	//! Retrieve this certificate's subject key extension data

	datum_t get_subject_key_id(//! Returns critical extension flag
				   bool &criticalRet)
		const;

	//! Retrieve this certificate's version
	int get_version() const;

	//! Pretty-print this certificate

	std::string print(//! Output format

			  //! Possible values: \c GNUTLS_X509_CRT_FULL,
			  //! \c GNUTLS_X509_CRT_ONELINE,
			  //! \c GNUTLS_X509_CRT_UNSIGNED_FULL.


			  gnutls_certificate_print_formats_t format=
			  GNUTLS_X509_CRT_FULL)
		const;
};

namespace gnutls {
	namespace x509 {

#if 0
	};
};
#endif

inline void crtBase
::import_cert_list(std::list<crt> &certList,
		   gnutls::datum_t certs,
		   gnutls_x509_crt_fmt_t format)

{
	gnutls::x509::crtObj::import_cert_list(certList, certs, format);
}

inline void crtBase
::import_cert_list(std::list<crt> &certList,
		   const gnutls::datumwrapper &certs,
		   gnutls_x509_crt_fmt_t format)

{
	gnutls::x509::crtObj::import_cert_list(certList,
					       certs.operator gnutls::datum_t(),
					       format);
}

inline void crtBase
::import_cert_list(std::list<crt> &certList,
		   const std::string &filename,
		   gnutls_x509_crt_fmt_t format)

{
	gnutls::datum_t datum(gnutls::datum_t::create());

	datum->load(filename);

	gnutls::x509::crtObj::import_cert_list(certList, datum, format);
}

inline void crtBase
::import_cert_list(std::list<crt> &certList,
		   const char *filename,
		   gnutls_x509_crt_fmt_t format)

{
	import_cert_list(certList, std::string(filename), format);
}

#ifndef DOXYGEN_FIXME

template<typename cert_chain_container_t,
	 typename ca_list_container_t>
unsigned int crtBase
::verify_cert_list(const cert_chain_container_t &certList,
		   const ca_list_container_t &caList,
		   unsigned int flags)

{
	std::vector<gnutls_x509_crt_t> cert_list, ca_list;

	{
		typename cert_chain_container_t::const_iterator cb, ce;

		cert_list.reserve(certList.size());

		cb=certList.begin();
		ce=certList.end();

		while (cb != ce)
			cert_list.push_back( (*cb++)->crt );
	}

	{
		typename ca_list_container_t::const_iterator cb, ce;

		ca_list.reserve(caList.size());


		cb=caList.begin();
		ce=caList.end();

		while (cb != ce)
			ca_list.push_back( (*cb++)->crt );
	}

	return verify_cert_list_internal(cert_list, ca_list, flags);
}
#endif

#if 0
{
	{
		{
#endif
		}
	}
}
#endif
