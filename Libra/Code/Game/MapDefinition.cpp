#include "Game/MapDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

std::vector<MapDefinition*> MapDefinition::s_MapDefs;

MapDefinition::MapDefinition(XmlElement const& mapDefElement)
{
	m_name					= ParseXmlAttribute( mapDefElement, "name", m_name);
	m_dimensions			= ParseXmlAttribute( mapDefElement, "dimensions", m_dimensions);
	m_fillTileType			= ParseXmlAttribute( mapDefElement, "fillTileType", m_fillTileType);
	m_edgeTileType			= ParseXmlAttribute( mapDefElement, "edgeTileType", m_edgeTileType);
	m_worm1TileType			= ParseXmlAttribute( mapDefElement, "worm1TileType", m_worm1TileType);
	m_worm1Count			= ParseXmlAttribute( mapDefElement, "worm1Count", m_worm1Count);
	m_worm1MaxLength		= ParseXmlAttribute( mapDefElement, "worm1MaxLength", m_worm1MaxLength);
	m_worm2TileType			= ParseXmlAttribute( mapDefElement, "worm2TileType", m_worm2TileType);
	m_worm2Count			= ParseXmlAttribute( mapDefElement, "worm2Count", m_worm2Count);
	m_worm2MaxLength		= ParseXmlAttribute( mapDefElement, "worm2MaxLength", m_worm2MaxLength);
	m_startFloorTileType	= ParseXmlAttribute( mapDefElement, "startFloorTileType", m_startFloorTileType);
	m_startBunkerTileType	= ParseXmlAttribute( mapDefElement, "startBunkerTileType", m_startBunkerTileType);
	m_endFloorTileType		= ParseXmlAttribute( mapDefElement, "endFloorTileType", m_endFloorTileType);
	m_endBunkerTileType		= ParseXmlAttribute( mapDefElement, "endBunkerTileType", m_endBunkerTileType);
	
	m_entitySpawnCounts[ENTITY_TYPE_EVIL_LEO]		= ParseXmlAttribute( mapDefElement, "leoCount", 0);
	m_entitySpawnCounts[ENTITY_TYPE_EVIL_ARIES]		= ParseXmlAttribute( mapDefElement, "ariesCount", 0);
	m_entitySpawnCounts[ENTITY_TYPE_EVIL_SCORPIO]	= ParseXmlAttribute( mapDefElement, "scorpioCount", 0);
	m_entitySpawnCounts[ENTITY_TYPE_EVIL_CAPRICORN] = ParseXmlAttribute( mapDefElement, "capricornCount", 0);

}

void MapDefinition::InitializeMapDefs(const char* docFilePath)
{
	XmlDocument mapDefsDoc;
	mapDefsDoc.LoadFile(docFilePath);
	XmlElement* rootElement = mapDefsDoc.RootElement();
	XmlElement* mapDefElement = rootElement->FirstChildElement();
	while (mapDefElement) {
		std::string elementName = mapDefElement->Name();
		MapDefinition* newMapDef = new MapDefinition(*mapDefElement);
		s_MapDefs.push_back(newMapDef);
		mapDefElement = mapDefElement->NextSiblingElement();
	}
}

MapDefinition const* MapDefinition::GetMapDef(std::string const& mapDefName)
{
	for (int mapIndex = 0; mapIndex < s_MapDefs.size(); mapIndex++) {
		if (mapDefName == s_MapDefs[mapIndex]->m_name) {
			return s_MapDefs[mapIndex];
		}
	}
	return nullptr;
}
