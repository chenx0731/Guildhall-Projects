#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>

typedef unsigned char BlockIDType;
typedef uint8_t LightInfluence;
typedef uint8_t BlockFlags;

enum class BlockFlagType
{
	IS_SKY,
	IS_LIGHT_DIRTY,
	IS_FULL_OPAQUE,
	IS_SOLID,
	IS_VISIBLE,

	NUM,
};

class Block
{
public:
	Block();
	~Block();

	void SetIndoorLight(LightInfluence light);
	void SetOutdoorLight(LightInfluence light);
	void SetFlagByType(BlockFlagType type, bool flag);
	void SetFlagsByDef();
	void SetAsSky();

	LightInfluence GetIndoorLight() const;
	LightInfluence GetOutdoorLight() const;
	bool GetFlagByType(BlockFlagType type) const;
	float GetIndoorPercent() const;
	float GetOutdoorPercent() const;
	
public:
	BlockIDType m_blockID = 255;
	LightInfluence m_light = 0;
	BlockFlags m_flags = 0;
};