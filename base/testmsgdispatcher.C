/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "msgdispatcher.H"

class testclass1 : public LIBCXX_NAMESPACE::msgdispatcherObj {

#include "testmsgdispatcher.testclass1.all.H"

};

class errorthread : public LIBCXX_NAMESPACE::msgdispatcherObj {

#include "testmsgdispatcher.testclass2.decl.H"

};

#include "testmsgdispatcher.testclass2.def.H"

void testclass1::dispatch(const method2_msg &msg)

{
}

void testclass1::dispatch(const method1_msg &msg)

{
}

void testclass1::dispatch(const method0_msg &msg)

{
}

void errorthread::dispatch(const logerror_msg &msg)
{
}

void errorthread::dispatch(const report_errors_msg &msg)
{
}

void errorthread::dispatch(const dump_msg &msg)
{
}

int main(int argc, char **argv)
{
	return 0;
}
