/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "gnutls/init.H"
#include "gnutls/x509_crt.H"
#include "gnutls/rsapubkey.H"
#include "gnutls/dsapubkey.H"
#include "sysexception.H"

#include <sys/time.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

#define CHK_GNUTLS_ALLOC(rc,buf_s,func)		\
	do {					\
	if ((rc) < 0 && (rc) != GNUTLS_E_SHORT_MEMORY_BUFFER)	\
		chkerr((rc),(func));				\
	} while (0)

#define FINISH_GNUTLS_ALLOC(buf, buf_s) ((buf)->resize(buf_s))

gnutls::x509::crtObj::crtObj(gnutls_x509_crt_t crtArg)
	: crt(crtArg)
{
}

gnutls::x509::crtObj::crtObj()
{
	gnutls::init::gnutls_init();

	chkerr(gnutls_x509_crt_init(&crt), "gnutls_x509_crt_init");
}

gnutls::x509::crtObj::~crtObj() noexcept
{
	gnutls_x509_crt_deinit(crt);
}

bool gnutls::x509::crtObj::check_hostname(const std::string &name) const noexcept
{
	return gnutls_x509_crt_check_hostname(crt, name.c_str()) != 0;
}

bool gnutls::x509::crtObj::check_issuer(const gnutls::x509::crt &issuer) const

{
	int rc=gnutls_x509_crt_check_issuer(this->crt, issuer->crt);

	chkerr(rc, "gnutls_x509_crt_check_issuer");

	return rc != 0;
}

gnutls::datum_t gnutls::x509::crtObj::export_cert(gnutls_x509_crt_fmt_t format)
	const
{
	size_t buf_s=0;

	int rc=gnutls_x509_crt_export(crt, format, NULL, &buf_s);

	CHK_GNUTLS_ALLOC(rc, buf_s, "gnutls_x509_crt_export");

	datum_t ret(datum_t::create());

	ret->reserve(buf_s);
	ret->resize(buf_s);

	chkerr(gnutls_x509_crt_export(crt, format, &(*ret)[0], &buf_s),
	       "gnutls_x509_crt_export");

	FINISH_GNUTLS_ALLOC(ret, buf_s);

	return ret;
}

void gnutls::x509::crtObj::export_cert(const std::string &filename,
				       gnutls_x509_crt_fmt_t format)
	const
{
	gnutls::datum_t exp=export_cert(format);

	fd file(fd::create(filename, 0666));

	if (format == GNUTLS_X509_FMT_PEM)
	{
		(*file->getostream()) << print() << std::endl;
	}

	exp->save(file);
}

//! Import a certificate

void gnutls::x509::crtObj::import_cert(datum_t cert,
				       gnutls_x509_crt_fmt_t format
				       )
{
	gnutls::init::gnutls_init();

	gnutls_datum_t datum;

	datum.data=&(*cert)[0];
	datum.size=cert->size();

	chkerr(gnutls_x509_crt_import(crt, &datum, format),
	       "gnutls_x509_crt_import");
}

void gnutls::x509::crtObj::set_activation_time(time_t act_time)
{
	chkerr(gnutls_x509_crt_set_activation_time(crt, act_time),
	       "gnutls_x509_crt_set_activation_time");
}

void gnutls::x509::crtObj::set_basic_constraints(bool ca,
						 int pathLenConstraint)

{
	chkerr(gnutls_x509_crt_set_basic_constraints(crt, ca,
						     pathLenConstraint),
	       "gnutls_x509_crt_set_basic_constraints");
}

void gnutls::x509::crtObj::set_dn_by_oid(const std::string &oid,
					 const std::string &value)

{
	chkerr(gnutls_x509_crt_set_dn_by_oid(crt, oid.c_str(), 0, value.c_str(),
					     value.size()),
	       "gnutls_x509_crt_set_dn_by_oid");
}

void gnutls::x509::crtObj::set_issuer_dn_by_oid(const std::string &oid,
						const std::string &value)

{
	chkerr(gnutls_x509_crt_set_issuer_dn_by_oid(crt, oid.c_str(), 0, value.c_str(),
						    value.size()),
	       "gnutls_x509_crt_set_issuer_dn_by_oid");
}

void gnutls::x509::crtObj::set_expiration_time(time_t exp_time)
{
	chkerr(gnutls_x509_crt_set_expiration_time(crt, exp_time),
	       "gnutls_x509_crt_set_expiration_time");
}

void gnutls::x509::crtObj::set_key(const gnutls::x509::privkey &key)

{
	chkerr(gnutls_x509_crt_set_key(crt, key->privkey),
	       "gnutls_x509_crt_set_key");
}

void gnutls::x509::crtObj::set_key_usage(unsigned int flags)
{
	chkerr(gnutls_x509_crt_set_key_usage(crt, flags),
	       "gnutls_x509_crt_set_key_usage");
}


void gnutls::x509::crtObj::set_key_purpose_oid(const std::string &oid,
					       bool critical)
{
	chkerr(gnutls_x509_crt_set_key_purpose_oid(crt, oid.c_str(),
						   critical ? 1:0),
	       "gnutls_x509_crt_set_key_purpose_oid");
}

void gnutls::x509::crtObj::set_serial(const unsigned char *buf,
				      size_t buf_size)

{
	chkerr(gnutls_x509_crt_set_serial(crt, buf, buf_size),
	       "gnutls_x509_crt_set_serial");
}

void gnutls::x509::crtObj::set_subject_alternative_name(gnutls_x509_subject_alt_name_t type,
							const std::string &name)

{
	chkerr(gnutls_x509_crt_set_subject_alternative_name(crt, type,
							name.c_str()),
	       "gnutls_x509_crt_set_subject_alternative_name");
}

void gnutls::x509::crtObj::set_subject_key_id(datum_t id)
{
	chkerr(gnutls_x509_crt_set_subject_key_id(crt, &(*id)[0],
						  id->size()),
	       "gnutls_x509_crt_set_subject_key_id");
}

void gnutls::x509::crtObj::set_version(unsigned intversion)
{
	chkerr(gnutls_x509_crt_set_version(crt, intversion),
	       "gnutls_x509_crt_set_version");
}

void gnutls::x509::crtObj::sign(const gnutls::x509::crt &issuer,
				const gnutls::x509::privkey &issuer_key)

{
	return sign(issuer, issuer_key, GNUTLS_DIG_SHA1);
}

void gnutls::x509::crtObj::sign(const gnutls::x509::crt &issuer,
				const gnutls::x509::privkey &issuer_key,
				const digest_algorithm &algo)

{
	size_t buf_s=0;

	if (gnutls_x509_crt_get_serial(crt, NULL, &buf_s) ==
	    GNUTLS_E_ASN1_VALUE_NOT_FOUND)
	{
		struct timeval tv;

		if (gettimeofday(&tv, 0) < 0)
			throw SYSEXCEPTION("gettimeofday");

		// usec ranges from 0 to 999999, or 3 bytes' worth

		unsigned char tvbuf[sizeof(tv.tv_sec) + 3];

		size_t i;

		for (i=0; i<3; i++)
		{
			tvbuf[sizeof(tvbuf)-1-i]= (unsigned char)tv.tv_usec;

			tv.tv_usec >>= 8;
		}

		for (i=0; i<sizeof(tv.tv_sec); i++)
		{
			tvbuf[sizeof(tvbuf)-3-1-i]=
				(unsigned char)tv.tv_sec;

			tv.tv_sec >>= 8;
		}

		set_serial(tvbuf, sizeof(tvbuf));
	}

	chkerr(gnutls_x509_crt_sign2(crt, issuer->crt, issuer_key->privkey,
				     algo, 0),
	       "gnutls_x509_crt_sign2");
}

unsigned int gnutls::x509::crtObj::crt_verify_internal(std::vector<gnutls_x509_crt_t> &ca,

						       unsigned int flags)
	const
{
	unsigned verify=0;

	chkerr(gnutls_x509_crt_verify(crt, &ca[0], ca.size(), flags, &verify),
	       "gnutls_x509_crt_verify");

	return verify;
}

time_t gnutls::x509::crtObj::get_activation_time() const
{
	time_t t=gnutls_x509_crt_get_activation_time(crt);

	if (t == (time_t)-1)
		throw EXCEPTION("gnutls_x509_crt_get_activation_time");

	return t;
}

void gnutls::x509::crtObj::get_basic_constraints(bool &ca,
						 bool &critical,
						 int &pathLenConstraint)
	const
{
	unsigned int caRet;
	unsigned criticalRet;

	chkerr(gnutls_x509_crt_get_basic_constraints(crt, &criticalRet,
						     &caRet,
						     &pathLenConstraint),
	       "gnutls_x509_crt_get_basic_constraints");

	ca= caRet != 0;
	critical=criticalRet != 0;
}

std::string gnutls::x509::crtObj::get_dn() const
{
	size_t buf_s=0;
	int rc=gnutls_x509_crt_get_dn(crt, NULL, &buf_s);

	CHK_GNUTLS_ALLOC(rc, buf_s, "gnutls_x509_crt_get_dn");

	char buf[buf_s];
	chkerr(gnutls_x509_crt_get_dn(crt, buf, &buf_s),
	       "gnutls_x509_crt_get_dn");

	return std::string(buf, buf+buf_s);
}

std::string gnutls::x509::crtObj::get_dn(const std::string &oid) const

{
	std::list<datum_t> dnList;

	get_dn_by_oid(oid, dnList, false);

	std::ostringstream o;
	const char *sep="";

	while (!dnList.empty())
	{
		o << sep;
		sep="\n";
		
		std::vector<char> n;

		n.reserve(dnList.front()->size()+1);

		n.insert(n.end(), dnList.front()->begin(),
			 dnList.front()->end());

		n.push_back(0);
		o << &n[0];
		dnList.pop_front();
	}

	return o.str();
}

void gnutls::x509::crtObj::get_dn_by_oid(const std::string &oid,
					 std::list<datum_t> &dnRet,
					 bool raw) const
{
	dnRet.clear();

	int indx=0;

	while (1)
	{
		size_t buf_s=0;
		int rc=gnutls_x509_crt_get_dn_by_oid(crt, oid.c_str(),
						     indx, raw ? 1:0,
						     NULL, &buf_s);

		if (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
			break;

		CHK_GNUTLS_ALLOC(rc, buf_s, "gnutls_x509_crt_get_dn_by_oid");

		datum_t buf=datum_t::create();

		buf->reserve(buf_s);
		buf->resize(buf_s);

		chkerr(gnutls_x509_crt_get_dn_by_oid(crt, oid.c_str(),
						     indx, raw ? 1:0,
						     &(*buf)[0], &buf_s),
		       "gnutls_x509_crt_get_dn_by_oid");

		FINISH_GNUTLS_ALLOC(buf, buf_s);

		dnRet.push_back(buf);
		++indx;
	}
}

void gnutls::x509::crtObj::get_all_dns(std::list<std::string> &dnRet)
	const
{
	dnRet.clear();

	int indx=0;

	while (1)
	{
		size_t buf_s=0;
		int rc=gnutls_x509_crt_get_dn_oid(crt, indx, NULL, &buf_s);

		if (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
			break;

		CHK_GNUTLS_ALLOC(rc, buf_s, "gnutls_x509_crt_get_dn_oid");

		char buf[buf_s];

		chkerr(gnutls_x509_crt_get_dn_oid(crt, indx, buf, &buf_s),
		       "gnutls_x509_crt_get_dn_oid");
		dnRet.push_back(std::string(buf, buf+buf_s));
		++indx;
	}
}

time_t gnutls::x509::crtObj::get_expiration_time() const
{
	time_t t=gnutls_x509_crt_get_expiration_time(crt);

	if (t == (time_t)-1)
		throw EXCEPTION("gnutls_x509_crt_get_expiration_time");

	return t;
}

void gnutls::x509::crtObj::get_extension_by_oid(const std::string &oid,
						std::list<datum_t> &dnRet,
						bool &critical)
	const
{
	dnRet.clear();

	int indx=0;

	unsigned int critflag=0;

	while (1)
	{
		size_t buf_s=0;
		int rc=gnutls_x509_crt_get_extension_by_oid(crt, oid.c_str(),
							    indx,
							    NULL, &buf_s,
							    &critflag);

		if (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
			break;

		CHK_GNUTLS_ALLOC(rc, buf_s,
				 "gnutls_x509_crt_get_extension_by_oid");

		datum_t buf=datum_t::create();

		buf->reserve(buf_s);
		buf->resize(buf_s);

		chkerr(gnutls_x509_crt_get_extension_by_oid(crt, oid.c_str(),
							    indx,
							    &(*buf)[0], &buf_s,
							    &critflag),
		       "gnutls_x509_crt_get_dn_by_oid");
		FINISH_GNUTLS_ALLOC(buf, buf_s);

		dnRet.push_back(buf);
		++indx;
	}

	critical=critflag != 0;
}

void gnutls::x509::crtObj::get_all_extensions_list(std::list< std::pair<std::string, bool> >
						   &extensionListRet)
	const
{
	extensionListRet.clear();

	int indx=0;

	while (1)
	{
		unsigned int critflag=0;
		size_t buf_s=0;
		int rc=gnutls_x509_crt_get_extension_info(crt, indx,
							  NULL, &buf_s,
							  &critflag);

		if (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
			break;

		CHK_GNUTLS_ALLOC(rc, buf_s,
				 "gnutls_x509_crt_get_extension_info");

		char buf[buf_s];

		chkerr(gnutls_x509_crt_get_extension_info(crt, indx,
							  buf, &buf_s,
							  &critflag),
		       "gnutls_x509_crt_get_extension_info");

		// The returned buffer count seems to include the trailing \0
		extensionListRet.push_back
			(std::make_pair(std::string(std::string(buf, buf+buf_s)
						    .c_str()),
					(bool)(critflag != 0)));
		++indx;
	}
}

gnutls::datum_t gnutls::x509::crtObj::get_fingerprint() const
{
	return get_fingerprint(GNUTLS_DIG_SHA1);
}

gnutls::datum_t gnutls::x509::crtObj
::get_fingerprint(const digest_algorithm &algo)
	const
{
	size_t buf_s=0;

	int rc=gnutls_x509_crt_get_fingerprint(crt, algo, NULL, &buf_s);

	CHK_GNUTLS_ALLOC(rc, buf_s, "gnutls_x509_crt_get_fingerprint");

	datum_t ret(datum_t::create());

	ret->reserve(buf_s);
	ret->resize(buf_s);

	chkerr(gnutls_x509_crt_get_fingerprint(crt, algo, &(*ret)[0], &buf_s),
			"gnutls_x509_crt_get_fingerprint");
	FINISH_GNUTLS_ALLOC(ret, buf_s);
	return ret;
}

std::string gnutls::x509::crtObj::get_issuer_dn() const
{
	size_t buf_s=0;

	int rc=gnutls_x509_crt_get_issuer_dn(crt, NULL, &buf_s);

	CHK_GNUTLS_ALLOC(rc, buf_s, "gnutls_x509_crt_get_issuer_dn");

	char buf[buf_s];

	chkerr(gnutls_x509_crt_get_issuer_dn(crt, buf, &buf_s),
			"gnutls_x509_crt_get_issuer_dn");

	return std::string(buf, buf+buf_s);
}

std::string gnutls::x509::crtObj::get_issuer_dn(const std::string &oid)
	const
{
	std::list<datum_t> dnList;

	get_issuer_dn_by_oid(oid, dnList, false);

	std::ostringstream o;
	const char *sep="";

	while (!dnList.empty())
	{
		o << sep;
		sep="\n";
		
		std::vector<char> n;

		n.reserve(dnList.front()->size()+1);

		n.insert(n.end(), dnList.front()->begin(),
			 dnList.front()->end());

		n.push_back(0);
		o << &n[0];
		dnList.pop_front();
	}

	return o.str();
}

void gnutls::x509::crtObj::get_issuer_dn_by_oid(const std::string &oid,
						std::list<datum_t> &dnRet,
						bool raw) const
{
	dnRet.clear();

	int indx=0;

	while (1)
	{
		size_t buf_s=0;
		int rc=gnutls_x509_crt_get_issuer_dn_by_oid(crt, oid.c_str(),
							    indx, raw ? 1:0,
							    NULL, &buf_s);

		if (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
			break;

		CHK_GNUTLS_ALLOC(rc, buf_s,
				 "gnutls_x509_crt_get_issuer_dn_by_oid");

		datum_t buf=datum_t::create();

		buf->reserve(buf_s);
		buf->resize(buf_s);

		chkerr(gnutls_x509_crt_get_issuer_dn_by_oid(crt, oid.c_str(),
							    indx, raw ? 1:0,
							    &(*buf)[0], &buf_s),
		       "gnutls_x509_crt_get_issuer_dn_by_oid");
		FINISH_GNUTLS_ALLOC(buf, buf_s);

		dnRet.push_back(buf);
		++indx;
	}
}

gnutls::datum_t gnutls::x509::crtObj::get_key_id() const
{
	size_t buf_s=0;

	int rc=gnutls_x509_crt_get_key_id(crt, 0, NULL, &buf_s);

	CHK_GNUTLS_ALLOC(rc, buf_s, "gnutls_x509_crt_get_key_id");

	datum_t ret(datum_t::create());

	ret->reserve(buf_s);
	ret->resize(buf_s);

	chkerr(gnutls_x509_crt_get_key_id(crt, 0, &(*ret)[0], &buf_s),
			"gnutls_x509_crt_get_key_id");
	FINISH_GNUTLS_ALLOC(ret, buf_s);

	return ret;
}

void gnutls::x509::crtObj
::get_key_purpose_oid(std::list<std::pair<std::string, bool> >
		      &retList) const
{
	retList.clear();

	int indx=0;

	while (1)
	{
		unsigned critical=0;
		size_t buf_s=0;

		int rc=gnutls_x509_crt_get_key_purpose_oid(crt, indx,
							   NULL, &buf_s,
							   &critical);

		if (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
			break;

		CHK_GNUTLS_ALLOC(rc, buf_s,
				 "gnutls_x509_crt_get_key_purpose_oid");

		char buf[buf_s];

		chkerr(gnutls_x509_crt_get_key_purpose_oid(crt, indx,
							   buf, &buf_s,
							   &critical),
		       "gnutls_x509_crt_get_key_purpose_oid");

		retList.push_back(std::make_pair(std::string(buf, buf+buf_s),
						 (bool)(critical != 0)));
		++indx;
	}
}

unsigned int gnutls::x509::crtObj::get_key_usage(bool &critical)
	const
{
	unsigned int key_usage, critflag;

	chkerr(gnutls_x509_crt_get_key_usage(crt, &key_usage, &critflag),
	       "gnutls_x509_crt_get_key_usage");

	critical=critflag != 0;
	return key_usage;
}

gnutls_pk_algorithm_t gnutls::x509::crtObj
::get_pk_algorithm(unsigned int &bitsArg)
	const
{
	int rc=gnutls_x509_crt_get_pk_algorithm(crt, &bitsArg);

	chkerr(rc, "gnutls_x509_crt_get_pk_algorithm");

	return (gnutls_pk_algorithm_t)rc;
}

gnutls::pubkey gnutls::x509::crtObj::get_pk() const
{
	unsigned int bits;

	switch (get_pk_algorithm(bits)) {
	case GNUTLS_PK_RSA:

		{
			gnutls_datum_t m, e;

			chkerr(gnutls_x509_crt_get_pk_rsa_raw(crt, &m, &e),
			       "gnutls_x509_crt_get_pk_rsa_raw");

			gnutls::datumwrapper mw(m), ew(e);

			gnutls::rsapubkey rpk(gnutls::rsapubkey::create());

			rpk->m=mw;
			rpk->e=ew;

			return rpk;
		}

	case GNUTLS_PK_DSA:

		{
			gnutls_datum_t p, q, g, y;

			chkerr(gnutls_x509_crt_get_pk_dsa_raw(crt, &p, &q,
							      &g, &y),
			       "gnutls_x509_crt_get_pk_dsa_raw");

			gnutls::datumwrapper pw(p), qw(q), gw(g), yw(y);

			gnutls::dsapubkey dpk(gnutls::dsapubkey::create());

			dpk->p=pw;
			dpk->q=qw;
			dpk->g=gw;
			dpk->y=yw;

			return dpk;
		}
	default:
		break;
	}

	throw EXCEPTION("Unknown certificate public key type");
}

gnutls::datum_t gnutls::x509::crtObj::get_serial_raw() const
{
	size_t buf_s=0;

	int rc=gnutls_x509_crt_get_serial(crt, NULL, &buf_s);

	CHK_GNUTLS_ALLOC(rc, buf_s, "gnutls_x509_crt_get_serial");

	datum_t ser(datum_t::create());

	ser->reserve(buf_s);
	ser->resize(buf_s);

	chkerr(gnutls_x509_crt_get_serial(crt, &(*ser)[0], &buf_s),
	       "gnutls_x509_crt_get_serial");
	FINISH_GNUTLS_ALLOC(ser, buf_s);

	return ser;
}

uint64_t gnutls::x509::crtObj::get_serial() const
{
	datum_t ser=get_serial_raw();
	uint64_t n=0;

	datum_t::base::iterator b=ser->begin(), e=ser->end();

	while (b != e)
	{
		n <<= 8;
		n |= (*b) & 0xFF;
		++b;
	}
	return n;
}

gnutls::datum_t gnutls::x509::crtObj::get_signature() const
{
	size_t buf_s=0;

	int rc=gnutls_x509_crt_get_signature(crt, NULL, &buf_s);

	CHK_GNUTLS_ALLOC(rc, buf_s, "gnutls_x509_crt_get_signature");

	char tmpbuf[buf_s];

	chkerr(gnutls_x509_crt_get_signature(crt, tmpbuf, &buf_s),
	       "gnutls_x509_crt_get_signature");

	datum_t ser(datum_t::create());

	ser->reserve(buf_s);
	ser->resize(buf_s);

	std::copy(tmpbuf, tmpbuf+buf_s, ser->begin());
	FINISH_GNUTLS_ALLOC(ser, buf_s);

	return ser;
}

gnutls_sign_algorithm_t gnutls::x509::crtObj::get_signature_algorithm()
	const
{
	int rc=gnutls_x509_crt_get_signature_algorithm(crt);

	chkerr(rc, "gnutls_x509_crt_get_signature_type");

	return (gnutls_sign_algorithm_t)rc;
}

#if 1
void gnutls::x509::crtObj
::get_subject_alternative_names(std::list<std::pair<
				gnutls_x509_subject_alt_name_t,
				datum_t> > &namesRet,
				bool &criticalRet)
	const
{
	namesRet.clear();
	criticalRet=false;

	int indx=0;

	while (1)
	{
		unsigned critical=0;
		size_t buf_s=0;
		unsigned ret_type;

		int rc=gnutls_x509_crt_get_subject_alt_name2(crt, indx,
							     NULL, &buf_s,
							     &ret_type,
							     &critical);

		if (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
			break;

		CHK_GNUTLS_ALLOC(rc, buf_s,
				 "gnutls_x509_crt_get_subject_alt_name2");

		datum_t ret(datum_t::create());

		ret->reserve(buf_s);
		ret->resize(buf_s);

		chkerr(gnutls_x509_crt_get_subject_alt_name2(crt, indx,
							     &(*ret)[0], &buf_s,
							     &ret_type,
							     &critical),
		       "gnutls_x509_crt_get_subject_alt_name2");
		FINISH_GNUTLS_ALLOC(ret, buf_s);

		criticalRet= critical != 0;

		namesRet.push_back(std::make_pair
				   (
				    (gnutls_x509_subject_alt_name_t)
				    ret_type, ret));
		++indx;
	}
}
#endif

gnutls::datum_t gnutls::x509::crtObj::get_subject_key_id(bool &criticalRet)
	const
{
	size_t buf_s=0;
	unsigned int critical=0;

	int rc=gnutls_x509_crt_get_subject_key_id(crt, NULL, &buf_s,
						  &critical);

	CHK_GNUTLS_ALLOC(rc, buf_s, "gnutls_x509_crt_get_subject_key_id");

	datum_t ret(datum_t::create());

	ret->reserve(buf_s);
	ret->resize(buf_s);

	chkerr(gnutls_x509_crt_get_subject_key_id(crt, &(*ret)[0], &buf_s,
						  &critical),
			"gnutls_x509_crt_get_subject_key_id");
	FINISH_GNUTLS_ALLOC(ret, buf_s);

	criticalRet=critical != 0;
	return ret;
}

int gnutls::x509::crtObj::get_version() const
{
	int rc;

	chkerr((rc=gnutls_x509_crt_get_version(crt)),
	       "gnutls_x509_crt_get_version");
	return rc;
}

void gnutls::x509::crtObj
::import_cert_list(std::list<gnutls::x509::crt> &certList,
		   datum_t certs,
		   gnutls_x509_crt_fmt_t format)

{
	gnutls::init::gnutls_init();

	certList.clear();

	unsigned int bufs=0;

	gnutls_datum_t datum;

	datum.data=&(*certs)[0];
	datum.size=certs->size();

	int rc=gnutls_x509_crt_list_import(NULL, &bufs, &datum, format,
					   GNUTLS_X509_CRT_LIST_IMPORT_FAIL_IF_EXCEED);

	CHK_GNUTLS_ALLOC(rc, buf_s, "gnutls_x509_crt_list_import");

	std::vector<gnutls_x509_crt_t> buf;

	buf.reserve(bufs);
	buf.resize(bufs);

	chkerr(gnutls_x509_crt_list_import(&buf[0], &bufs, &datum, format,
					   GNUTLS_X509_CRT_LIST_IMPORT_FAIL_IF_EXCEED),
	       "gnutls_x509_crt_list_import");
	FINISH_GNUTLS_ALLOC(&buf, bufs);

	size_t n=buf.size();

	try {
		while (n)
		{
			auto newCert=gnutls::x509::crt::create(buf[--n]);

			certList.push_back(newCert);
		}
	} catch (...)
	{
		while (n)
			gnutls_x509_crt_deinit(buf[--n]);
		certList.clear();
		throw;
	}
}

unsigned int gnutls::x509::crtBase
::verify_cert_list_internal(std::vector<gnutls_x509_crt_t> &certList,
			    std::vector<gnutls_x509_crt_t> &caList,
			    unsigned int flags)
{
	unsigned int result=0;

	chkerr(gnutls_x509_crt_list_verify(&certList[0],
					   certList.size(),

					   &caList[0],
					   caList.size(),

					   NULL, 0,

					   flags, &result),
	       "gnutls_x509_crt_list_verify");
	return result;
}

std::string gnutls::x509::crtObj::print(gnutls_certificate_print_formats_t
					format)
	const
{
	gnutls_datum_t output_buf;
	std::string s;

	chkerr(gnutls_x509_crt_print(crt, format, &output_buf),
	       "gnutls_x509_crt_print");

	try {
		s=std::string(output_buf.data,
			      output_buf.data + output_buf.size);
	} catch (...)
	{
		gnutls_free(output_buf.data);
		throw;
	}

	gnutls_free(output_buf.data);
	return s;
}

#if 0
{
#endif
};
