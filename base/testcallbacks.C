#include <iostream>
#include <string>
#include "x/callback_multimap.H"
#include "x/callback_map.H"
#include "x/callback_list.H"

static void testcallbacks1()
{
	typedef LIBCXX_NAMESPACE::callback_list<void (int)> my_callback_list;

	auto l=my_callback_list::create();

	int counter=0;

	l->invoke(3);

	l->create_mcguffin();

	my_callback_list::base::mcguffinptr
		m1=l->create_mcguffin_for([&]
					  (int n)
					  {
						  counter += n;
					  }),
		m2,
		m3=l->create_mcguffin_for([&]
					  (int n)
					  {
						  counter += n*3;
					  });

	m2=l->create_mcguffin_for([&]
				  (int n)
				  {
					  if (n == 1)
						  m2=my_callback_list::
							  base::mcguffinptr();
					  counter += n*2;
				  });

	l->invoke(2);

	if (counter != 2+4+6)
		throw EXCEPTION("Test 1 failed");

	m1=my_callback_list::base::mcguffinptr();

	l->invoke(2);

	if (counter != 2+4+6 + 4+6)
		throw EXCEPTION("Test 2 failed");

	l->invoke(1);

	if (counter != 2+4+6 + 4+6 + 2+3)
		throw EXCEPTION("Test 3 failed");

	l->invoke(1);
	if (counter != 2+4+6 + 4+6 + 2+3 + 3)
		throw EXCEPTION("Test 4 failed");
}

static void testcallbacks2()
{
	typedef LIBCXX_NAMESPACE::callback_map<std::string, bool(int, int)>
		my_callback_map;

	auto m=my_callback_map::create([]
				       (bool v) { return v; },
				       [] { return false; });

	int sum=0;

	auto m1=m->create_mcguffin_for([&]
				       (int a, int b)
				       {
					       sum += (a+b);
					       return true;
				       },
				       "a");

	if (!m1->installed())
		throw EXCEPTION("Test 5 failed");

	auto m2=m->create_mcguffin_for([&]
				       (int a, int b)
				       {
					       sum += sum*(a+b);
					       return true;
				       },
				       "b");
	if (!m2->installed())
		throw EXCEPTION("Test 6 failed");

	auto m3=m->create_mcguffin_for([&]
				       (int a, int b)
				       {
					       sum += sum*(a+b);
					       return true;
				       },
				       "b");

	if (m3->installed())
		throw EXCEPTION("Test 7 failed");

	if (m->invoke(2, 2))
		throw EXCEPTION("Test 8a failed");

	if (sum != (2+2) + (2+2) * (2+2))
		throw EXCEPTION("Test 8 failed");

	m3=m->create_mcguffin_for([&]
				  (int a, int b)
				  {
					  sum += 2;
					  return false;
				  },
				  "0");

	if (!m3->installed())
		throw EXCEPTION("Test 9 failed");

	if (m->invoke(2, 2))
		throw EXCEPTION("Test 9a failed");

	if (sum != (2+2) + (2+2) * (2+2) + 2)
		throw EXCEPTION("Test 10 failed");
}

static void testcallbacks3()
{
	typedef LIBCXX_NAMESPACE::callback_multimap<std::string, bool()>
		my_callback_map;

	auto m=my_callback_map::create([]
				       (bool v) { return v; },
				       [] { return false; });

	int sum=0;

	auto m1=m->create_mcguffin_for([&]
				       {
					       ++sum;
					       return true;
				       },
				       "a");

	auto m2=m->create_mcguffin_for([&]
				       {
					       ++sum;
					       return true;
				       },
				       "a");

	m->invoke();

	if (sum != 2)
		throw EXCEPTION("Test 11 failed");
}

int main()
{
	try {
		testcallbacks1();
		testcallbacks2();
		testcallbacks3();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
	}
	return 0;
}
