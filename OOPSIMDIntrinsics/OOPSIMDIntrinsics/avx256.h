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

template<typename T>
std::ostream& operator<< (std::ostream& out, const AVX256<T>& myAvx)
{
	for (int i = 0; i < (256 / 8) / sizeof(T); ++i)
		out << "|" << myAvx.Data[i];

	out << '|';

	return out;
}


#endif