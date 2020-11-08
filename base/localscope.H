class LIBCXX_HIDDEN mainscope_destructor;

// Helper class that destroys mainscope at application termination.

class mainscope_destructor {

public:
	mainscope_destructor();

	~mainscope_destructor();

	static mainscope_destructor instance;
};
