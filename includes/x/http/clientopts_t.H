/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

//! Client options

//! This is a set of client-side options.
//!
typedef int clientopts_t;

//! Default value, no client options
static constexpr clientopts_t none=0;

//! Option - this connection is to a proxy.
static constexpr clientopts_t isproxy=1;

//! TLS-specific client option: do not verify server's name.

//! This is for diagnostic purposes.
//!

static constexpr clientopts_t noverifypeer=2;

//! TLS-specific client option: do not verify server's certificate

//! This is for diagnostic purposes. This includes \c noverifypeer,
//! and skips all checking of the server's certificate

static constexpr clientopts_t noverifycert=4;
