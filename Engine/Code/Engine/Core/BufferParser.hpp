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

class BufferParser
{
public:
	BufferParser(void* bufferstart, size_t size);
	~BufferParser();

	void		SetEndianMode(EndianMode endianMode);
	char		ParseChar();
	uint8_t		ParseByte();
	bool		ParseBool();
	short		ParseShort();
	uint16_t	ParseUint16();
	uint32_t	ParseUint32();
	int			ParseInt32();
	float		ParseFloat();
	double		ParseDouble();
	void		ParseStringZeroTerminated(std::string& str);
	void		ParseStringAfter32BitLength(std::string& str);
	Rgba8		ParseRgba();
	Rgba8		ParseRgb();
	IntVec2		ParseIntVec2();
	Vec2		ParseVec2();
	Vec3		ParseVec3();
	Vertex_PCU	ParseVertexPCU();
	AABB2		ParseAABB2();

private:
	//std::vector<uint8_t>	m_buffer;
	EndianMode				m_endianMode = EndianMode::LITTLE;
	//int						m_currIndex;
	unsigned char const*	m_bufferstart;
	size_t					m_size;
	size_t					m_currIndex = 0;
	bool					m_isEndiannessReversed = false;
};

