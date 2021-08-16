/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cups_availablefwd_H
#define x_cups_availablefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

class availableObj;

//! Available printers

//! \code
//! std::vector<INSERT_LIBX_NAMESPACE::cups::available> destinations=INSERT_LIBX_NAMESPACE::cups::available_destinations();
//!
//! auto default_printer_iter=std::find_if(destinations.begin(),
//!                                        destinations.end(),
//!                                        []
//!                                        (const auto &d)
//!                                        {
//!                                            return d->is_default();
//!                                        });
//!
//! if (default_printer_iter != destinations.end())
//! {
//!     auto default_pinter=*default_printer_iter;
//!
//!     std::cout << default_printer->name();
//!
//!     if (default_printer->is_discovered())
//!     {
//!          std::cout << " (discovered)";
//!     }
//!
//!     std::cout << std::endl;
//!
//!     std::unordered_map<std::string, std::string> options=default_printer->options();
//! }
//! \endcode
//!
//! available_destinations() returns a list of available printers.
//!
//! name() returns the printer's name. is_default() indicates whether this is
//! the user's default printer. is_discovered() indicates whether this is
//! a remote printer located via IPP autodiscovery.
//!
//! options() returns a list of printer attributes,
//! or options. See https://www.cups.org/doc/cupspm.html#basic-destination-information
//! for more information.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::cups::destination printer=default_printer->info();
//! \endcode
//!
//! info() connects to the print destination, and queries
//! its properties. This executes cupsCopyDestInfo(), and
//! cupsConnectDest(). An exception gets thrown if the connection fails.

typedef ref<availableObj> available;

//! A nullable pointer reference to an available printer.

typedef ptr<availableObj> availableptr;

//! A constant available printer object.

typedef const_ref<availableObj> const_available;

//! A nullable pointer reference to a const available printer.

typedef const_ptr<availableObj> const_availableptr;

#if 0
{
#endif
}
#endif
