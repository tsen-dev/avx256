#include <iostream>
#include <cassert>
#include <cstdint>
#include <sstream>

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

void testAVX256Constructor()
{
	float floats[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0 };
	double doubles[] = { 1.0, 2.0, 3.0, 4.0 };
	uint32_t ints[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	uint16_t shorts[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	uint8_t chars[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 
		18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };	

	AVX256<float> avxFloats{ floats };
	assert(std::equal(std::begin(floats), std::end(floats), avxFloats.Data) == true);

	AVX256<double> avxDoubles{ doubles };
	assert(std::equal(std::begin(doubles), std::end(doubles), avxDoubles.Data) == true);

	AVX256<uint32_t> avxInts{ ints };
	assert(std::equal(std::begin(ints), std::end(ints), avxInts.Data) == true);

	AVX256<uint16_t> avxShorts{ shorts };
	assert(std::equal(std::begin(shorts), std::end(shorts), avxShorts.Data) == true);

	AVX256<uint8_t> avxChars{ chars };
	assert(std::equal(std::begin(chars), std::end(chars), avxChars.Data) == true);
}

void testAVX256Print()
{
	float floats[] = { 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5 };
	AVX256<float> myFloats{ floats };

	std::stringstream printStream;
	printStream << myFloats;

	assert(printStream.str() == "|1.5|2.5|3.5|4.5|5.5|6.5|7.5|8.5|");	
}

void runTests()
{
	testHasCPUIDSupport();
	testHasAVXSupport();
	testAVX256Constructor();
	testAVX256Print();
}

int main()
{
	runTests();

	std::cout << "All tests passed\n";

	return 0;
}

#endif