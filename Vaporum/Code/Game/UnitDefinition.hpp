#pragma once
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <vector>
#include <string>
#include <map>

class Model;

enum class UnitType
{
	Tank,
	Artillery
};

class UnitDefinition
{
public: 
	UnitDefinition(XmlElement& unitDefElement);
	~UnitDefinition();
	static void InitializeUnitDefs(const char* docFilePath);
	static UnitDefinition const* GetUnitDefByName(std::string const& unitDefName);
	static UnitDefinition const* GetUnitDefBySymbol(char const& unitDefSymbol);
	static void ClearDefinitions();
public:
	static std::vector<UnitDefinition*> s_UnitDefs;

	char			m_symbol;
	std::string		m_name;
	Model*			m_model = nullptr;
	UnitType		m_type;

	float			m_groundAttackDamage = 0.f;
	float			m_groundAttackStrength = 0.f;
	float			m_groundAttackRange_Min = 0.f;
	float			m_groundAttackRange_Max = 0.f;
	float			m_movementRange = 0.f;
	float			m_groundDefenese = 0.f;
	float			m_moveSpeed = 0.f;
	float			m_health = 0.f;
};
