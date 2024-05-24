#include "Game/BlockIterator.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Chunk.hpp"
#include "Game/Block.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/MathUtils.hpp"

BlockIterator::BlockIterator()
{
	
}
BlockIterator::BlockIterator(Chunk* chunk, int blockIndex)
{
	m_chunk = chunk;
	m_blockIndex = blockIndex;
}

BlockIterator::~BlockIterator()
{
}

bool BlockIterator::IsEmpty() const
{
	if (m_chunk == nullptr && m_blockIndex != -1)
		return true;
	return false;
}

Block* BlockIterator::GetBlock() const
{
	if (!IsEmpty()) {
		return &m_chunk->m_blocks[m_blockIndex];
	}
	return nullptr;
}

Vec3 BlockIterator::GetWorldCenter() const
{
	return m_chunk->GetWorldBoundsByIndex(m_blockIndex).GetCenter();
}

bool BlockIterator::GetFaceWorldBounds(FaceDirt face, Vec3& bottomleft, Vec3& bottomRight, Vec3& topLeft, Vec3& topRight) const
{
	if (IsEmpty()) {
		return false;
	}
	AABB3 bounds = m_chunk->GetWorldBoundsByIndex(m_blockIndex);
	IntVec3 coords = m_chunk->GetBlockCoordsByIndex(m_blockIndex);
	float minX = bounds.m_mins.x;
	float minY = bounds.m_mins.y;
	float minZ = bounds.m_mins.z;
	float maxX = bounds.m_maxs.x;
	float maxY = bounds.m_maxs.y;
	float maxZ = bounds.m_maxs.z;
	if (face == FaceDirt::EAST) {
		bottomleft	= Vec3(maxX, minY, minZ);
		bottomRight = Vec3(maxX, maxY, minZ);
		topLeft		= Vec3(maxX, minY, maxZ);
		topRight	= Vec3(maxX, maxY, maxZ);
	}
	if (face == FaceDirt::WEST) {
		bottomleft	= Vec3(minX, maxY, minZ);
		bottomRight = Vec3(minX, minY, minZ);
		topLeft		= Vec3(minX, maxY, maxZ);
		topRight	= Vec3(minX, minY, maxZ);
	}
	if (face == FaceDirt::NORTH) {
		bottomleft	= Vec3(maxX, maxY, minZ);
		bottomRight = Vec3(minX, maxY, minZ);
		topLeft		= Vec3(maxX, maxY, maxZ);
		topRight	= Vec3(minX, maxY, maxZ);
	}
	if (face == FaceDirt::SOUTH) {
		bottomleft	= Vec3(minX, minY, minZ);
		bottomRight = Vec3(maxX, minY, minZ);
		topLeft		= Vec3(minX, minY, maxZ);
		topRight	= Vec3(maxX, minY, maxZ);
	}
	if (face == FaceDirt::UP) {
		bottomleft	= Vec3(maxX, minY, maxZ);
		bottomRight = Vec3(maxX, maxY, maxZ);
		topLeft		= Vec3(minX, minY, maxZ);
		topRight	= Vec3(minX, maxY, maxZ);
	}
	if (face == FaceDirt::DOWN) {
		bottomleft	= Vec3(maxX, minY, maxZ);
		bottomRight = Vec3(maxX, maxY, maxZ);
		topLeft		= Vec3(minX, minY, maxZ);
		topRight	= Vec3(minX, maxY, maxZ);
	}
	
	return true;
}

Rgba8  BlockIterator::GetNeighborLight(FaceDirt dirt) const
{
	Rgba8 color;
	BlockIterator neighbor;
	if (dirt == FaceDirt::EAST) {
		neighbor = GetEastNeighbor();
		if (!neighbor.IsEmpty()) {
			color.r = DenormalizeByte(neighbor.GetBlock()->GetOutdoorPercent());
			color.g = DenormalizeByte(neighbor.GetBlock()->GetIndoorPercent());
		}
	}
	if (dirt == FaceDirt::WEST) {
		neighbor = GetWestNeighbor();
		if (!neighbor.IsEmpty()) {
			color.r = DenormalizeByte(neighbor.GetBlock()->GetOutdoorPercent());
			color.g = DenormalizeByte(neighbor.GetBlock()->GetIndoorPercent());
		}
	}
	if (dirt == FaceDirt::NORTH) {
		neighbor = GetNorthNeighbor();
		if (!neighbor.IsEmpty()) {
			color.r = DenormalizeByte(neighbor.GetBlock()->GetOutdoorPercent());
			color.g = DenormalizeByte(neighbor.GetBlock()->GetIndoorPercent());
		}
	}
	if (dirt == FaceDirt::SOUTH) {
		neighbor = GetSouthNeighbor();
		if (!neighbor.IsEmpty()) {
			color.r = DenormalizeByte(neighbor.GetBlock()->GetOutdoorPercent());
			color.g = DenormalizeByte(neighbor.GetBlock()->GetIndoorPercent());
		}
	}
	if (dirt == FaceDirt::UP) {
		neighbor = GetUpNeighbor();
		if (!neighbor.IsEmpty()) {
			color.r = DenormalizeByte(neighbor.GetBlock()->GetOutdoorPercent());
			color.g = DenormalizeByte(neighbor.GetBlock()->GetIndoorPercent());
		}
	}
	if (dirt == FaceDirt::DOWN) {
		neighbor = GetDownNeighbor();
		if (!neighbor.IsEmpty()) {
			color.r = DenormalizeByte(neighbor.GetBlock()->GetOutdoorPercent());
			color.g = DenormalizeByte(neighbor.GetBlock()->GetIndoorPercent());
		}
	}
	return color;
}

BlockIterator BlockIterator::GetEastNeighbor() const
{
	IntVec3 coord = m_chunk->GetBlockCoordsByIndex(m_blockIndex);
	IntVec3 newCoord = coord;
	newCoord.x++;
	bool isInBounds = m_chunk->IsCoordsInBounds(newCoord);
	int newIndex = -1;
	if (isInBounds) {
		newIndex = m_chunk->GetBlockIndexByCoords(newCoord);
		return BlockIterator(m_chunk, newIndex);
	}
	else
	{
		newCoord.x = 0;
		if (m_chunk->m_eastNeighbor != nullptr) {
			newIndex = m_chunk->m_eastNeighbor->GetBlockIndexByCoords(newCoord);
			return BlockIterator(m_chunk->m_eastNeighbor, newIndex);
		}
	}
	return BlockIterator();
}

BlockIterator BlockIterator::GetWestNeighbor() const
{
	IntVec3 coord = m_chunk->GetBlockCoordsByIndex(m_blockIndex);
	IntVec3 newCoord = coord;
	newCoord.x--;
	bool isInBounds = m_chunk->IsCoordsInBounds(newCoord);
	int newIndex = -1;
	if (isInBounds) {
		newIndex = m_chunk->GetBlockIndexByCoords(newCoord);
		return BlockIterator(m_chunk, newIndex);
	}
	else
	{
		newCoord.x = LOCAL_X_SIZE - 1;
		if (m_chunk->m_westNeighbor != nullptr) {
			newIndex = m_chunk->m_westNeighbor->GetBlockIndexByCoords(newCoord);
			return BlockIterator(m_chunk->m_westNeighbor, newIndex);
		}
	}
	return BlockIterator();
}

BlockIterator BlockIterator::GetNorthNeighbor() const
{
	IntVec3 coord = m_chunk->GetBlockCoordsByIndex(m_blockIndex);
	IntVec3 newCoord = coord;
	newCoord.y++;
	bool isInBounds = m_chunk->IsCoordsInBounds(newCoord);
	int newIndex = -1;
	if (isInBounds) {
		newIndex = m_chunk->GetBlockIndexByCoords(newCoord);
		return BlockIterator(m_chunk, newIndex);
	}
	else
	{
		newCoord.y = 0;
		if (m_chunk->m_northNeighbor != nullptr) {
			newIndex = m_chunk->m_northNeighbor->GetBlockIndexByCoords(newCoord);
			return BlockIterator(m_chunk->m_northNeighbor, newIndex);
		}
	}
	return BlockIterator();
}

BlockIterator BlockIterator::GetSouthNeighbor() const
{
	IntVec3 coord = m_chunk->GetBlockCoordsByIndex(m_blockIndex);
	IntVec3 newCoord = coord;
	newCoord.y--;
	bool isInBounds = m_chunk->IsCoordsInBounds(newCoord);
	int newIndex = -1;
	if (isInBounds) {
		newIndex = m_chunk->GetBlockIndexByCoords(newCoord);
		return BlockIterator(m_chunk, newIndex);
	}
	else
	{
		newCoord.y = LOCAL_Y_SIZE - 1;
		if (m_chunk->m_southNeighbor != nullptr) {
			newIndex = m_chunk->m_southNeighbor->GetBlockIndexByCoords(newCoord);
			return BlockIterator(m_chunk->m_southNeighbor, newIndex);
		}
	}
	return BlockIterator();
}

BlockIterator BlockIterator::GetUpNeighbor() const
{
	IntVec3 coord = m_chunk->GetBlockCoordsByIndex(m_blockIndex);
	IntVec3 newCoord = coord;
	newCoord.z++;
	bool isInBounds = m_chunk->IsCoordsInBounds(newCoord);
	int newIndex = -1;
	if (isInBounds) {
		newIndex = m_chunk->GetBlockIndexByCoords(newCoord);
		return BlockIterator(m_chunk, newIndex);
	}
	return BlockIterator();
}

BlockIterator BlockIterator::GetDownNeighbor() const
{
	IntVec3 coord = m_chunk->GetBlockCoordsByIndex(m_blockIndex);
	IntVec3 newCoord = coord;
	newCoord.z--;
	bool isInBounds = m_chunk->IsCoordsInBounds(newCoord);
	int newIndex = -1;
	if (isInBounds) {
		newIndex = m_chunk->GetBlockIndexByCoords(newCoord);
		return BlockIterator(m_chunk, newIndex);
	}
	return BlockIterator();
}

BlockIterator BlockIterator::GetNeighborByFace(FaceDirt face) const
{
	if (face == FaceDirt::EAST) {
		return GetEastNeighbor();
	}
	if (face == FaceDirt::WEST) {
		return GetWestNeighbor();
	}
	if (face == FaceDirt::NORTH) {
		return GetNorthNeighbor();
	}
	if (face == FaceDirt::SOUTH) {
		return GetSouthNeighbor();
	}
	if (face == FaceDirt::UP) {
		return GetUpNeighbor();
	}
	if (face == FaceDirt::DOWN) {
		return GetDownNeighbor();
	}
	return BlockIterator();
}

void BlockIterator::AddHorizontalNeighbor(std::vector<BlockIterator>& horizontalNeighbors)
{
	BlockIterator east = GetEastNeighbor();
	BlockIterator west = GetWestNeighbor();
	BlockIterator north = GetNorthNeighbor();
	BlockIterator south = GetSouthNeighbor();

	if (!east.IsEmpty()) {
		horizontalNeighbors.push_back(east);
	}
	if (!west.IsEmpty()) {
		horizontalNeighbors.push_back(west);
	}
	if (!north.IsEmpty()) {
		horizontalNeighbors.push_back(north);
	}
	if (!south.IsEmpty()) {
		horizontalNeighbors.push_back(south);
	}
}


