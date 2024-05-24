#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"

AABB3 Tile::GetBounds() const
{
	return m_bounds;
}

bool Tile::IsBlocked() const
{
	return m_tileDef->m_isBlocked;
}

bool Tile::IsPointInside(const Vec3& point) const
{
	Vec3 verts[6];
	Vec3 i = Vec3(1.f, 0.f, 0.f) * m_radius;
	Vec3 j = Vec3(0.5f, 0.866f, 0.f) * m_radius;
	Vec3 k = Vec3(-0.5f, 0.866f, 0.f) * m_radius;
	verts[0] = m_center + i;
	verts[1] = m_center + j;
	verts[2] = m_center + k;
	verts[3] = m_center - i;
	verts[4] = m_center - j;
	verts[5] = m_center - k;
	bool result = true;
	for (int index = 0; index < 6; index++) {
		Vec3 edge = verts[(index + 1) % 6] - verts[index];
		Vec3 T = point - verts[index];
		Vec3 N = CrossProduct3D(edge, T);
		if (N.z <= 0.f) {
			result = false;
			break;
		}
	}
	return result;
}

bool Tile::IsTileInsideBounds(const AABB3& bounds) const
{
	if (bounds.IsPointInside(m_center)) {
		return true;
	}
	return false;
}



