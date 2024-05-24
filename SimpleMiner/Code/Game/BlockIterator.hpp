#pragma once

#include <vector>
class Chunk;
class Block;
struct Vec3;
struct Rgba8;

enum class FaceDirt
{
	EAST,
	WEST,
	NORTH,
	SOUTH,
	UP,
	DOWN,

	NUM
};

class BlockIterator
{
public:
	BlockIterator();
	BlockIterator(Chunk* chunk, int blockIndex);
	~BlockIterator();
	bool IsEmpty() const;
	Block* GetBlock() const;
	Vec3 GetWorldCenter() const;

	bool GetFaceWorldBounds(FaceDirt face, Vec3& bottomleft, Vec3& bottomRight, Vec3& topLeft, Vec3& topRight) const;

	Rgba8 GetNeighborLight(FaceDirt dirt) const;
	BlockIterator GetEastNeighbor() const;
	BlockIterator GetWestNeighbor() const;
	BlockIterator GetNorthNeighbor() const;
	BlockIterator GetSouthNeighbor() const;
	BlockIterator GetUpNeighbor() const;
	BlockIterator GetDownNeighbor() const;

	BlockIterator GetNeighborByFace(FaceDirt face) const;

	void AddHorizontalNeighbor(std::vector<BlockIterator>& horizontalNeighbors);
	
public:
	Chunk* m_chunk = nullptr;
	int		m_blockIndex = 0;
};