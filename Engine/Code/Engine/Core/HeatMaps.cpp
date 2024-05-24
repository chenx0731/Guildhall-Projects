#include "Engine/Core/HeatMaps.hpp"

TileHeatMap::TileHeatMap()
{
}

TileHeatMap::TileHeatMap(IntVec2 const& dimensions)
{
	m_dimension = dimensions;
	int size = m_dimension.x * m_dimension.y;
	m_values.resize(size);
	SetAllValues(0.f);
}

void TileHeatMap::SetAllValues(float value)
{
	for (int index = 0; index < m_dimension.x * m_dimension.y; index++) {
		m_values[index] = value;
	}
}

void TileHeatMap::SetValue(int coordX, int coordY, float value)
{
	int index = coordY * m_dimension.x + coordX;
	m_values[index] = value;
}

void TileHeatMap::SetValue(int index, float value)
{
	
	m_values[index] = value;
}

void TileHeatMap::AddValue(int coordX, int coordY, float value)
{
	if (coordX < m_dimension.x && coordY < m_dimension.y && coordX >= 0 && coordY >= 0) {
		int index = coordY * m_dimension.x + coordX;
		m_values[index] += value;
	}
}

void TileHeatMap::AddValue(int index, float value)
{
	m_values[index] += value;
}

float TileHeatMap::GetValue(int coordX, int coordY) const
{
	int index = coordY * m_dimension.x + coordX;
	return m_values[index];
}

float TileHeatMap::GetValue(int index) const
{
	return m_values[index];
}

float TileHeatMap::GetValue(IntVec2 tile) const
{
	int index = tile.y * m_dimension.x + tile.x;
	return m_values[index];
}

std::vector<Vec2> TileHeatMap::GeneratePath(IntVec2 start, IntVec2 goal)
{
	std::vector<Vec2> res;
	res.push_back(GetCenterPosition(start));
	IntVec2 now = start;
	IntVec2 up		= IntVec2(0, 1);
	IntVec2 down	= IntVec2(0, -1);
	IntVec2 right	= IntVec2(1, 0);
	IntVec2 left	= IntVec2(-1, 0);

	while (now != goal) {
		if (IsTileInBound(now + up) && GetValue(now + up) < GetValue(now)) {
			res.push_back(GetCenterPosition(now + up));
			now = now + up;
			continue;
		}
		if (IsTileInBound(now + down) && GetValue(now + down) < GetValue(now)) {
			res.push_back(GetCenterPosition(now + down));
			now = now + down;
			continue;
		}
		if (IsTileInBound(now + right) && GetValue(now + right) < GetValue(now)) {
			res.push_back(GetCenterPosition(now + right));
			now = now + right;
			continue;
		}
		if (IsTileInBound(now + left) && GetValue(now + left) < GetValue(now)) {
			res.push_back(GetCenterPosition(now + left));
			now = now + left;
			continue;
		}
	}
	std::vector<Vec2> result;
	for (int nodeIndex = (int)res.size() - 1; nodeIndex >= 0; nodeIndex--) {
		result.push_back(res[nodeIndex]);
	}
	return result;
}

Vec2 TileHeatMap::GetCenterPosition(IntVec2 tile)
{
	float x = static_cast<float> (tile.x);
	float y = static_cast<float> (tile.y);
	return Vec2(x + 0.5f, y + 0.5f);
}

bool TileHeatMap::IsTileInBound(IntVec2 tile)
{
	if (tile.x >= 0 && tile.x < m_dimension.x && tile.y >= 0 && tile.y < m_dimension.y)
		return true;
	return false;
}
