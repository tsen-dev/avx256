#ifndef AVX256_H
#define AVX256_H

#include <type_traits>
#include <cstdint>
#include <intrin.h>
#include <array>

namespace AVX256Utils
{
	extern "C" bool HasAVX2Support(void);
};

template <typename T, typename = std::enable_if_t<std::is_fundamental_v<T> && !std::is_void_v<T>>>
class AVX256
{
public:
	T* Data;

	AVX256(T* data) : Data{ data } { }

	T& operator[] (int index) const { return Data[index]; }	

	operator T* () { return Data; }

	void Next() { Data += (256 / 8) / sizeof(T); } 
	
	void Previous() { Data -= (256 / 8) / sizeof(T); } 

	// Addition //

	void Add(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_add_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_add_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_add_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_add_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_add_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_add_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void Add(const std::array<T, 32 / sizeof(T)>& operand) { Add(&operand[0]); }

	void AddSaturate(const T* operand)
	{
		if constexpr (std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_adds_epu16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int16_t>) _mm256_storeu_epi16(Data, _mm256_adds_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_adds_epu8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		else if constexpr (std::is_same_v<T, int8_t>) _mm256_storeu_epi8(Data, _mm256_adds_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		else if constexpr (true) static_assert(false, "AddSaturate() is only available for 16 or 8 bit addition");
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void AddSatuate(const std::array<T, 32 / sizeof(T)>& operand) { AddSatuate(&operand[0]); }

	// Performs saturation arithmetic on 16 and 8 bit types, wraparound arithmetic otherwise
	AVX256& operator+=(const T* operand) 
	{ 
		if constexpr (sizeof(T) > 2) { Add(operand);  return *this; }
		else if constexpr (sizeof(T) <= 2) { AddSatuate(operand);  return *this; }
	}

	// Performs saturation arithmetic on 16 and 8 bit types, wraparound arithmetic otherwise. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator+=(const std::array<T, 32 / sizeof(T)>& operand)
	{
		if constexpr (sizeof(T) > 2) { Add(&operand[0]);  return *this; }
		else if constexpr (sizeof(T) <= 2) { AddSatuate(&operand[0]);  return *this; }
	}


	// Subtraction //

	void Sub(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_sub_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_sub_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_sub_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_sub_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_sub_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_sub_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void Sub(const std::array<T, 32 / sizeof(T)>& operand) { Sub(&operand[0]); }

	void SubSaturate(const T* operand)
	{
		if constexpr (std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_subs_epu16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int16_t>) _mm256_storeu_epi16(Data, _mm256_subs_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_subs_epu8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		else if constexpr (std::is_same_v<T, int8_t>) _mm256_storeu_epi8(Data, _mm256_subs_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		else if constexpr (true) static_assert(false, "SubSaturate() is only available for 16 or 8 bit subtraction");
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void SubSatuate(const std::array<T, 32 / sizeof(T)>& operand) { SubSatuate(&operand[0]); }

	// Performs saturation arithmetic on 16 and 8 bit types, wraparound arithmetic otherwise
	AVX256& operator-=(const T* operand)
	{
		if constexpr (sizeof(T) > 2) { Sub(operand);  return *this; }
		else if constexpr (sizeof(T) <= 2) { SubSatuate(operand);  return *this; }
	}

	// Performs saturation arithmetic on 16 and 8 bit types, wraparound arithmetic otherwise. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator-=(const std::array<T, 32 / sizeof(T)>& operand)
	{
		if constexpr (sizeof(T) > 2) { Sub(&operand[0]);  return *this; }
		else if constexpr (sizeof(T) <= 2) { SubSatuate(&operand[0]);  return *this; }
	}

	
	// Multiplication //

	template<typename = std::enable_if_t<std::is_same_v<T, double>>>
	void Mul(const double* operand) { _mm256_storeu_pd(Data, _mm256_mul_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand))); }

	template<typename = std::enable_if_t<std::is_same_v<T, float>>>
	void Mul(const float* operand) { _mm256_storeu_ps(Data, _mm256_mul_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand))); }
	
	// Multiply the unsigned low 32-bits of each 64-bit element
	template<typename = std::enable_if_t<std::is_same_v<T, uint64_t>>>
	void Mul(const uint64_t* operand) { _mm256_storeu_epi64(Data, _mm256_mul_epu32(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand))); }

	// Multiply the signed low 32-bits of each 64-bit element
	template<typename = std::enable_if_t<std::is_same_v<T, int64_t>>>
	void Mul(const int64_t* operand) { _mm256_storeu_epi64(Data, _mm256_mul_epi32(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand))); }

	// Multiply unsigned 32-bit elements, save the low 32-bits of the result
	template<typename = std::enable_if_t<std::is_same_v<T, uint32_t>>>
	void Mul(const uint32_t* operand) { _mm256_storeu_epi32(Data, _mm256_mullo_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand))); }

	// Multiply signed 32-bit elements, save the low 32-bits of the result
	template<typename = std::enable_if_t<std::is_same_v<T, int32_t>>>
	void Mul(const int32_t* operand) { _mm256_storeu_epi32(Data, _mm256_mullo_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand))); }

	// Multiply unsigned 16-bit elements, save the low 16-bits of the result
	template<typename = std::enable_if_t<std::is_same_v<T, uint16_t>>>
	void Mul(const uint16_t* operand) { _mm256_storeu_epi16(Data, _mm256_mullo_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }

	// Multiply signed 16-bit elements, save the low 16-bits of the result
	template<typename = std::enable_if_t<std::is_same_v<T, int16_t>>>
	void Mul(const int16_t* operand) { _mm256_storeu_epi16(Data, _mm256_mullo_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }

	// Multiply unsigned 8-bit elements, save the low 8-bits of the result (which is unsigned saturated before saving)
	template<typename = std::enable_if_t<std::is_same_v<T, uint8_t>>>
	void Mul(const uint8_t* operand) 
	{ 
		_mm256_storeu_epi8(
			Data, 
			_mm256_packus_epi16(
				_mm256_mullo_epi16(
					_mm256_unpacklo_epi8(_mm256_loadu_epi8(Data), _mm256_set1_epi8(0)), 
					_mm256_unpacklo_epi8(_mm256_loadu_epi8(operand), _mm256_set1_epi8(0))
				),
				_mm256_mullo_epi16(
					_mm256_unpackhi_epi8(_mm256_loadu_epi8(Data), _mm256_set1_epi8(0)),
					_mm256_unpackhi_epi8(_mm256_loadu_epi8(operand), _mm256_set1_epi8(0))
				)
			)
		); 
	}

	// Multiply signed 8-bit elements, save the low 8-bits of the result (which is signed saturated before saving)
	template<typename = std::enable_if_t<std::is_same_v<T, int8_t>>>
	void Mul(const int8_t* operand) 
	{ 
		_mm256_storeu_epi8(
			Data,
			_mm256_packs_epi16(
				_mm256_mullo_epi16(
					_mm256_unpacklo_epi8(_mm256_loadu_epi8(Data), _mm256_cmpgt_epi8(_mm256_set1_epi8(0), _mm256_loadu_epi8(Data))),
					_mm256_unpacklo_epi8(_mm256_loadu_epi8(operand), _mm256_cmpgt_epi8(_mm256_set1_epi8(0), _mm256_loadu_epi8(operand)))
				),
				_mm256_mullo_epi16(
					_mm256_unpackhi_epi8(_mm256_loadu_epi8(Data), _mm256_cmpgt_epi8(_mm256_set1_epi8(0), _mm256_loadu_epi8(Data))),
					_mm256_unpackhi_epi8(_mm256_loadu_epi8(operand), _mm256_cmpgt_epi8(_mm256_set1_epi8(0), _mm256_loadu_epi8(operand)))
				)
			)
		);
	}

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used.
	AVX256& operator*=(const T* operand) { Mul(operand); return *this; }
	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator*=(const std::array<T, 32 / sizeof(T)>& operand) { Mul(&operand[0]); return *this; }
	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void Mul(const std::array<T, 32 / sizeof(T)>& operand) { Mul(&operand[0]); }


	// Division

	template<typename = std::enable_if_t<std::is_same_v<T, double>>>
	void Div(const double* operand) { _mm256_storeu_pd(Data, _mm256_div_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand))); }

	template<typename = std::enable_if_t<std::is_same_v<T, float>>>
	void Div(const float* operand) { _mm256_storeu_ps(Data, _mm256_div_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand))); }

	AVX256& operator/=(const T* operand) { Div(operand); return *this; }
	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator/=(const std::array<T, 32 / sizeof(T)>& operand) { Div(&operand[0]); return *this; }
	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void Div(const std::array<T, 32 / sizeof(T)>& operand) { Div(&operand[0]); }


	// Set

	template<typename = std::enable_if_t<std::is_same_v<T, double>>>
	void Set(const double value) { _mm256_storeu_pd(Data, _mm256_set1_pd(value)); }

	template<typename = std::enable_if_t<std::is_same_v<T, double>>>
	void Set(const double * values) { _mm256_storeu_pd(Data, _mm256_loadu_pd(values)); }

	template<typename = std::enable_if_t<std::is_same_v<T, float>>>
	void Set(const float value) { _mm256_storeu_ps(Data, _mm256_set1_ps(value)); }

	template<typename = std::enable_if_t<std::is_same_v<T, float>>>
	void Set(const float * values) { _mm256_storeu_ps(Data, _mm256_loadu_ps(values)); }

	template<typename = std::enable_if_t<std::is_same_v<T, int64_t>>>
	void Set(const int64_t value) { _mm256_storeu_epi64(Data, _mm256_set1_epi64x(value)); }

	template<typename = std::enable_if_t<std::is_same_v<T, int64_t>>>
	void Set(const int64_t* values) { _mm256_storeu_epi64(Data, _mm256_loadu_epi64(values)); }

	template<typename = std::enable_if_t<std::is_same_v<T, uint64_t>>>
	void Set(const uint64_t value) { _mm256_storeu_epi64(Data, _mm256_set1_epi64x(value)); }

	template<typename = std::enable_if_t<std::is_same_v<T, uint64_t>>>
	void Set(const uint64_t* values) { _mm256_storeu_epi64(Data, _mm256_loadu_epi64(values)); }

	template<typename = std::enable_if_t<std::is_same_v<T, int32_t>>>
	void Set(const int32_t value) { _mm256_storeu_epi32(Data, _mm256_set1_epi32(value)); }

	template<typename = std::enable_if_t<std::is_same_v<T, int32_t>>>
	void Set(const int32_t* values) { _mm256_storeu_epi32(Data, _mm256_loadu_epi32(values)); }

	template<typename = std::enable_if_t<std::is_same_v<T, uint32_t>>>
	void Set(const uint32_t value) { _mm256_storeu_epi32(Data, _mm256_set1_epi32(value)); }

	template<typename = std::enable_if_t<std::is_same_v<T, uint32_t>>>
	void Set(const uint32_t* values) { _mm256_storeu_epi32(Data, _mm256_loadu_epi32(values)); }

	template<typename = std::enable_if_t<std::is_same_v<T, int16_t>>>
	void Set(const int16_t value) { _mm256_storeu_epi16(Data, _mm256_set1_epi16(value)); }

	template<typename = std::enable_if_t<std::is_same_v<T, int16_t>>>
	void Set(const int16_t* values) { _mm256_storeu_epi16(Data, _mm256_loadu_epi16(values)); }

	template<typename = std::enable_if_t<std::is_same_v<T, uint16_t>>>
	void Set(const uint16_t value) { _mm256_storeu_epi16(Data, _mm256_set1_epi16(value)); }

	template<typename = std::enable_if_t<std::is_same_v<T, uint16_t>>>
	void Set(const uint16_t* values) { _mm256_storeu_epi16(Data, _mm256_loadu_epi16(values)); }

	template<typename = std::enable_if_t<std::is_same_v<T, int8_t>>>
	void Set(const int8_t value) { _mm256_storeu_epi8(Data, _mm256_set1_epi8(value)); }

	template<typename = std::enable_if_t<std::is_same_v<T, int8_t>>>
	void Set(const int8_t* values) { _mm256_storeu_epi8(Data, _mm256_loadu_epi8(values)); }

	template<typename = std::enable_if_t<std::is_same_v<T, uint8_t>>>
	void Set(const uint8_t value) { _mm256_storeu_epi8(Data, _mm256_set1_epi8(value)); }

	template<typename = std::enable_if_t<std::is_same_v<T, uint8_t>>>
	void Set(const uint8_t* values) { _mm256_storeu_epi8(Data, _mm256_loadu_epi8(values)); }

	AVX256& operator=(const T value) { Set(value); return *this; }
	AVX256& operator=(const T* values) { Set(values); return *this; }

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void Set(const std::array<T, 32 / sizeof(T)>& values) { Set(&values[0]); }
	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator=(const std::array<T, 32 / sizeof(T)>& values) { Set(&values[0]); return *this; }	


	// Clear

	void Clear() 
	{ 
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_setzero_pd());
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_setzero_ps());
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_setzero_si256());
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_setzero_si256());
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_setzero_si256());
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_setzero_si256());
	}


private:
	void DefaultSub(const T* operand, std::true_type) { Sub(operand); }
	void DefaultSub(const T* operand, std::false_type) { SubSaturate(operand); }
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