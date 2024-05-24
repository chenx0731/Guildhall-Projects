#include "Game/MapDefinition.hpp"
#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Unit.hpp"
#include "Game/UnitDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/FileUtils.hpp"

std::vector<MapDefinition*> MapDefinition::s_MapDefs;

MapDefinition::MapDefinition(XmlElement& mapDefElement)
{
	m_name					= ParseXmlAttribute(mapDefElement, "name", m_name);
	m_gridSize				= ParseXmlAttribute(mapDefElement, "gridSize", m_gridSize);
	Vec3 worldBoundsMin		= ParseXmlAttribute(mapDefElement, "worldBoundsMin", Vec3());
	Vec3 worldBoundsMax		= ParseXmlAttribute(mapDefElement, "worldBoundsMax", Vec3());
	m_worldBounds			= AABB3(worldBoundsMin, worldBoundsMax);
	//std::string shaderPath	= ParseXmlAttribute(mapDefElement, "shader", "Default");
	//shaderPath				= shaderPath;
	//m_overlayShader			= g_theRenderer->CreateShader(shaderPath.c_str());
	const int gridX = m_gridSize.x;
	const int gridY = m_gridSize.y;
	float radius = 0.577f;
		//m_worldBounds.GetDimensions().x / (float(gridX) * 2.f);
	//m_tiles = new Tile * [gridX];
	//for (int i = 0; i < gridX; i++) {
	//	m_tiles[i] = new Tile[gridY];
	//}
	//m_tiles[gridY]	= new Tile[gridX][gridY];
	//Tile tile = m_tiles[1][0];
	XmlElement* tileElement = mapDefElement.FirstChildElement();
	while (tileElement) {

		std::string tiles = tileElement->GetText();
		Strings tilesByLineOrg = SplitStringOnDelimiter(tiles, "\n");
		Strings tilesByLine;
		for (int i = 0; i < (int)tilesByLineOrg.size(); i++) {
			if (tilesByLineOrg[i].size() > 0) {
				tilesByLine.push_back(tilesByLineOrg[i]);
			}
		}
		// change the gridSize.x.y to be size of strings
		for (int yIndex = 0; yIndex < m_gridSize.y; yIndex++) {
			Strings tilesInRow = SplitStringOnDelimiter(tilesByLine[yIndex], " ");
			for (int xIndex = 0; xIndex < m_gridSize.x; xIndex++) {
				std::string type = tileElement->Name();
				if (type == "Tiles") {
					char tileSymbol = tilesInRow[xIndex][0];
					Tile* tile = new Tile();
					tile->m_tileDef = TileDefinition::GetTileDefBySymbol(tileSymbol);
					//if (tile->m_tileDef == nullptr)
					tile->m_center = Vec3(0.866f * float(xIndex), 0.5f * float(xIndex) + float(gridY - yIndex - 1), 0.01f) * radius * 1.732f;
					tile->m_center.z = 0.01f;
					tile->m_radius = radius;
					tile->m_tileCoords = IntVec2(xIndex, gridY - yIndex - 1);
					if (tile->IsTileInsideBounds(m_worldBounds))
						m_tiles.push_back(tile);
				}
				else if (type == "Units") {
					char unitSymbol = tilesInRow[xIndex][0];
					Unit* unit = new Unit();
					unit->m_unitDef = UnitDefinition::GetUnitDefBySymbol(unitSymbol);
					if (unit->m_unitDef != nullptr) {
						unit->m_position = Vec3(0.866f * float(xIndex), 0.5f * float(xIndex) + float(gridY - yIndex - 1), 0.01f) * radius * 1.732f;
						unit->m_position.z = 0.01f;
						unit->m_currentHealth = unit->m_unitDef->m_health;
						unit->m_coords = IntVec2(xIndex, gridY - yIndex - 1);
						int player = ParseXmlAttribute(*tileElement, "player", 0);
						if (player == 1) {
							unit->m_color = Rgba8(0, 100, 200);
							m_unitsP1.push_back(unit);
						}
						if (player == 2) {
							unit->m_color = Rgba8(200, 100, 0);
							m_unitsP2.push_back(unit);
						}
					}
				}
				//m_tiles[xIndex][gridY - 1 - yIndex].m_tileDef = TileDefinition::GetTileDefBySymbol(tileSymbol);
			}
		}
		tileElement = tileElement->NextSiblingElement();
	}

}

MapDefinition::~MapDefinition()
{
	m_overlayShader = nullptr;
	for (int unitIndex = 0; unitIndex < (int)m_unitsP1.size(); unitIndex++) {
		if (m_unitsP1[unitIndex]) {
			delete m_unitsP1[unitIndex];
			m_unitsP1[unitIndex] = nullptr;
		}
	}
	for (int unitIndex = 0; unitIndex < (int)m_unitsP2.size(); unitIndex++) {
		if (m_unitsP2[unitIndex]) {
			delete m_unitsP2[unitIndex];
			m_unitsP2[unitIndex] = nullptr;
		}
	}
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

void MapDefinition::ClearDefinitions()
{
	for (int defIndex = 0; defIndex < s_MapDefs.size(); defIndex++) {
		delete s_MapDefs[defIndex];
		s_MapDefs[defIndex] = nullptr;
	}
	s_MapDefs.clear();
}
