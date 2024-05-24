#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

std::vector<TileDefinition*> TileDefinition::s_tileDefs;

TileDefinition::TileDefinition()
{
}

TileDefinition::TileDefinition(bool isSolid, Rgba8 tint, AABB2 spriteSheetUV)
{
	m_isSolid = isSolid;
	m_tintColor = tint;
	m_spriteSheetUV = spriteSheetUV;
}

TileDefinition::TileDefinition(bool isSolid, Rgba8 tint, Vec2 uvAtMins, Vec2 uvAtMaxs)
{
	m_isSolid = isSolid;
	m_tintColor = tint;
	m_spriteSheetUV = AABB2(uvAtMins, uvAtMaxs);
}

TileDefinition::TileDefinition(XmlElement const& tileDefElement)
{
	m_name			= ParseXmlAttribute(tileDefElement, "name", m_name);
	m_tintColor		= ParseXmlAttribute(tileDefElement, "tint", m_tintColor);
	m_isSolid		= ParseXmlAttribute(tileDefElement, "isSolid", m_isSolid);
	m_isWater		= ParseXmlAttribute(tileDefElement, "isWater", m_isWater);
	m_spriteCoords	= ParseXmlAttribute(tileDefElement, "spriteCoords", IntVec2(-1, -1));
	int spriteIndex = m_spriteCoords.x + m_spriteCoords.y * TERRAIN_SPRITE_WIDTH;
	m_spriteSheetUV = g_terrainSprites->GetSpriteUVs(spriteIndex);
}

void TileDefinition::InitializeTileDefs(const char* docFilePath)
{	
	XmlDocument tileDoc;
	tileDoc.LoadFile(docFilePath);
	XmlElement* rootElement = tileDoc.RootElement();
	XmlElement* tileDefElement = rootElement->FirstChildElement();
	while (tileDefElement) {
		std::string elementName = tileDefElement->Name();
		TileDefinition* newTileDef = new TileDefinition(*tileDefElement);
		s_tileDefs.push_back(newTileDef);
		tileDefElement = tileDefElement->NextSiblingElement();
	}

}

TileDefinition const* TileDefinition::GetTileDef(std::string const& tileDefName)
{
	for (int tileIndex = 0; tileIndex < s_tileDefs.size(); tileIndex++) {
		if (s_tileDefs[tileIndex]->m_name == tileDefName) {
			return s_tileDefs[tileIndex];
		}
	}
	return nullptr;
}
