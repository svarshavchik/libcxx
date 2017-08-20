#include <x/ref.H>
#include <x/hier.H>

namespace LIBCXX_NAMESPACE {

	namespace http {

class domaincookiesObj;

//! A cookie jar object.

class cookiejarObj : virtual public obj {

	//! A hierarchical container, by domain component

	typedef hier<std::string, ref<domaincookiesObj> > cookiedomainhier_t;


	void remove(const cookiedomainhier_t::base::writelock &writer_lock) LIBCXX_HIDDEN;

};
	}
}
