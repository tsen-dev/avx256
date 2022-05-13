#include <iostream>

#include "test.h"
#include "avx256.h"

#ifndef TEST

#pragma optimize( "", off )
float sum(float array[], int size)
{
	float sum = 0;

	for (int i = 0; i < size; ++i) 
		sum += array[i];

	return sum;
}

#pragma optimize( "", off )
float avxSum(float array[], int size)
{	
	AVX256<float> avxArray{ array };
	AVX256<float> avxSum{ {0, 0, 0, 0, 0, 0, 0, 0} };

	int count = size / (32 / sizeof(float));

	for (int i = 0; i < count; ++i, avxArray.Next())
	{
		avxSum += avxArray;
	}

	return avxSum.Sum();
}

#pragma optimize( "", off )
void sumDemo()
{
	const int SIZE = (32 / sizeof(float)) * 100000000;
	float* array = new float[SIZE];

	clock_t scalarStart = clock();
	float scalarSum = sum(array, SIZE);
	clock_t scalarTime = clock() - scalarStart;

	clock_t vectorStart = clock();
	float vectorSum = avxSum(array, SIZE);
	clock_t vectorTime = clock() - vectorStart;

	std::cout << (float)scalarTime / CLOCKS_PER_SEC << "s " << (float)vectorTime / CLOCKS_PER_SEC << "s";

	delete[] array;
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