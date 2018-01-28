/*
** Copyright 2018 Double Precision, Inc.
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

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	};
};
#endif

class LIBCXX_HIDDEN destination_implObj : public destinationObj {

 public:

	const available_dests available_destinations;

	// As long as cups.h insists on the library taking non-const
	// cups_dest_t parameters we will use locking to protect ourselves.
	//
	// We will make sure that available_destination's lock is always
	// acquired before the lock on this object.

	struct lock_info_s {
		cups_dest_t *dest;
		cups_dinfo_t *info;
	};

	struct info_t : public mpobj<lock_info_s> {

		struct lock : public available_destsObj::dests_t::lock,
			      public mpobj<lock_info_s>::lock {

			lock(const destination_implObj &me);

			~lock();

			using mpobj<lock_info_s>::lock::operator*;
			using mpobj<lock_info_s>::lock::operator->;
		};
	};

	mutable info_t info;

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
		parse_attribute_values(ipp_attribute_t *, const char *, bool);

};

#if 0
{
	{
#endif
	};
};

#endif
