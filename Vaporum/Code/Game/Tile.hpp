#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB3.hpp"
#include <string>

class TileDefinition;

struct Node {
	IntVec2 m_coords;
	int m_distance = 9999999;
	bool m_isOpened = false;
	bool m_isClosed = false;

	bool operator>(const Node& compare) const {
		return m_distance > compare.m_distance;
	}
};


class Tile
{
public:
	TileDefinition const*	m_tileDef = nullptr;
	AABB3					m_bounds;
	Vec3					m_center;
	float					m_radius;
	IntVec2					m_tileCoords;
public: 
	AABB3 GetBounds() const;
	bool IsBlocked() const;
	bool IsPointInside(const Vec3& point) const;
	bool IsTileInsideBounds(const AABB3& bounds) const;
};
