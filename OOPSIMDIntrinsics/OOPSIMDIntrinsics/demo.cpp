#include <iostream>
#include <array>
#include <random>

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

	uint64_t count = size / (32 / sizeof(uint8_t));
	int packedCount = 32 / sizeof(uint8_t);

	for (uint64_t i = 0; i < count; ++i, array += packedCount)
		avxSum = _mm256_add_epi8(avxSum, _mm256_loadu_epi8(array));

	__m256i sums = _mm256_sad_epu8(avxSum, _mm256_setzero_si256()); // sums = |0|0|0|s3|0|0|0|s2|0|0|0|s1|0|0|0|s0| (16-bit packing)
	sums = _mm256_add_epi64(_mm256_permute4x64_epi64(sums, 0b00011011), sums); // sums = |0|0|0|s0|0|0|0|s1|0|0|0|s2|0|0|0|s3| + |0|0|0|s3|0|0|0|s2|0|0|0|s1|0|0|0|s0| = |0|s0+s3|0|s1+s2|0|s2+s1|0|s3+s0| (32-bit packing)			
	sums = _mm256_add_epi32(sums, _mm256_shuffle_epi32(sums, 0b01010110)); // sums = |0|s0+s3|0|s1+s2|0|s2+s1|0|s3+s0| + |0|0|0|s0+s3|0|0|0|s2+s1| = |0|0|0|s1+s2+s0+s3|0|s2+s1|0|s3+s0+s2+s1|				

	return sums.m256i_u32[0];
}

void thresholdScalar(uint8_t array[], uint64_t size, uint8_t boundary)
{
	for (uint64_t i = 0; i < size; ++i)
		array[i] = (array[i] > boundary ? 255 : 0);
}

void thresholdAVX256(uint8_t array[], uint64_t size, uint8_t boundary)
{
	AVX256<uint8_t> avxArray{ array };
	AVX256<uint8_t> avxBoundary, avxValue;
	
	std::array<uint8_t, 32> mask;
	AVX256<uint8_t> avxMask{ mask.data() };

	avxBoundary = boundary, avxValue = 255;
	uint64_t count = size / (32 / sizeof(uint8_t));

	for (uint64_t i = 0; i < count; ++i, avxArray.Next())
	{
		mask = avxArray > avxBoundary;
		avxMask &= avxValue;
		avxArray = avxMask;
	}
}

void thresholdIntrinsics(uint8_t array[], uint64_t size, uint8_t boundary)
{
	__m256i avxBoundary = 
		_mm256_xor_si256(
			_mm256_set1_epi8(boundary),
			_mm256_set1_epi8(static_cast<uint8_t>(0x80)
			)
		);

	__m256i avxSignExtension = _mm256_set1_epi8(static_cast<uint8_t>(0x80));

	__m256i avxValue = _mm256_set1_epi8(static_cast<uint8_t>(255));

	uint64_t packedSize = 32 / sizeof(uint8_t);

	for (uint64_t i = 0; i < size; i += packedSize)
	{
		_mm256_storeu_epi8(&array[i],
			_mm256_and_si256(
				_mm256_cmpgt_epi8(
					_mm256_xor_si256(
						_mm256_loadu_epi8(&array[i]),
						avxSignExtension
					),
					avxBoundary
				),
				avxValue
			)
		);
	}
}

#pragma optimize( "", off ) // Disabling optimisations to ensure clock() calls aren't re-ordered
/* Best of 5 runs shown (MSVC compiler):
*	Optimisations disabled:
*	  sumScalar(): 14.472s
*	  sumAVX256(): 1.336s
*	  sumIntrinsics(): 0.919s
*   Optimisations enabled:
*	  sumScalar() - auto-vectorises using 128-bit SSE: 0.936s
*	  sumAVX256(): 1.498s
*	  sumIntrinsics(): 0.305s
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

	std::cout << (float)scalarTime / CLOCKS_PER_SEC<< "s " << (float)vectorTime / CLOCKS_PER_SEC << "s " << (float)intrinTime / CLOCKS_PER_SEC << "s\n";	
}

#pragma optimize( "", off ) // Disabling optimisations to ensure clock() calls aren't re-ordered
/* Best of 5 runs shown (MSVC compiler):
*	Optimisations disabled:
*	  thresholdScalar(): 0.417s
*	  thresholdAVX256(): 0.074s
*	  thresholdIntrinsics(): 0.009s
*   Optimisations enabled:
*	  thresholdScalar(): 0.037s
*	  thresholdAVX256(): 0.171s
*	  thresholdIntrinsics(): 0.004s
*/
void thresholdDemo()
{
	const uint64_t SIZE = (32 / sizeof(uint8_t)) * 1000000;
	uint8_t* array = new uint8_t[SIZE];
	uint8_t* array2 = new uint8_t[SIZE];
	uint8_t* array3 = new uint8_t[SIZE];

	std::default_random_engine randomEngine(static_cast<int>(time(NULL)));
	std::normal_distribution<float> normalDistribution{128, 5};
	for (uint64_t i = 0; i < SIZE; ++i) 
	{ 
		array[i] = static_cast<uint8_t>(normalDistribution(randomEngine)); 
		array2[i] = array[i];
		array3[i] = array[i]; 
	}

	clock_t scalarStart = clock();
	thresholdScalar(array, SIZE, 128);
	clock_t scalarTime = clock() - scalarStart;
	delete[] array;

	clock_t vectorStart = clock();
	thresholdAVX256(array2, SIZE, 128);
	clock_t vectorTime = clock() - vectorStart;
	delete[] array2;

	clock_t intrinStart = clock();
	thresholdIntrinsics(array3, SIZE, 128);
	clock_t intrinTime = clock() - intrinStart;
	delete[] array3;

	std::cout << (float)scalarTime / CLOCKS_PER_SEC << "s " << (float)vectorTime / CLOCKS_PER_SEC << "s " << (float)intrinTime / CLOCKS_PER_SEC << "s\n";
}

void runDemos()
{
	//sumDemo();
	thresholdDemo();
}

int main(void)
{
	runDemos();

	return 0;
}

#endif