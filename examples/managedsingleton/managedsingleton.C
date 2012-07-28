#include <x/managedsingletonapp.H>

#include <iostream>

// An object representing the arguments to an application singleton instance
// invocation. This example just passes the raw contents of argv.

class app_argsObj : virtual public x::obj {

public:

	// They get stored in a vector proper.

	std::vector<std::string> argv;

	app_argsObj() {}
	app_argsObj(int argc, char **argv_args)
		: argv(argv_args, argv_args+argc)
	{
	}

	~app_argsObj() noexcept 
	{
	}

	// ... and serialized

	template<typename iter_type> void serialize(iter_type &iter)
	{
		iter(argv);
	}

	void dump() const
	{
		const char *sep="";

		for (auto &arg:argv)
		{
			std::cout << sep << arg;
			sep=" ";
		}
		std::cout << std::endl;
	}
};

typedef x::ref<app_argsObj> app_args;
typedef x::ptr<app_argsObj> app_argsptr;

// A return value from the application singleton instance. This example
// returns a std::string, a simple message.

class ret_argsObj : virtual public x::obj {

public:
	ret_argsObj() {}
	~ret_argsObj() noexcept {}

	std::string message;

	ret_argsObj(const std::string &messageArg) : message(messageArg) {}

	template<typename iter_type> void serialize(iter_type &iter)
	{
		iter(message);
	}
};

typedef x::ref<ret_argsObj> ret_args;

class appObj : virtual public x::obj {

public:

	std::mutex mutex;
	std::condition_variable cond;

	bool quitflag;

	appObj() : quitflag(false) {}
	~appObj() noexcept {}

	// The first singleton instance waits for another instance to be
	// called with a 'stop' argument.

	ret_args run(uid_t uid, const app_args &args)
	{
		std::cout << "Initial (uid " << uid << "): ";
		args->dump();

		std::unique_lock<std::mutex> lock(mutex);

		cond.wait(lock, [this] { return quitflag; });

		return ret_args::create("Bye!");
	}

	// Additional application singleton instance invocation.

	void instance(uid_t uid, const app_args &args,
		      const ret_args &ret,
		      const x::singletonapp::processed &flag,
		      const x::ref<x::obj> &mcguffin)
	{
		flag->processed();
		std::cout << "Additional (uid " << uid << "): ";
		args->dump();
		ret->message="Processed";
		if (args->argv.size() > 1 && args->argv[1] == "stop")
			stop();
	}

	// Another way that the application singleton gets stopped. Typically
	// by a SIGINT or SIGHUP.

	void stop()
	{
		std::unique_lock<std::mutex> lock(mutex);

		quitflag=true;
		cond.notify_all();
	}
};

typedef x::ref<appObj> app;

int main(int argc, char **argv)
{
	const char *uid=getenv("SUID");
	const char *same_env=getenv("SAME");
	bool same=!same_env || atoi(same_env);

	auto ret=x::singletonapp::managed(
					  // The application singleton factory
					  [] { return app::create(); },

					  // Argument for this instance
					  // invocation.

					  app_args::create(argc, argv),
					  (uid ? (uid_t)atoi(uid):getuid()),
					  same ? 0700:0755, same);

	std::cout << "Result (uid "
		  << ret.second << "): " << ret.first->message << std::endl;
	return 0;
}

