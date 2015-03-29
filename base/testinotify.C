#include "x/inotify.H"
#include "x/threads/run.H"
#include "x/mpobj.H"
#include <iostream>

void testinotify()
{
	auto i=LIBCXX_NAMESPACE::inotify::create();

	LIBCXX_NAMESPACE::mpobj<bool> flag(false);

	auto w=i->create(".",
			 LIBCXX_NAMESPACE::inotify_create |
			 LIBCXX_NAMESPACE::inotify_delete,
			 [&]
			 (uint32_t mask, uint32_t cookie, const char *name)
			 {
				 if (mask & LIBCXX_NAMESPACE::inotify_create)
				 {
					 std::cout << "Created " << name
					 << std::endl;

					 if (std::string(name) == "testx")
						 unlink("testx");
				 }
				 if (mask & LIBCXX_NAMESPACE::inotify_delete)
				 {
					 std::cout << "Deleted " << name
					 << std::endl;
					 if (std::string(name) == "testx")
					 {
						 LIBCXX_NAMESPACE::mpobj<bool>
							 ::lock lock(flag);

						 *lock=true;
					 }
				 }
			 });

	close(open("testx", O_CREAT|O_RDWR, 0600));

	while (!*LIBCXX_NAMESPACE::mpobj<bool>::lock(flag))
	{
		i->read();
	}
}

int main()
{
	try {
		testinotify();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}
