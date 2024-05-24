#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Texture.hpp"



SpriteSheet::SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout):m_texture(texture)
{
	float uPerUnitX = 1.f / static_cast<float>(simpleGridLayout.x);
	float vPerUnitY = 1.f / static_cast<float>(simpleGridLayout.y);
	
	float uCorrection = 1.f / (static_cast<float>(texture.GetDimensions().x) * 128.f);
	float vCorrection = 1.f / (static_cast<float>(texture.GetDimensions().y) * 128.f);

	m_gridLayout = simpleGridLayout;

	int totalNum = simpleGridLayout.x * simpleGridLayout.y;
	m_spriteDefs.reserve(totalNum);
	int spriteIndex = 0;
	for (int spriteY = simpleGridLayout.y - 1; spriteY >= 0; spriteY--) {
		for (int spriteX = 0; spriteX < simpleGridLayout.x; spriteX++) {
			float uMins = static_cast<float>(spriteX) * uPerUnitX;
			float uMaxs = uMins + uPerUnitX;
			float vMins = static_cast<float>(spriteY) * vPerUnitY;
			float vMaxs = vMins + vPerUnitY;
			SpriteDefinition spDef = SpriteDefinition(*this, spriteIndex, Vec2(uMins + uCorrection, vMins + vCorrection), Vec2(uMaxs - uCorrection, vMaxs - vCorrection));
			m_spriteDefs.push_back(spDef);
			spriteIndex++;
		}
	}
}

Texture& SpriteSheet::GetTexture() const
{
	return m_texture;
}

int SpriteSheet::GetNumSprites() const
{
	return (int)m_spriteDefs.size();
}

SpriteDefinition const& SpriteSheet::GetSpriteDef(int spriteIndex) const
{
	return m_spriteDefs[spriteIndex];
}

void SpriteSheet::GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const
{
	return m_spriteDefs[spriteIndex].GetUVs(out_uvAtMins, out_uvAtMaxs);
}

AABB2 SpriteSheet::GetSpriteUVs(int spriteIndex) const
{
	return AABB2(m_spriteDefs[spriteIndex].GetUVs());
}

void SpriteSheet::GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, IntVec2 spriteCoords) const
{
	int index = GetIndexFromCoords(spriteCoords);
	GetSpriteUVs(out_uvAtMins, out_uvAtMaxs, index);
}

AABB2 SpriteSheet::GetSpriteUVs(IntVec2 spriteCoords) const
{
	int index = GetIndexFromCoords(spriteCoords);
	return GetSpriteUVs(index);
}

int SpriteSheet::GetIndexFromCoords(IntVec2 spriteCoords) const
{
	return spriteCoords.x + spriteCoords.y * m_gridLayout.x;
}
