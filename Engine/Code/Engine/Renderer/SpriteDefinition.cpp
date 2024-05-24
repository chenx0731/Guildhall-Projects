#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
SpriteDefinition::SpriteDefinition(SpriteSheet const& spriteSheet, int spriteIndex, Vec2 const& uvAtMins, Vec2 const& uvAtMaxs) : m_spriteSheet(spriteSheet)
{
	m_spriteIndex = spriteIndex;
	m_uvAtMaxs = uvAtMaxs;
	m_uvAtMins = uvAtMins;

}

void SpriteDefinition::GetUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs) const
{
	out_uvAtMaxs = m_uvAtMaxs;
	out_uvAtMins = m_uvAtMins;
}

AABB2 SpriteDefinition::GetUVs() const
{
	return AABB2(m_uvAtMins, m_uvAtMaxs);
}

SpriteSheet const& SpriteDefinition::GetSpriteSheet() const
{
	return m_spriteSheet;
}

Texture& SpriteDefinition::GetTexture() const
{
	return m_spriteSheet.GetTexture();
}

float SpriteDefinition::GetAspect() const
{
	float width = m_uvAtMaxs.x - m_uvAtMins.x;
	float height = m_uvAtMaxs.y - m_uvAtMins.y;
	return width / height;
}
