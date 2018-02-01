/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef available_impl_h
#define available_impl_h

#include "libcxx_config.h"
#include "x/ref.H"
#include "x/obj.H"
#include "x/cups/available.H"
#include "available_dests.H"

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	};
};
#endif

class LIBCXX_HIDDEN available_implObj : public availableObj {

 public:

	const available_dests available_destinations;

	// As long as cups.h insists on the library taking non-const
	// cups_dest_t parameters we will use locking to protect ourselves.
	//
	// We will make sure that available_destination's lock is always
	// acquired before the lock on this object.

	struct locked_dests_s {
		cups_dest_t *dest;
	};

	struct dest_t : public mpobj<locked_dests_s> {

		struct lock : public available_destsObj::dests_t::lock,
			      public mpobj<locked_dests_s>::lock {

			lock(const available_implObj &me);

			~lock();

			using mpobj<locked_dests_s>::lock::operator*;
			using mpobj<locked_dests_s>::lock::operator->;
		};
	};

	mutable dest_t dest;

	available_implObj(const available_dests &available_destinations,
			  cups_dest_t *dest);

	~available_implObj();

	std::string name() const override;

	bool is_default() const override;

	bool is_discovered() const override;

	std::unordered_map<std::string, std::string> options() const override;

	const_destination info() const override;

	destination info() override;
};

#if 0
{
	{
#endif
	};
};

#endif
