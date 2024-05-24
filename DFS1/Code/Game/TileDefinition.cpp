#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

std::vector<TileDefinition*> TileDefinition::s_tileDefs;

TileDefinition::TileDefinition()
{
}


TileDefinition::TileDefinition(XmlElement const& tileDefElement)
{
	m_name					= ParseXmlAttribute(tileDefElement, "name", m_name);
	std::string	solidType	= ParseXmlAttribute(tileDefElement, "solidType", "null");
	if (solidType == "null") {
		m_solidType = SolidType::Null;
	}
	else if (solidType == "wall") {
		m_solidType = SolidType::Wall;
	}
	else if (solidType == "ceiling") {
		m_solidType = SolidType::Ceiling;
	}
	else if (solidType == "floor") {
		m_solidType = SolidType::Floor;
	}
	//m_isSolid				= ParseXmlAttribute(tileDefElement, "isSolid", m_isSolid);
	m_mapImagePixelColor	= ParseXmlAttribute(tileDefElement, "mapImagePixelColor", m_mapImagePixelColor);
	m_floorSpriteCoords		= ParseXmlAttribute(tileDefElement, "floorSpriteCoords", m_floorSpriteCoords);
	m_wallSpriteCoords		= ParseXmlAttribute(tileDefElement, "wallSpriteCoords", m_wallSpriteCoords);
	m_ceilingSpriteCoords	= ParseXmlAttribute(tileDefElement, "ceilingSpriteCoords", m_ceilingSpriteCoords);
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

TileDefinition const* TileDefinition::GetTileDefByColor(Rgba8 const& color)
{
	for (int defIndex = 0; defIndex < (int)s_tileDefs.size(); defIndex++) {
		TileDefinition* const& tileDef = s_tileDefs[defIndex];
		if (tileDef) {
			if (tileDef->m_mapImagePixelColor == color)
				return tileDef;
		}
	}
	return nullptr;
}

void TileDefinition::ClearDefinitions()
{
	for (int defIndex = 0; defIndex < (int)s_tileDefs.size(); defIndex++) {
		delete s_tileDefs[defIndex];
		s_tileDefs[defIndex] = nullptr;
	}
}
