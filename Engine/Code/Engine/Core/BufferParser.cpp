#include "Engine/Core/BufferParser.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"


BufferParser::BufferParser(void* bufferstart, size_t size)
{
	m_bufferstart = reinterpret_cast<unsigned char const*>(bufferstart);
	m_size = size;
}

BufferParser::~BufferParser()
{

}

void BufferParser::SetEndianMode(EndianMode endianMode)
{
	m_endianMode = endianMode;
	if (GetNativeEndianness() != endianMode) {
		m_isEndiannessReversed = true;
	}
}

char BufferParser::ParseChar()
{
	if (m_currIndex + sizeof(char) > m_size)
	{
		ERROR_AND_DIE("Parse data(char) out of range!");
	}
	unsigned char const* memoryAddressOfCharAsByteInBuffer = &m_bufferstart[m_currIndex];
	char const* memoryAddressofCharInBuffer = reinterpret_cast<char const*>(memoryAddressOfCharAsByteInBuffer);
	m_currIndex += sizeof(char);
	return *memoryAddressofCharInBuffer;
}

unsigned char BufferParser::ParseByte()
{
	if (m_currIndex + sizeof(unsigned char) > m_size)
	{
		ERROR_AND_DIE("Parse data(byte) out of range!");
	}
	unsigned char const* memoryAddressOfByteInBuffer = &m_bufferstart[m_currIndex];
	m_currIndex += sizeof(unsigned char);
	//char const* memoryAddressofCharInBuffer = reinterpret_cast<char const*>(memoryAddressOfCharAsByteInBuffer);
	return *memoryAddressOfByteInBuffer;
}

bool BufferParser::ParseBool()
{
	if (m_currIndex + sizeof(bool) > m_size)
	{
		ERROR_AND_DIE("Parse data(bool) out of range!");
	}
	unsigned char const* memoryAddressOfBoolAsByteInBuffer = &m_bufferstart[m_currIndex];
	bool const* memoryAddressofBoolInBuffer = reinterpret_cast<bool const*>(memoryAddressOfBoolAsByteInBuffer);
	m_currIndex += sizeof(bool);
	return *memoryAddressofBoolInBuffer;
}

short BufferParser::ParseShort()
{
	if (m_currIndex + sizeof(short) > m_size)
	{
		ERROR_AND_DIE("Parse data(uint16_t) out of range!");
	}
	unsigned char const* memoryAddressOfShortAsByteInBuffer = &m_bufferstart[m_currIndex];
	short const* memoryAddressofShortInBuffer = reinterpret_cast<short const*>(memoryAddressOfShortAsByteInBuffer);
	m_currIndex += sizeof(short);
	if (m_isEndiannessReversed)
		Reverse2BytesInPlace((void*)memoryAddressofShortInBuffer);
	return *memoryAddressofShortInBuffer;
}

uint16_t BufferParser::ParseUint16()
{
	if (m_currIndex + sizeof(uint16_t) > m_size)
	{
		ERROR_AND_DIE("Parse data(uint16_t) out of range!");
	}
	unsigned char const* memoryAddressOfUint16AsByteInBuffer = &m_bufferstart[m_currIndex];
	uint16_t const* memoryAddressofUint16InBuffer = reinterpret_cast<uint16_t const*>(memoryAddressOfUint16AsByteInBuffer);
	m_currIndex += sizeof(uint16_t);
	if (m_isEndiannessReversed)
		Reverse2BytesInPlace((void*)memoryAddressofUint16InBuffer);
	return *memoryAddressofUint16InBuffer;
}

uint32_t BufferParser::ParseUint32()
{
	if (m_currIndex + sizeof(uint32_t) > m_size)
	{
		ERROR_AND_DIE("Parse data(uint32_t) out of range!");
	}
	unsigned char const* memoryAddressOfUint32AsByteInBuffer = &m_bufferstart[m_currIndex];
	uint32_t const* memoryAddressofUint32InBuffer = reinterpret_cast<uint32_t const*>(memoryAddressOfUint32AsByteInBuffer);
	m_currIndex += sizeof(uint32_t);
	if (m_isEndiannessReversed)
		Reverse4BytesInPlace((void*)memoryAddressofUint32InBuffer);
	return *memoryAddressofUint32InBuffer;
}

int BufferParser::ParseInt32()
{
	if (m_currIndex + sizeof(int) > m_size)
	{
		ERROR_AND_DIE("Parse data(int) out of range!");
	}
	unsigned char const* memoryAddressOfInt32AsByteInBuffer = &m_bufferstart[m_currIndex];
	int const* memoryAddressofInt32InBuffer = reinterpret_cast<int const*>(memoryAddressOfInt32AsByteInBuffer);
	m_currIndex += sizeof(int);
	if (m_isEndiannessReversed)
		Reverse4BytesInPlace((void*)memoryAddressofInt32InBuffer);
	return *memoryAddressofInt32InBuffer;
}

float BufferParser::ParseFloat()
{
	//unsigned char const* memoryAddressOfFloatAsByteInBuffer = &m_bufferstart[m_currIndex];
	//float const* memoryAddressofFloatInBuffer = reinterpret_cast<float const*>(memoryAddressOfFloatAsByteInBuffer);
	//return *memoryAddressofFloatInBuffer;
	if (m_currIndex + sizeof(float) > m_size)
	{
		ERROR_AND_DIE("Parse data(float) out of range!");
	}
	unsigned char const* memoryAddressOfFloatAsByteInBuffer = &m_bufferstart[m_currIndex];
	float const* memoryAddressofFloatInBuffer = reinterpret_cast<float const*>(memoryAddressOfFloatAsByteInBuffer);
	m_currIndex += sizeof(float);
	if (m_isEndiannessReversed)
		Reverse4BytesInPlace((void*)memoryAddressofFloatInBuffer);
	return *memoryAddressofFloatInBuffer;
}

double BufferParser::ParseDouble()
{
	if (m_currIndex + sizeof(double) > m_size)
	{
		ERROR_AND_DIE("Parse data(double) out of range!");
	}
	unsigned char const* memoryAddressOfDoubleAsByteInBuffer = &m_bufferstart[m_currIndex];
	double const* memoryAddressofDoubleInBuffer = reinterpret_cast<double const*>(memoryAddressOfDoubleAsByteInBuffer);
	m_currIndex += sizeof(double);
	if (m_isEndiannessReversed)
		Reverse8BytesInPlace((void*)memoryAddressofDoubleInBuffer);
	return *memoryAddressofDoubleInBuffer;
}


void BufferParser::ParseStringZeroTerminated(std::string& str)
{
	while (m_currIndex < m_size) {
		char temp = ParseChar();
		if (temp != 0)
			str += temp;
		else return;
	}
	ERROR_AND_DIE("Parse data(string) out of range!");
}


void BufferParser::ParseStringAfter32BitLength(std::string& str)
{
	unsigned int length = ParseUint32();
	for (unsigned int i = 0; i < length; i++) {
		char temp = ParseChar();
		str += temp;
	}
	return;
}

Rgba8 BufferParser::ParseRgba()
{
	Rgba8 res;
	res.r = ParseByte();
	res.g = ParseByte();
	res.b = ParseByte();
	res.a = ParseByte();
	return res;
}

Rgba8 BufferParser::ParseRgb()
{
	Rgba8 res;
	res.r = ParseByte();
	res.g = ParseByte();
	res.b = ParseByte();
	return res;
}

IntVec2 BufferParser::ParseIntVec2()
{
	IntVec2 res;
	res.x = ParseInt32();
	res.y = ParseInt32();
	return res;
}

Vec2 BufferParser::ParseVec2()
{
	Vec2 res;
	res.x = ParseFloat();
	res.y = ParseFloat();
	return res;
}

Vec3 BufferParser::ParseVec3()
{
	Vec3 res;
	res.x = ParseFloat();
	res.y = ParseFloat();
	res.z = ParseFloat();
	return res;
}

Vertex_PCU BufferParser::ParseVertexPCU()
{
	Vertex_PCU res;
	res.m_position = ParseVec3();
	res.m_color = ParseRgba();
	res.m_uvTexCoords = ParseVec2();
	return res;
}

AABB2 BufferParser::ParseAABB2()
{
	AABB2 res;
	res.m_mins = ParseVec2();
	res.m_maxs = ParseVec2();
	return res;
}

