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

	// Creates an AVX256 that points to a newly allocated 32-bytes
	AVX256() : Data{ new T[32 / sizeof(T)] }, OwnsData{ true } { }

	// Creates an AVX256 that points to the specified data
	AVX256(T* const data) : Data{ data }, OwnsData{ false } { }

	// Creates an AVX256 that points to a newly created copy of the specified array. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256(const std::array<T, 32 / sizeof(T)>& data) : Data{ new T[32 / sizeof(T)] }, OwnsData{ true } { std::copy(data.cbegin(), data.cend(), Data); }

	// Moves the data of the specifed AVX256 into a new AVX256
	AVX256(AVX256&& avx) noexcept : Data{ avx.Data }, OwnsData{ true }  { avx.Data = nullptr; }

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

	AVX256 operator+(const T* operand)
	{
		T* data = new T[32 / sizeof(T)];
		std::copy(Data, Data + 32 / sizeof(T), data);
		AVX256<T> x{data};
		x += operand;
		return x; 
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
	* 64-bit (integer): The low 32-bits of each element are multiplied, the 64-bit result is saved
	* 32-bit (integer): 32-bits are multiplied, the low 32-bits of the result is saved
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

	// Broadcast the specified value into all elements of the AVX256 
	void Set(const T value)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_set1_pd(value));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_set1_ps(value));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_set1_epi64x(value));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_set1_epi32(value));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_set1_epi16(value));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_set1_epi8(value));
	}

	// Copy the specified data into the data AVX256 points to
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

	// Broadcast the specified value into all elements of the AVX256 
	AVX256& operator=(const T value) { Set(value); return *this; }

	// Copy the specified data into the data AVX256 points to
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


	// Not ///////////

	void Not()
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_andnot_pd(_mm256_loadu_pd(Data), _mm256_set1_pd(1)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_andnot_ps(_mm256_loadu_ps(Data), _mm256_set1_ps(1)));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_andnot_si256(_mm256_loadu_epi64(Data), _mm256_set1_epi64x(1)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_andnot_si256(_mm256_loadu_epi32(Data), _mm256_set1_epi32(1)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_andnot_si256(_mm256_loadu_epi16(Data), _mm256_set1_epi16(1)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_andnot_si256(_mm256_loadu_epi8(Data), _mm256_set1_epi8(1)));
	}

	AVX256& operator~() { Not(); return *this; }


	// And ///////////

	void And(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_and_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_and_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_and_si256(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_and_si256(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_and_si256(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_and_si256(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void And(const std::array<T, 32 / sizeof(T)>& operand) { And(&operand[0]); }

	AVX256& operator&=(const T* operand) { And(operand); return *this; }

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator&=(const std::array<T, 32 / sizeof(T)>& operand) { And(&operand[0]); return *this; }


	// Or ///////////

	void Or(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_or_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_or_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_or_si256(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_or_si256(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_or_si256(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_or_si256(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void Or(const std::array<T, 32 / sizeof(T)>& operand) { Or(&operand[0]); }

	AVX256& operator|=(const T* operand) { Or(operand); return *this; }

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator|=(const std::array<T, 32 / sizeof(T)>& operand) { Or(&operand[0]); return *this; }


	// Xor ///////////

	void Xor(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_xor_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_xor_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_xor_si256(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_xor_si256(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_xor_si256(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_xor_si256(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void Xor(const std::array<T, 32 / sizeof(T)>& operand) { Xor(&operand[0]); }

	AVX256& operator^=(const T* operand) { Xor(operand); return *this; }

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator^=(const std::array<T, 32 / sizeof(T)>& operand) { Xor(&operand[0]); return *this; }


	// Shift /////////

	// Performs a logical left shift. Available on 64, 32, and 16-bit integers only.
	void ShiftLeft(const int shift)
	{
		if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_slli_epi64(_mm256_loadu_epi64(Data), shift));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_slli_epi32(_mm256_loadu_epi32(Data), shift));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_slli_epi16(_mm256_loadu_epi16(Data), shift));
		else if constexpr (true) static_assert(false, "AVX256: ShiftLeft(shift) is not available for floating-point types or 8-bit integers");
	}

	// Performs a logical left shift. Available on 32 and 64-bit integers only.
	void ShiftLeft(const T* shifts)
	{
		if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_sllv_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(shifts)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_sllv_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(shifts)));
		else if constexpr (true) static_assert(false, "AVX256: ShiftLeft(shifts) is only 32 and 64-bit integers");
	}

	// Performs a logical left shift. Available on 32 and 64-bit integers only. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	void ShiftLeft(const std::array<T, 32 / sizeof(T)>& shifts) { ShiftLeft(&shifts[0]); }

	// Performs a logical left shift. Available on 64, 32, and 16-bit integers only.
	AVX256& operator<<=(const int shift) { ShiftLeft(shift); return *this; }

	// Performs a logical left shift. Available on 32 and 64-bit integers only.
	AVX256& operator<<=(const T* shifts) { ShiftLeft(shifts); return *this; }

	// Performs a logical left shift. Available on 32 and 64-bit integers only. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator<<=(const std::array<T, 32 / sizeof(T)>& shifts) { ShiftLeft(&shifts[0]); return *this; }

	/*
	* Signed types (32 and 16-bit integers): Arithmetic shift
	* Unsigned types (64, 32, and 16-bit integers): Logical shift
	*/
	void ShiftRight(const int shift)
	{
		if constexpr (std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_srli_epi64(_mm256_loadu_epi64(Data), shift));
		else if constexpr (std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_srli_epi32(_mm256_loadu_epi32(Data), shift));
		else if constexpr (std::is_same_v<T, int32_t>) _mm256_storeu_epi32(Data, _mm256_srai_epi32(_mm256_loadu_epi32(Data), shift));
		else if constexpr (std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_srli_epi16(_mm256_loadu_epi16(Data), shift));
		else if constexpr (std::is_same_v<T, int16_t>) _mm256_storeu_epi16(Data, _mm256_srai_epi16(_mm256_loadu_epi16(Data), shift));
		else if constexpr (true) { static_assert(false, "AVX256: ShiftRight(shift) is only available for 64 (unsigned), 32, and 16-bit integers"); }
	}

	/*
	* Signed types (32-bit integers): Arithmetic shift
	* Unsigned types (64 and 32-bit integers): Logical shift
	*/
	void ShiftRight(const T* shifts)
	{
		if constexpr (std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_srlv_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(shifts)));
		else if constexpr (std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_srlv_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(shifts)));
		else if constexpr (std::is_same_v<T, int32_t>) _mm256_storeu_epi32(Data, _mm256_srav_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(shifts)));
		else if constexpr (true) static_assert(false, "AVX256: ShiftRight(shifts) is only available for 64 (unsigned) and 32-bit integers");
	}

	/*
	* Signed types (32-bit integers): Arithmetic shift
	* Unsigned types (64 and 32-bit integers): Logical shift
	* If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	*/
	void ShiftRight(const std::array<T, 32 / sizeof(T)>& shifts) { ShiftRight(&shifts[0]); }

	/*
	* Signed types (32 and 16-bit integers): Arithmetic shift
	* Unsigned types (64, 32, and 16-bit integers): Logical shift
	*/
	AVX256& operator>>=(const int shift) { ShiftRight(shift); return *this; }

	/*
	* Signed types (32-bit integers): Arithmetic shift
	* Unsigned types (64 and 32-bit integers): Logical shift
	*/
	AVX256& operator>>=(const T* shifts) { ShiftRight(shifts); return *this; }

	/*
	* Signed types (32-bit integers): Arithmetic shift
	* Unsigned types (64 and 32-bit integers): Logical shift
	* If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	*/
	AVX256& operator>>=(const std::array<T, 32 / sizeof(T)>& shifts) { ShiftRight(&shifts[0]); return *this; }


	// IsZero ///////////

	// Returns true if all elements are 0, false otherwise
	bool IsZero() 
	{ 
		if constexpr (std::is_same_v<T, double>) switch (_mm256_testz_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(Data))) { case 0: return false; case 1: return true; }
		else if constexpr (std::is_same_v<T, float>) switch (_mm256_testz_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(Data))) { case 0: return false; case 1: return true; }
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) switch (_mm256_testz_si256(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(Data))) { case 0: return false; case 1: return true; }
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) switch (_mm256_testz_si256(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(Data))) { case 0: return false; case 1: return true; }
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) switch (_mm256_testz_si256(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(Data))) { case 0: return false; case 1: return true; }
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) switch (_mm256_testz_si256(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(Data))) { case 0: return false; case 1: return true; }
		return false;
	}	


	// Sum ///////////

	// Returns the sum of all packed elements. The result is returned in full precision except with 32-bit integers, whose sum is accumulated into 32-bits and hence can overflow. This function is not available for 64-bit integers.
	auto Sum() 
	{
		if constexpr (std::is_same_v<T, uint8_t>)
		{			
			__m256i sums = _mm256_sad_epu8(_mm256_loadu_epi8(Data), _mm256_setzero_si256()); // sums = |0|0|0|s3|0|0|0|s2|0|0|0|s1|0|0|0|s0| (16-bit packing)
			sums = _mm256_add_epi64(_mm256_permute4x64_epi64(sums, 0b00011011), sums); // sums = |0|0|0|s0|0|0|0|s1|0|0|0|s2|0|0|0|s3| + |0|0|0|s3|0|0|0|s2|0|0|0|s1|0|0|0|s0| = |0|s0+s3|0|s1+s2|0|s2+s1|0|s3+s0| (32-bit packing)			
			sums = _mm256_add_epi32(sums, _mm256_shuffle_epi32(sums, 0b01010110)); // sums = |0|s0+s3|0|s1+s2|0|s2+s1|0|s3+s0| + |0|0|0|s0+s3|0|0|0|s2+s1| = |0|0|0|s1+s2+s0+s3|0|s2+s1|0|s3+s0+s2+s1|				
			return sums.m256i_u32[0];
		}
		else if constexpr (std::is_same_v<T, int8_t>)
		{
			__m256i sums = _mm256_sad_epu8( // sums = |0|0|0|s3+8*128|0|0|0|s2+8*128|0|0|0|s1+8*128|0|0|0|s0+8*128| (16-bit packing)
				_mm256_add_epi8(_mm256_loadu_epi8(Data), _mm256_set1_epi8(static_cast<int8_t>(128))),
				_mm256_setzero_si256()
			);
			sums = _mm256_add_epi64(_mm256_permute4x64_epi64(sums, 0b00011011), sums); // sums = |0|0|0|s0+8*128|0|0|0|s1+8*128|0|0|0|s2+8*128|0|0|0|s3+8*128| + |0|0|0|s3+8*128|0|0|0|s2+8*128|0|0|0|s1+8*128|0|0|0|s0+8*128| = |0|s0+s3+16*128|0|s1+s2+16*128|0|s2+s1+16*128|0|s3+s0+16*128| (32-bit packing)			
			sums = _mm256_add_epi64(sums, _mm256_shuffle_epi32(sums, 0b01010110)); // sums = |0|s0+s3+16*128|0|s1+s2+16*128|0|s2+s1+16*128|0|s3+s0+16*128| + |0|0|0|s0+s3+16*128|0|0|0|s2+s1+16*128| = |0|0|0|s1+s2+s0+s3+32*128|0|s2+s1+16*128|0|s3+s0+s2+s1+32*128|				
			return sums.m256i_i32[0] - (32 * 128);
		}
		else if constexpr (std::is_same_v<T, uint16_t>)
		{
			return _mm256_hadd_epi32( // = |0|0|0|0|0|0|0|s7+s6+s5+s4+s3+s2+s1+s0|
				_mm256_hadd_epi32( // = |0|0|0|0|0|0|s7+s6+s5+s4|s3+s2+s1+s0| 
					_mm256_permute4x64_epi64( // = |0|0|0|0|s7+s6|s5+s4|s3+s2|s1+s0| 
						_mm256_hadd_epi32( // = |0|0|s7+s6|s5+s4|0|0|s3+s2|s1+s0| 
							_mm256_hadd_epi32( // = |s7|s6|s5|s4|s3|s2|s1|s0| (32-bit packing)
								_mm256_unpacklo_epi16(
									_mm256_loadu_epi16(Data),
									_mm256_setzero_si256()
								),
								_mm256_unpackhi_epi16(
									_mm256_loadu_epi16(Data),
									_mm256_setzero_si256()
								)
							),
							_mm256_setzero_si256()
						),
						0b01011000
					),
					_mm256_setzero_si256()
				),
				_mm256_setzero_si256()
			).m256i_u32[0];
		}
		else if constexpr (std::is_same_v<T, int16_t>)
		{
			return _mm256_hadd_epi32( // = |0|0|0|0|0|0|0|s7+s6+s5+s4+s3+s2+s1+s0|
				_mm256_hadd_epi32( // = |0|0|0|0|0|0|s7+s6+s5+s4|s3+s2+s1+s0|
					_mm256_permute4x64_epi64( // = |0|0|0|0|s7+s6|s5+s4|s3+s2|s1+s0|
						_mm256_hadd_epi32( // = |0|0|s7+s6|s5+s4|0|0|s3+s2|s1+s0|
							_mm256_madd_epi16( // = |s7|s6|s5|s4|s3|s2|s1|s0| (32-bit packing)
								_mm256_loadu_epi16(Data), 
								_mm256_set1_epi16(1)), 
							_mm256_setzero_si256()), 
						0b01011000
					),
					_mm256_setzero_si256()
				), 
				_mm256_setzero_si256()
			).m256i_i32[0];
		}	
		else if constexpr (std::is_same_v<T, uint32_t>)
		{
			return _mm256_hadd_epi32( // = |0|0|0|0|0|0|0|s3+s2+s1+s0|
				_mm256_hadd_epi32( // = |0|0|0|0|0|0|s3+s2|s1+s0|
					_mm256_permute4x64_epi64( // = |0|0|0|0|s3|s2|s1|s0|
						_mm256_hadd_epi32( // = |0|0|s3|s2|0|0|s1|s0| (32-bit packing)
							_mm256_loadu_epi16(Data),
							_mm256_setzero_si256()
						),
						0b01011000
					),
					_mm256_setzero_si256()
				),
				_mm256_setzero_si256()
			).m256i_u32[0];
		}
		else if constexpr (std::is_same_v<T, int32_t>)
		{
			return _mm256_hadd_epi32( // = |0|0|0|0|0|0|0|s3+s2+s1+s0|
				_mm256_hadd_epi32( // = |0|0|0|0|0|0|s3+s2|s1+s0|
					_mm256_permute4x64_epi64( // = |0|0|0|0|s3|s2|s1|s0|
						_mm256_hadd_epi32( // = |0|0|s3|s2|0|0|s1|s0| (32-bit packing)
							_mm256_loadu_epi16(Data),
							_mm256_setzero_si256()
						),
						0b01011000
					),
					_mm256_setzero_si256()
				),
				_mm256_setzero_si256()
			).m256i_i32[0];
		}
		else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t>) { static_assert(false, "AVX256: Sum() is not available for 64-bit integers"); }
		else if constexpr (std::is_same_v<T, float>)
		{
			return _mm256_hadd_ps( // = |0|0|0|0|0|0|0|s3+s2+s1+s0|
				_mm256_hadd_ps( // = |0|0|0|0|0|0|s3+s2|s1+s0|					
					_mm256_castpd_ps(_mm256_permute4x64_pd( // = |0|0|0|0|s3|s2|s1|s0|
						_mm256_castps_pd(_mm256_hadd_ps( // = |0|0|s3|s2|0|0|s1|s0| (32-bit packing)
							_mm256_loadu_ps(Data),
							_mm256_setzero_ps()							
						)),
						0b01011000
					)),
					_mm256_setzero_ps()
				),
				_mm256_setzero_ps()
			).m256_f32[0];
		}
		else if constexpr (std::is_same_v<T, double>)
		{
		return _mm256_hadd_pd( // = |0|0|0|s1+s0|
					_mm256_permute4x64_pd( // = |0|0|s1|s0|
						_mm256_hadd_pd( // = |0|s1|0|s0| (64-bit packing)
							_mm256_loadu_pd(Data),
							_mm256_setzero_pd()
						),
						0b01011000
					),
					_mm256_setzero_pd()			
			).m256d_f64[0];
		}
	}

	
	// Average ///////////

	// Computes the mean of corresponding elements. Fractional results are rounded up to the nearest integer
	void Average(const T* operand)
	{
		if constexpr (std::is_same_v<T, uint16_t>) { _mm256_storeu_epi16(Data, _mm256_avg_epu16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }
		else if constexpr (std::is_same_v<T, uint8_t>) { _mm256_storeu_epi8(Data, _mm256_avg_epu8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand))); }
		else if constexpr (true) { static_assert(false, "AVX256: Average() only available for uint8_t and uint16_t types"); }
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