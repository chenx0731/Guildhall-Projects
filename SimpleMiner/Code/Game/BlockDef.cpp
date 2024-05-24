#include "Game/BlockDef.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Clock.hpp"

std::vector<BlockDef*> BlockDef::s_blockDefs;
BlockDef::BlockDef()
{

}

BlockDef::~BlockDef()
{
}

void BlockDef::InitializeBlockDefs()
{
	CreateNewBlockDef("air",				false,  false,  false,  IntVec2(0,0),	IntVec2(0,0),	IntVec2(0,0), Rgba8::WHITE);
	CreateNewBlockDef("stone",				true,	true,	true,	IntVec2(33,32),	IntVec2(33,32),	IntVec2(33,32));
	CreateNewBlockDef("dirt",				true,	true,	true,	IntVec2(32,34),	IntVec2(32,34),	IntVec2(32,34));
	CreateNewBlockDef("grass",				true,	true,	true,	IntVec2(32,33),	IntVec2(33,33),	IntVec2(32,33));
	CreateNewBlockDef("coal",				true,	true,	true,	IntVec2(63,34),	IntVec2(63,34),	IntVec2(63,34));
	CreateNewBlockDef("iron",				true,	true,	true,	IntVec2(63,35),	IntVec2(63,35),	IntVec2(63,35));
	CreateNewBlockDef("gold",				true,	true,	true,	IntVec2(63,36),	IntVec2(63,36),	IntVec2(63,36));
	CreateNewBlockDef("diamond",			true,	true,	true,	IntVec2(63,37),	IntVec2(63,37),	IntVec2(63,37));
	CreateNewBlockDef("water",				true,	true,	true,	IntVec2(32,44),	IntVec2(32,44),	IntVec2(32,44));
	CreateNewBlockDef("cobbleStone",		true,	true,	true,	IntVec2(34,32),	IntVec2(34,32), IntVec2(34,32));
	CreateNewBlockDef("glowStone",			true,	true,	true,	IntVec2(36,36),	IntVec2(36,36), IntVec2(36,36), Rgba8::YELLOW, 15);
	CreateNewBlockDef("ice",				true,	true,	true,	IntVec2(45,34),	IntVec2(45,34), IntVec2(45,34));
	CreateNewBlockDef("sand",				true,	true,	true,	IntVec2(34,34),	IntVec2(34,34), IntVec2(34,34));
	CreateNewBlockDef("oakLog",				true,	true,	true,	IntVec2(34,33),	IntVec2(34,33), IntVec2(34,33));
	CreateNewBlockDef("oakLeaf",			true,	true,	true,	IntVec2(34,35),	IntVec2(34,35), IntVec2(34,35));
	CreateNewBlockDef("cactus",				true,	true,	true,	IntVec2(37,36),	IntVec2(37,36), IntVec2(37,36));
	CreateNewBlockDef("spruceLog",			true,	true,	true,	IntVec2(35,33),	IntVec2(35,33), IntVec2(35,33));
	CreateNewBlockDef("spruceLeaf",			true,	true,	true,	IntVec2(32,35),	IntVec2(32,35), IntVec2(32,35));
	CreateNewBlockDef("wood",				true,	true,	true,	IntVec2(39,33),	IntVec2(39,33), IntVec2(39,33), Rgba8::RED);
	CreateNewBlockDef("pinkBrick",			true,	true,	true,	IntVec2(22,23),	IntVec2(22,23), IntVec2(22,23), Rgba8(255, 0, 255));
}

void BlockDef::CreateNewBlockDef(std::string name, bool isVisible, bool isSolid, bool isOpaque, IntVec2 topSprite, IntVec2 sidesSprite, IntVec2 bottomSprite, Rgba8 voxelColor, int indoorLight, int outdoorLight)
{
	BlockDef* newDef = new BlockDef();
	newDef->m_name = name;
	newDef->m_isVisible = isVisible;
	newDef->m_isSolid = isSolid;
	newDef->m_isOpaque = isOpaque;
	newDef->m_topSprite = topSprite;
	newDef->m_sidesSprite = sidesSprite;
	newDef->m_bottomSprite = bottomSprite;
	newDef->m_mapVoxelColor = voxelColor;
	newDef->m_indoorLight = indoorLight;
	newDef->m_outdoorLight = outdoorLight;
	s_blockDefs.push_back(newDef);
}

unsigned char BlockDef::GetBlockDefByName(std::string name)
{
	for (int defIndex = 0; defIndex < (int)s_blockDefs.size(); defIndex++) {
		BlockDef* const& blockDef = s_blockDefs[defIndex];
		if (blockDef != nullptr) {
			//if (blockDef->m_name == name ) {
			if (_strcmpi(blockDef->m_name.c_str(), name.c_str()) == 0) {
				return unsigned char(defIndex);
			}
		}
	}
	//if (name == "air")
	//	return 0;
	//if (name == "stone")
	//	return 1;
	//if (name == "dirt")
	//	return 2;
	//if (name == "grass")
	//	return 3;
	//if (name == "coal")
	//	return 4;
	//if (name == "iron")
	//	return 5;
	//if (name == "gold")
	//	return 6;
	//if (name == "diamond")
	//	return 7;
	//if (name == "water")
	//	return 8;
	//if (name == "cobbleStone")
	//	return 9;

	return 0;
}

unsigned char BlockDef::GetBlockDefByColor(Rgba8 color)
{
	for (int defIndex = 0; defIndex < (int)s_blockDefs.size(); defIndex++) {
		BlockDef* const& blockDef = s_blockDefs[defIndex];
		if (blockDef != nullptr) {
			//if (blockDef->m_name == name ) {
			if (color == blockDef->m_mapVoxelColor) {
 				return unsigned char(defIndex);
			}
		}
	}
	return 0;
}


