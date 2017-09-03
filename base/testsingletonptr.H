#include "x/singletonptr.H"
#include "x/obj.H"

class testSingletonPtrObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	int x=4;
};

typedef LIBCXX_NAMESPACE::singletonptr<testSingletonPtrObj> global;
