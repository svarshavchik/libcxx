/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "gnutls/pkparamsobj.H"
#include "gnutls/init.H"
#include "gnutls/x509_privkey.H"
#include "gnutls/dhparams.H"
#include "gnutls/datumwrapper.H"
#include "pwd.H"
#include "grp.H"
#include "tlsparamsdir.h"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

gnutls::pkparams gnutls::pkparamsBase::newobj
::create(const std::string &pkparamname)
{
	return create(x509::privkey::base::get_algorithm_id(pkparamname));
}

gnutls::pkparams gnutls::pkparamsBase::newobj
::create(gnutls_pk_algorithm_t pkid)
{
	switch (pkid) {
	case GNUTLS_PK_DSA:
		return gnutls::dhparams::create();
	default:
		break;
	}

	throw EXCEPTION("Unknown public key algorithm");
}

unsigned int gnutls::pkparamsObj::get_bits() const
{
	std::vector<datum_t> factors;
	unsigned int nbits;

	export_raw(factors, nbits);
	return nbits;
}

gnutls::pkparamsObj::pkparamsObj()
{
}

gnutls::pkparamsObj::~pkparamsObj() noexcept
{
}

property::value<std::string> LIBCXX_HIDDEN
tlsparamsdir(LIBCXX_NAMESPACE_WSTR
	     L"::gnutls::tlsparamsdir", TLSPARAMSDIR);

gnutls::datum_t gnutls::pkparamsObj::open_default(const std::string &suffix)

{
	std::vector<std::string> filenames;

	std::string dir=tlsparamsdir.getValue() + "/";

	filenames.push_back(dir + "user." +
			    passwd(geteuid())->pw_name + "."
			    + suffix);

	std::vector<gid_t> groups;

	group::getgroups(groups);

	for (std::vector<gid_t>::const_iterator b=groups.begin(),
		     e=groups.end(); b != e; ++b)
	{
		filenames.push_back(dir + "group." + group(*b)->gr_name + "."
				    + suffix);
	}

	filenames.push_back(dir + "system." + suffix);

	for (std::vector<std::string>::const_iterator b=filenames.begin(),
		     e=filenames.end(); b != e; ++b)
	{
		if (access(b->c_str(), R_OK) == 0)
		{
			datum_t d(datum_t::create());
			d->load(*b);
			return d;
		}
	}
	throw EXCEPTION(gettextmsg(_("Cannot open the default %1% parameter file"), suffix));
}

#if 0
{
#endif
};

