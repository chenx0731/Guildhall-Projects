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

class Shader;
class Texture;
class SpriteSheet;

struct SpawnInfo
{
	std::string m_actor;
	Vec3		m_position;
	EulerAngles m_orientation;
	Vec3		m_velocity;
};

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
	int				m_level;
	Image			m_image;
	Shader*			m_shader;
	Texture*		m_spriteSheetTexture;
	SpriteSheet*	m_spriteSheet;
	IntVec2			m_spriteSheetCellCount;

	std::vector<SpawnInfo> m_spawnInfos;
};
