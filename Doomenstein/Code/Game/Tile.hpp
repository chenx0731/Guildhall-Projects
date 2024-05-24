#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB3.hpp"
#include <string>

class TileDefinition;

class Tile
{
public:
	TileDefinition const*	m_tileDef = nullptr;
	AABB3					m_bounds;
	//IntVec2					m_tileCoords;
public: 
	AABB3 GetBounds() const;
	bool IsSolid() const;
};
