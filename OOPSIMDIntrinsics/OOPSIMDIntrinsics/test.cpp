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
	uint32_t uInts[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	uint16_t uShorts[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	uint8_t uChars[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 
		18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };	

	AVX256<float> avxFloats{ floats };
	assert(std::equal(std::begin(floats), std::end(floats), avxFloats.Data) == true);

	AVX256<double> avxDoubles{ doubles };
	assert(std::equal(std::begin(doubles), std::end(doubles), avxDoubles.Data) == true);

	AVX256<uint32_t> avxUInts{ uInts };
	assert(std::equal(std::begin(uInts), std::end(uInts), avxUInts.Data) == true);

	AVX256<uint16_t> avxUShorts{ uShorts };
	assert(std::equal(std::begin(uShorts), std::end(uShorts), avxUShorts.Data) == true);

	AVX256<uint8_t> avxUChars{ uChars };
	assert(std::equal(std::begin(uChars), std::end(uChars), avxUChars.Data) == true);
}

void testAVX256SubscriptOperator()
{
	double doubles[] = { 1.5, 2.5, 3.5, 4.5 };
	uint64_t uLongs[] = { 1, 2, 3, 4 };
	int64_t longs[] = { -1, 2, -3, 4 };	

	AVX256<double> avxDoubles{ doubles };
	AVX256<uint64_t> avxULongs{ uLongs };
	AVX256<int64_t> avxLongs{ longs };

	for (int i = 0; i < 4; ++i)
	{
		assert(
			avxDoubles[i] == doubles[i] &&
			avxULongs[i] == uLongs[i] &&
			avxLongs[i] == longs[i]
		);
	}
}

void testAVX256PrintOperator()
{
	double doubles[] = { 1.5, 2.5, 3.5, 4.5 };
	uint64_t uLongs[] = { 1, 2, 3, 4 };
	int64_t longs[] = { -1, 2, -3, 4 };

	AVX256<double> avxDoubles{ doubles };
	AVX256<uint64_t> avxULongs{ uLongs };
	AVX256<int64_t> avxLongs{ longs };

	std::stringstream printStream;

	printStream << avxDoubles;
	assert(printStream.str() == "|1.5|2.5|3.5|4.5|");
	printStream.str("");

	printStream << avxULongs;
	assert(printStream.str() == "|1|2|3|4|");
	printStream.str("");

	printStream << avxLongs;
	assert(printStream.str() == "|-1|2|-3|4|");
}

void runTests()
{
	testHasCPUIDSupport();
	testHasAVXSupport();
	testAVX256Constructor();
	testAVX256SubscriptOperator();
	testAVX256PrintOperator();
	
}

int main()
{
	runTests();

	std::cout << "All tests passed\n";

	return 0;
}

#endif