#ifndef AVX256_H
#define AVX256_H

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
public:
	AVX256(T* data) : Data{ data } {}
	T* Data;

private:
};

#endif