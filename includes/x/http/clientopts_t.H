/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

//! Client options

//! This is a set of client-side options.
//!
typedef int clientopts_t;

//! Default value, no client options
static const clientopts_t none=0;

//! Option - this connection is to a proxy.
static const clientopts_t isproxy=1;

//! TLS-specific client option: do not verify server's name.

//! This is for diagnostic purposes.
//!

static const clientopts_t noverifypeer=2;

//! TLS-specific client option: do not verify server's certificate

//! This is for diagnostic purposes. This includes \c noverifypeer,
//! and skips all checking of the server's certificate

static const clientopts_t noverifycert=4;
