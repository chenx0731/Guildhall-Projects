#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/Vec2.hpp"


Rgba8 Tile::GetColor() const
{
	return m_tileDef->m_tintColor;
}

AABB2 Tile::GetBounds() const
{
	Vec2 mins = Vec2(static_cast<float>(m_tileCoords.x), static_cast<float>(m_tileCoords.y));
	Vec2 maxs = mins + Vec2(1.f, 1.f);
	return AABB2(mins, maxs);
}

bool Tile::IsSolid() const
{
	return m_tileDef->m_isSolid;
}

bool Tile::IsWater() const
{
	return m_tileDef->m_isWater;
}

