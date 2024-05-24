#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <string>
#include <vector>

enum class EndianMode
{
	NATIVE,
	LITTLE,
	BIG,
};

EndianMode GetNativeEndianness();

void Reverse2BytesInPlace(void* ptrTo16BitWord);
void Reverse4BytesInPlace(void* ptrTo32BitWord);
void Reverse8BytesInPlace(void* ptrTo64BitWord);

