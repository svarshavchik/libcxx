/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_alerts_H
#define x_tls_alerts_H

#include <x/namespace.h>
#include <x/exception.H>

#include <gnutls/gnutls.h>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

		//! This exception gets thrown when a gnutls call fails

		class errexception : public exception {

		public:
			const int errcode; //!< gnutls error code

			using exception::operator*;
			using exception::operator->;

			//! Constructor
			errexception(int errcodeArg,
				     const char *funcnameArg)
 LIBCXX_HIDDEN;

			//! Destructor
			~errexception();
		};

		//! This exception gets thrown by sessionObj::recv() when it receives a TLS alert record

		class alertexception : public exception {

		public:
			//! This is a fatal alert
			const bool fatal;

			//! Alert code
			const gnutls_alert_description_t alertcode;

			using exception::operator*;
			using exception::operator->;

			//! Constructor
			alertexception(bool fatalArg,
				       gnutls_alert_description_t alertcodeArg)
;

			//! Destructor
			~alertexception();
		};

	}
}
#endif
