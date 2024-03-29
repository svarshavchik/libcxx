/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cups_available_H
#define x_cups_available_H

#include <x/namespace.h>
#include <x/cups/availablefwd.H>
#include <x/cups/availableobj.H>
#include <x/ref.H>
#include <vector>

//! Printing

//! This namespace contains wrappers for CUPS library functions.
//!
//! This namespace requires an additional link with \c -lcxxcups
//!
//! \see available

namespace LIBCXX_NAMESPACE::cups {
#if 0
};
#endif

//! Return the list of all available print destinations.

//! Connects to CUPS's scheduler, and returns a list of available print
//! destinations, sorted by name.

std::vector<available> available_destinations();

#if 0
{
#endif
}
#endif
