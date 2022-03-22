#include <iostream>
#include <cassert>

#include "test.h"
#include "avx256.h"

#ifdef TEST

void testHasCPUIDSupport()
{
	assert(AVX256Utils::HasCPUIDSupport() == true);
}

void testHasAVXSupport()
{
	assert(AVX256Utils::HasAVXSupport() == true);
}

void runTests()
{
	testHasCPUIDSupport();
	testHasAVXSupport();
}

int main()
{
	runTests();

	std::cout << "All tests passed\n";

	return 0;
}

#endif