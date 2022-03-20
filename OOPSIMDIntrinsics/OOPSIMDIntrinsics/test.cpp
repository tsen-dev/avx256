#include <iostream>
#include <cassert>

#include "test.h"
#include "avx256.h"

#ifdef TEST

void testHasAVXSupport()
{
	assert(AVX256::HasAVXSupport() == true);
}

void runTests()
{
	testHasAVXSupport();
}

int main()
{
	runTests();

	std::cout << "All tests passed\n";

	return 0;
}

#endif