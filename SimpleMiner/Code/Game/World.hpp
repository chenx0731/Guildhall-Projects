#pragma once
#include "Game/GameCommon.hpp"
#include "Game/BlockIterator.hpp"
#include "Game/GameRaycastResult3D.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>
#include <map>
#include <deque>
#include <mutex>

class Game;
class Entity;
class Player;
class Prop;
class Clock;
class Chunk;
class Shader;

struct Vec2;
struct IntVec2;

class World
{
public:
	World( Game* owner);
	~World();
	void Update(float deltaSeconds);
	void Render() const;
	int GetTotalVertexes() const;
	Vec2 GetChunkCenterXYFromCoords(const IntVec2& coords) const;
	IntVec2 GetCoordsFromPosition(const Vec3& position) const;
	bool FindNearestInactiveChunk(Vec3 position, IntVec2& coords);
	bool FindFarestOutOfDateChunk(Vec3 position);
	bool IsChunkActive(IntVec2 coords);

	void ProcessDirtyLighting();
	void ProcessNextDirtyLightBlock(BlockIterator blockIterator);
	void MarkLightingDirty(BlockIterator blockIterator);
	void UndirtyAllBlocksInChunk(Chunk* chunk);

	void CalculateSeed();
	
	void CalculateCorrectLightValue(BlockIterator blockIterator);
	GameRaycastResult RaycastWorld(const Vec3& start, const Vec3& direction, float distance);
	void UpdateFromKeyboard(float deltaSeconds);
public:
	Game* m_game;
	std::vector<Chunk*> m_chunks;
	std::map<IntVec2, Chunk*> m_activeChunks;
	int m_maxChunks = 1024;
	int m_nowChunks = 0;
	float m_activitionRange = 0.f;
	int m_maxChunksRadiusX;
	int m_maxChunksRadiusY;
	float m_deactiveRange = 0.f;
	GameRaycastResult m_raycastFace;
	Rgba8 m_nightSkyColor;
	Rgba8 m_daySkyColor;
	Rgba8 m_nightOutdoorLightColor;
	Rgba8 m_dayOutdoorLightColor;
	Rgba8 m_indoorLightColor;
	Shader* m_shader;

	bool m_isPlacingCobbleStone = true;
	std::deque<BlockIterator> m_dirtyLightBlocks;
	
	float m_time = 0.f;
	bool m_isTimeFast = false;
	float m_timeScaler = 50.f;

	float m_lightningPerlin;
	float m_glowPerlin;

	unsigned int m_seed = 0;
	unsigned char m_version = 3;

	unsigned int m_terrainHeightSeed;
	unsigned int m_hillinessSeed;
	unsigned int m_humiditySeed;
	unsigned int m_temperatureSeed;
	unsigned int m_oceannessSeed;
	unsigned int m_treeSeed;
	unsigned int m_forestnessSeed;
	unsigned int m_caveSeed;
	unsigned int m_houseSeed;

	std::mutex	m_waitlistLock;
	std::map<IntVec2, Chunk*>	m_waitForGeneration;

	int		m_rebuildMeshTimes = 0;
};