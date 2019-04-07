/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN


LIBCXX_TEMPLATE_DECL template
listObj::parsepropname_iter<LIBCXX_TEMPLATE_ITER>
parsepropname(listObj::parsepropname_iter<LIBCXX_TEMPLATE_ITER>,
	      listObj::parsepropname_iter<LIBCXX_TEMPLATE_ITER>,
	      std::list<std::string> &,
	      const const_locale &);

LIBCXX_TEMPLATE_DECL template class
listObj::parsepropname_iter<LIBCXX_TEMPLATE_ITER>;

LIBCXX_TEMPLATE_DECL template
LIBCXX_TEMPLATE_ITER listObj::parse(LIBCXX_TEMPLATE_ITER,
				  LIBCXX_TEMPLATE_ITER,
				  const std::string &,
				  bool,
				  bool,
				  const errhandler &,
				  const const_locale &);

LIBCXX_TEMPLATE_DECL template
LIBCXX_TEMPLATE_ITER listObj::lock_parse(LIBCXX_TEMPLATE_ITER,
				       LIBCXX_TEMPLATE_ITER,
				       const std::string &,
				       bool,
				       bool,
				       listObj::loaded_t &,
				       const errhandler &,
				       const const_locale &);
#endif
