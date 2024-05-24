#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/FileUtils.hpp"

std::vector<TileDefinition*> TileDefinition::s_TileDefs;

TileDefinition::TileDefinition(XmlElement& tileDefElement)
{
	m_name	   	= ParseXmlAttribute(tileDefElement, "name", m_name);
	m_symbol   	= ParseXmlAttribute(tileDefElement, "symbol", m_symbol);
	m_isBlocked	= ParseXmlAttribute(tileDefElement, "isBlocked", m_isBlocked);
}

TileDefinition::~TileDefinition()
{

}

void TileDefinition::InitializeTileDefs(const char* docFilePath)
{
	XmlDocument mapDefsDoc;
	mapDefsDoc.LoadFile(docFilePath);
	XmlElement* rootElement = mapDefsDoc.RootElement();
	XmlElement* tileDefElement = rootElement->FirstChildElement();
	while (tileDefElement) {
		std::string elementName = tileDefElement->Name();
		TileDefinition* newMapDef = new TileDefinition(*tileDefElement);
		s_TileDefs.push_back(newMapDef);
		tileDefElement = tileDefElement->NextSiblingElement();
	}
}

TileDefinition const* TileDefinition::GetTileDefByName(std::string const& tileDefName)
{
	for (int tileIndex = 0; tileIndex < s_TileDefs.size(); tileIndex++) {
		if (tileDefName == s_TileDefs[tileIndex]->m_name) {
			return s_TileDefs[tileIndex];
		}
	}
	return nullptr;
}

TileDefinition const* TileDefinition::GetTileDefBySymbol(char const& tileDefSymbol)
{
	for (int tileIndex = 0; tileIndex < s_TileDefs.size(); tileIndex++) {
		if (tileDefSymbol == s_TileDefs[tileIndex]->m_symbol) {
			return s_TileDefs[tileIndex];
		}
	}
	return nullptr;
}

void TileDefinition::ClearDefinitions()
{
	for (int defIndex = 0; defIndex < s_TileDefs.size(); defIndex++) {
		delete s_TileDefs[defIndex];
		s_TileDefs[defIndex] = nullptr;
	}
	s_TileDefs.clear();
}
