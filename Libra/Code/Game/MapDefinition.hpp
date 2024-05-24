#pragma once
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include <vector>
#include <string>

class MapDefinition
{
public: 
	MapDefinition(XmlElement const& mapDefElement);

	static void InitializeMapDefs(const char* docFilePath);
	static MapDefinition const* GetMapDef(std::string const& mapDefName);
public:
	static std::vector<MapDefinition*> s_MapDefs;

	std::string m_name;
	IntVec2		m_dimensions = IntVec2(-1, -1);
	std::string m_fillTileType;
	std::string m_edgeTileType;
	std::string m_worm1TileType;
	int			m_worm1Count = 0;
	int			m_worm1MaxLength = 0;
	std::string m_worm2TileType;
	int			m_worm2Count = 0;
	int			m_worm2MaxLength = 0;
	std::string	m_startFloorTileType;
	std::string	m_startBunkerTileType;
	std::string	m_endFloorTileType;
	std::string	m_endBunkerTileType;
	int			m_entitySpawnCounts[20];
};
