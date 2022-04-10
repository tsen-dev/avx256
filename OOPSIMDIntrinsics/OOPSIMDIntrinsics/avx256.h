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

	AVX256(T* const data) : Data{ data }, OwnsData{ false } { }

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256(const std::array<T, 32 / sizeof(T)>& data) : Data{ new T[32 / sizeof(T)] }, OwnsData{ true } { std::copy(data.cbegin(), data.cend(), Data); }

	T& operator[] (int index) const { return Data[index]; }	

	operator T* () { return Data; }

	// Increments 'Data' to point to the next 32 bytes (or 256 bits). Should not be used if adjacent memory isn't safe to access.
	void Next() { Data += (256 / 8) / sizeof(T); } 
	
	// Increments 'Data' to point to the previous 32 bytes (or 256 bits). Should not be used if adjacent memory isn't safe to access.
	void Previous() { Data -= (256 / 8) / sizeof(T); } 

	~AVX256() { if (OwnsData) delete[] Data; }


	// Addition ////////////////////

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
		else if constexpr (true) static_assert(false, "AVX256: AddSaturate() is only available for 16 or 8 bit addition");
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


	// Subtraction ///////////////////

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
		else if constexpr (true) static_assert(false, "AVX256: SubSaturate() is only available for 16 or 8 bit subtraction");
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

	
	// Multiplication //////////////////

	/*
	* 64-bit: The low 32-bits of each element are multiplied, the 64-bit result is saved
	* 32-bit: 32-bits are multiplied, the low 32-bits of the result is saved
	* 16-bit: 16-bits are multiplied, the low 16-bits of the result is saved
	* 8-bit: 8-bits are multiplied, the low 8-bits of the result is saturated and saved
	*/
	void Mul(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_mul_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_mul_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_mul_epu32(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, int64_t>) _mm256_storeu_epi64(Data, _mm256_mul_epi32(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_mullo_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, int32_t>) _mm256_storeu_epi32(Data, _mm256_mullo_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_mullo_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int16_t>) _mm256_storeu_epi16(Data, _mm256_mullo_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, uint8_t>)
		{
			_mm256_storeu_epi8(
				Data,
				_mm256_packus_epi16(
					_mm256_mullo_epi16(
						_mm256_unpacklo_epi8(_mm256_loadu_epi8(Data), _mm256_setzero_si256()),
						_mm256_unpacklo_epi8(_mm256_loadu_epi8(operand), _mm256_setzero_si256())
					),
					_mm256_mullo_epi16(
						_mm256_unpackhi_epi8(_mm256_loadu_epi8(Data), _mm256_setzero_si256()),
						_mm256_unpackhi_epi8(_mm256_loadu_epi8(operand), _mm256_setzero_si256())
					)
				)
			);
		}
		else if constexpr (std::is_same_v<T, int8_t>)
		{
			_mm256_storeu_epi8(
				Data,
				_mm256_packs_epi16(
					_mm256_mullo_epi16(									// Sign extension
						_mm256_unpacklo_epi8(_mm256_loadu_epi8(Data), _mm256_cmpgt_epi8(_mm256_setzero_si256(), _mm256_loadu_epi8(Data))),
						_mm256_unpacklo_epi8(_mm256_loadu_epi8(operand), _mm256_cmpgt_epi8(_mm256_setzero_si256(), _mm256_loadu_epi8(operand)))
					),
					_mm256_mullo_epi16(									// Sign extension
						_mm256_unpackhi_epi8(_mm256_loadu_epi8(Data), _mm256_cmpgt_epi8(_mm256_setzero_si256(), _mm256_loadu_epi8(Data))),
						_mm256_unpackhi_epi8(_mm256_loadu_epi8(operand), _mm256_cmpgt_epi8(_mm256_setzero_si256(), _mm256_loadu_epi8(operand)))
					)
				)
			);
		}
	}

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void Mul(const std::array<T, 32 / sizeof(T)>& operand) { Mul(&operand[0]); }

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used.
	AVX256& operator*=(const T* operand) { Mul(operand); return *this; }

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator*=(const std::array<T, 32 / sizeof(T)>& operand) { Mul(&operand[0]); return *this; }


	// Division ///////////////////

	void Div(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_div_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_div_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (true) static_assert(false, "AVX256: Division is only available for double and float types");
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void Div(const std::array<T, 32 / sizeof(T)>& operand) { Div(&operand[0]); }

	AVX256& operator/=(const T* operand) { Div(operand); return *this; }

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator/=(const std::array<T, 32 / sizeof(T)>& operand) { Div(&operand[0]); return *this; }


	// Set // //////////////////

	void Set(const T value)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_set1_pd(value));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_set1_ps(value));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_set1_epi64x(value));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_set1_epi32(value));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_set1_epi16(value));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_set1_epi8(value));
	}

	void Set(const T* values)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_loadu_pd(values));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_loadu_ps(values));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_loadu_epi64(values));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_loadu_epi32(values));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_loadu_epi16(values));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_loadu_epi8(values));
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void Set(const std::array<T, 32 / sizeof(T)>& values) { Set(&values[0]); }

	AVX256& operator=(const T value) { Set(value); return *this; }

	AVX256& operator=(const T* values) { Set(values); return *this; }

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator=(const std::array<T, 32 / sizeof(T)>& values) { Set(&values[0]); return *this; }	


	// Clear //////////////////

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
		bool OwnsData; // Specifies whether the memory 'Data' points to was allocated at the constructor
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