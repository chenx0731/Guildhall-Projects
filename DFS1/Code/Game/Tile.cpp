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

AABB3 Tile::GetCollisionBounds() const
{
	if (m_tileDef->m_solidType == SolidType::Wall)
		return GetBounds();
	AABB3 bounds = m_bounds;
	if (m_tileDef->m_solidType == SolidType::Ceiling) {
		bounds.m_mins.z = bounds.m_maxs.z;
		bounds.m_maxs.z += 0.5f;
		return bounds;
	}
		
	if (m_tileDef->m_solidType == SolidType::Floor) {
		bounds.m_maxs.z = bounds.m_mins.z;
		bounds.m_mins.z -= 0.5f;
		return bounds;
	}
	return AABB3();
}


