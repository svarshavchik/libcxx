/*
** Copyright 2015-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <string>
#include <filesystem>
#include "x/config.H"
#include "x/pwd.H"
#include "x/fileattr.H"
#include "x/appid.H"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

std::string LIBCXX_NAMESPACE::appid() noexcept
{
	return "testconfig@libcxx.com";
}

using namespace LIBCXX_NAMESPACE;

void testconfig()
{
	LIBCXX_NAMESPACE::passwd p{geteuid()};

	std::string dir=std::string{
		p->pw_dir
	} + "/.libcxx/testconfig@libcxx.com";
	std::string dir2=std::string{
		p->pw_dir
	} + "/.libcxx/testconfig2@libcxx.com";
	std::string dir3=std::string{
		p->pw_dir
	} + "/.libcxx/testconfig3@libcxx.com";
	std::string dir4=std::string{
		p->pw_dir
	} + "/.libcxx/testconfig4@libcxx.com";

	std::filesystem::remove_all(dir);
	std::filesystem::remove_all(dir2);
	std::filesystem::remove_all(dir3);
	std::filesystem::remove_all(dir4);

	if (configdir("testconfig@libcxx.com") != configdir())
		throw EXCEPTION("appid() is not working");
	configdir("testconfig3@libcxx.com");
	configdir("testconfig4@libcxx.com");

	unlink((dir + "/.exe").c_str());
	if (symlink("/does/../not/../exist",
		    (dir + "/.exe").c_str()))
		;
	configdir("testconfig@libcxx.com");

	// Makes sure that the symlink is updated.
	fileattr::create(dir + "/.exe", false)->stat();

	mkdir(dir2.c_str(), 0700);

	struct timespec ts[2];

	ts[0]={};

	ts[0].tv_sec=time(NULL)- 1000 * 24 * 60 * 60;
	ts[1]=ts[0];

	utimensat(AT_FDCWD, (dir4 + "/.exe").c_str(), ts, AT_SYMLINK_NOFOLLOW);
	utimensat(AT_FDCWD, (dir2 + "/.exe").c_str(), ts, AT_SYMLINK_NOFOLLOW);
	utimensat(AT_FDCWD, (dir + "/.exe").c_str(), ts, AT_SYMLINK_NOFOLLOW);

	configdir("testconfig@libcxx.com");

	fileattr::create(dir + "/.exe", false)->stat();
	fileattr::create(dir3 + "/.exe", false)->stat();

	if (fileattr::create(dir2, false)->try_stat())
		throw EXCEPTION("dir2 should've been removed");

	if (fileattr::create(dir4, false)->try_stat())
		throw EXCEPTION("dir4 should've been removed");

	std::filesystem::remove_all(dir);
	std::filesystem::remove_all(dir2);
	std::filesystem::remove_all(dir3);
	std::filesystem::remove_all(dir4);
}

int main()
{
	try {
		testconfig();
	} catch(const exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	return (0);
}
