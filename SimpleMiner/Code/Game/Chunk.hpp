#pragma once
#include "Game/Block.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>
#include <atomic>
#include <string>
class Texture;
class World;
class VertexBuffer;
struct IntVec3;

constexpr int X_BIT_SIZE = 4;
constexpr int Y_BIT_SIZE = 4;
constexpr int Z_BIT_SIZE = 7;

constexpr int LOCAL_X_SIZE = (1 << X_BIT_SIZE);
constexpr int LOCAL_Y_SIZE = (1 << Y_BIT_SIZE);
constexpr int LOCAL_Z_SIZE = (1 << Z_BIT_SIZE);

constexpr int BLOCK_TOTAL_NUMBER = LOCAL_X_SIZE * LOCAL_Y_SIZE * LOCAL_Z_SIZE;

constexpr int BLOCK_TOTAL_VERTS = 6000;

struct CaveInfo
{
	IntVec2				m_startChunkCoords;
	Vec3				m_startWorldPos;
	std::vector<Vec3>	m_caveNodePositions;
};

enum class ChunkState
{
	MISSING,
	ON_DISK,
	CONSTRUCTING,

	ACTIVATING_QUEUED_LOAD,
	ACTIVATING_LOADING,
	ACTIVATING_LOAD_COMPLETE,

	ACTIVATING_QUEUED_GENERATE,
	ACTIVATING_GENERATING,
	ACTIVATING_GENERATE_COMPLETE,

	ACTIVE,

	DEACTIVATING_QUEUED_SAVE,
	DEACTIVATING_SAVING,
	DEACTIVATING_SAVE_COMPLETE,
	DECONSTRUCTING,

	NUM_CHUNK_STATES
};

class Chunk
{
public:
	Chunk(IntVec2 coords, World* world);
	~Chunk();

	void PopulateBlocks();
	void InitializeLight();
	bool LoadFromFile();
	bool SaveToFile();
	void RebuildChunkMeshVerts();
	void SpawnBlockTemplateAtCoords(std::string tempName, IntVec3 coords);
	void AddCaves();
	void AddHouse();
	void AddCaveLight();
	bool IsHouseChunk(int globalX, int globalY, unsigned int seed);

	AABB3 GetLocalBoundsByIndex(int index) const;
	AABB3 GetWorldBoundsByIndex(int index) const;
	IntVec3 GetBlockCoordsByIndex(int index) const;
	Vec3 GetBlockWorldCenterByIndex(int index) const;
	int		GetBlockIndexByCoords(IntVec3 coords) const;
	bool	IsCoordsInBounds(const IntVec3& coords) const;
	bool	IsBlockOnBorder(int index) const;
	bool	IsHavingAllNeighbors() const;
	bool	IsTreeColumn(int globalX, int globalY, unsigned int seed);
	bool	IsLeafColumn(int globalX, int globalY, unsigned int seed);
	float	GetHumidity(int globalX, int globalY);
	float	GetTemperature(int globalX, int globalY);
	int		GetTerrainHeightZ(int globalX, int globalY);
	bool	IsATreeFinally(int globalX, int globalY);
	IntVec3 GetLocalBlockCoordsByPosition(const Vec3& position) const;
	IntVec3 GetWorldBlockCoordsByPosition(const Vec3& position) const;
	IntVec3 GetFirstOneNotAir(const Vec3& position) const;

	void Render() const;

	void Update(float deltaSeconds);

	void UpdateFromKeyboard(float deltaSeconds);

	void AddVertsForBlock(std::vector<Vertex_PCU> &verts, int index);

public:

	World*					m_world;

	std::atomic<ChunkState>	m_chunkState = ChunkState::CONSTRUCTING;

	IntVec2					m_chunkCoords;
	AABB3					m_worldBounds;
	Block*					m_blocks = nullptr;
	std::vector<Vertex_PCU> m_cpuMesh;
	bool					m_isMeshDataDirty = true;
	bool					m_isSaveDirty = false;
	VertexBuffer*			m_gpuMesh;

	bool					m_isDebug;
	Chunk*					m_eastNeighbor;
	Chunk*					m_westNeighbor;
	Chunk*					m_northNeighbor;
	Chunk*					m_southNeighbor;

};