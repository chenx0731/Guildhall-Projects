#pragma once
#include <vector>
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"
class Texture;
struct Vec2;
struct AABB2;
class SpriteSheet
{
public:
	explicit SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout);

	Texture&				GetTexture() const;
	int						GetNumSprites() const;
	SpriteDefinition const& GetSpriteDef(int spriteIndex) const;
	void					GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const;
	AABB2					GetSpriteUVs(int spriteIndex) const;
	void					GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, IntVec2 spriteCoords) const;
	AABB2					GetSpriteUVs(IntVec2 spriteCoords) const;
	int						GetIndexFromCoords(IntVec2 spriteCoords) const;

protected:
	Texture& m_texture;
	IntVec2	m_gridLayout;
	std::vector<SpriteDefinition> m_spriteDefs;
};