#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/Vec2.hpp"


AABB3 Tile::GetBounds() const
{
	return m_bounds;
}

bool Tile::IsSolid() const
{
	return m_tileDef->m_isSolid;
}


