#ifndef AVX256_H
#define AVX256_H

#include <type_traits>

class AVX256Utils
{
public:
	static bool HasAVXSupport(void);
	friend void testHasCPUIDSupport(void);

private:
	static bool HasCPUIDSupport(void);	
};

template <typename T, typename = std::enable_if_t<std::is_fundamental_v<T> && !std::is_void_v<T>>>
class AVX256
{
public:
	AVX256(T* data) : Data{ data } { }

	T& operator[] (int index) const { return Data[index]; }
	
	void Next() { Data += (256 / 8) / sizeof(T); } 
	
	void Previous() { Data -= (256 / 8) / sizeof(T); } 

	friend void testAVX256Constructor();

private:
	T* Data;
};

template<typename T>
std::ostream& operator<< (std::ostream& out, const AVX256<T>& myAvx)
{
	for (int i = 0; i < (256 / 8) / sizeof(T); ++i)
		out << "|" << myAvx[i];

	out << '|';

	return out;
}

#endif