#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>
#include <string>

enum class BlockType
{
	Air,
	Stone,
	Dirt,
	Grass,
	Coal,
	Iron,
	Gold,
	Diamond,
	Water,
	CobbleStone,

	Num
};
class BlockDef
{
public:
	BlockDef();
	~BlockDef();
	static void InitializeBlockDefs();
	static void CreateNewBlockDef(std::string name, bool isVisible, bool isSolid, bool isOpaque, IntVec2 topSprite, IntVec2 sidesSprite, IntVec2 bottomSprite, Rgba8 voxelColor = Rgba8::BLACK, int indoorLight = 0, int outdoorLight = 0);
	static unsigned char GetBlockDefByName(std::string name);
	static unsigned char GetBlockDefByColor(Rgba8 color);
public:
	static std::vector<BlockDef*> s_blockDefs;
	std::string m_name;
	bool		m_isVisible;
	bool		m_isSolid;
	bool		m_isOpaque;
	IntVec2		m_topSprite;
	IntVec2		m_sidesSprite;
	IntVec2		m_bottomSprite;
	int			m_indoorLight;
	int			m_outdoorLight;
	Rgba8		m_mapVoxelColor;
};