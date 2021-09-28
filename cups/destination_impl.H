/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef destination_impl_h
#define destination_impl_h

#include "libcxx_config.h"
#include "x/ref.H"
#include "x/obj.H"
#include "x/cups/destination.H"
#include "available_dests.H"
#include <cups/cups.h>

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

class LIBCXX_HIDDEN destination_implObj;

class destination_implObj : public destinationObj {

public:

	const available_dests available_destinations;

private:

	// Destination information, accessed only by the CUPS connection thread
	cups_dest_t *dest;
	http_t *http;
	cups_dinfo_t *info;

public:
	// Execute a closure, passing our destination information to it.

	void in_thread(const functionref<void (cups_dest_t *,
					       http_t *,
					       cups_dinfo_t *)> &closure)
		const;
	destination_implObj(const available_dests &available_destinations,
			    cups_dest_t *dest);

	~destination_implObj();

	bool supported(const std::string_view &option) const override;

	bool supported(const std::string_view &option,
		       const std::string_view &value) const override;

	std::unordered_set<std::string> supported_options() const override;

	option_values_t
		option_values(const std::string_view &option) const
		override;

	option_values_t
		default_option_values(const std::string_view &option) const
		override;

	option_values_t
		ready_option_values(const std::string_view &option) const
		override;

	std::unordered_map<std::string, std::string> user_defaults() const
		override;

	job create_job() override;

private:
	static option_values_t
	parse_attribute_values(	cups_dest_t *dest,
				http_t *http,
				cups_dinfo_t *info,
				ipp_attribute_t *, const char *, bool);
};

#if 0
{
#endif
}

#endif
