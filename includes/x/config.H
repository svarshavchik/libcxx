/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_config_H
#define x_config_H

#include <x/namespace.h>
#include <string>
#include <string_view>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Return the configuration directory name.

//! configdir() returns the string "$HOME/.libcxx-config/<appid>", which is
//! a directory, and performs some housekeeping.
//!
//! \c appid is the application identifier. Identifiers use the Internet
//! Domain Name System hierarchy to generate unique application identifiers.
//! The identifiers must follow the naming convention
//! of "subdomain.domain.com" or
//! "name@domain.com", where the "domain.com" is an Internet domain
//! that belongs to the application.
//!
//! configdir() creates "$HOME/.libcxx-config/<appid>" if it does not exist
//! already, and creates a "$HOME/.libcxx-config/<appid>/.exe" that points to
//! the application's executable. configdir() reads all directories in
//! "$HOME/.libcxx-config" at regular intervals and checks that these symbolic
//! links are valid, then rudely deletes every subdirectory with a broken .exe
//! symbolic link. This maintenance check does not occur with every
//! call to configdir(), but happens at predetermined intervals.
//!
//! In this manner, once an application gets uninstalled its configuration
//! directory still gets removed, at some point, by a different LibCXX
//! application.
//!
//! The
//! \ref explicit_property_namespace "INSERT_LIBX_NAMESPACE::configdir::update_interval property"
//! sets
//! the number of days (60 by default), that determines the frequency of
//! these maintenance checks, in the following manner:
//!
//! - the next maintenance check occurs when configdir() gets called the
//! first time it's called after half the update interval from the previous
//! maintenance check; the timestamp on its own symbolic link gets refreshed
//! at that time.
//!
//! - broken symbolic links that are older than the update interval result
//! in their entire directory getting removed.
//!
//! The default setting of 60 days results in maintenance checks every 30
//! days, the first
//! configdir() gets called 30 days after the previous maintenance check;
//! this refreshes its own symbolic links,
//! and configuration directories with broken symbolic links that are older
//! than 60 days get removed.

std::string configdir(const std::string_view &appid);

#if 0
{
#endif
}
#endif
