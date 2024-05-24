#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <vector>
#include <string>
#include <map>

class Tile;
class Unit;
class Shader;

class MapDefinition
{
public: 
	MapDefinition(XmlElement& mapDefElement);
	~MapDefinition();
	static void InitializeMapDefs(const char* docFilePath);
	static MapDefinition const* GetMapDef(std::string const& mapDefName);
	static void ClearDefinitions();
public:
	static std::vector<MapDefinition*> s_MapDefs;

	std::string		m_name;
	Shader*			m_overlayShader;
	IntVec2			m_gridSize = IntVec2();
	AABB3			m_worldBounds = AABB3();
	std::vector<Tile*> m_tiles;
	std::vector<Unit*> m_unitsP1;
	std::vector<Unit*> m_unitsP2;
	//Tile**			m_tiles = nullptr;
};
