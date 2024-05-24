#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include <string>

class TileDefinition;

//enum TileType
//{
//	TILE_TYPE_NULL = -1,
//
//	SHORT_GRASS,
//	LONG_GRASS,
//	DARK_GRASS,
//	DIRT,
//	COBBLE,
//	PAVED,
//	STONE_WALL,
//	BRICK_WALL,
//	IRON_WALL,
//	MAP_ENTRY,
//	MAP_EXIT,
//
//	NUM_TYPES
//};

class Tile
{
public:
	TileDefinition const*	m_tileDef = nullptr;
	IntVec2					m_tileCoords;
public: 
	Rgba8 GetColor() const;
	AABB2 GetBounds() const;
	bool IsSolid() const;
	bool IsWater() const;
};
