#include "Game/Block.hpp"
#include "Game/BlockDef.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Clock.hpp"


Block::Block()
{
	
}

Block::~Block()
{
}

void Block::SetIndoorLight(LightInfluence light)
{
	//if (!GetFlagByType(BlockFlagType::IS_FULL_OPAQUE)) {
		LightInfluence outdoor = m_light >> 4;
		m_light = outdoor << 4 | light;
	//}
}

void Block::SetOutdoorLight(LightInfluence light)
{
	if (GetFlagByType(BlockFlagType::IS_FULL_OPAQUE)) {
		light = 0;
	}
	LightInfluence indoor = m_light & 15;
	m_light = light << 4 | indoor;
}

void Block::SetFlagByType(BlockFlagType type, bool flag)
{
	BlockFlags mask = ~ (1 << (BlockFlags)type);
	BlockFlags otherFlags = mask & m_flags;
	m_flags = otherFlags | (flag << (BlockFlags)type);
}

void Block::SetFlagsByDef()
{
	BlockDef* def = BlockDef::s_blockDefs[m_blockID];
	if (def != nullptr) {
		SetFlagByType(BlockFlagType::IS_FULL_OPAQUE, def->m_isOpaque);
		SetFlagByType(BlockFlagType::IS_SOLID, def->m_isSolid);
		SetFlagByType(BlockFlagType::IS_VISIBLE, def->m_isVisible);
		if (def->m_indoorLight > 0) {
			SetFlagByType(BlockFlagType::IS_LIGHT_DIRTY, true);
			//SetIndoorLight((LightInfluence)def->m_indoorLight);
		}
	}
}

void Block::SetAsSky()
{
	SetFlagByType(BlockFlagType::IS_SKY, true);
	SetOutdoorLight(15);
}

LightInfluence Block::GetIndoorLight() const
{
	
	return m_light & 15;
}

LightInfluence Block::GetOutdoorLight() const
{
	return m_light >> 4;
}

bool Block::GetFlagByType(BlockFlagType type) const
{
	BlockFlags mask = 1 << (BlockFlags)type;
	return mask & m_flags;
}

float Block::GetIndoorPercent() const
{
	return float(GetIndoorLight()) / 15.f;
}

float Block::GetOutdoorPercent() const
{
	return float(GetOutdoorLight()) / 15.f;
}
