#include <iostream>
#include "avx256.h"

int main(void)
{
	std::cout << "AVX support is " << (AVX256::HasAVXSupport() ? "available" : "unavailable");

	return 0;
}