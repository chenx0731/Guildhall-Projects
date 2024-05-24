#include "Game/UnitDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Model.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/OBJLoader.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/FileUtils.hpp"


std::vector<UnitDefinition*> UnitDefinition::s_UnitDefs;

UnitDefinition::UnitDefinition(XmlElement& unitDefElement)
{
	m_symbol				= ParseXmlAttribute(unitDefElement, "symbol", m_symbol);
	m_name					= ParseXmlAttribute(unitDefElement, "name", m_name);
	std::string modelFile	= ParseXmlAttribute(unitDefElement, "modelFilename", "");
	if (IsFileExist(modelFile)) {
		m_model = new Model(modelFile.c_str());
	}
	std::string type		= ParseXmlAttribute(unitDefElement, "type", "Tank");
	if (type == "Tank") {
		m_type = UnitType::Tank;
	}
	else if (type == "Artillery")
	{
		m_type = UnitType::Artillery;
	}
	m_groundAttackDamage	= ParseXmlAttribute(unitDefElement, "groundAttackDamage", m_groundAttackDamage);
	m_groundAttackStrength	= ParseXmlAttribute(unitDefElement, "groundAttackRangeMin", m_groundAttackStrength);
	m_groundAttackRange_Min	= ParseXmlAttribute(unitDefElement, "groundAttackRangeMin", m_groundAttackRange_Min);
	m_groundAttackRange_Max	= ParseXmlAttribute(unitDefElement, "groundAttackRangeMax", m_groundAttackRange_Max);
	m_groundDefenese		= ParseXmlAttribute(unitDefElement, "defense", m_groundDefenese);
	m_movementRange			= ParseXmlAttribute(unitDefElement, "movementRange", m_movementRange);
	m_moveSpeed				= ParseXmlAttribute(unitDefElement, "groundAttackDamage", m_moveSpeed);
	m_health               	= ParseXmlAttribute(unitDefElement, "health", m_health);
}

UnitDefinition::~UnitDefinition()
{
	delete m_model;
	m_model = nullptr;
}

void UnitDefinition::InitializeUnitDefs(const char* docFilePath)
{
	XmlDocument mapDefsDoc;
	mapDefsDoc.LoadFile(docFilePath);
	XmlElement* rootElement = mapDefsDoc.RootElement();
	XmlElement* unitDefElement = rootElement->FirstChildElement();
	while (unitDefElement) {
		std::string elementName = unitDefElement->Name();
		UnitDefinition* newMapDef = new UnitDefinition(*unitDefElement);
		s_UnitDefs.push_back(newMapDef);
		unitDefElement = unitDefElement->NextSiblingElement();
	}
}

UnitDefinition const* UnitDefinition::GetUnitDefByName(std::string const& uintDefName)
{
	for (int unitIndex = 0; unitIndex < s_UnitDefs.size(); unitIndex++) {
		if (uintDefName == s_UnitDefs[unitIndex]->m_name) {
			return s_UnitDefs[unitIndex];
		}
	}
	return nullptr;
}

UnitDefinition const* UnitDefinition::GetUnitDefBySymbol(char const& unitDefSymbol)
{
	for (int unitIndex = 0; unitIndex < s_UnitDefs.size(); unitIndex++) {
		if (unitDefSymbol == s_UnitDefs[unitIndex]->m_symbol) {
			return s_UnitDefs[unitIndex];
		}
	}
	return nullptr;
}

void UnitDefinition::ClearDefinitions()
{
	for (int defIndex = 0; defIndex < s_UnitDefs.size(); defIndex++) {
		delete s_UnitDefs[defIndex];
		s_UnitDefs[defIndex] = nullptr;
	}
	s_UnitDefs.clear();
}
