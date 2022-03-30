#ifndef AVX256_H
#define AVX256_H

#include <type_traits>
#include <cstdint>
#include <initializer_list>

namespace AVX256Utils
{
	extern "C" bool HasAVX2Support(void);
};

namespace
{
	extern "C" void Add64Double(double * source, double * operand);
	extern "C" void Add32Float(float* source, float* operand);
	extern "C" void Add64(uint64_t * source, uint64_t * operand);	
	extern "C" void Add32(uint32_t * source, uint32_t * operand);
	extern "C" void Add16(uint16_t * source, uint16_t * operand);
	extern "C" void Add16SaturateSigned(int16_t * source, int16_t * operand);
	extern "C" void Add16SaturateUnsigned(uint16_t * source, uint16_t * operand);
	extern "C" void Add8(uint8_t * source, uint8_t * operand);
	extern "C" void Add8SaturateSigned(int8_t * source, int8_t * operand);
	extern "C" void Add8SaturateUnsigned(uint8_t * source, uint8_t * operand);

	extern "C" void Sub64Double(double * source, double * operand);
	extern "C" void Sub32Float(float* source, float* operand);
	extern "C" void Sub64(uint64_t * source, uint64_t * operand);	
	extern "C" void Sub32(uint32_t * source, uint32_t * operand);
	extern "C" void Sub16(uint16_t * source, uint16_t * operand);
	extern "C" void Sub16SaturateSigned(int16_t * source, int16_t * operand);
	extern "C" void Sub16SaturateUnsigned(uint16_t * source, uint16_t * operand);
	extern "C" void Sub8(uint8_t * source, uint8_t * operand);
	extern "C" void Sub8SaturateSigned(int8_t * source, int8_t * operand);
	extern "C" void Sub8SaturateUnsigned(uint8_t * source, uint8_t * operand);
}

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

	void Add(double* operand) { Add64Double(this->Data, operand); }
	void Add(float* operand) { Add32Float(this->Data, operand); }

	void Add(uint64_t* operand) { Add64(this->Data, operand); }
	void Add(int64_t* operand) { Add64(reinterpret_cast<uint64_t*>(this->Data), reinterpret_cast<uint64_t*>(operand)); }

	void Add(uint32_t* operand) { Add32(this->Data, operand); }
	void Add(int32_t* operand) { Add32(reinterpret_cast<uint32_t*>(this->Data), reinterpret_cast<uint32_t*>(operand)); }

	void Add(uint16_t* operand) { Add16(this->Data, operand); }
	void Add(int16_t* operand) { Add16(reinterpret_cast<uint16_t*>(this->Data), reinterpret_cast<uint16_t*>(operand)); }
	void AddSaturate(uint16_t* operand) { Add16SaturateUnsigned(this->Data, operand); }
	void AddSaturate(int16_t* operand) { Add16Saturate(reinterpret_cast<uint16_t*>(this->Data), reinterpret_cast<uint16_t*>(operand)); }

	void Add(uint8_t* operand) { Add8(this->Data, operand); }
	void Add(int8_t* operand) { Add8(reinterpret_cast<uint8_t*>(this->Data), reinterpret_cast<uint8_t*>(operand)); }
	void AddSaturate(uint8_t* operand) { Add8SaturateUnsigned(this->Data, operand); }
	void AddSaturate(int8_t* operand) { Add8Saturate(reinterpret_cast<uint8_t*>(this->Data), reinterpret_cast<uint8_t*>(operand)); }

	AVX256& operator+=(T* operand) { this->Add(operand); return *this; }
	void Add(std::initializer_list<T>& operand) { static_assert(operand.size()); this->Add(operand.begin()); }

	// Subtraction

	AVX256<double>& operator-=(double* operand) { Sub64Double(this->Data, operand); return *this; }
	AVX256<float>& operator-=(float* operand) { Sub32Float(this->Data, operand); return *this; }

	AVX256<uint64_t>& operator-=(uint64_t* operand) { Sub64(this->Data, operand); return *this; }
	AVX256<int64_t>& operator-=(int64_t* operand) { Sub64(reinterpret_cast<uint64_t*>(this->Data), reinterpret_cast<uint64_t*>(operand)); return *this; }

	AVX256<uint32_t>& operator-=(uint32_t* operand) { Sub32(this->Data, operand); return *this; }
	AVX256<int32_t>& operator-=(int32_t* operand) { Sub32(reinterpret_cast<uint32_t*>(this->Data), reinterpret_cast<uint32_t*>(operand)); return *this; }

	AVX256<uint16_t>& operator-=(uint16_t* operand) { Sub16SaturateUnsigned(this->Data, operand); return *this; }
	AVX256<int16_t>& operator-=(int16_t* operand) { Sub16SaturateSigned(this->Data, operand); return *this; }

	AVX256<uint8_t>& operator-=(uint8_t* operand) { Sub8SaturateUnsigned(this->Data, operand); return *this; }
	AVX256<int8_t>& operator-=(int8_t* operand) { Sub8SaturateSigned(this->Data, operand); return *this; }

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