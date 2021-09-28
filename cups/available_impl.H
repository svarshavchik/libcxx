/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef available_impl_h
#define available_impl_h

#include "libcxx_config.h"
#include "x/ref.H"
#include "x/obj.H"
#include "x/cups/available.H"
#include "available_dests.H"

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

class LIBCXX_HIDDEN available_implObj;

class available_implObj : public availableObj {

public:

	const available_dests available_destinations;

private:
	// Our CUPS destination

	// Accessed by the CUPS connection thread

	cups_dest_t *dest;

public:

	available_implObj(const available_dests &available_destinations,
			  cups_dest_t *dest);

	~available_implObj();

	// Invoke a closure, passing it our print destination.

	void in_thread(const functionref<void (cups_dest_t *)> &closure)
		const;

	std::string name() const override;

	bool is_default() const override;

	bool is_discovered() const override;

	std::unordered_map<std::string, std::string> options() const override;

	const_destination info() const override;

	destination info() override;
};

#if 0
{
#endif
}

#endif
