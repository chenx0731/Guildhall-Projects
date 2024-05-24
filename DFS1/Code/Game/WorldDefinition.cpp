#include "Game/WorldDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/XmlUtils.hpp"

std::vector<WorldDefinition*> WorldDefinition::s_WorldDefs;

WorldDefinition::WorldDefinition(XmlElement& mapDefElement)
{
	m_name					= ParseXmlAttribute(mapDefElement, "name", m_name);
	std::string imagePath	= ParseXmlAttribute(mapDefElement, "image", ""); // should change to default map?
	m_image					= Image(imagePath.c_str());
	std::string shaderPath	= ParseXmlAttribute(mapDefElement, "shader", "Default");
	shaderPath				= shaderPath;
	m_shader				= g_theRenderer->CreateShader(shaderPath.c_str());
	std::string sheetPath	= ParseXmlAttribute(mapDefElement, "spriteSheetTexture", "");
	m_level					= ParseXmlAttribute(mapDefElement, "level", 0);
	m_world					= ParseXmlAttribute(mapDefElement, "world", "");
	m_spriteSheetTexture	= g_theRenderer->CreateOrGetTextureFromFile(sheetPath.c_str());
	m_spriteSheetCellCount	= ParseXmlAttribute(mapDefElement, "spriteSheetCellCount", IntVec2(8, 8));

	
	XmlElement* spawnDefElement = mapDefElement.FirstChildElement();
	if (spawnDefElement != nullptr) {
		spawnDefElement = spawnDefElement->FirstChildElement();
		while (spawnDefElement)
		{
			SpawnInfo spawnInfo;
			spawnInfo.m_actor = ParseXmlAttribute(*spawnDefElement, "actor", "");
			std::string pos = ParseXmlAttribute(*spawnDefElement, "position", "0, 0, 0");
			std::string orientation = ParseXmlAttribute(*spawnDefElement, "orientation", "0, 0, 0");
			std::string velocity = ParseXmlAttribute(*spawnDefElement, "velocity", "0, 0, 0");
			spawnInfo.m_position.SetFromText(pos.c_str(), ",");
			spawnInfo.m_orientation.SetFromText(orientation.c_str(), ",");
			spawnInfo.m_velocity.SetFromText(velocity.c_str(), ",");
			m_spawnInfos.push_back(spawnInfo);

			spawnDefElement = spawnDefElement->NextSiblingElement();

		}
	}
	m_spriteSheet			= new SpriteSheet(*m_spriteSheetTexture, m_spriteSheetCellCount);
}

WorldDefinition::~WorldDefinition()
{
	delete m_spriteSheet;

	m_spriteSheet = nullptr;
	m_shader = nullptr;
	m_spriteSheetTexture = nullptr;
}

void WorldDefinition::InitializeWorldDefs(const char* docFilePath)
{
	XmlDocument mapDefsDoc;
	mapDefsDoc.LoadFile(docFilePath);
	XmlElement* rootElement = mapDefsDoc.RootElement();
	XmlElement* mapDefElement = rootElement->FirstChildElement();
	while (mapDefElement) {
		std::string elementName = mapDefElement->Name();
		WorldDefinition* newMapDef = new WorldDefinition(*mapDefElement);
		s_WorldDefs.push_back(newMapDef);
		mapDefElement = mapDefElement->NextSiblingElement();
	}
}

WorldDefinition const* WorldDefinition::GetWorldDef(std::string const& worldDefName)
{
	for (int worldIndex = 0; worldIndex < s_WorldDefs.size(); worldIndex++) {
		if (worldDefName == s_WorldDefs[worldIndex]->m_name) {
			return s_WorldDefs[worldIndex];
		}
	}
	return nullptr;
}

void WorldDefinition::ClearDefinitions()
{
	for (int defIndex = 0; defIndex < s_WorldDefs.size(); defIndex++) {
		delete s_WorldDefs[defIndex];
		s_WorldDefs[defIndex] = nullptr;
	}
}
