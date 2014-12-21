#ifndef x_derivedvaluefwd_H
#define x_derivedvaluefwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif

template<typename basedon_t> class derivedvalueListBase;

template<typename basedon_t> class derivedvalueListObj;

template<typename basedon_t, typename ...vipobj_type> class derivedvalueObj;

template<typename basedon_t, typename ...vipobj_type> class derivedvalueBase;

//////////////////////////////////////////////////////////////////////////////

//! A weak list of some values that some other value is derived from.

template<typename basedon_t>
using derivedvaluelist=ref<derivedvalueListObj<basedon_t>,
			   derivedvalueListBase<basedon_t>>;

//! A nullable pointer reference to a weak list of some values that some other value is derived from.

template<typename basedon_t>
using derivedvaluelistptr=ptr<derivedvalueListObj<basedon_t>,
			      derivedvalueListBase<basedon_t>>;

//! A non-mutable weak list of some values that some other value is derived from.

template<typename basedon_t>
using const_derivedvaluelist=const_ref<derivedvalueListObj<basedon_t>,
				       derivedvalueListBase<basedon_t>>;

//! A non-mutable nullable pointer reference to a weak list of some values that some other value is derived from.

template<typename basedon_t>
using const_derivedvaluelistptr=const_ptr<derivedvalueListObj<basedon_t>,
					  derivedvalueListBase<basedon_t>>;

//! A very important value that's derived from a collection of other values.

template<typename basedon_t, typename ...vipobj_type>
using derivedvalue=ref<derivedvalueObj<basedon_t, vipobj_type...>,
		       derivedvalueBase<basedon_t, vipobj_type...>>;

//! A nullable pointer reference to a \ref derivedvalue "derived value".
template<typename basedon_t, typename ...vipobj_type>
using derivedvalueptr=ptr<derivedvalueObj<basedon_t, vipobj_type...>,
			  derivedvalueBase<basedon_t, vipobj_type...>>;

//! A non-mutable \ref derivedvalue "derived value".
template<typename basedon_t, typename ...vipobj_type>
using const_derivedvalue=const_ref<derivedvalueObj<basedon_t, vipobj_type...>,
				   derivedvalueBase<basedon_t, vipobj_type...>>;

//! A nullable pointer reference to a non-mutable \ref derivedvalue "derived value".

template<typename basedon_t, typename ...vipobj_type>
using const_derivedvalueptr=const_ptr<derivedvalueObj<basedon_t,
						      vipobj_type...>,
				      derivedvalueBase<basedon_t,
						       vipobj_type...>>;
#if 0
{
#endif
}
#endif
