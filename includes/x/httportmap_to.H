/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN
LIBCXX_TEMPLATE_DECL template
LIBCXX_TEMPLATE_ITER_TYPE
httportmapObj::list(LIBCXX_TEMPLATE_ITER_TYPE, const fdptr &) const;

LIBCXX_TEMPLATE_DECL template
LIBCXX_TEMPLATE_ITER_TYPE
httportmapObj::list(LIBCXX_TEMPLATE_ITER_TYPE,
		    const std::string &, const fdptr &) const;

LIBCXX_TEMPLATE_DECL template
LIBCXX_TEMPLATE_ITER_TYPE
httportmapObj::list(LIBCXX_TEMPLATE_ITER_TYPE,
		    const std::string &, uid_t, const fdptr &)
	const;

LIBCXX_TEMPLATE_DECL template
LIBCXX_TEMPLATE_ITER_TYPE
httportmapObj::list(LIBCXX_TEMPLATE_ITER_TYPE,
		    const std::set<std::string> &,
		    const std::set<uid_t> &,
		    const std::set<pid_t> &,
		    const fdptr &) const;

#endif
