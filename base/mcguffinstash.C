/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mcguffinstash.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template class mcguffinstashObj<std::string>;
template mcguffinstashObj<std::string>::mcguffinstashObj();

template class mcguffinstashObj<std::string, true>;
template mcguffinstashObj<std::string, true>::mcguffinstashObj();

template mcguffinstash<std::string>
ptrrefBase::objfactory<mcguffinstash<std::string>>::create();

template mcguffinstash<std::string, true>
ptrrefBase::objfactory<mcguffinstash<std::string, true>>::create();

template mcguffinstash<std::string>::~mcguffinstash();
template mcguffinstash<std::string, true>::~mcguffinstash();

// Just making sure this compiles

typedef mcguffinstash<>::base::map_t map_t_works;
typedef mcguffinstash<>::base::container_t container_t_works;

#if 0
{
#endif
}
