#pragma once
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include <vector>
#include <string>

class TileDefinition
{
public: 
	TileDefinition();
	TileDefinition(bool isSolid, Rgba8 tint, AABB2 spriteSheetUV);
	TileDefinition(bool isSolid, Rgba8 tint, Vec2 uvAtMins, Vec2 uvAtMaxs);
	TileDefinition(XmlElement const& tileDefElement);

	static void InitializeTileDefs(const char* docFilePath);
	static TileDefinition const* GetTileDef(std::string const& tileDefName);
public:
	static std::vector<TileDefinition*> s_tileDefs;

	std::string m_name;
	bool		m_isSolid = false;
	bool		m_isWater = false;
	IntVec2		m_spriteCoords;
	Rgba8		m_tintColor = Rgba8::WHITE;
	AABB2		m_spriteSheetUV = AABB2::ZERO_TO_ONE;
};
