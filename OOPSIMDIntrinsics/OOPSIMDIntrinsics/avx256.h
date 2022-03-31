#ifndef AVX256_H
#define AVX256_H

#include <type_traits>
#include <cstdint>
#include <initializer_list>
#include <intrin.h>

namespace AVX256Utils
{
	extern "C" bool HasAVX2Support(void);
};

template <typename T, typename = std::enable_if_t<std::is_fundamental_v<T> && !std::is_void_v<T>>>
class AVX256
{
public:
	AVX256(T* data) : Data{ data } { }

	T& operator[] (int index) const { return Data[index]; }	

	operator T* () { return Data; }

	void Next() { Data += (256 / 8) / sizeof(T); } 
	
	void Previous() { Data -= (256 / 8) / sizeof(T); } 

	// Addition

	void Add(const double* operand) { _mm256_storeu_pd(Data, _mm256_add_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand))); }
	void Add(const float* operand) { _mm256_storeu_ps(Data, _mm256_add_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand))); }

	void Add(const uint64_t* operand) { _mm256_storeu_epi64(Data, _mm256_add_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand))); }
	void Add(const int64_t* operand) { _mm256_storeu_epi64(Data, _mm256_add_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand))); }

	void Add(const uint32_t* operand) { _mm256_storeu_epi32(Data, _mm256_add_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand))); }
	void Add(const int32_t* operand) { _mm256_storeu_epi32(Data, _mm256_add_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand))); }

	void Add(const uint16_t* operand) { _mm256_storeu_epi16(Data, _mm256_add_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }
	void Add(const int16_t* operand) { _mm256_storeu_epi16(Data, _mm256_add_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }
	void AddSaturate(const uint16_t* operand) { _mm256_storeu_epi16(Data, _mm256_adds_epu16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }
	void AddSaturate(const int16_t* operand) { _mm256_storeu_epi16(Data, _mm256_adds_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }

	void Add(const uint8_t* operand) { _mm256_storeu_epi8(Data, _mm256_add_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand))); }
	void Add(const int8_t* operand) { _mm256_storeu_epi8(Data, _mm256_add_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand))); }
	void AddSaturate(const uint8_t* operand) { _mm256_storeu_epi8(Data, _mm256_adds_epu8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand))); }
	void AddSaturate(const int8_t* operand) { _mm256_storeu_epi8(Data, _mm256_adds_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand))); }

	AVX256& operator+=(const T* operand) { this->Add(operand); return *this; }
	void Add(std::initializer_list<T> operand) {  this->Add(operand.begin()); }

	// Subtraction

	void Sub(const double* operand) { _mm256_storeu_pd(Data, _mm256_sub_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand))); }
	void Sub(const float* operand) { _mm256_storeu_ps(Data, _mm256_sub_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand))); }

	void Sub(const uint64_t* operand) { _mm256_storeu_epi64(Data, _mm256_sub_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand))); }
	void Sub(const int64_t* operand) { _mm256_storeu_epi64(Data, _mm256_sub_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand))); }

	void Sub(const uint32_t* operand) { _mm256_storeu_epi32(Data, _mm256_sub_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand))); }
	void Sub(const int32_t* operand) { _mm256_storeu_epi32(Data, _mm256_sub_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand))); }

	void Sub(const uint16_t* operand) { _mm256_storeu_epi16(Data, _mm256_sub_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }
	void Sub(const int16_t* operand) { _mm256_storeu_epi16(Data, _mm256_sub_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }
	void SubSaturate(const uint16_t* operand) { _mm256_storeu_epi16(Data, _mm256_subs_epu16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }
	void SubSaturate(const int16_t* operand) { _mm256_storeu_epi16(Data, _mm256_subs_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }

	void Sub(const uint8_t* operand) { _mm256_storeu_epi8(Data, _mm256_sub_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand))); }
	void Sub(const int8_t* operand) { _mm256_storeu_epi8(Data, _mm256_sub_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand))); }
	void SubSaturate(const uint8_t* operand) { _mm256_storeu_epi8(Data, _mm256_subs_epu8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand))); }
	void SubSaturate(const int8_t* operand) { _mm256_storeu_epi8(Data, _mm256_subs_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand))); }

	AVX256& operator-=(const T* operand) { this->Sub(operand); return *this; }
	void Sub(std::initializer_list<T> operand) { this->Sub(operand.begin()); }

	friend void testAVX256Constructor();

private:
	T* Data;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const AVX256<T>& myAvx)
{
	for (int i = 0; i < (256 / 8) / sizeof(T); ++i)
		out << "|" << myAvx[i];

	out << '|';

	return out;
}

#endif