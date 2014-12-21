#include "libcxx_config.h"
#include "x/callback.H"

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif

template class callbackBase<void>::objfactory<callback<void>>;
template class callbackObj<void>;

template void invoke_callbacks(const weaklist<callbackObj<void>> &);
template void invoke_callbacks_log_exceptions(const weaklist<callbackObj<void>> &);
template
void callback_invocations<void>::invoke(const weaklist<callbackObj<void>> &);
template
void callback_invocations<void>::invoke_log(const weaklist<callbackObj<void>> &);


#if 0
{
#endif
}
