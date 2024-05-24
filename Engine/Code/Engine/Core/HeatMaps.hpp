#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>
class TileHeatMap
{
public:
	TileHeatMap();
	TileHeatMap(IntVec2 const& dimensions);

	void SetAllValues(float value);
	void SetValue(int coordX, int coordY, float value);
	void SetValue(int index, float value);
	void AddValue(int coordX, int coordY, float value);
	void AddValue(int index, float value);

	float GetValue(int coordX, int coordY) const;
	float GetValue(int index) const;
	float GetValue(IntVec2 tile) const;
	std::vector<Vec2> GeneratePath(IntVec2 start, IntVec2 goal);
	Vec2 GetCenterPosition(IntVec2 tile);
	bool IsTileInBound(IntVec2 tile);

public:
	IntVec2 m_dimension;
	std::vector<float> m_values;
	float	m_maxValue;
};