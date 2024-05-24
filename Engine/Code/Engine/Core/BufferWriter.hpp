#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/BufferUtils.hpp"
#include <string>
#include <vector>

struct Rgba8;
struct IntVec2;
struct Vec2;
struct Vec3;
struct Vertex_PCU;
struct AABB2;

class BufferWriter
{
public:
	BufferWriter(std::vector<uint8_t>& data);
	~BufferWriter();

	void SetEndianMode(EndianMode endianMode);
	void AppendChar(char data);
	void AppendByte(uint8_t data);
	void AppendBool(bool data);
	void AppendShort(short data);
	void AppendUint16(uint16_t data);
	void AppendUint32(uint32_t data);
	void AppendInt32(int data);
	void AppendFloat(float data);
	void AppendDouble(double data);
	void AppendStringZeroTerminated(std::string data);
	void AppendStringAfter32BitLength(std::string data);
	void AppendRgba(Rgba8 data);
	void AppendRgb(Rgba8 data);
	void AppendIntVec2(IntVec2 data);
	void AppendVec2(Vec2 data);
	void AppendVec3(Vec3 data);
	void AppendVertexPCU(Vertex_PCU data);
	void AppendAABB2(AABB2 data);

	void ModifyUint32ValueByLocation(uint32_t locationOffset, void* data, size_t size);

	uint32_t GetBufferSize() const;

	void ClearBuffer();

	EndianMode GetEndianMode() const;
	bool						m_isEndiannessReversed = false;
private:
	std::vector<uint8_t>&		m_buffer;
	EndianMode					m_endianMode = EndianMode::LITTLE;
	
};
