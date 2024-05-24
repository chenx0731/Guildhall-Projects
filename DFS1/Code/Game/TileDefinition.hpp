#pragma once
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include <vector>
#include <string>

enum class SolidType {
	Null,
	Wall,
	Ceiling,
	Floor
};

class TileDefinition
{
public: 
	TileDefinition();
	TileDefinition(XmlElement const& tileDefElement);

	static void InitializeTileDefs(const char* docFilePath);
	static TileDefinition const* GetTileDef(std::string const& tileDefName);
	static TileDefinition const* GetTileDefByColor(Rgba8 const& color);
	static void ClearDefinitions();
public:
	static std::vector<TileDefinition*> s_tileDefs;

	std::string m_name;
	bool		m_isSolid = false;
	SolidType	m_solidType = SolidType::Null;
	
	Rgba8		m_mapImagePixelColor;
	IntVec2		m_floorSpriteCoords = IntVec2(-1, -1);
	IntVec2		m_ceilingSpriteCoords = IntVec2(-1, -1);
	IntVec2		m_wallSpriteCoords = IntVec2(-1, -1);
};
