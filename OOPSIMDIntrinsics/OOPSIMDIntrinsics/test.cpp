#include <iostream>
#include <cassert>
#include <cstdint>
#include <sstream>
#include <array>
#include <algorithm>
#include <numeric>

#include "test.h"
#include "avx256.h"

#ifdef TEST

namespace
{
	extern "C" bool HasCPUIDSupport(void);

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
	float floats[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	double doubles[] = { 0, 1, 2, 3 };
	uint32_t uInts[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	uint16_t uShorts[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	uint8_t uChars[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };	

	AVX256<float> avxFloats{ floats };
	AVX256<double> avxDoubles{ doubles };
	AVX256<uint32_t> avxUInts{ uInts };
	AVX256<uint16_t> avxUShorts{ uShorts };
	AVX256<uint8_t> avxUChars{ uChars };

	assert(std::equal(std::begin(floats), std::end(floats), avxFloats.Data) == true);	
	assert(std::equal(std::begin(doubles), std::end(doubles), avxDoubles.Data) == true);	
	assert(std::equal(std::begin(uInts), std::end(uInts), avxUInts.Data) == true);	
	assert(std::equal(std::begin(uShorts), std::end(uShorts), avxUShorts.Data) == true);	
	assert(std::equal(std::begin(uChars), std::end(uChars), avxUChars.Data) == true);

	AVX256<float> avxFloats2{ { 0, 1, 2, 3, 4, 5, 6, 7 } };
	AVX256<double> avxDoubles2{ { 0, 1, 2, 3 } };
	AVX256<uint32_t> avxUInts2{ { 0, 1, 2, 3, 4, 5, 6, 7 } };
	AVX256<uint16_t> avxUShorts2{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 } };
	AVX256<uint8_t> avxUChars2{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 } };
	
	for (int i = 0; i < 8; ++i) assert(avxFloats2.Data[i] == i);
	for (int i = 0; i < 4; ++i) assert(avxDoubles2.Data[i] == i);
	for (int i = 0; i < 8; ++i) assert(avxUInts2.Data[i] == i);
	for (int i = 0; i < 16; ++i) assert(avxUShorts2.Data[i] == i);
	for (int i = 0; i < 32; ++i) assert(avxUChars2.Data[i] == i);
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
	double myDoubles0[4] = { 0, 1.5, 2.8, -10};
	double myDoubles1[4] = { 1, 2, -1.4, 10};
	double myDoublesResults[4] = { 1, 3.5, 1.4, 0};

	int64_t myLongs0[4] = { INT64_MAX, INT64_MAX - 1, INT64_MIN, INT64_MIN + 1 };
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

	AVX256<double> avxDoubles0{ myDoubles0 };
	AVX256<uint64_t> avxULongs0{ myULongs0 };
	AVX256<int64_t> avxLongs0{ myLongs0 };
	AVX256<uint32_t> avxUInts0{ myUInts0 };
	AVX256<int32_t> avxInts0{ myInts0 };
	AVX256<uint16_t> avxUShorts0{ myUShorts0 };
	AVX256<int16_t> avxShorts0{ myShorts0 };
	AVX256<uint8_t> avxUChars0{ myUChars0 };
	AVX256<int8_t> avxChars0{ myChars0 };

	avxDoubles0.Add(myDoubles1);
	avxULongs0.Add(myULongs1);
	avxLongs0.Add(myLongs1);
	avxUInts0.Add(myUInts1);
	avxInts0.Add(myInts1);
	avxUShorts0.AddSaturate(myUShorts1);
	avxShorts0.AddSaturate(myShorts1);
	avxUChars0.AddSaturate(myUChars1);
	avxChars0.AddSaturate(myChars1);

	assert(std::equal(std::begin(myDoubles0), std::end(myDoubles0), myDoublesResults));
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
	double myDoubles0[4] = { 0, 1.5, 2.8, -10 };
	double myDoubles1[4] = { -1, -2, 1.4, -10 };
	double myDoublesResults[4] = { 1, 3.5, 1.4, 0 };

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

	AVX256<double> avxDoubles0{ myDoubles0 };
	AVX256<uint64_t> avxULongs0{ myULongs0 };
	AVX256<int64_t> avxLongs0{ myLongs0 };
	AVX256<uint32_t> avxUInts0{ myUInts0 };
	AVX256<int32_t> avxInts0{ myInts0 };
	AVX256<uint16_t> avxUShorts0{ myUShorts0 };
	AVX256<int16_t> avxShorts0{ myShorts0 };
	AVX256<uint8_t> avxUChars0{ myUChars0 };
	AVX256<int8_t> avxChars0{ myChars0 };

	avxDoubles0.Sub(myDoubles1);
	avxULongs0.Sub(myULongs1);
	avxLongs0.Sub(myLongs1);
	avxUInts0.Sub(myUInts1);
	avxInts0.Sub(myInts1);
	avxUShorts0.SubSaturate(myUShorts1);
	avxShorts0.SubSaturate(myShorts1);
	avxUChars0.SubSaturate(myUChars1);
	avxChars0.SubSaturate(myChars1);

	assert(std::equal(std::begin(myDoubles0), std::end(myDoubles0), myDoublesResults));
	assert(std::equal(std::begin(myLongs0), std::end(myLongs0), myLongsResults));
	assert(std::equal(std::begin(myULongs0), std::end(myULongs0), myULongsResults));
	assert(std::equal(std::begin(myInts0), std::end(myInts0), myIntsResults));
	assert(std::equal(std::begin(myUInts0), std::end(myUInts0), myUIntsResults));
	assert(std::equal(std::begin(myShorts0), std::end(myShorts0), myShortsResults));
	assert(std::equal(std::begin(myUShorts0), std::end(myUShorts0), myUShortsResults));
	assert(std::equal(std::begin(myChars0), std::end(myChars0), myCharsResults));
	assert(std::equal(std::begin(myUChars0), std::end(myUChars0), myUCharsResults));
}

void testAVX256Mul()
{
	double myDoubles0[4] = { 0, 10.5, -2.8, -100.5 };
	double myDoubles1[4] = { 0, 2, 0.5, -10 };
	double myDoublesResults[4] = { 0, 21, -1.4, 1005 };

	int64_t myLongs0[4] = { 10, -10, 10, static_cast<int64_t>(0xFFFFFFFF00000004)};
	int64_t myLongs1[4] = { -10, -10, 10, 2 };
	int64_t myLongsResults[4] = { -100, 100, 100, 8 };

	uint64_t myULongs0[4] = { static_cast<uint64_t>(1) << 31, static_cast<uint64_t>(1) << 32, 10, 10 };
	uint64_t myULongs1[4] = { 2, 2, 10, 0 };
	uint64_t myULongsResults[4] = { static_cast<uint64_t>(1) << 32, 0, 100, 0 };

	int32_t myInts0[8] = { static_cast<int32_t>(1) << 31, static_cast<int32_t>(1) << 30, 10, -10, 10, -10, 10, -10 };
	int32_t myInts1[8] = { 2, 2, 10, -10, -10, 10, 10, -10 };
	int32_t myIntsResults[8] = { 0, static_cast<int32_t>(1) << 31, 100, 100, -100, -100, 100, 100 };

	uint32_t myUInts0[8] = { static_cast<uint32_t>(1) << 31, static_cast<uint32_t>(1) << 30, 10, 10, static_cast<uint32_t>(1) << 31, static_cast<uint32_t>(1) << 30, 10, 10 };
	uint32_t myUInts1[8] = { 2, 2, 10, 0, 2, 2, 10, 0 };
	uint32_t myUIntsResults[8] = { 0, static_cast<uint32_t>(1) << 31, 100, 0, 0, static_cast<uint32_t>(1) << 31, 100, 0 };

	int16_t myShorts0[16] = { 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10 };
	int16_t myShorts1[16] = { -10, 10, 10, -10, -10, 10, 10, -10, -10, 10, 10, -10, -10, 10, 10, -10 };
	int16_t myShortsResults[16] = { -100, -100, 100, 100, -100, -100, 100, 100, -100, -100, 100, 100, -100, -100, 100, 100 };

	uint16_t myUShorts0[16] = { static_cast<uint16_t>(0x8002), 10, 10, 100, static_cast<uint16_t>(0x8002), 10, 10, 100, 
								static_cast<uint16_t>(0x8002), 10, 10, 100, static_cast<uint16_t>(0x8002), 10, 10, 100 };
	uint16_t myUShorts1[16] = { 2, 10, 0, 10, 2, 10, 0, 10, 2, 10, 0, 10, 2, 10, 0, 10 };
	uint16_t myUShortsResults[16] = { 4, 100, 0, 1000, 4, 100, 0, 1000, 4, 100, 0, 1000, 4, 100, 0, 1000 };

	int8_t myChars0[32] = { 10, -10, 10, -10, 100, -100, 100, -100, 10, -10, 10, -10, 100, -100, 100, -100,
							10, -10, 10, -10, 100, -100, 100, -100, 10, -10, 10, -10, 100, -100, 100, -100 };
	int8_t myChars1[32] = { -10, 10, 10, -10, -10, 10, 10, -10, -10, 10, 10, -10, -10, 10, 10, -10,
							-10, 10, 10, -10, -10, 10, 10, -10, -10, 10, 10, -10, -10, 10, 10, -10 };
	int8_t myCharsResults[32] = { -100, -100, 100, 100, INT8_MIN, INT8_MIN, INT8_MAX, INT8_MAX, -100, -100, 100, 100, INT8_MIN, INT8_MIN, INT8_MAX, INT8_MAX,
								  -100, -100, 100, 100, INT8_MIN, INT8_MIN, INT8_MAX, INT8_MAX, -100, -100, 100, 100, INT8_MIN, INT8_MIN, INT8_MAX, INT8_MAX };

	uint8_t myUChars0[32] = { 1, 10, 100, 200, 1, 10, 100, 200, 1, 10, 100, 200, 1, 10, 100, 200, 
							  1, 10, 100, 200, 1, 10, 100, 200, 1, 10, 100, 200, 1, 10, 100, 200 };
	uint8_t myUChars1[32] = { 0, 10, 2, 2, 0, 10, 2, 2, 0, 10, 2, 2, 0, 10, 2, 2,
							  0, 10, 2, 2, 0, 10, 2, 2, 0, 10, 2, 2, 0, 10, 2, 2 };
	uint8_t myUCharsResults[32] = { 0, 100, 200, UINT8_MAX, 0, 100, 200, UINT8_MAX, 0, 100, 200, UINT8_MAX, 0, 100, 200, UINT8_MAX,
									0, 100, 200, UINT8_MAX, 0, 100, 200, UINT8_MAX, 0, 100, 200, UINT8_MAX, 0, 100, 200, UINT8_MAX };

	AVX256<double> avxDoubles0{ myDoubles0 };
	AVX256<uint64_t> avxULongs0{ myULongs0 };
	AVX256<int64_t> avxLongs0{ myLongs0 };
	AVX256<uint32_t> avxUInts0{ myUInts0 };
	AVX256<int32_t> avxInts0{ myInts0 };
	AVX256<uint16_t> avxUShorts0{ myUShorts0 };
	AVX256<int16_t> avxShorts0{ myShorts0 };
	AVX256<uint8_t> avxUChars0{ myUChars0 };
	AVX256<int8_t> avxChars0{ myChars0 };

	avxDoubles0.Mul(myDoubles1);
	avxULongs0.Mul(myULongs1);
	avxLongs0.Mul(myLongs1);
	avxUInts0.Mul(myUInts1);
	avxInts0.Mul(myInts1);
	avxUShorts0.Mul(myUShorts1);
	avxShorts0.Mul(myShorts1);
	avxUChars0.Mul(myUChars1);
	avxChars0.Mul(myChars1);

	assert(std::equal(std::begin(myDoubles0), std::end(myDoubles0), myDoublesResults));
	assert(std::equal(std::begin(myLongs0), std::end(myLongs0), myLongsResults));
	assert(std::equal(std::begin(myULongs0), std::end(myULongs0), myULongsResults));
	assert(std::equal(std::begin(myInts0), std::end(myInts0), myIntsResults));
	assert(std::equal(std::begin(myUInts0), std::end(myUInts0), myUIntsResults));
	assert(std::equal(std::begin(myShorts0), std::end(myShorts0), myShortsResults));
	assert(std::equal(std::begin(myUShorts0), std::end(myUShorts0), myUShortsResults));
	assert(std::equal(std::begin(myChars0), std::end(myChars0), myCharsResults));
	assert(std::equal(std::begin(myUChars0), std::end(myUChars0), myUCharsResults));
}

void testAVX256Div()
{
	double myDoubles0[4] = { 0, 10.5, -10.5, -21 };
	double myDoubles1[4] = { 2, 2, -4, 2 };
	double myDoublesResults[4] = { 0, 5.25, 2.625, -10.5 };

	float myFloats0[8] = { 0, 10.5, -10.5, -21, 0, 10.5, -10.5, -21 };
	float myFloats1[8] = { 2, 2, -4, 2, 2, 2, -4, 2 };
	float myFloatsResults[8] = { 0, 5.25, 2.625, -10.5, 0, 5.25, 2.625, -10.5 };

	AVX256<double> avxDoubles0{ myDoubles0 };
	AVX256<float> avxFloats0{ myFloats0 };

	avxDoubles0.Div(myDoubles1);
	avxFloats0.Div(myFloats1);

	assert(std::equal(std::begin(myDoubles0), std::end(myDoubles0), myDoublesResults));
	assert(std::equal(std::begin(myFloats0), std::end(myFloats0), myFloatsResults));
}

void testAVX256Set()
{
	double myDoubles[4];
	float myFloats[8];
	int64_t myLongs[4];
	uint32_t myUInts[8];
	int16_t myShorts[16];
	uint8_t myUChars[32];

	const double FLOAT = 10.5;
	const int8_t INT = 100;

	AVX256<double> avxDoubles{ myDoubles };
	AVX256<float> avxFloats{ myFloats };
	AVX256<int64_t> avxLongs{ myLongs };
	AVX256<uint32_t> avxUInts{ myUInts };
	AVX256<int16_t> avxShorts{ myShorts };
	AVX256<uint8_t> avxUChars{ myUChars };

	avxDoubles.Set(FLOAT);
	avxFloats.Set(FLOAT);
	avxLongs.Set(INT);
	avxUInts.Set(INT);
	avxShorts.Set(INT);
	avxUChars.Set(INT);

	assert(std::all_of(std::begin(myDoubles), std::end(myDoubles), [&](double element) {return element == FLOAT; }));
	assert(std::all_of(std::begin(myFloats), std::end(myFloats), [&](float element) {return element == FLOAT; }));
	assert(std::all_of(std::begin(myLongs), std::end(myLongs), [&](int64_t element) {return element == INT; }));
	assert(std::all_of(std::begin(myUInts), std::end(myUInts), [&](uint32_t element) {return element == INT; }));
	assert(std::all_of(std::begin(myShorts), std::end(myShorts), [&](int16_t element) {return element == INT; }));
	assert(std::all_of(std::begin(myUChars), std::end(myUChars), [&](uint8_t element) {return element == INT; }));

	avxDoubles.Set({ 0, 1, 2, 3 });
	avxFloats.Set({ 0, 1, 2, 3, 4, 5, 6, 7 });
	avxLongs.Set({ 0, 1, 2, 3 });
	avxUInts.Set({ 0, 1, 2, 3, 4, 5, 6, 7 });
	avxShorts.Set({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 });
	avxUChars.Set({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 });

	for (int i = 0; i < 4; ++i) assert(myDoubles[i] == i);
	for (int i = 0; i < 8; ++i) assert(myFloats[i] == i);
	for (int i = 0; i < 4; ++i) assert(myLongs[i] == i);
	for (int i = 0; i < 8; ++i) assert(myUInts[i] == i);
	for (int i = 0; i < 16; ++i) assert(myShorts[i] == i);
	for (int i = 0; i < 32; ++i) assert(myUChars[i] == i);
}

void testAVX256Clear()
{
	double myDoubles[4] = { 1, 2, 3, 4 };
	float myFloats[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	int64_t myLongs[4] = { 1, 2, 3, 4 };
	int32_t myInts[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	int16_t myShorts[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	int8_t myChars[32] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };

	AVX256<double> avxDoubles{ myDoubles };
	AVX256<float> avxFloats{ myFloats };
	AVX256<int64_t> avxLongs{ myLongs };
	AVX256<int32_t> avxInts{ myInts };
	AVX256<int16_t> avxShorts{ myShorts };
	AVX256<int8_t> avxChars{ myChars };

	avxDoubles.Clear();
	avxFloats.Clear();
	avxLongs.Clear();
	avxInts.Clear();
	avxShorts.Clear();
	avxChars.Clear();

	assert(std::all_of(std::begin(myDoubles), std::end(myDoubles), [](double element) {return element == 0; }));
	assert(std::all_of(std::begin(myFloats), std::end(myFloats), [](float element) {return element == 0; }));
	assert(std::all_of(std::begin(myLongs), std::end(myLongs), [](int64_t element) {return element == 0; }));
	assert(std::all_of(std::begin(myInts), std::end(myInts), [](int32_t element) {return element == 0; }));
	assert(std::all_of(std::begin(myShorts), std::end(myShorts), [](int16_t element) {return element == 0; }));
	assert(std::all_of(std::begin(myChars), std::end(myChars), [](int8_t element) {return element == 0; }));
}

void testAVX256Not()
{
	AVX256<uint64_t> avxULongs{ {UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX} };
	AVX256<double> avxDoubles{ {std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max()} };

	avxULongs.Not();
	avxDoubles.Not();

	assert(std::all_of(avxULongs.Data, avxULongs.Data + 4, [](uint64_t element) {return element == 0; }));
	assert(std::all_of(avxDoubles.Data, avxDoubles.Data + 4, [](double element) {return element == std::numeric_limits<double>::min(); }));
}

void testAVX256And()
{
	AVX256<uint64_t> avxULongs{ {UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX} };
	AVX256<double> avxDoubles{ {std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max()} };

	avxULongs.And({ 0, 0, 0, 0 });
	avxDoubles.And({ 0, 0, 0, 0 });

	assert(std::all_of(avxULongs.Data, avxULongs.Data + 4, [](uint64_t element) {return element == 0; }));
	assert(std::all_of(avxDoubles.Data, avxDoubles.Data + 4, [](double element) {return element == 0; }));
}

void testAVX256Or()
{
	AVX256<uint64_t> avxULongs{ {0, 0, 0, 0} };
	AVX256<double> avxDoubles{ {0, 0, 0, 0} };

	avxULongs.Or({ UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX });
	avxDoubles.Or({ std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max() });

	assert(std::all_of(avxULongs.Data, avxULongs.Data + 4, [](uint64_t element) {return element == UINT64_MAX; }));
	assert(std::all_of(avxDoubles.Data, avxDoubles.Data + 4, [](double element) {return element == std::numeric_limits<double>::max(); }));
}

void testAVX256Xor()
{
	AVX256<uint64_t> avxULongs{ {0, 0, 0, 0} };
	AVX256<double> avxDoubles{ {0, 0, 0, 0} };

	avxULongs.Xor(avxULongs);
	avxDoubles.Xor(avxDoubles);

	assert(std::all_of(avxULongs.Data, avxULongs.Data + 4, [](uint64_t element) {return element == 0; }));
	assert(std::all_of(avxDoubles.Data, avxDoubles.Data + 4, [](double element) {return element == 0; }));
}

void testAVX256Sum()
{
	AVX256<uint8_t> avxUChars0{ { UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX,
								  UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX,
								  UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX,
								  UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX } };
	AVX256<int8_t> avxChars0{ { INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN,
							    INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN,
							    INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN,
							    INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN, INT8_MIN } };

	AVX256<uint16_t> avxUShorts0{ { UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX,
							        UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX } };
	AVX256<int16_t> avxShorts0{ { INT16_MIN, INT16_MIN, INT16_MIN, INT16_MIN, INT16_MIN, INT16_MIN, INT16_MIN, INT16_MIN,
								  INT16_MIN, INT16_MIN, INT16_MIN, INT16_MIN, INT16_MIN, INT16_MIN, INT16_MIN, INT16_MIN } };

	AVX256<uint32_t> avxUInts0{ { UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX } };
	AVX256<int32_t> avxInts0{ { INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN } };

	AVX256<float> avxFloats0{ { 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5 } };
	AVX256<double> avxDoubles0{ { 1.5, 2.5, 3.5, 4.5 } };

	assert(avxUChars0.Sum() == std::accumulate(avxUChars0.Data, avxUChars0.Data + 32, static_cast<uint32_t>(0)));
	assert(avxChars0.Sum() == std::accumulate(avxChars0.Data, avxChars0.Data + 32, static_cast<int32_t>(0)));
	assert(avxUShorts0.Sum() == std::accumulate(avxUShorts0.Data, avxUShorts0.Data + 16, static_cast <uint32_t>(0)));
	assert(avxShorts0.Sum() == std::accumulate(avxShorts0.Data, avxShorts0.Data + 16, static_cast <int32_t>(0)));
	assert(avxUInts0.Sum() == std::accumulate(avxUInts0.Data, avxUInts0.Data + 8, static_cast <uint32_t>(0)));
	assert(avxInts0.Sum() == std::accumulate(avxInts0.Data, avxInts0.Data + 8, static_cast <int32_t>(0)));
	assert(avxFloats0.Sum() == std::accumulate(avxFloats0.Data, avxFloats0.Data + 8, static_cast <float>(0)));
	assert(avxDoubles0.Sum() == std::accumulate(avxDoubles0.Data, avxDoubles0.Data + 4, static_cast <double>(0)));
}

void testAVX256Average()
{
	uint16_t myUShorts0[16] = { 0, 0, 11, UINT16_MAX, 0, 0, 11, UINT16_MAX, 0, 0, 11, UINT16_MAX, 0, 0, 11, UINT16_MAX };
	uint16_t myUShorts1[16] = { 0, 4, 10, 200, 0, 4, 10, 200, 0, 4, 10, 200, 0, 4, 10, 200 };
	uint16_t myUShortsResults[16] = { 0, 2, 11, 1 + (UINT16_MAX + 200) / 2, 0, 2, 11, 1 + (UINT16_MAX + 200) / 2, 0, 2, 11, 1 + (UINT16_MAX + 200) / 2, 0, 2, 11, 1 + (UINT16_MAX + 200) / 2 };

	uint8_t myUChars0[32] = { 0, 0, 11, UINT8_MAX, 0, 0, 11, UINT8_MAX, 0, 0, 11, UINT8_MAX, 0, 0, 11, UINT8_MAX,
							  0, 0, 11, UINT8_MAX, 0, 0, 11, UINT8_MAX, 0, 0, 11, UINT8_MAX, 0, 0, 11, UINT8_MAX };
	uint8_t myUChars1[32] = { 0, 4, 10, 200, 0, 4, 10, 200, 0, 4, 10, 200, 0, 4, 10, 200,
							  0, 4, 10, 200, 0, 4, 10, 200, 0, 4, 10, 200, 0, 4, 10, 200 };
	uint8_t myUCharsResults[32] = { 0, 2, 11, 1 + (UINT8_MAX + 200) / 2, 0, 2, 11, 1 + (UINT8_MAX + 200) / 2, 0, 2, 11, 1 + (UINT8_MAX + 200) / 2, 0, 2, 11, 1 + (UINT8_MAX + 200) / 2,
									0, 2, 11, 1 + (UINT8_MAX + 200) / 2, 0, 2, 11, 1 + (UINT8_MAX + 200) / 2, 0, 2, 11, 1 + (UINT8_MAX + 200) / 2, 0, 2, 11, 1 + (UINT8_MAX + 200) / 2 };

	AVX256<uint16_t> avxUShorts0{ myUShorts0 };
	AVX256<uint8_t> avxUChars0{ myUChars0 };

	avxUShorts0.Average(myUShorts1);
	avxUChars0.Average(myUChars1);

	assert(std::equal(std::begin(myUShorts0), std::end(myUShorts0), myUShortsResults));
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
	testAVX256Mul();
	testAVX256Div();
	testAVX256Set();
	testAVX256Clear();	
	testAVX256Not();
	testAVX256And();
	testAVX256Or();
	testAVX256Xor();
	testAVX256Sum();
	testAVX256Average();
	
}

int main()
{
	runTests();

	std::cout << "All tests passed\n";

	return 0;
}

#endif