#include <iostream>
#include <cassert>
#include <cstdint>
#include <sstream>

#include "test.h"
#include "avx256.h"

#ifdef TEST

namespace
{
	extern "C" bool HasCPUIDSupport(void);

	extern "C" void Add64(uint64_t * source, uint64_t * operand);
	extern "C" void Add32(uint32_t * source, uint32_t * operand);
	extern "C" void Add16(uint16_t * source, uint16_t * operand);
	extern "C" void Add16Saturate(int16_t * source, int16_t * operand);
	extern "C" void Add16USaturate(uint16_t * source, uint16_t * operand);
	extern "C" void Add8(uint8_t * source, uint8_t * operand);
	extern "C" void Add8Saturate(int8_t * source, int8_t * operand);
	extern "C" void Add8USaturate(uint8_t * source, uint8_t * operand);

	extern "C" void Sub64(uint64_t * source, uint64_t * operand);
	extern "C" void Sub32(uint32_t * source, uint32_t * operand);
	extern "C" void Sub16(uint16_t * source, uint16_t * operand);
	extern "C" void Sub16SaturateSigned(int16_t * source, int16_t * operand);
	extern "C" void Sub16SaturateUnsigned(uint16_t * source, uint16_t * operand);
	extern "C" void Sub8(uint8_t * source, uint8_t * operand);
	extern "C" void Sub8SaturateSigned(int8_t * source, int8_t * operand);
	extern "C" void Sub8SaturateUnsigned(uint8_t * source, uint8_t * operand);
}

void testHasCPUIDSupport()
{
	assert(HasCPUIDSupport() == true);
}

void testHasAVX2Support()
{
	assert(AVX256Utils::HasAVX2Support() == true);
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

	/* Testing invalid constructions that should not compile:

	void* voids;
	AVX256<void> avxVoid{ voids }; // 'AVX256': class template cannot be constructed

	class X {};
	X* xs;
	AVX256<X> avxX{ xs }; // 'AVX256': class template cannot be constructed

	uint8_t* uCharPtrs[] = { &uChars[0], &uChars[1], &uChars[2], &uChars[3] };
	AVX256<uint8_t*> avxUCharPtrs{ uCharPtrs }; // 'AVX256': class template cannot be constructed */
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

	avxDoubles[0] = 100;
	assert(avxDoubles[0] == 100);
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

void testAVX256NextandPrevious()
{
	float floats[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 };
	double doubles[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0 };
	uint32_t uInts[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	uint16_t uShorts[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
		17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
	uint8_t uChars[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
		18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
		44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};

	AVX256<float> avxFloats{ floats };
	AVX256<double> avxDoubles{ doubles };
	AVX256<uint32_t> avxUInts{ uInts };
	AVX256<uint16_t> avxUShorts{ uShorts };
	AVX256<uint8_t> avxUChars{ uChars };

	avxFloats.Next();
	avxDoubles.Next();
	avxUInts.Next();
	avxUShorts.Next();
	avxUChars.Next();

	for (int i = 0; i < 8; ++i) assert(avxFloats[i] == floats[8 + i] && avxUInts[i] == uInts[8 + i]);
	for (int i = 0; i < 4; ++i) assert(avxDoubles[i] == doubles[4 + i]);
	for (int i = 0; i < 16; ++i) assert(avxUShorts[i] == uShorts[16 + i]);
	for (int i = 0; i < 32; ++i) assert(avxUChars[i] == uChars[32 + i]);

	avxFloats.Previous();
	avxDoubles.Previous();
	avxUInts.Previous();
	avxUShorts.Previous();
	avxUChars.Previous();

	for (int i = 0; i < 8; ++i) assert(avxFloats[i] == floats[i] && avxUInts[i] == uInts[i]);
	for (int i = 0; i < 4; ++i) assert(avxDoubles[i] == doubles[i]);
	for (int i = 0; i < 16; ++i) assert(avxUShorts[i] == uShorts[i]);
	for (int i = 0; i < 32; ++i) assert(avxUChars[i] == uChars[i]);
}

void testAVX256Add()
{
	int64_t myLongs0[4] = { INT64_MAX, INT64_MAX - 1, INT64_MIN, INT64_MIN + 1};
	int64_t myLongs1[4] = { 0, 3, 0, -3 };
	int64_t myLongsResults[4] = { INT64_MAX, INT64_MIN + 1, INT64_MIN, INT64_MAX - 1 };

	uint64_t myULongs0[4] = { UINT64_MAX, UINT64_MAX - 1, 0, 10};
	uint64_t myULongs1[4] = { 0, 3, 5, 6};
	uint64_t myULongsResults[4] = { UINT64_MAX, 1, 5, 16 };

	int32_t myInts0[8] = { INT32_MAX, INT32_MAX - 1, INT32_MIN, INT32_MIN + 1, INT32_MAX, INT32_MAX - 1, INT32_MIN, INT32_MIN + 1 };
	int32_t myInts1[8] = { 0, 3, 0, -3, 0, 3, 0, -3 };
	int32_t myIntsResults[8] = { INT32_MAX, INT32_MIN + 1, INT32_MIN, INT32_MAX - 1, INT32_MAX, INT32_MIN + 1, INT32_MIN, INT32_MAX - 1 };

	uint32_t myUInts0[8] = { UINT32_MAX, UINT32_MAX - 1, 0, 10, UINT32_MAX, UINT32_MAX - 1, 0, 10 };
	uint32_t myUInts1[8] = { 0, 3, 5, 6, 0, 3, 5, 6};
	uint32_t myUIntsResults[8] = { UINT32_MAX, 1, 5, 16, UINT32_MAX, 1, 5, 16 };

	int16_t myShorts0[16] = { INT16_MAX, INT16_MAX - 1, INT16_MIN, INT16_MIN + 1, 0, 1, 2, 3, INT16_MAX, INT16_MAX - 1, INT16_MIN, INT16_MIN + 1, 0, 1, 2, 3 };
	int16_t myShorts1[16] = { 0, 3, 0, -3, 1, -2, 3, -3, 0, 3, 0, -3, 1, -2, 3, -3 };
	int16_t myShortsResults[16] = { INT16_MAX, INT16_MAX, INT16_MIN, INT16_MIN, 1, -1, 5, 0, INT16_MAX, INT16_MAX, INT16_MIN, INT16_MIN, 1, -1, 5, 0 };

	uint16_t myUShorts0[16] = { UINT16_MAX, UINT16_MAX - 1, 0, 1, 2, 3, 4, 5, UINT16_MAX, UINT16_MAX - 1, 0, 1, 2, 3, 4, 5 };
	uint16_t myUShorts1[16] = { 0, 3, 1, 2, 3, 4, 5, 6, 0, 3, 1, 2, 3, 4, 5, 6 };
	uint16_t myUShortsResults[16] = { UINT16_MAX, UINT16_MAX, 1, 3, 5, 7, 9, 11, UINT16_MAX, UINT16_MAX, 1, 3, 5, 7, 9, 11 };

	int8_t myChars0[32] = { INT8_MAX, INT8_MAX - 1, INT8_MIN, INT8_MIN + 1, 0, 1, 2, 3, INT8_MAX, INT8_MAX - 1, INT8_MIN, INT8_MIN + 1, 0, 1, 2, 3,
							INT8_MAX, INT8_MAX - 1, INT8_MIN, INT8_MIN + 1, 0, 1, 2, 3,  INT8_MAX, INT8_MAX - 1, INT8_MIN, INT8_MIN + 1, 0, 1, 2, 3 };
	int8_t myChars1[32] = { 0, 3, 0, -3, 1, -2, 3, -3, 0, 3, 0, -3, 1, -2, 3, -3, 0, 3, 0, -3, 1, -2, 3, -3, 0, 3, 0, -3, 1, -2, 3, -3 };
	int8_t myCharsResults[32] = { INT8_MAX, INT8_MAX, INT8_MIN, INT8_MIN, 1, -1, 5, 0,  INT8_MAX, INT8_MAX, INT8_MIN, INT8_MIN, 1, -1, 5, 0,
								  INT8_MAX, INT8_MAX, INT8_MIN, INT8_MIN, 1, -1, 5, 0,  INT8_MAX, INT8_MAX, INT8_MIN, INT8_MIN, 1, -1, 5, 0 };

	uint8_t myUChars0[32] = { UINT8_MAX, UINT8_MAX - 1, 0, 1, 2, 3, 4, 5, UINT8_MAX, UINT8_MAX - 1, 0, 1, 2, 3, 4, 5,
							  UINT8_MAX, UINT8_MAX - 1, 0, 1, 2, 3, 4, 5, UINT8_MAX, UINT8_MAX - 1, 0, 1, 2, 3, 4, 5 };
	uint8_t myUChars1[32] = { 0, 3, 1, 2, 3, 4, 5, 6, 0, 3, 1, 2, 3, 4, 5, 6, 0, 3, 1, 2, 3, 4, 5, 6, 0, 3, 1, 2, 3, 4, 5, 6 };
	uint8_t myUCharsResults[32] = { UINT8_MAX, UINT8_MAX, 1, 3, 5, 7, 9, 11, UINT8_MAX, UINT8_MAX, 1, 3, 5, 7, 9, 11,
									UINT8_MAX, UINT8_MAX, 1, 3, 5, 7, 9, 11, UINT8_MAX, UINT8_MAX, 1, 3, 5, 7, 9, 11 };

	Add64(reinterpret_cast<uint64_t*>(myLongs0), reinterpret_cast<uint64_t*>(myLongs1));
	Add64(myULongs0, myULongs1);
	Add32(reinterpret_cast<uint32_t*>(myInts0), reinterpret_cast<uint32_t*>(myInts1));
	Add32(myUInts0, myUInts1);
	Add16SaturateSigned(myShorts0, myShorts1);
	Add16SaturateUnsigned(myUShorts0, myUShorts1);
	Add8SaturateSigned(myChars0, myChars1);
	Add8SaturateUnsigned(myUChars0, myUChars1);

	assert(std::equal(std::begin(myLongs0), std::end(myLongs0), myLongsResults));
	assert(std::equal(std::begin(myULongs0), std::end(myULongs0), myULongsResults));
	assert(std::equal(std::begin(myInts0), std::end(myInts0), myIntsResults));
	assert(std::equal(std::begin(myUInts0), std::end(myUInts0), myUIntsResults));
	assert(std::equal(std::begin(myShorts0), std::end(myShorts0), myShortsResults));
	assert(std::equal(std::begin(myUShorts0), std::end(myUShorts0), myUShortsResults));
	assert(std::equal(std::begin(myChars0), std::end(myChars0), myCharsResults));
	assert(std::equal(std::begin(myUChars0), std::end(myUChars0), myUCharsResults));
}

void testAVX256Sub()
{
	int64_t myLongs0[4] = { INT64_MAX, INT64_MAX - 1, INT64_MIN, INT64_MIN + 1 };
	int64_t myLongs1[4] = { 0, -3, 0, 3 };
	int64_t myLongsResults[4] = { INT64_MAX, INT64_MIN + 1, INT64_MIN, INT64_MAX - 1 };

	uint64_t myULongs0[4] = { UINT64_MAX, UINT64_MAX - 1, 0, 10 };
	uint64_t myULongs1[4] = { 0, 3, 5, 6 };
	uint64_t myULongsResults[4] = { UINT64_MAX, UINT64_MAX - 4, UINT64_MAX - 4, 4 };

	int32_t myInts0[8] = { INT32_MAX, INT32_MAX - 1, INT32_MIN, INT32_MIN + 1, INT32_MAX, INT32_MAX - 1, INT32_MIN, INT32_MIN + 1 };
	int32_t myInts1[8] = { 0, -3, 0, 3, 0, -3, 0, 3 };
	int32_t myIntsResults[8] = { INT32_MAX, INT32_MIN + 1, INT32_MIN, INT32_MAX - 1, INT32_MAX, INT32_MIN + 1, INT32_MIN, INT32_MAX - 1 };

	uint32_t myUInts0[8] = { UINT32_MAX, UINT32_MAX - 1, 0, 10, UINT32_MAX, UINT32_MAX - 1, 0, 10 };
	uint32_t myUInts1[8] = { 0, 3, 5, 6, 0, 3, 5, 6 };
	uint32_t myUIntsResults[8] = { UINT32_MAX, UINT32_MAX - 4, UINT32_MAX - 4, 4, UINT32_MAX, UINT32_MAX - 4, UINT32_MAX - 4, 4 };

	int16_t myShorts0[16] = { INT16_MAX, INT16_MAX - 1, INT16_MIN, INT16_MIN + 1, 0, 1, 2, 3, INT16_MAX, INT16_MAX - 1, INT16_MIN, INT16_MIN + 1, 0, 1, 2, 3 };
	int16_t myShorts1[16] = { 0, -3, 0, 3, -1, 2, -3, 3, 0, -3, 0, 3, -1, 2, -3, 3 };
	int16_t myShortsResults[16] = { INT16_MAX, INT16_MAX, INT16_MIN, INT16_MIN, 1, -1, 5, 0, INT16_MAX, INT16_MAX, INT16_MIN, INT16_MIN, 1, -1, 5, 0 };

	uint16_t myUShorts0[16] = { 0, 1, 2, 4, 6, 8, 10, 12, 0, 1, 2, 4, 6, 8, 10, 12 };
	uint16_t myUShorts1[16] = { 0, 3, 1, 2, 3, 4, 5, 6, 0, 3, 1, 2, 3, 4, 5, 6 };
	uint16_t myUShortsResults[16] = { 0, 0, 1, 2, 3, 4, 5, 6, 0, 0, 1, 2, 3, 4, 5, 6 };

	int8_t myChars0[32] = { INT8_MAX, INT8_MAX - 1, INT8_MIN, INT8_MIN + 1, 0, 1, 2, 3, INT8_MAX, INT8_MAX - 1, INT8_MIN, INT8_MIN + 1, 0, 1, 2, 3, 
							INT8_MAX, INT8_MAX - 1, INT8_MIN, INT8_MIN + 1, 0, 1, 2, 3, INT8_MAX, INT8_MAX - 1, INT8_MIN, INT8_MIN + 1, 0, 1, 2, 3 };
	int8_t myChars1[32] = { 0, -3, 0, 3, -1, 2, -3, 3, 0, -3, 0, 3, -1, 2, -3, 3, 0, -3, 0, 3, -1, 2, -3, 3, 0, -3, 0, 3, -1, 2, -3, 3 };
	int8_t myCharsResults[32] = { INT8_MAX, INT8_MAX, INT8_MIN, INT8_MIN, 1, -1, 5, 0, INT8_MAX, INT8_MAX, INT8_MIN, INT8_MIN, 1, -1, 5, 0,
								  INT8_MAX, INT8_MAX, INT8_MIN, INT8_MIN, 1, -1, 5, 0, INT8_MAX, INT8_MAX, INT8_MIN, INT8_MIN, 1, -1, 5, 0 };

	uint8_t myUChars0[32] = { 0, 1, 2, 4, 6, 8, 10, 12, 0, 1, 2, 4, 6, 8, 10, 12, 0, 1, 2, 4, 6, 8, 10, 12, 0, 1, 2, 4, 6, 8, 10, 12 };
	uint8_t myUChars1[32] = { 0, 3, 1, 2, 3, 4, 5, 6, 0, 3, 1, 2, 3, 4, 5, 6, 0, 3, 1, 2, 3, 4, 5, 6, 0, 3, 1, 2, 3, 4, 5, 6 };
	uint8_t myUCharsResults[32] = { 0, 0, 1, 2, 3, 4, 5, 6, 0, 0, 1, 2, 3, 4, 5, 6, 0, 0, 1, 2, 3, 4, 5, 6, 0, 0, 1, 2, 3, 4, 5, 6 };

	Sub64(reinterpret_cast<uint64_t*>(myLongs0), reinterpret_cast<uint64_t*>(myLongs1));
	Sub64(myULongs0, myULongs1);
	Sub32(reinterpret_cast<uint32_t*>(myInts0), reinterpret_cast<uint32_t*>(myInts1));
	Sub32(myUInts0, myUInts1);
	Sub16SaturateSigned(myShorts0, myShorts1);
	Sub16SaturateUnsigned(myUShorts0, myUShorts1);
	Sub8SaturateSigned(myChars0, myChars1);
	Sub8SaturateUnsigned(myUChars0, myUChars1);

	assert(std::equal(std::begin(myLongs0), std::end(myLongs0), myLongsResults));
	assert(std::equal(std::begin(myULongs0), std::end(myULongs0), myULongsResults));
	assert(std::equal(std::begin(myInts0), std::end(myInts0), myIntsResults));
	assert(std::equal(std::begin(myUInts0), std::end(myUInts0), myUIntsResults));
	assert(std::equal(std::begin(myShorts0), std::end(myShorts0), myShortsResults));
	assert(std::equal(std::begin(myUShorts0), std::end(myUShorts0), myUShortsResults));
	assert(std::equal(std::begin(myChars0), std::end(myChars0), myCharsResults));
	assert(std::equal(std::begin(myUChars0), std::end(myUChars0), myUCharsResults));
}

void runTests()
{
	testHasCPUIDSupport();
	testHasAVX2Support();
	testAVX256Constructor();
	testAVX256SubscriptOperator();
	testAVX256PrintOperator();
	testAVX256NextandPrevious();
	testAVX256Add();
	testAVX256Sub();
}

int main()
{
	runTests();

	std::cout << "All tests passed\n";

	return 0;
}

#endif