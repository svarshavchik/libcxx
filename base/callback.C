#include "libcxx_config.h"
#include "x/invoke_callbacks.H"
#include "x/callback_list.H"

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif

template class functionObj<void()>;

template void invoke_callbacks(const weaklist<functionObj<void ()>> &);
template void invoke_callbacks_log_exceptions(const weaklist<functionObj<void ()>> &);
template
void callback_invocations<void>::invoke(const weaklist<functionObj<void ()>> &);
template
void callback_invocations<void>::invoke_log(const weaklist<functionObj<void ()>> &);

template class callback_list_impl<void()>;
template class callback_containerObj<void (),
				     callback_list_impl<void ()>>;
template class callback_container_implObj<void (),
					  callback_list_impl<void ()>>;

#if 0
{
#endif
}
