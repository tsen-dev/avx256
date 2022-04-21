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

template <typename T>
class AVX256
{
public:	
	T* Data;

	// Creates an AVX256 that points to a newly allocated 32-bytes
	AVX256() : Data{ new T[32 / sizeof(T)] }, OwnsData{ true } { static_assert(std::is_fundamental_v<T> && !std::is_void_v<T>, "AVX256: AVX256 is only available for non-void primitive types!"); }

	// Creates an AVX256 that points to the specified data
	AVX256(T* const data) : Data{ data }, OwnsData{ false } { static_assert(std::is_fundamental_v<T> && !std::is_void_v<T>, "AVX256: AVX256 is only available for non-void primitive types!"); }

	// Creates an AVX256 that points to a newly created copy of the specified array. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256(const std::array<T, 32 / sizeof(T)>& data) : Data{ new T[32 / sizeof(T)] }, OwnsData{ true } { static_assert(std::is_fundamental_v<T> && !std::is_void_v<T>, "AVX256: AVX256 is only available for non-void primitive types!"); this->Set(data); }

	// Creates an AVX256 that points to a newly created copy of the specified AVX256's data
	AVX256(const AVX256& avx) : Data{ new T[32 / sizeof(T)] }, OwnsData{ true } { static_assert(std::is_fundamental_v<T> && !std::is_void_v<T>, "AVX256: AVX256 is only available for non-void primitive types!"); this->Set(avx.Data); }

	T& operator[] (int index) const { return Data[index]; }	

	// Increments 'Data' to point to the next 32 bytes (or 256 bits). Should not be used if adjacent memory isn't safe to access.
	void Next() { Data += (256 / 8) / sizeof(T); } 
	
	// Increments 'Data' to point to the previous 32 bytes (or 256 bits). Should not be used if adjacent memory isn't safe to access.
	void Previous() { Data -= (256 / 8) / sizeof(T); } 

	~AVX256() { if (OwnsData) delete[] Data; }


	// Addition ////////////////////

	AVX256& Add(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_add_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_add_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_add_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_add_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_add_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_add_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		return *this;
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& Add(const std::array<T, 32 / sizeof(T)>& operand) { return Add(operand.data()); }

	AVX256& Add(const AVX256& operand) { return Add(operand.Data);}

	AVX256& AddSaturate(const T* operand)
	{
		if constexpr (std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_adds_epu16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int16_t>) _mm256_storeu_epi16(Data, _mm256_adds_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_adds_epu8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		else if constexpr (std::is_same_v<T, int8_t>) _mm256_storeu_epi8(Data, _mm256_adds_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		else if constexpr (true) static_assert(false, "AVX256: AddSaturate() is only available for 16 or 8 bit addition");
		return *this;
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& AddSatuate(const std::array<T, 32 / sizeof(T)>& operand) { return AddSatuate(operand.data()); }

	AVX256& AddSaturate(const AVX256& operand) { return AddSatuate(operand.Data);}

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise
	AVX256& operator+=(const T* operand) 
	{ 
		if constexpr (sizeof(T) > 2) return Add(operand);
		else if constexpr (sizeof(T) <= 2) return AddSatuate(operand);
	}

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator+=(const std::array<T, 32 / sizeof(T)>& operand) { return operator+=(operand.data()); }

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise
	AVX256& operator+=(const AVX256& operand) { return operator+=(operand.Data); }

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise
	std::array<T, 32 / sizeof(T)> operator+(const T* operand)
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();
		
		this->Set(dataOld);
		*this += operand;

		Data = dataOld;
		return result; 
	}

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> operator+(const std::array<T, 32 / sizeof(T)>& operand) { return *this + operand.data(); }

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise
	std::array<T, 32 / sizeof(T)> operator+(const AVX256& operand) { return *this + operand.Data; }


	// Subtraction ///////////////////

	AVX256& Sub(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_sub_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_sub_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_sub_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_sub_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_sub_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_sub_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		return *this;
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& Sub(const std::array<T, 32 / sizeof(T)>& operand) { return Sub(operand.data()); }

	AVX256& Sub(const AVX256& operand) { return Sub(operand.Data); }

	AVX256& SubSaturate(const T* operand)
	{
		if constexpr (std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_subs_epu16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int16_t>) _mm256_storeu_epi16(Data, _mm256_subs_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_subs_epu8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		else if constexpr (std::is_same_v<T, int8_t>) _mm256_storeu_epi8(Data, _mm256_subs_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		else if constexpr (true) static_assert(false, "AVX256: SubSaturate() is only available for 16 or 8 bit subtraction");
		return *this;
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& SubSatuate(const std::array<T, 32 / sizeof(T)>& operand) { return SubSatuate(operand.data()); }

	AVX256& SubSatuate(const AVX256& operand) { return SubSatuate(operand.Data); }

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise
	AVX256& operator-=(const T* operand)
	{
		if constexpr (sizeof(T) > 2) return Sub(operand);
		else if constexpr (sizeof(T) <= 2) return SubSatuate(operand);
	}

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator-=(const std::array<T, 32 / sizeof(T)>& operand) { return operator-=(operand.data()); }

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise
	AVX256& operator-=(const AVX256& operand) { return operator-=(operand.Data); }

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise
	std::array<T, 32 / sizeof(T)> operator-(const T* operand)
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();

		this->Set(dataOld);
		*this -= operand;

		Data = dataOld;
		return result;
	}

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> operator-(const std::array<T, 32 / sizeof(T)>& operand) { return *this - operand.data(); }

	// Performs saturation arithmetic on 16 and 8-bit integers, wraparound arithmetic otherwise
	std::array<T, 32 / sizeof(T)> operator-(const AVX256& operand) { return *this - operand.Data; }

	
	// Multiplication //////////////////

	/*
	* 64-bit (integer): The low 32-bits of each element are multiplied, the 64-bit result is saved
	* 32-bit (integer): 32-bits are multiplied, the low 32-bits of the result is saved
	* 16-bit: 16-bits are multiplied, the low 16-bits of the result is saved
	* 8-bit: 8-bits are multiplied, the low 8-bits of the result is saturated and saved
	*/
	AVX256& Mul(const T* operand)
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
		return *this;
	}

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& Mul(const std::array<T, 32 / sizeof(T)>& operand) { return Mul(operand.data()); }

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used 
	AVX256& Mul(const AVX256& operand) { return Mul(operand.Data); }

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used.
	AVX256& operator*=(const T* operand) { return Mul(operand); }

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator*=(const std::array<T, 32 / sizeof(T)>& operand) { return Mul(operand.data()); }

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used.
	AVX256& operator*=(const AVX256& operand) { return Mul(operand.Data); }

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used.
	std::array<T, 32 / sizeof(T)> operator*(const T* operand)
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();

		this->Set(dataOld);
		*this *= operand;

		Data = dataOld;
		return result;
	}

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> operator*(const std::array<T, 32 / sizeof(T)>& operand) { return *this * operand.data(); }

	// Call the Mul() method. For details on its operation, see the Mul() method for the AVX256 type being used.
	std::array<T, 32 / sizeof(T)> operator*(const AVX256& operand) { return *this * operand.Data; }


	// Division ///////////////////

	// Available for floating point types only
	AVX256& Div(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_div_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_div_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (true) static_assert(false, "AVX256: Division is only available for double and float types");
		return *this;
	}

	// Available for floating point types only. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& Div(const std::array<T, 32 / sizeof(T)>& operand) { return Div(operand.data()); }

	// Available for floating point types only
	AVX256& Div(const AVX256& operand) { return Div(operand.Data); }
	
	// Available for floating point types only
	AVX256& operator/=(const T* operand) { return Div(operand); }

	// Available for floating point types only. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator/=(const std::array<T, 32 / sizeof(T)>& operand) { return Div(operand.data()); }

	// Available for floating point types only
	AVX256& operator/=(const AVX256& operand) { return Div(operand.Data); }

	// Available for floating point types only.
	std::array<T, 32 / sizeof(T)> operator/(const T* operand)
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();

		this->Set(dataOld);
		*this /= operand;

		Data = dataOld;
		return result;
	}

	// Available for floating point types only. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> operator/(const std::array<T, 32 / sizeof(T)>& operand) { return *this / operand.data(); }

	// Available for floating point types only.
	std::array<T, 32 / sizeof(T)> operator/(const AVX256& operand) { return *this / operand.Data; }


	// Set // //////////////////

	// Broadcast the specified value into all elements of the AVX256 
	AVX256& Set(const T value)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_set1_pd(value));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_set1_ps(value));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_set1_epi64x(value));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_set1_epi32(value));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_set1_epi16(value));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_set1_epi8(value));
		return *this;
	}

	// Copy the specified data into the data AVX256 points to
	AVX256& Set(const T* values)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_loadu_pd(values));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_loadu_ps(values));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_loadu_epi64(values));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_loadu_epi32(values));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_loadu_epi16(values));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_loadu_epi8(values));
		return *this;
	}

	// Copy the specified data into the data AVX256 points to. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& Set(const std::array<T, 32 / sizeof(T)>& values) { return Set(values.data()); }

	// Copy the specified data into the data AVX256 points to
	AVX256& Set(const AVX256& values) { return Set(values.Data); }

	// Broadcast the specified value into all elements of the AVX256 
	AVX256& operator=(const T value) { return Set(value); }

	// Copy the specified data into the data AVX256 points to
	AVX256& operator=(const T* values) { return Set(values); }

	// Copy the specified data into the data AVX256 points to. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator=(const std::array<T, 32 / sizeof(T)>& values) { return Set(values.data()); }

	// Copy the specified data into the data AVX256 points to
	AVX256& operator=(const AVX256& values) { return Set(values.Data); }


	// Clear //////////////////

	AVX256& Clear() 
	{ 
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_setzero_pd());
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_setzero_ps());
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_setzero_si256());
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_setzero_si256());
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_setzero_si256());
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_setzero_si256());
		return *this;
	}


	// Negate ///////////

	AVX256& Negate()
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_andnot_pd(_mm256_loadu_pd(Data), _mm256_castsi256_pd(_mm256_set1_epi64x(-1))));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_andnot_ps(_mm256_loadu_ps(Data), _mm256_castsi256_ps(_mm256_set1_epi32(-1))));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_andnot_si256(_mm256_loadu_epi64(Data), _mm256_set1_epi64x(-1)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_andnot_si256(_mm256_loadu_epi32(Data), _mm256_set1_epi32(-1)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_andnot_si256(_mm256_loadu_epi16(Data), _mm256_set1_epi16(-1)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_andnot_si256(_mm256_loadu_epi8(Data), _mm256_set1_epi8(-1)));
		return *this;
	}

	std::array<T, 32 / sizeof(T)> operator~() 
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();

		this->Set(dataOld);
		*this->Negate();

		Data = dataOld;
		return result;
	}


	// And ///////////

	AVX256& And(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_and_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_and_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_and_si256(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_and_si256(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_and_si256(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_and_si256(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		return *this;
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& And(const std::array<T, 32 / sizeof(T)>& operand) { return And(operand.data()); }

	AVX256& And(const AVX256& operand) { return And(operand.Data); }

	AVX256& operator&=(const T* operand) { return And(operand); }

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator&=(const std::array<T, 32 / sizeof(T)>& operand) { return And(operand.data()); }

	AVX256& operator&=(const AVX256& operand) { return And(operand.Data); }

	std::array<T, 32 / sizeof(T)> operator&(const T* operand)
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();

		this->Set(dataOld);
		*this &= operand;

		Data = dataOld;
		return result;
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> operator&(const std::array<T, 32 / sizeof(T)>& operand) { return *this & operand.data(); }

	std::array<T, 32 / sizeof(T)> operator&(const AVX256& operand) { return *this & operand.Data; }


	// Or ///////////

	AVX256& Or(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_or_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_or_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_or_si256(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_or_si256(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_or_si256(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_or_si256(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		return *this;
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& Or(const std::array<T, 32 / sizeof(T)>& operand) { return Or(operand.data()); }

	AVX256& Or(const AVX256& operand) { return Or(operand.Data); }

	AVX256& operator|=(const T* operand) { return Or(operand); }

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator|=(const std::array<T, 32 / sizeof(T)>& operand) { return Or(operand.data()); }

	AVX256& operator|=(const AVX256& operand) { return Or(operand.Data); }

	std::array<T, 32 / sizeof(T)> operator|(const T* operand)
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();

		this->Set(dataOld);
		*this |= operand;

		Data = dataOld;
		return result;
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> operator|(const std::array<T, 32 / sizeof(T)>& operand) { return *this | operand.data(); }

	std::array<T, 32 / sizeof(T)> operator|(const AVX256& operand) { return *this | operand.Data; }


	// Xor ///////////

	AVX256& Xor(const T* operand)
	{
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_xor_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(operand)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_xor_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(operand)));
		else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_xor_si256(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(operand)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_xor_si256(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(operand)));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_xor_si256(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand)));
		else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(Data, _mm256_xor_si256(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand)));
		return *this;
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& Xor(const std::array<T, 32 / sizeof(T)>& operand) { return Xor(operand.data()); }

	AVX256& Xor(const AVX256& operand) { return Xor(operand.Data); }

	AVX256& operator^=(const T* operand) { return Xor(operand); }

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator^=(const std::array<T, 32 / sizeof(T)>& operand) { return Xor(operand.data()); }

	AVX256& operator^=(const AVX256& operand) { return Xor(operand.Data); }

	std::array<T, 32 / sizeof(T)> operator^(const T* operand)
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();

		this->Set(dataOld);
		*this ^= operand;

		Data = dataOld;
		return result;
	}

	// If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> operator^(const std::array<T, 32 / sizeof(T)>& operand) { return *this ^ operand.data(); }

	std::array<T, 32 / sizeof(T)> operator^(const AVX256& operand) { return *this ^ operand.Data; }


	// Shift /////////

	// Performs a logical left shift. Available on 64, 32, and 16-bit integers only.
	AVX256& ShiftLeft(const int shift)
	{
		if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_slli_epi64(_mm256_loadu_epi64(Data), shift));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_slli_epi32(_mm256_loadu_epi32(Data), shift));
		else if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_slli_epi16(_mm256_loadu_epi16(Data), shift));
		else if constexpr (true) static_assert(false, "AVX256: ShiftLeft(shift) is not available for floating-point types or 8-bit integers");
		return *this;
	}

	// Performs a logical left shift. Available on 32 and 64-bit integers only.
	AVX256& ShiftLeft(const T* shifts)
	{
		if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_sllv_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(shifts)));
		else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_sllv_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(shifts)));
		else if constexpr (true) static_assert(false, "AVX256: ShiftLeft(shifts) is only 32 and 64-bit integers");
		return *this;
	}

	// Performs a logical left shift. Available on 32 and 64-bit integers only. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& ShiftLeft(const std::array<T, 32 / sizeof(T)>& shifts) { return ShiftLeft(shifts.data()); }

	// Performs a logical left shift. Available on 32 and 64-bit integers only.
	AVX256& ShiftLeft(const AVX256& shifts) { return ShiftLeft(shifts.Data); }

	// Performs a logical left shift. Available on 64, 32, and 16-bit integers only.
	AVX256& operator<<=(const int shift) { return ShiftLeft(shift); }

	// Performs a logical left shift. Available on 32 and 64-bit integers only.
	AVX256& operator<<=(const T* shifts) { return ShiftLeft(shifts); }

	// Performs a logical left shift. Available on 32 and 64-bit integers only. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	AVX256& operator<<=(const std::array<T, 32 / sizeof(T)>& shifts) { return ShiftLeft(shifts.data()); }

	// Performs a logical left shift. Available on 32 and 64-bit integers only.
	AVX256& operator<<=(const AVX256& shifts) { return ShiftLeft(shifts.Data); }

	// Performs a logical left shift. Available on 64, 32, and 16-bit integers only.
	std::array<T, 32 / sizeof(T)> operator<<(const int shift)
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();

		this->Set(dataOld);
		*this <<= shift;

		Data = dataOld;
		return result;
	}

	// Performs a logical left shift. Available on 64, 32, and 16-bit integers only.
	std::array<T, 32 / sizeof(T)> operator<<(const T* shifts)
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();

		this->Set(dataOld);
		*this <<= shifts;

		Data = dataOld;
		return result;
	}

	// Performs a logical left shift. Available on 64, 32, and 16-bit integers only. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> operator<<(const std::array<T, 32 / sizeof(T)>& shifts) { return *this << shifts.data(); }

	// Performs a logical left shift. Available on 64, 32, and 16-bit integers only.
	std::array<T, 32 / sizeof(T)> operator<<(const AVX256& shifts) { return *this << shifts.Data; }

	/*
	* Signed types (32 and 16-bit integers): Arithmetic shift
	* Unsigned types (64, 32, and 16-bit integers): Logical shift
	*/
	AVX256& ShiftRight(const int shift)
	{
		if constexpr (std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_srli_epi64(_mm256_loadu_epi64(Data), shift));
		else if constexpr (std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_srli_epi32(_mm256_loadu_epi32(Data), shift));
		else if constexpr (std::is_same_v<T, int32_t>) _mm256_storeu_epi32(Data, _mm256_srai_epi32(_mm256_loadu_epi32(Data), shift));
		else if constexpr (std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(Data, _mm256_srli_epi16(_mm256_loadu_epi16(Data), shift));
		else if constexpr (std::is_same_v<T, int16_t>) _mm256_storeu_epi16(Data, _mm256_srai_epi16(_mm256_loadu_epi16(Data), shift));
		else if constexpr (true) { static_assert(false, "AVX256: ShiftRight(shift) is only available for 64 (unsigned), 32, and 16-bit integers"); }
		return *this;
	}

	/*
	* Signed types (32-bit integers): Arithmetic shift
	* Unsigned types (64 and 32-bit integers): Logical shift
	*/
	AVX256& ShiftRight(const T* shifts)
	{
		if constexpr (std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(Data, _mm256_srlv_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(shifts)));
		else if constexpr (std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(Data, _mm256_srlv_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(shifts)));
		else if constexpr (std::is_same_v<T, int32_t>) _mm256_storeu_epi32(Data, _mm256_srav_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(shifts)));
		else if constexpr (true) static_assert(false, "AVX256: ShiftRight(shifts) is only available for 64 (unsigned) and 32-bit integers");
		return *this;
	}

	/*
	* Signed types (32-bit integers): Arithmetic shift
	* Unsigned types (64 and 32-bit integers): Logical shift
	* If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	*/
	AVX256& ShiftRight(const std::array<T, 32 / sizeof(T)>& shifts) { return ShiftRight(shifts.data()); }

	/*
	* Signed types (32-bit integers): Arithmetic shift
	* Unsigned types (64 and 32-bit integers): Logical shift
	* If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	*/
	AVX256& ShiftRight(const AVX256& shifts) { return ShiftRight(shifts.Data); }

	/*
	* Signed types (32 and 16-bit integers): Arithmetic shift
	* Unsigned types (64, 32, and 16-bit integers): Logical shift
	*/
	AVX256& operator>>=(const int shift) { return ShiftRight(shift); }

	/*
	* Signed types (32-bit integers): Arithmetic shift
	* Unsigned types (64 and 32-bit integers): Logical shift
	*/
	AVX256& operator>>=(const T* shifts) { return ShiftRight(shifts); }

	/*
	* Signed types (32-bit integers): Arithmetic shift
	* Unsigned types (64 and 32-bit integers): Logical shift
	* If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	*/
	AVX256& operator>>=(const std::array<T, 32 / sizeof(T)>& shifts) { return ShiftRight(shifts.data()); }

	/*
	* Signed types (32-bit integers): Arithmetic shift
	* Unsigned types (64 and 32-bit integers): Logical shift
	*/
	AVX256& operator>>=(const AVX256& shifts) { return ShiftRight(shifts.Data); }

	/*
	* Signed types (32 and 16-bit integers): Arithmetic shift
	* Unsigned types (64, 32, and 16-bit integers): Logical shift
	*/
	std::array<T, 32 / sizeof(T)> operator>>(const int shift)
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();

		this->Set(dataOld);
		*this >>= shift;

		Data = dataOld;
		return result;
	}

	/*
	* Signed types (32 and 16-bit integers): Arithmetic shift
	* Unsigned types (64, 32, and 16-bit integers): Logical shift
	*/
	std::array<T, 32 / sizeof(T)> operator>>(const T* shifts)
	{
		std::array<T, 32 / sizeof(T)> result{};
		T* dataOld = Data;
		Data = result.data();

		this->Set(dataOld);
		*this >>= shifts;

		Data = dataOld;
		return result;
	}

	/*
	* Signed types (32 and 16-bit integers): Arithmetic shift
	* Unsigned types (64, 32, and 16-bit integers): Logical shift
	* If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	*/ 
	std::array<T, 32 / sizeof(T)> operator>>(const std::array<T, 32 / sizeof(T)>& shifts) { return *this >> shifts.data(); }

	/*
	* Signed types (32 and 16-bit integers): Arithmetic shift
	* Unsigned types (64, 32, and 16-bit integers): Logical shift
	*/
	std::array<T, 32 / sizeof(T)> operator>>(const AVX256& shifts) { return *this >> shifts.Data; }


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


	// IsEqualTo /////////

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are unordered and non-signaling
	std::array<T, 32 / sizeof(T)> IsEqualTo(const T* values)
	{
		std::array<T, 32 / sizeof(T)> mask{};
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(mask.data(), _mm256_cmp_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(values), _CMP_EQ_UQ));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(mask.data(), _mm256_cmp_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(values), _CMP_EQ_UQ));
		else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t>) _mm256_storeu_epi64(mask.data(), _mm256_cmpeq_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(values)));
		else if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, int32_t>) _mm256_storeu_epi32(mask.data(), _mm256_cmpeq_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(values)));
		else if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t>) _mm256_storeu_epi16(mask.data(), _mm256_cmpeq_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(values)));
		else if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) _mm256_storeu_epi8(mask.data(), _mm256_cmpeq_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(values)));
		return mask;
	}

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are unordered and non-signaling. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> IsEqualTo(const std::array<T, 32 / sizeof(T)>& values) { return IsEqualTo(values.data()); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are unordered and non-signaling
	std::array<T, 32 / sizeof(T)> IsEqualTo(const AVX256& values) { return IsEqualTo(values.Data); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are unordered and non-signaling
	std::array<T, 32 / sizeof(T)> operator==(const T* values) { return IsEqualTo(values); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are unordered and non-signaling. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> operator==(const std::array<T, 32 / sizeof(T)>& values) { return IsEqualTo(values.data()); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are unordered and non-signaling
	std::array<T, 32 / sizeof(T)> operator==(const AVX256& values) { return IsEqualTo(values.Data); }


	// IsGreaterThan /////////

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling
	std::array<T, 32 / sizeof(T)> IsGreaterThan(const T* values)
	{
		std::array<T, 32 / sizeof(T)> mask{};
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(mask.data(), _mm256_cmp_pd(_mm256_loadu_pd(Data), _mm256_loadu_pd(values), _CMP_GT_OQ));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(mask.data(), _mm256_cmp_ps(_mm256_loadu_ps(Data), _mm256_loadu_ps(values), _CMP_GT_OQ));
		else if constexpr (std::is_same_v<T, int64_t>) _mm256_storeu_epi64(mask.data(), _mm256_cmpgt_epi64(_mm256_loadu_epi64(Data), _mm256_loadu_epi64(values)));
		else if constexpr (std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(mask.data(), _mm256_cmpgt_epi64(_mm256_xor_si256(_mm256_loadu_epi64(Data), _mm256_set1_epi64x(static_cast<uint64_t>(0x8000000000000000))), _mm256_xor_si256(_mm256_loadu_epi64(values), _mm256_set1_epi64x(static_cast<uint64_t>(0x8000000000000000)))));		
		else if constexpr (std::is_same_v<T, int32_t>) _mm256_storeu_epi32(mask.data(), _mm256_cmpgt_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(values)));
		else if constexpr (std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(mask.data(), _mm256_cmpgt_epi32(_mm256_xor_si256(_mm256_loadu_epi32(Data), _mm256_set1_epi32(static_cast<uint32_t>(0x80000000))), _mm256_xor_si256(_mm256_loadu_epi32(values), _mm256_set1_epi32(static_cast<uint32_t>(0x80000000)))));		
		else if constexpr (std::is_same_v<T, int16_t>) _mm256_storeu_epi16(mask.data(), _mm256_cmpgt_epi16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(values)));
		else if constexpr (std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(mask.data(), _mm256_cmpgt_epi16(_mm256_xor_si256(_mm256_loadu_epi16(Data), _mm256_set1_epi16(static_cast<uint16_t>(0x8000))), _mm256_xor_si256(_mm256_loadu_epi16(values), _mm256_set1_epi16(static_cast<uint16_t>(0x8000)))));
		else if constexpr (std::is_same_v<T, int8_t>) _mm256_storeu_epi8(mask.data(), _mm256_cmpgt_epi8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(values)));
		else if constexpr (std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(mask.data(), _mm256_cmpgt_epi8(_mm256_xor_si256(_mm256_loadu_epi8(Data), _mm256_set1_epi8(static_cast<uint8_t>(0x80))), _mm256_xor_si256(_mm256_loadu_epi8(values), _mm256_set1_epi8(static_cast<uint8_t>(0x80)))));
		return mask;
	}

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> IsGreaterThan(const std::array<T, 32 / sizeof(T)>& values) { return IsGreaterThan(values.data()); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling
	std::array<T, 32 / sizeof(T)> IsGreaterThan(const AVX256& values) { return IsGreaterThan(values.Data); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling
	std::array<T, 32 / sizeof(T)> operator>(const T* values) { return IsGreaterThan(values); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> operator>(const std::array<T, 32 / sizeof(T)>& values) { return IsGreaterThan(values.data()); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling
	std::array<T, 32 / sizeof(T)> operator>(const AVX256& values) { return IsGreaterThan(values.Data); }


	// IsLessThan /////////

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling
	std::array<T, 32 / sizeof(T)> IsLessThan(const T* values)
	{
		std::array<T, 32 / sizeof(T)> mask{};
		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(mask.data(), _mm256_cmp_pd(_mm256_loadu_pd(values), _mm256_loadu_pd(Data), _CMP_GT_OQ));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(mask.data(), _mm256_cmp_ps(_mm256_loadu_ps(values), _mm256_loadu_ps(Data), _CMP_GT_OQ));
		else if constexpr (std::is_same_v<T, int64_t>) _mm256_storeu_epi64(mask.data(), _mm256_cmpgt_epi64(_mm256_loadu_epi64(values), _mm256_loadu_epi64(Data)));
		else if constexpr (std::is_same_v<T, uint64_t>) _mm256_storeu_epi64(mask.data(), _mm256_cmpgt_epi64(_mm256_xor_si256(_mm256_loadu_epi64(values), _mm256_set1_epi64x(static_cast<uint64_t>(0x8000000000000000))), _mm256_xor_si256(_mm256_loadu_epi64(Data), _mm256_set1_epi64x(static_cast<uint64_t>(0x8000000000000000)))));
		else if constexpr (std::is_same_v<T, int32_t>) _mm256_storeu_epi32(mask.data(), _mm256_cmpgt_epi32(_mm256_loadu_epi32(values), _mm256_loadu_epi32(Data)));
		else if constexpr (std::is_same_v<T, uint32_t>) _mm256_storeu_epi32(mask.data(), _mm256_cmpgt_epi32(_mm256_xor_si256(_mm256_loadu_epi32(values), _mm256_set1_epi32(static_cast<uint32_t>(0x80000000))), _mm256_xor_si256(_mm256_loadu_epi32(Data), _mm256_set1_epi32(static_cast<uint32_t>(0x80000000)))));
		else if constexpr (std::is_same_v<T, int16_t>) _mm256_storeu_epi16(mask.data(), _mm256_cmpgt_epi16(_mm256_loadu_epi16(values), _mm256_loadu_epi16(Data)));
		else if constexpr (std::is_same_v<T, uint16_t>) _mm256_storeu_epi16(mask.data(), _mm256_cmpgt_epi16(_mm256_xor_si256(_mm256_loadu_epi16(values), _mm256_set1_epi16(static_cast<uint16_t>(0x8000))), _mm256_xor_si256(_mm256_loadu_epi16(Data), _mm256_set1_epi16(static_cast<uint16_t>(0x8000)))));
		else if constexpr (std::is_same_v<T, int8_t>) _mm256_storeu_epi8(mask.data(), _mm256_cmpgt_epi8(_mm256_loadu_epi8(values), _mm256_loadu_epi8(Data)));
		else if constexpr (std::is_same_v<T, uint8_t>) _mm256_storeu_epi8(mask.data(), _mm256_cmpgt_epi8(_mm256_xor_si256(_mm256_loadu_epi8(values), _mm256_set1_epi8(static_cast<uint8_t>(0x80))), _mm256_xor_si256(_mm256_loadu_epi8(Data), _mm256_set1_epi8(static_cast<uint8_t>(0x80)))));
		return mask;
	}

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> IsLessThan(const std::array<T, 32 / sizeof(T)>& values) { return IsLessThan(values.data()); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling
	std::array<T, 32 / sizeof(T)> IsLessThan(const AVX256& values) { return IsLessThan(values.Data); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling
	std::array<T, 32 / sizeof(T)> operator<(const T* values) { return IsLessThan(values); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling. If the number of items in the aggregate initialiser is less than the number of packed items in AVX256, the unspecified items are set to 0
	std::array<T, 32 / sizeof(T)> operator<(const std::array<T, 32 / sizeof(T)>& values) { return IsLessThan(values.data()); }

	// Returns a condition mask where each element whose corresponding condition evaluated to true is set to all 1's, otherwise to all 0's. Floating-point comparisons are ordered and non-signaling
	std::array<T, 32 / sizeof(T)> operator<(const AVX256& values) { return IsLessThan(values.Data); }


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
				_mm256_xor_si256(_mm256_loadu_epi8(Data), _mm256_set1_epi8(static_cast<int8_t>(0x80))), // Add 128
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

	// Computes the mean of corresponding elements, fractional results are rounded up to the nearest integer. This function is only available for 16 and 8-bit integers
	AVX256& Average(const T* operand)
	{
		if constexpr (std::is_same_v<T, uint16_t>) { _mm256_storeu_epi16(Data, _mm256_avg_epu16(_mm256_loadu_epi16(Data), _mm256_loadu_epi16(operand))); }
		else if constexpr (std::is_same_v<T, int16_t>)
		{
			_mm256_storeu_epi16(
				Data,
				_mm256_xor_si256( // Subrtract 32768
					_mm256_avg_epu16(
						_mm256_xor_si256( // Add 32768
							_mm256_loadu_epi16(Data), 
							_mm256_set1_epi16(static_cast<int16_t>(0x8000))
						),
						_mm256_xor_si256( // Add 32768
							_mm256_loadu_epi16(operand),
							_mm256_set1_epi16(static_cast<int16_t>(0x8000))
						)
					),
					_mm256_set1_epi16(static_cast<int16_t>(0x8000))
				)
			);
		}
		else if constexpr (std::is_same_v<T, uint8_t>) { _mm256_storeu_epi8(Data, _mm256_avg_epu8(_mm256_loadu_epi8(Data), _mm256_loadu_epi8(operand))); }
		else if constexpr (std::is_same_v<T, int8_t>) 
		{ 
			_mm256_storeu_epi8(
				Data,
				_mm256_xor_si256( // Subract 128
					_mm256_avg_epu8(
						_mm256_xor_si256( // Add 128
							_mm256_loadu_epi8(Data),
							_mm256_set1_epi8(static_cast<int8_t>(0x80))
						),
						_mm256_xor_si256( // Add 128
							_mm256_loadu_epi8(operand),
							_mm256_set1_epi8(static_cast<int8_t>(0x80))
						)
					),
					_mm256_set1_epi8(static_cast<int8_t>(0x80))
				)
			);
		}
		else if constexpr (true) { static_assert(false, "AVX256: Average() only available for 16 and 8-bit integers"); }
		return *this;
	}

	// Computes the mean of corresponding elements, fractional results are rounded up to the nearest integer. This function is only available for 16 and 8-bit integers
	AVX256& Average(const AVX256& operand) { return Average(operand.Data); }


	// Permute ///////////

	// Re-orders 64-bit elements using the specified order. Each argument specifies the index of the element that will be copied to that element, one element can be copied to many elements
	template<int dst0, int dst1, int dst2, int dst3>
	AVX256& Permute64()
	{
		static_assert(dst0 >= 0 && dst0 <= 3 && dst1 >= 0 && dst1 <= 3 && dst2 >= 0 && dst2 <= 3 && dst3 >= 0 && dst3 <= 3, "AVX256: Indices must be between 0 and 3 inclusive");
		constexpr uint8_t ORDER = dst0 | (dst1 << 2) | (dst2 << 4) | (dst3 << 6);

		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_permute4x64_pd(_mm256_loadu_pd(Data), ORDER));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_castpd_ps(_mm256_permute4x64_pd(_mm256_castps_pd(_mm256_loadu_ps(Data)), ORDER)));
		else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t>) _mm256_storeu_epi64(Data, _mm256_permute4x64_epi64(_mm256_loadu_epi64(Data), ORDER));
		else if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, int32_t>) _mm256_storeu_epi32(Data, _mm256_permute4x64_epi64(_mm256_loadu_epi32(Data), ORDER));
		else if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t>) _mm256_storeu_epi16(Data, _mm256_permute4x64_epi64(_mm256_loadu_epi16(Data), ORDER));
		else if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) _mm256_storeu_epi8(Data, _mm256_permute4x64_epi64(_mm256_loadu_epi8(Data), ORDER));

		return *this;
	}

	// Re-orders 32-bit the elements using the specified order. Each element in order specifies the index of the element that will be copied to that element, one element can be copied to many elements. Order indices should be between 0 and 7 inclusive
	template <typename U>
	AVX256& Permute32(const U* order)
	{
		if constexpr (!std::is_same_v<U, uint32_t> && !std::is_same_v<U, int32_t>) static_assert(false, "AVX256: order must point to 32-bit integers");

		if constexpr (std::is_same_v<T, double>) _mm256_storeu_pd(Data, _mm256_permutevar8x32_pd(_mm256_loadu_pd(Data), _mm256_loadu_epi32(order)));
		else if constexpr (std::is_same_v<T, float>) _mm256_storeu_ps(Data, _mm256_castpd_ps(_mm256_permutevar8x32_pd(_mm256_castps_pd_(mm256_loadu_ps(Data)), _mm256_loadu_epi32(order))));
		else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t>) _mm256_storeu_epi64(Data, _mm256_permutevar8x32_epi32(_mm256_loadu_epi64(Data), _mm256_loadu_epi32(order)));
		else if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, int32_t>) _mm256_storeu_epi32(Data, _mm256_permutevar8x32_epi32(_mm256_loadu_epi32(Data), _mm256_loadu_epi32(order)));
		else if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t>) _mm256_storeu_epi16(Data, _mm256_permutevar8x32_epi32(_mm256_loadu_epi16(Data), _mm256_loadu_epi32(order)));
		else if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) _mm256_storeu_epi8(Data, _mm256_permutevar8x32_epi32(_mm256_loadu_epi8(Data), _mm256_loadu_epi32(order)));
		return *this;
	}

	// Re-orders 32-bit the elements using the specified order. Each element in order specifies the index of the element that will be copied to that element, one element can be copied to many elements. Order indices should be between 0 and 7 inclusive
	template <typename U>
	AVX256& Permute32(const std::array<U, 32 / sizeof(U)>& order)
	{
		if constexpr (!std::is_same_v<U, uint32_t> && !std::is_same_v<U, int32_t>) static_assert(false, "AVX256: order must be an array of 32-bit integers");
		else if constexpr (true) return Permute32(order.data());
	}

	// Re-orders 32-bit the elements using the specified order. Each element in order specifies the index of the element that will be copied to that element, one element can be copied to many elements. Order indices should be between 0 and 7 inclusive
	template <typename U>
	AVX256& Permute32(const AVX256<U>& order)
	{
		if constexpr (!std::is_same_v<U, uint32_t> && !std::is_same_v<U, int32_t>) static_assert(false, "AVX256: order must be an AVX256<uint32_t> or AVX256<int32_t>");
		else if constexpr (true) return Permute32(order.Data);
	}


	friend void testAVX256Constructor();

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