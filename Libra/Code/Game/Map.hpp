#pragma once
#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include "Game/Bullet.hpp"
#include "Game/MapDefinition.hpp"
#include <vector>
struct RaycastResult2D;
class TileHeatMap;

enum HeatMapType
{
	HEAT_MAP_NULL = -1,

	HEAT_MAP_DISTANCE,
	HEAT_MAP_SOLID,
	HEAT_MAP_OPAQUE,

	HEAT_MAP_NUM
};

class Map
{
public:
	Map(Game* owner, const char* name);
	~Map();
	
	void Render() const;
	void Update(float deltaSeconds);

	void SetTileType(int tileX, int tileY, const char* name);
	void SetTileType(int tileIndex, const char* name);

	void AddEntityToMap(Entity* entity);
	void RemoveEntityFromMap(Entity* entity);
	void DeleteGarbageEntities();
	IntVec2	GetMapDimension() const { return m_dimensions; };
	int		GetTileIndexForTileCoords(int tileX, int tileY) const;
	IntVec2 GetTileCoordsForWorldPos(Vec2 const& worldPos) const;
	IntVec2 GetTileCoordsForTileIndex(int tileIndex) const;
	Vec2	GetTileCenter(int tileX, int tileY) const;
	Vec2	GetExitPosition() const { return m_exitPosition; };
	Vec2	GetRandomWalkableTile() const;
	void	SetSprinkleChance(float chance1, float chance2);

	Entity* SpawnNewEntity(EntityType type, Vec2 const& position, float orientationDegrees);
	//void SpawnEntity

	bool IsPointInSolid(Vec2 const& worldPos) const;
	bool IsTileHaveScorpio(int tileX, int tileY) const;
	bool IsTileOutOfBounds(IntVec2 const& tileCoords) const;
	bool IsTileSolid(IntVec2 const& tileCoords) const;
	bool IsTileWater(IntVec2 const& tileCoords) const;
	RaycastResult2D RaycastVsTiles(Vec2 start, Vec2 forwardDirection, float maxDistance);
	bool HasLineOfSight(Vec2 const& start, Vec2 const& target, float targetRadius);

	bool CheckBulletOverlapOfWalls(Vec2 const& point);
	bool CheckBulletOverlapOfEntities(Bullet *bullet);
	bool CheckBulletOverlapOfEntity(Bullet *bullet, Entity* entity);

	void PopulateDistanceField(TileHeatMap& out_distanceField, IntVec2 referenceCoords, float maxCost, bool treatWaterAsSolid = true, bool isScorpioAsSolid = false);

protected:
	void RenderEntities() const;
	void RenderEntityList(EntityList const& entityList) const;
	void RenderTiles() const;
	void RenderDebug() const;
	void RenderHeatMap() const;

	void PopulateTiles();
	void PopulateUnreachableTiles();

	void AddEntityToList(Entity* entity, std::vector<Entity*>& list);
	void UpdateEntities(float deltaSeconds);
	void PushEntitiesOutOfWalls(float deltaSeconds);
	void PushEntityOutOfWalls(Entity& entity, float deltaSeconds);
	void PushEntityOutOfTileIfSolid(Entity& entity, IntVec2 const& tileCoords);
	void PushEntitiesOutOfEachOther();
	void PushEntityOutOfEachOther(Entity& entityA, Entity& entityB);

	void AddVertsForTile(std::vector<Vertex_PCU>& verts, int tileIndex) const;
	void AddVertsForHeatMapTile(std::vector<Vertex_PCU>& verts, int tileIndex) const;

protected:
	Game*					m_game = nullptr;
	MapDefinition const*	m_mapDef = nullptr;
	float					m_sprinkle1Chance = 0.f;
	float					m_sprinkle2Chance = 0.f;
	std::vector<Tile>		m_tiles;
	EntityList				m_allEntities;
	EntityList				m_entitiesByType[NUM_ENTITY_TYPES];
	IntVec2					m_dimensions = IntVec2(0, 0);
	Vec2					m_exitPosition;
	TileHeatMap*			m_tileHeatMap;
	std::vector<Vec2>		m_walkableTiles;
};