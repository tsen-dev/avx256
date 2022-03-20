#define TEST

#include <iostream>
#include <cassert>

#include "avx256.h"

void testHasAVXSupport()
{
	assert(AVX256::HasAVXSupport() == true);
}

int runTests()
{
	testHasAVXSupport();
}

int main()
{
	runTests();

	std::cout << "All tests passed\n";

	return 0;
}