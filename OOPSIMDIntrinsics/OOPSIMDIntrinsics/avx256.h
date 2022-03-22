#ifndef AVX256_H
#define AVX256_H

#include <array>

class AVX256Utils
{
public:
	static bool HasAVXSupport(void);
	friend void testHasCPUIDSupport(void);

private:
	static bool HasCPUIDSupport(void);
};

template <typename T>
class AVX256
{
};

#endif