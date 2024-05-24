#include "Engine/Core/BufferUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


EndianMode GetNativeEndianness()
{
	unsigned int num = 0x12345678;

	unsigned char const* memoryAddressOfNumAsByteInBuffer = reinterpret_cast<unsigned char const*>(&num);


	if (*memoryAddressOfNumAsByteInBuffer == 0x12)
	{
		return EndianMode::BIG;
	}
	else if (*memoryAddressOfNumAsByteInBuffer == 0x78) return EndianMode::LITTLE;
	return EndianMode::NATIVE;
}

void Reverse2BytesInPlace(void* ptrTo16BitWord)
{
	uint16_t num = *reinterpret_cast<uint16_t*>(ptrTo16BitWord);
	num = ((0x00ff & num) << 8) |
		((0xff00 & num) >> 8);
	*(uint16_t*)ptrTo16BitWord = num;
}

void Reverse4BytesInPlace(void* ptrTo32BitWord)
{
	uint32_t num = *reinterpret_cast<uint32_t*>(ptrTo32BitWord);
	num = ((0x000000ff & num) << 24) |
		((0x0000ff00 & num) << 8) |
		((0xff000000 & num) >> 24) |
		((0x00ff0000 & num) >> 8);
	*(uint32_t*)ptrTo32BitWord = num;
}

void Reverse8BytesInPlace(void* ptrTo64BitWord)
{
	uint64_t num = *reinterpret_cast<uint64_t*>(ptrTo64BitWord);
	num = ((0x00000000000000ff & num) << 56) |
		((0x000000000000ff00 & num) << 40) |
		((0x0000000000ff0000 & num) << 24) |
		((0x00000000ff000000 & num) << 8) |
		((0xff00000000000000 & num) >> 56) |
		((0x00ff000000000000 & num) >> 40) |
		((0x0000ff0000000000 & num) >> 24) |
		((0x000000ff00000000 & num) >> 8);
	*(uint64_t*)ptrTo64BitWord = num;
}
