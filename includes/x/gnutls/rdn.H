/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_rdn_H
#define x_tls_rdn_H

#include <x/namespace.h>
#include <x/exception.H>
#include <gnutls/gnutls.h>
#include <string>
#include <set>
#include <vector>
#include <list>
#include <iterator>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

		//! Utility functions for parsing raw DER-encoded RDN sequences

		namespace rdn {

#if 0
		};
	};
};
#endif

//! Internal implementation function

bool raw_get_oid_internal(const gnutls_datum_t &datum,
			  size_t i,
			  std::vector<char> &oid);

//! Internal implementation function

bool raw_get_oid_internal(const gnutls_datum_t &datum,
			  size_t i,
			  const char *oid,
			  std::vector<char> &value,
			  size_t &value_s,
			  bool raw);

//! Retrieve all OIDs in a raw DER-encoded RDN sequences

//! The list of all OIDs in the datum are inserted into the given iterator.
//! The strings given to the iterator will be the various \c GNUTLS_OID literals
//! defined in gnutls/x509.h

template<typename OutputIterator>
OutputIterator raw_get_oid_iter(//! Output iterator
				OutputIterator iter,

				//! The raw DER-encoded RDN sequence
				const gnutls_datum_t &datum)

{
	size_t i;
	std::vector<char> oid;

	for (i=0; raw_get_oid_internal(datum, i, oid); ++i)
		*iter++ = std::string(&oid[0]);

	return iter;
}

//! Retrieve all values of an OID in a raw DER-encoded RDN sequences

//! The list of all values for a given OIDs in the datum are inserted
//! into the given iterator.

template<typename OutputIterator>
OutputIterator raw_get_oid_iter(//! Output iterator
				OutputIterator iter,

				//! Raw DER-encoded RDN sequence
				const gnutls_datum_t &datum,

				//! The OID to retrieve, one of \c GNUTLS_OID values
				const char *oid,

				//! Retrieve raw values
				bool raw=false)

{
	typedef typename OutputIterator::container_type::value_type value_type;

	size_t i;
	std::vector<char> value;
	size_t value_s;

	for (i=0; raw_get_oid_internal(datum, i, oid, value, value_s, raw); ++i)
	{
		if (value_s)
			*iter++=value_type(&value[0], &value[0]+value_s);
	}

	return iter;
}

//! Retrieve all values of an OID in a raw DER-encoded RDN sequences

//! \overload
//!

template<typename OutputIterator>
OutputIterator raw_get_oid_iter(//! Output iterator
				OutputIterator iter,

				//! Raw DER-encoded RDN sequence
				const gnutls_datum_t &datum,

				//! The OID to retrieve, one of \c GNUTLS_OID values
				const std::string &oid,

				//! Retrieve raw values
				bool raw=false)

{
	return raw_get_oid_iter(iter, datum, oid.c_str(), false);
}

//! Use raw_get_oid_iter() to populate a container with the list of OIDs.

//! \note
//! The existing contents of the container are not erased.

template<typename container_type_t>
void raw_get_oid(//! The raw DER-encoded RDN sequence
		 const gnutls_datum_t &datum,

		 //! Container for the OIDs
		 container_type_t &oid)
{
	raw_get_oid_iter(std::insert_iterator<container_type_t>(oid, oid.end()),
			 datum);
}

//! Use raw_get_oid_iter() to populate a container with the list of values for an OID

//! \note
//! The existing contents of the container are not erased.

template<typename container_type_t, typename oid_t>
void raw_get_oid(//! The raw DER-encoded RDN sequence
		 const gnutls_datum_t &datum,

		 //! Container for the OID values
		 container_type_t &values,

		 //! The OID
		 const oid_t &oid,

		 //! Retrieve raw values

		 bool raw=false)
{
	raw_get_oid_iter(std::insert_iterator<container_type_t>
			 (values, values.end()),
			 datum, oid, raw);
}

//! Format a raw DER-encoded RDN sequence as a verbose string.

std::string raw_get(//! The raw DER-encoded RDN sequence
		    const gnutls_datum_t &datum);

//! This template is instantiated inside the library.

extern template
void raw_get_oid(const gnutls_datum_t &datum,
		 std::set<std::string> &oid);

//! This template is instantiated inside the library.

extern template
void raw_get_oid(const gnutls_datum_t &datum,
		 std::vector<std::string> &oid);
//! This template is instantiated inside the library.

extern template
void raw_get_oid(const gnutls_datum_t &datum,
		 std::list<std::string> &oid);

//! This template is instantiated inside the library.

extern template
void raw_get_oid(const gnutls_datum_t &datum,
		 std::set<std::string> &values,
		 const std::string &oid,
		 bool raw);

//! This template is instantiated inside the library.

extern template
void raw_get_oid(const gnutls_datum_t &datum,
		 std::vector<std::string> &values,
		 const std::string &oid,
		 bool raw);

//! This template is instantiated inside the library.

extern template
void raw_get_oid(const gnutls_datum_t &datum,
		 std::list<std::string> &values,
		 const std::string &oid,
		 bool raw);

//! This template is instantiated inside the library.

extern template
void raw_get_oid(const gnutls_datum_t &datum,
		 std::set<std::string> &values,
		 const char * const &oid,
		 bool raw);

//! This template is instantiated inside the library.

extern template
void raw_get_oid(const gnutls_datum_t &datum,
		 std::vector<std::string> &values,
		 const char * const &oid,
		 bool raw);

//! This template is instantiated inside the library.

extern template
void raw_get_oid(const gnutls_datum_t &datum,
		 std::list<std::string> &values,
		 const char * const &oid,
		 bool raw);

#if 0
{
	{
		{
#endif
		}
	}
}

#endif
