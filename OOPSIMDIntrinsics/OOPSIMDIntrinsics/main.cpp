#include <iostream>

#include "test.h"
#include "avx256.h"

#ifndef TEST

int main(void)
{
	std::cout << "AVX support is " << (AVX256::HasAVXSupport() ? "available" : "unavailable");

	return 0;
}

#endif