#define UNUSED(x) (void)(x);
#include "Engine/Core/BufferWriter.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"


BufferWriter::BufferWriter(std::vector<uint8_t>& data) : m_buffer(data)
{

}

BufferWriter::~BufferWriter()
{

}

void BufferWriter::SetEndianMode(EndianMode endianMode)
{
	m_endianMode = endianMode;
	if (endianMode != GetNativeEndianness())
		m_isEndiannessReversed = true;
}

void BufferWriter::AppendChar(char data)
{
	m_buffer.push_back((uint8_t)data);
}

void BufferWriter::AppendByte(uint8_t data)
{
	m_buffer.push_back(data);
}

void BufferWriter::AppendBool(bool data)
{
	m_buffer.push_back((uint8_t)data);
}

void BufferWriter::AppendShort(short data)
{
	unsigned char* buf;
	if (m_isEndiannessReversed)
		Reverse2BytesInPlace(&data);
	buf = reinterpret_cast<unsigned char*>(&data);
	for (int i = 0; i < sizeof(short); i++) {
		m_buffer.push_back(buf[i]);
	}
}

void BufferWriter::AppendUint16(uint16_t data)
{
	/*
	char buf[sizeof(uint16_t)];
	if (m_isEndiannessReversed)
		Reverse2BytesInPlace(&data);
	memcpy(buf, &data, sizeof(uint16_t));
	for (int i = 0; i < sizeof(uint16_t); i++) {
		m_buffer.push_back(buf[i]);
	}*/
	unsigned char* buf;
	if (m_isEndiannessReversed)
		Reverse2BytesInPlace(&data);
	buf = reinterpret_cast<unsigned char*>(&data);
	for (int i = 0; i < sizeof(uint16_t); i++) {
		m_buffer.push_back(buf[i]);
	}
}

void BufferWriter::AppendUint32(uint32_t data)
{
	/*
	char buf[sizeof(uint32_t)];
	if (m_isEndiannessReversed)
		Reverse4BytesInPlace(&data);
	memcpy(buf, &data, sizeof(uint32_t));
	for (int i = 0; i < sizeof(uint32_t); i++) {
		m_buffer.push_back(buf[i]);
	}*/
	unsigned char* buf;
	if (m_isEndiannessReversed)
		Reverse4BytesInPlace(&data);
	buf = reinterpret_cast<unsigned char*>(&data);
	for (int i = 0; i < sizeof(uint32_t); i++) {
		m_buffer.push_back(buf[i]);
	}
}

void BufferWriter::AppendInt32(int data)
{
	/*
	char buf[sizeof(int)];
	if (m_isEndiannessReversed)
		Reverse4BytesInPlace(&data);
	memcpy(buf, &data, sizeof(int));
	for (int i = 0; i < sizeof(int); i++) {
		m_buffer.push_back(buf[i]);
	}*/

	unsigned char* buf;
	if (m_isEndiannessReversed)
		Reverse4BytesInPlace(&data);
	buf = reinterpret_cast<unsigned char*>(&data);
	for (int i = 0; i < sizeof(int); i++) {
		m_buffer.push_back(buf[i]);
	}
}

void BufferWriter::AppendFloat(float data)
{
	/*char buf[sizeof(float)];
	if (m_isEndiannessReversed)
		Reverse4BytesInPlace(&data);
	memcpy(buf, &data, sizeof(float));
	for (int i = 0; i < sizeof(float); i++) {
		m_buffer.push_back(buf[i]);
	}*/
	unsigned char* buf;
	if (m_isEndiannessReversed)
		Reverse4BytesInPlace(&data);
	buf = reinterpret_cast<unsigned char*>(&data);
	for (int i = 0; i < sizeof(float); i++) {
		m_buffer.push_back(buf[i]);
	}
}

void BufferWriter::AppendDouble(double data)
{
	/*
uint32_t u32;
unsigned char* start;
unsigned int* writePos = unsigned int* (start + offset);
*writePos = u32;
*/
	unsigned char* buf;
		//[sizeof(double)];
	if (m_isEndiannessReversed)
		Reverse8BytesInPlace(&data);
	//memcpy(buf, &data, sizeof(double));
	buf = reinterpret_cast<unsigned char *>(&data);
	for (int i = 0; i < sizeof(double); i++) {
		m_buffer.push_back(buf[i]);
	}
}

void BufferWriter::AppendStringZeroTerminated(std::string data)
{
	for (int i = 0; i < (int)data.size(); i++)
	{
		m_buffer.push_back((uint8_t)data[i]);
	}
	m_buffer.push_back((uint8_t)0);
}

void BufferWriter::AppendStringAfter32BitLength(std::string data)
{
	AppendUint32((uint32_t)data.size());
	for (int i = 0; i < (int)data.size(); i++)
	{
		m_buffer.push_back((uint8_t)data[i]);
	}
}

void BufferWriter::AppendRgba(Rgba8 data)
{
	AppendByte(data.r);
	AppendByte(data.g);
	AppendByte(data.b);
	AppendByte(data.a);
}

void BufferWriter::AppendRgb(Rgba8 data)
{
	AppendByte(data.r);
	AppendByte(data.g);
	AppendByte(data.b);
}

void BufferWriter::AppendIntVec2(IntVec2 data)
{
	AppendInt32(data.x);
	AppendInt32(data.y);
}

void BufferWriter::AppendVec2(Vec2 data)
{
	AppendFloat(data.x);
	AppendFloat(data.y);
}

void BufferWriter::AppendVec3(Vec3 data)
{
	AppendFloat(data.x);
	AppendFloat(data.y);
	AppendFloat(data.z);
}

void BufferWriter::AppendVertexPCU(Vertex_PCU data)
{
	AppendVec3(data.m_position);

	AppendRgba(data.m_color);

	AppendVec2(data.m_uvTexCoords);
}

void BufferWriter::AppendAABB2(AABB2 data)
{
	AppendVec2(data.m_mins);
	AppendVec2(data.m_maxs);
}

void BufferWriter::ModifyUint32ValueByLocation(uint32_t locationOffset, void* data, size_t size)
{
	//memcpy(&m_buffer[locationOffset], data, size);
	unsigned char* start = m_buffer.data();
	unsigned int* writePos = (unsigned int*) (start + locationOffset);
	*writePos = *(unsigned int*)data;
	UNUSED(size);
}

uint32_t BufferWriter::GetBufferSize() const
{
	return (uint32_t)m_buffer.size();
}

void BufferWriter::ClearBuffer()
{
	m_buffer.clear();
}

EndianMode BufferWriter::GetEndianMode() const
{
	return m_endianMode;
}
