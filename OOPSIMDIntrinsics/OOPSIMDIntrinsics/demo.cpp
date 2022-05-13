#include <iostream>

#include "test.h"
#include "avx256.h"

#ifndef TEST

uint8_t sumScalar(uint8_t array[], uint64_t size)
{
	uint8_t sum = 0;

	for (uint64_t i = 0; i < size; ++i)
		sum += array[i];

	return sum;
}

uint8_t sumAVX256(uint8_t array[], uint64_t size)
{	
	AVX256<uint8_t> avxArray{ array };
	AVX256<uint8_t> avxSum{ {0, 0, 0, 0, 0, 0, 0, 0} };

	uint64_t count = size / (32 / sizeof(uint8_t));

	for (uint64_t i = 0; i < count; ++i, avxArray.Next())
		avxSum.Add(avxArray);

	return avxSum.Sum();
}

uint8_t sumIntrinsics(uint8_t array[], uint64_t size)
{
	__m256i avxSum = _mm256_setzero_si256();
	__m256i avxArray = _mm256_loadu_epi8(array);

	uint64_t count = size / (32 / sizeof(uint8_t));
	int packedCount = 32 / sizeof(uint8_t);

	for (uint64_t i = 0; i < count; ++i, array += packedCount)
		avxSum = _mm256_add_epi8(avxSum, avxArray);

	__m256i sums = _mm256_sad_epu8(avxSum, _mm256_setzero_si256()); // sums = |0|0|0|s3|0|0|0|s2|0|0|0|s1|0|0|0|s0| (16-bit packing)
	sums = _mm256_add_epi64(_mm256_permute4x64_epi64(sums, 0b00011011), sums); // sums = |0|0|0|s0|0|0|0|s1|0|0|0|s2|0|0|0|s3| + |0|0|0|s3|0|0|0|s2|0|0|0|s1|0|0|0|s0| = |0|s0+s3|0|s1+s2|0|s2+s1|0|s3+s0| (32-bit packing)			
	sums = _mm256_add_epi32(sums, _mm256_shuffle_epi32(sums, 0b01010110)); // sums = |0|s0+s3|0|s1+s2|0|s2+s1|0|s3+s0| + |0|0|0|s0+s3|0|0|0|s2+s1| = |0|0|0|s1+s2+s0+s3|0|s2+s1|0|s3+s0+s2+s1|				

	return sums.m256i_u32[0];
}

#pragma optimize( "", off ) // Disabling optimisations to ensure clock() calls aren't re-ordered
/* Best of 5 runs shown:
*	Optimisations disabled:
*	  sumScalar() with optimisation disabled: 14.472s
*	  sumAVX256() with optimisation disabled: 1.336s
*	  sumIntrinsics() with optimisation disabled: 0.891s
*   Optimisations enabled (MSVC compiler):
*	  sumScalar() - auto-vectorises using 128-bit SSE: 0.936s
*	  sumAVX256(): 1.498s
*	  sumIntrinsics(): 0.061s
*/
void sumDemo()
{	
	const uint64_t SIZE = (32 / sizeof(uint8_t)) * 100000000;	
	uint8_t* array = new uint8_t[SIZE];

	clock_t scalarStart = clock();
	uint8_t scalarSum = sumScalar(array, SIZE);
	clock_t scalarTime = clock() - scalarStart;

	clock_t vectorStart = clock();
	uint8_t vectorSum = sumAVX256(array, SIZE);
	clock_t vectorTime = clock() - vectorStart;

	clock_t intrinStart = clock();
	uint8_t intrinsicSum = sumIntrinsics(array, SIZE);
	clock_t intrinTime = clock() - intrinStart;

	delete[] array;

	std::cout << (float)scalarTime / CLOCKS_PER_SEC<< "s " << (float)vectorTime / CLOCKS_PER_SEC << "s " << (float)intrinTime / CLOCKS_PER_SEC << "s";	
}

void runDemos()
{
	sumDemo();
}

int main(void)
{
	runDemos();

	return 0;
}

#endif