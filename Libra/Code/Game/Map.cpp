#define UNUSED(x) (void)(x);
#include "Game/Map.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Player.hpp"
#include "Game/Bullet.hpp"
#include "Game/Aries.hpp"
#include "Game/Capricorn.hpp"
#include "Game/Leo.hpp"
#include "Game/Scorpio.hpp"
#include "Game/Explosion.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/HeatMaps.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"

Map::Map(Game* owner, const char* name) : m_game(owner)
{
	m_mapDef = MapDefinition::GetMapDef(name);
	m_dimensions = m_mapDef->m_dimensions;
	m_tileHeatMap = new TileHeatMap(m_dimensions);
	m_exitPosition = Vec2(static_cast<float> (m_dimensions.x) - 1.5f, static_cast<float> (m_dimensions.y) - 1.5f);

	bool flag = true;
	int times = 0;
	while (flag) {
		flag = false;
		PopulateTiles();
		PopulateDistanceField(*m_tileHeatMap, IntVec2(1, 1), 9999.f);
		if (m_tileHeatMap->GetValue(m_dimensions.x - 2, m_dimensions.y - 2) == 9999.f)
			flag = true;
		times++;
		if (times >= 100) {
			ERROR_AND_DIE(Stringf("Please Reset the Worm Count & MaxLength of Map \"%s\"", m_mapDef->m_name.c_str()));
		}
	}
	PopulateUnreachableTiles();
		

	for (int ariesIndex = 0; ariesIndex < m_mapDef->m_entitySpawnCounts[ENTITY_TYPE_EVIL_ARIES]; ariesIndex++) {
		float orientation = g_rng->RollRandomFloatInRange(0.f, 360.f);
		Entity* aries = SpawnNewEntity(ENTITY_TYPE_EVIL_ARIES, GetRandomWalkableTile(), orientation);
		AddEntityToMap(aries);
	}

	for (int leoIndex = 0; leoIndex < m_mapDef->m_entitySpawnCounts[ENTITY_TYPE_EVIL_LEO]; leoIndex++) {
		float orientation = g_rng->RollRandomFloatInRange(0.f, 360.f);	
		Entity* leo = SpawnNewEntity(ENTITY_TYPE_EVIL_LEO, GetRandomWalkableTile(), orientation);
		AddEntityToMap(leo);
	}

	for (int scorpioIndex = 0; scorpioIndex < m_mapDef->m_entitySpawnCounts[ENTITY_TYPE_EVIL_SCORPIO]; scorpioIndex++) {
		float orientation = g_rng->RollRandomFloatInRange(0.f, 360.f);
		Entity* scorpio = SpawnNewEntity(ENTITY_TYPE_EVIL_SCORPIO, GetRandomWalkableTile(), orientation);
		AddEntityToMap(scorpio);
	}

	for (int capricornIndex = 0; capricornIndex < m_mapDef->m_entitySpawnCounts[ENTITY_TYPE_EVIL_CAPRICORN]; capricornIndex++) {
		float orientation = g_rng->RollRandomFloatInRange(0.f, 360.f);
		Entity* capricorn = SpawnNewEntity(ENTITY_TYPE_EVIL_CAPRICORN, GetRandomWalkableTile(), orientation);
		AddEntityToMap(capricorn);
	}
}

Map::~Map()
{
}

void Map::Render() const
{
	RenderTiles();
	RenderHeatMap();
	RenderEntities();
	RenderDebug();
}

void Map::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	UpdateEntities(deltaSeconds);
	PushEntitiesOutOfEachOther();
	if (!g_debugNoclip)
	PushEntitiesOutOfWalls(deltaSeconds);
	
}

void Map::SetTileType(int tileX, int tileY, const char* name)
{
	int tileIndex = GetTileIndexForTileCoords(tileX, tileY);
	m_tiles[tileIndex].m_tileDef = TileDefinition::GetTileDef(name);
	m_tiles[tileIndex].m_tileCoords = IntVec2(tileX, tileY);
}

void Map::SetTileType(int tileIndex, const char* name)
{
	m_tiles[tileIndex].m_tileDef = TileDefinition::GetTileDef(name);
	m_tiles[tileIndex].m_tileCoords = GetTileCoordsForTileIndex(tileIndex);
}

void Map::AddEntityToMap(Entity* entity)
{
	AddEntityToList(entity, m_entitiesByType[entity->m_entityType]);
	
	for (int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++) {
		if (m_allEntities[entityIndex] == nullptr) {
			m_allEntities[entityIndex] = entity;
			return;
		}
	}

	m_allEntities.push_back(entity);
}

void Map::RemoveEntityFromMap(Entity* entity)
{
	for (int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++) {
		if (m_allEntities[entityIndex] == entity) {
			m_allEntities[entityIndex] = nullptr;
			return;
		}
	}
	for (int entityIndex = 0; entityIndex < m_entitiesByType[entity->m_entityType].size(); entityIndex++) {
		if (m_entitiesByType[entity->m_entityType][entityIndex] == entity) {
			m_entitiesByType[entity->m_entityType][entityIndex] = nullptr;
			return;
		}
	}
}

void Map::DeleteGarbageEntities()
{
	for (int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++) {
		
		if (m_allEntities[entityIndex] && m_allEntities[entityIndex]->m_isGarbage) {
			if (m_allEntities[entityIndex] == g_theGame->m_player) {
				g_theGame->m_playerRespawnPoint =  m_allEntities[entityIndex]->m_position;
				g_theGame->m_player = nullptr;
			}
			delete m_allEntities[entityIndex];
			m_allEntities[entityIndex] = nullptr;
		}
	}

	for (int typeIndex = 0; typeIndex < NUM_ENTITY_TYPES; typeIndex++) {
		for (int entityIndex = 0; entityIndex < m_entitiesByType[typeIndex].size(); entityIndex++) {
			if (m_entitiesByType[typeIndex][entityIndex] && m_entitiesByType[typeIndex][entityIndex]->m_isGarbage) {
				//delete m_entitiesByType[typeIndex][entityIndex];
				m_entitiesByType[typeIndex][entityIndex] = nullptr;
			}
		}
	}
}

Entity* Map::SpawnNewEntity(EntityType type, Vec2 const& position, float orientationDegrees)
{
	switch (type)
	{
	case ENTITY_TYPE_GOOD_PLAYER:		return new Player(		type, FACTION_GOOD, this, position, orientationDegrees);
	case ENTITY_TYPE_GOOD_BULLET:		return new Bullet(		type, FACTION_GOOD, this, position, orientationDegrees);
	case ENTITY_TYPE_GOOD_BOLT:			return new Bullet(		type, FACTION_GOOD, this, position, orientationDegrees);
	case ENTITY_TYPE_EVIL_BOLT:			return new Bullet(		type, FACTION_EVIL, this, position, orientationDegrees);
	case ENTITY_TYPE_EVIL_BULLET:		return new Bullet(		type, FACTION_EVIL, this, position, orientationDegrees);
	case ENTITY_TYPE_EVIL_LEO:			return new Leo(			type, FACTION_EVIL, this, position, orientationDegrees);
	case ENTITY_TYPE_EVIL_CAPRICORN:	return new Capricorn(	type, FACTION_EVIL, this, position, orientationDegrees);
	case ENTITY_TYPE_EVIL_SCORPIO:		return new Scorpio(		type, FACTION_EVIL, this, position, orientationDegrees);
	case ENTITY_TYPE_EVIL_ARIES:		return new Aries(		type, FACTION_EVIL, this, position, orientationDegrees);
	case ENTITY_TYPE_GOOD_EXPLOSION:	return new Explosion(	type, 2.f,	  0.5f, this, position, orientationDegrees);
	case ENTITY_TYPE_EVIL_EXPLOSION:	return new Explosion(	type, 1.f,	  0.5f, this, position, orientationDegrees);
	case ENTITY_TYPE_BULLET_EXPLOSION:	return new Explosion(	type, 0.2f,	  0.1f, this, position, orientationDegrees);

	}
	ERROR_RECOVERABLE(Stringf("WARNING: failed to create entity of type #%i (at %.02f, %0.2f)", type, position.x, position.y));
	return nullptr;
}

bool Map::IsPointInSolid(Vec2 const& worldPos) const
{
	IntVec2 tileCoords = GetTileCoordsForWorldPos(worldPos);
	int tileIndex = GetTileIndexForTileCoords(tileCoords.x, tileCoords.y);
	return m_tiles[tileIndex].IsSolid();
}

bool Map::IsTileHaveScorpio(int tileX, int tileY) const
{
	IntVec2 tileCoords = IntVec2(tileX, tileY);
	for (int entityIndex = 0; entityIndex < m_entitiesByType[ENTITY_TYPE_EVIL_SCORPIO].size(); entityIndex++) {
		if (!m_entitiesByType[ENTITY_TYPE_EVIL_SCORPIO][entityIndex]) continue;
		if (tileCoords == GetTileCoordsForWorldPos(m_entitiesByType[ENTITY_TYPE_EVIL_SCORPIO][entityIndex]->m_position)) {
			return true;
		}
	}
	return false;
}

bool Map::IsTileOutOfBounds(IntVec2 const& tileCoords) const
{
	if (tileCoords.x < 0 || tileCoords.x >= m_dimensions.x || tileCoords.y < 0 || tileCoords.y >= m_dimensions.y)
	{
		return true;
	}
	return false;
}

bool Map::IsTileSolid(IntVec2 const& tileCoords) const
{
	int tileIndex = GetTileIndexForTileCoords(tileCoords.x, tileCoords.y);
	return m_tiles[tileIndex].IsSolid();
}

bool Map::IsTileWater(IntVec2 const& tileCoords) const
{
	int tileIndex = GetTileIndexForTileCoords(tileCoords.x, tileCoords.y);
	return m_tiles[tileIndex].IsWater();
}

RaycastResult2D Map::RaycastVsTiles(Vec2 start, Vec2 forwardDirection, float maxDistance)
{
	int maxStep = 30;
	float singleStep = maxDistance / static_cast<float>(maxStep);
	Vec2 hitPoint = start;
	for (int stepIndex = 0; stepIndex < maxStep; stepIndex++)
	{
		hitPoint += forwardDirection * singleStep;
		IntVec2 tile = GetTileCoordsForWorldPos(hitPoint);
		if (!IsTileOutOfBounds(tile) && IsPointInSolid(hitPoint))
		{
			RaycastResult2D result = RaycastResult2D(hitPoint, true, singleStep * static_cast<float>(stepIndex));
			return result;
		}
	}
	return RaycastResult2D(hitPoint, false, maxDistance);
}

bool Map::HasLineOfSight(Vec2 const& start, Vec2 const& target, float targetRadius)
{
	float visiableDistance = g_gameConfigBlackboard.GetValue("enemyVisibleRange", 10.f);
	if ((start - target).GetLengthSquared() > visiableDistance * visiableDistance) {
		return false;
	}
	RaycastResult2D rr2d = RaycastVsTiles(start, (target - start).GetNormalized(), (start - target).GetLength());
	if (rr2d.m_didImpact) return false;
	if ((rr2d.m_impactPos - target).GetLengthSquared() < targetRadius * targetRadius){
		return true;
	}
	return false;
}

bool Map::CheckBulletOverlapOfWalls(Vec2 const& point)
{
	IntVec2 tile = GetTileCoordsForWorldPos(point);
	if (IsTileOutOfBounds(tile))
		return false;
	int index = GetTileIndexForTileCoords(tile.x, tile.y);
	if (m_tiles[index].IsSolid()) {
		return true;
	}
	return false;
}

bool Map::CheckBulletOverlapOfEntities(Bullet* bullet)
{
	for (int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++) {
		Entity*& entity = m_allEntities[entityIndex];
		if (m_allEntities[entityIndex] && entity->m_isHitByBullets) {
			if (bullet->m_faction != entity->m_faction) {
				if (CheckBulletOverlapOfEntity(bullet, entity)) {
					entity->ReactToBulletImpact(bullet);
					return true;
				}
			}
		}
	}
	return false;
}

bool Map::CheckBulletOverlapOfEntity(Bullet* bullet, Entity* entity)
{
	return IsPointInsideDisc2D(bullet->m_position, entity->m_position, entity->m_physicsRadius);
}

void Map::RenderEntities() const
{
	std::vector<Vertex_PCU> healthBarVerts;
	for (int typeIndex = 0; typeIndex < NUM_ENTITY_TYPES; typeIndex++) {
		//RenderEntityList(m_entitiesByType[typeIndex]);
		EntityList entityList = m_entitiesByType[typeIndex];
		for (int entityIndex = 0; entityIndex < entityList.size(); entityIndex++) {
			Entity const* entity = entityList[entityIndex];
			if (entity) {
				entity->Render();
				if (entity->m_entityType == ENTITY_TYPE_EVIL_ARIES || entity->m_entityType == ENTITY_TYPE_EVIL_LEO || entity->m_entityType == ENTITY_TYPE_EVIL_SCORPIO || entity->m_entityType == ENTITY_TYPE_GOOD_PLAYER) {
					entity->AddVertsForHealthBar(healthBarVerts);
				}
			}
			
		}
	}
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)healthBarVerts.size(), healthBarVerts.data());
}

void Map::RenderEntityList(EntityList const& entityList) const
{
	std::vector<Vertex_PCU> healthBarVerts;
	for (int entityIndex = 0; entityIndex < entityList.size(); entityIndex++) {
		Entity const* entity = entityList[entityIndex];
		if (entity) {
			entity->Render();
		}
		if (entity->m_entityType == ENTITY_TYPE_EVIL_ARIES || entity->m_entityType == ENTITY_TYPE_EVIL_LEO || entity->m_entityType == ENTITY_TYPE_EVIL_SCORPIO || entity->m_entityType == ENTITY_TYPE_GOOD_PLAYER) {
			entity->AddVertsForHealthBar(healthBarVerts);
		}
	}
}

void Map::RenderTiles() const
{
	if (g_debugHeatMap) {
		return;
	}
	int allVertexCount = 3 * 2 * m_dimensions.x * m_dimensions.y;
	std::vector<Vertex_PCU> tileVerts;
	tileVerts.reserve(allVertexCount);

	for (int tileIndex = 0; tileIndex < static_cast<int> (m_tiles.size()); tileIndex++) {
		AddVertsForTile(tileVerts, tileIndex);
	}
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_TERRAIN]);
	g_theRenderer->DrawVertexArray(static_cast<int> (tileVerts.size()), tileVerts.data());
	g_theRenderer->BindTexture(nullptr);
}

void Map::RenderDebug() const
{
	if (!g_debugDraw) {
		return;
	}

	for (int typeIndex = 0; typeIndex < NUM_ENTITY_TYPES; typeIndex++) {
		for (int entityIndex = 0; entityIndex < m_entitiesByType[typeIndex].size(); entityIndex++) {
			Entity const* entity = m_entitiesByType[typeIndex][entityIndex];
			if (entity) {
				entity->DebugRender();
			}
		}
	}
}

void Map::RenderHeatMap() const
{
	if (!g_debugHeatMap) {
		return;
	}
	int allVertexCount = 3 * 2 * m_dimensions.x * m_dimensions.y;
	std::vector<Vertex_PCU> tileVerts;
	tileVerts.reserve(allVertexCount);

	for (int tileIndex = 0; tileIndex < static_cast<int> (m_tiles.size()); tileIndex++) {
		AddVertsForHeatMapTile(tileVerts, tileIndex);
	}
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(static_cast<int> (tileVerts.size()), tileVerts.data());
	g_theRenderer->BindTexture(nullptr);
}

void Map::PopulateTiles()
{
	int totalTiles = m_dimensions.x * m_dimensions.y;
	int startAreaSize = g_gameConfigBlackboard.GetValue("startAreaSize", 5);
	int endAreaSize = g_gameConfigBlackboard.GetValue("endAreaSize", 6);
	m_tiles.resize(totalTiles);

	for (int tileIndex = 0; tileIndex < totalTiles; tileIndex++) {
		m_tiles[tileIndex].m_tileDef = nullptr;
	}

	std::vector<IntVec2> directions;
	directions.push_back(IntVec2(1, 0));
	directions.push_back(IntVec2(-1, 0));
	directions.push_back(IntVec2(0, 1));
	directions.push_back(IntVec2(0, -1));

	for (int worm1Index = 0; worm1Index < m_mapDef->m_worm1Count; worm1Index++) {
		int startX = g_rng->RollRandomIntInRange(1, m_dimensions.x - 2);
		int startY = g_rng->RollRandomIntInRange(1, m_dimensions.y - 2);
		IntVec2 start = IntVec2(startX, startY);
		IntVec2 now = start;
		for (int stepIndex = 0; stepIndex < m_mapDef->m_worm1MaxLength; stepIndex++) {
			SetTileType(GetTileIndexForTileCoords(now.x, now.y), m_mapDef->m_worm1TileType.c_str());
			int randomIndex = g_rng->RollRandomIntInRange(0, 3);
			if (!IsTileOutOfBounds(now + directions[randomIndex])) {
				now = now + directions[randomIndex];
			}
			else break;
		}
	}

	for (int worm2Index = 0; worm2Index < m_mapDef->m_worm2Count; worm2Index++) {
		int startX = g_rng->RollRandomIntInRange(1, m_dimensions.x - 2);
		int startY = g_rng->RollRandomIntInRange(1, m_dimensions.y - 2);
		IntVec2 start = IntVec2(startX, startY);
		IntVec2 now = start;
		for (int stepIndex = 0; stepIndex < m_mapDef->m_worm2MaxLength; stepIndex++) {
			SetTileType(GetTileIndexForTileCoords(now.x, now.y), m_mapDef->m_worm2TileType.c_str());
			int randomIndex = g_rng->RollRandomIntInRange(0, 3);
			if (!IsTileOutOfBounds(now + directions[randomIndex])) {
				now = now + directions[randomIndex];
			}
			else break;
		}
	}

	for (int tileIndex = 0; tileIndex < totalTiles; tileIndex++) {
		IntVec2 coords = GetTileCoordsForTileIndex(tileIndex);
		//m_tiles.push_back(Tile());

		if (coords.x == 1 && coords.y == 1) {
			SetTileType(tileIndex, "MapEntry");
			continue;
		}

		if (coords.x == m_dimensions.x - 2 && coords.y == m_dimensions.y - 2) {
			SetTileType(tileIndex, "MapExit");
			continue;
		}

		// Wall
		if (coords.x == 0 || coords.y == 0 || coords.x == m_dimensions.x - 1 || coords.y == m_dimensions.y - 1) {
			SetTileType(tileIndex, m_mapDef->m_edgeTileType.c_str());
			continue;
		}
		// Start/End Points
		if ((coords.x == startAreaSize - 1 && (coords.y <= startAreaSize - 1 && coords.y > 1)) ||
			(coords.y == startAreaSize - 1 && (coords.x < startAreaSize - 1 && coords.x > 1)) ) {
			SetTileType(tileIndex, m_mapDef->m_startBunkerTileType.c_str());
			continue;
		}
		if ((coords.x == m_dimensions.x - endAreaSize && (coords.y < m_dimensions.y - 2 && coords.y >= m_dimensions.y - endAreaSize)) ||
			(coords.y == m_dimensions.y - endAreaSize && (coords.x < m_dimensions.x - 2 && coords.x > m_dimensions.x - endAreaSize))) {
			SetTileType(tileIndex, m_mapDef->m_endBunkerTileType.c_str());
			continue;
		}
		// Safe Zone
		if (coords.x < startAreaSize + 1 && coords.x > 0 && coords.y > 0 && coords.y < startAreaSize + 1) {
			SetTileType(tileIndex, m_mapDef->m_startFloorTileType.c_str());
			continue;
		}

		if (coords.x < m_dimensions.x && coords.x > m_dimensions.x - endAreaSize - 2 && coords.y < m_dimensions.y && coords.y > m_dimensions.y - endAreaSize - 2) {
			SetTileType(tileIndex, m_mapDef->m_endFloorTileType.c_str());
			continue;
		}
		if (m_tiles[tileIndex].m_tileDef == nullptr) {
			SetTileType(tileIndex, m_mapDef->m_fillTileType.c_str());
		}
			
	}
}

void Map::PopulateDistanceField(TileHeatMap& out_distanceField, IntVec2 referenceCoords, float maxCost, bool treatWaterAsSolid, bool isScorpioAsSolid)
{
	bool flag = true;
	float initCost = 0.f;
	float nowCost = initCost;

	out_distanceField.SetAllValues(maxCost);
	out_distanceField.SetValue(referenceCoords.x, referenceCoords.y, initCost);
	
	while (flag) {
		flag = false;
		for (int i = 0; i < m_dimensions.x; i++) {
			for (int j = 0; j < m_dimensions.y; j++) {
				if (out_distanceField.GetValue(i, j) == nowCost) {
					// east
					if (i + 1 < m_dimensions.x 
						&& out_distanceField.GetValue(i + 1, j) > nowCost + 1.f
						&& !IsTileSolid(IntVec2(i + 1, j)) ) {

						if ((treatWaterAsSolid && !IsTileWater(IntVec2(i + 1, j)) || !treatWaterAsSolid) 
							&& (isScorpioAsSolid && !IsTileHaveScorpio(i + 1, j) || !isScorpioAsSolid) ){
							out_distanceField.SetValue(i + 1, j, nowCost + 1.f);
							flag = true;
						}	
					}
					// west
					if (i - 1 >= 0 
						&& out_distanceField.GetValue(i - 1, j) > nowCost + 1.f
						&& !IsTileSolid(IntVec2(i - 1, j))) {

						if ((treatWaterAsSolid && !IsTileWater(IntVec2(i - 1, j)) || !treatWaterAsSolid)
							&& (isScorpioAsSolid && !IsTileHaveScorpio(i - 1, j) || !isScorpioAsSolid)) {
							out_distanceField.SetValue(i - 1, j, nowCost + 1.f);
							flag = true;
						}
					}
					// north
					if (j + 1 < m_dimensions.y 
						&& out_distanceField.GetValue(i, j + 1) > nowCost + 1.f
						&& !IsTileSolid(IntVec2(i, j + 1))) {

						if ((treatWaterAsSolid && !IsTileWater(IntVec2(i, j + 1)) || !treatWaterAsSolid)
							&& (isScorpioAsSolid && !IsTileHaveScorpio(i, j + 1) || !isScorpioAsSolid)) {
							out_distanceField.SetValue(i, j + 1, nowCost + 1.f);
							flag = true;
						}
					}
					// south
					if (j - 1 >= 0 
						&& out_distanceField.GetValue(i, j - 1) > nowCost + 1.f
						&& !IsTileSolid(IntVec2(i, j - 1))) {

						if ((treatWaterAsSolid && !IsTileWater(IntVec2(i, j - 1)) || !treatWaterAsSolid)
							&& (isScorpioAsSolid && !IsTileHaveScorpio(i, j - 1) || !isScorpioAsSolid)) {
							out_distanceField.SetValue(i, j - 1, nowCost + 1.f);
							flag = true;
						}
					}
				}
			}
		}
		nowCost += 1.f;
	}
	out_distanceField.m_maxValue = nowCost - 1;
}

void Map::PopulateUnreachableTiles()
{
	int startAreaSize = g_gameConfigBlackboard.GetValue("startAreaSize", 5);
	int endAreaSize = g_gameConfigBlackboard.GetValue("endAreaSize", 6);
	for (int i = 0; i < m_dimensions.x; i++) {
		for (int j = 0; j < m_dimensions.y; j++) {
			if (m_tileHeatMap->GetValue(i, j) < 9999.f) {
				if (i < startAreaSize + 1 && i > 0 && j > 0 && j < startAreaSize + 1) {
					continue;
				}
				if (i < m_dimensions.x && i > m_dimensions.x - endAreaSize - 2 && j < m_dimensions.y && j > m_dimensions.y - endAreaSize - 2) {
					continue;
				}
				m_walkableTiles.push_back(GetTileCenter(i ,j));
			}
			if (m_tileHeatMap->GetValue(i, j) == 9999.f && !IsTileSolid(IntVec2(i ,j))) {
				int index = i + j * m_dimensions.x;
				SetTileType(index, m_mapDef->m_edgeTileType.c_str());
			}
		}
	}
}

void Map::AddEntityToList(Entity* entity, std::vector<Entity*>& list)
{
	for (int entityIndex = 0; entityIndex < list.size(); entityIndex++) {
		if (list[entityIndex] == nullptr) {
			list[entityIndex] = entity;
			return;
		}
	}

	list.push_back(entity);
}

void Map::UpdateEntities(float deltaSeconds)
{
	for (int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++) {
		Entity*& entity = m_allEntities[entityIndex];
		if (entity) {
			entity->Update(deltaSeconds);
		}
	}
}

void Map::PushEntitiesOutOfWalls(float deltaSeconds)
{
	for (int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++) {
		Entity*& entity = m_allEntities[entityIndex];
		if (entity && entity->m_isPushedByWall) {
			PushEntityOutOfWalls(*entity, deltaSeconds);
		}
	}
}

void Map::PushEntityOutOfWalls(Entity& entity, float deltaSeconds)
{
	UNUSED(deltaSeconds);
	IntVec2 tileOfEntity = GetTileCoordsForWorldPos(entity.m_position);

	IntVec2 up		= IntVec2(tileOfEntity.x, tileOfEntity.y + 1);
	IntVec2 down	= IntVec2(tileOfEntity.x, tileOfEntity.y - 1);
	IntVec2 left	= IntVec2(tileOfEntity.x + 1, tileOfEntity.y);
	IntVec2 right	= IntVec2(tileOfEntity.x - 1, tileOfEntity.y);
	PushEntityOutOfTileIfSolid(entity, up);
	PushEntityOutOfTileIfSolid(entity, down);
	PushEntityOutOfTileIfSolid(entity, left);
	PushEntityOutOfTileIfSolid(entity, right);


	IntVec2 upLeft		= IntVec2(tileOfEntity.x + 1, tileOfEntity.y + 1);
	IntVec2 upRight		= IntVec2(tileOfEntity.x - 1, tileOfEntity.y + 1);
	IntVec2 downLeft	= IntVec2(tileOfEntity.x + 1, tileOfEntity.y - 1);
	IntVec2 downRight	= IntVec2(tileOfEntity.x - 1, tileOfEntity.y - 1);
	PushEntityOutOfTileIfSolid(entity, upLeft);
	PushEntityOutOfTileIfSolid(entity, upRight);
	PushEntityOutOfTileIfSolid(entity, downLeft);
	PushEntityOutOfTileIfSolid(entity, downRight);
}

void Map::PushEntityOutOfTileIfSolid(Entity& entity, IntVec2 const& tileCoords)
{
	int tileIndex = GetTileIndexForTileCoords(tileCoords.x, tileCoords.y);
	// border is water
	if (tileIndex < 0 || tileIndex >= m_dimensions.x * m_dimensions.y) {
		IntVec2 coords = GetTileCoordsForTileIndex(tileIndex);
		Vec2 mins = Vec2(static_cast<float>(coords.x), static_cast<float>(coords.y));
		Vec2 maxs = mins + Vec2(1.f, 1.f);
		PushDiscOutOfFixedAABB2D(entity.m_position, entity.m_physicsRadius, AABB2(mins, maxs));
		return;
	}

	if (m_tiles[tileIndex].IsSolid())
		PushDiscOutOfFixedAABB2D(entity.m_position, entity.m_physicsRadius, m_tiles[tileIndex].GetBounds());
	if (!entity.m_canSwim && m_tiles[tileIndex].IsWater())
		PushDiscOutOfFixedAABB2D(entity.m_position, entity.m_physicsRadius, m_tiles[tileIndex].GetBounds());
}

void Map::PushEntitiesOutOfEachOther()
{
	for (int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++) {
		for (int entityNum = entityIndex + 1; entityNum < m_allEntities.size(); entityNum++) {
			if (m_allEntities[entityIndex] && m_allEntities[entityNum])
			PushEntityOutOfEachOther(*m_allEntities[entityIndex], *m_allEntities[entityNum]);
		}
	}
}

void Map::PushEntityOutOfEachOther(Entity& entityA, Entity& entityB)
{
	if ((entityA.m_isPushedByEntities && entityB.m_doesPushEntities) && (entityA.m_doesPushEntities && entityB.m_isPushedByEntities)) {
		PushDiscsOutOfEachOther2D(entityA.m_position, entityA.m_physicsRadius, entityB.m_position, entityB.m_physicsRadius);
		return;
	}

	if (entityA.m_isPushedByEntities && entityB.m_doesPushEntities) {
		PushDiscOutOfFixedDisc2D(entityA.m_position, entityA.m_physicsRadius, entityB.m_position, entityB.m_physicsRadius);
		return;
	}

	if (entityA.m_doesPushEntities && entityB.m_isPushedByEntities) {
		PushDiscOutOfFixedDisc2D(entityB.m_position, entityB.m_physicsRadius, entityA.m_position, entityA.m_physicsRadius);
	}
}

int Map::GetTileIndexForTileCoords(int tileX, int tileY) const
{
	return tileX + tileY * m_dimensions.x;
}

IntVec2 Map::GetTileCoordsForWorldPos(Vec2 const& worldPos) const
{
	int tileX = RoundDownToInt(worldPos.x);
	int tileY = RoundDownToInt(worldPos.y);
	return IntVec2(tileX, tileY);
}

IntVec2 Map::GetTileCoordsForTileIndex(int tileIndex) const
{
	int tileX = tileIndex % m_dimensions.x;
	int tileY = (tileIndex - tileX) / m_dimensions.x;
	return IntVec2(tileX, tileY);
}

Vec2 Map::GetTileCenter(int tileX, int tileY) const
{
	float x = static_cast<float> (tileX);
	float y = static_cast<float> (tileY);
	return Vec2(x + 0.5f, y + 0.5f);
}

Vec2 Map::GetRandomWalkableTile() const
{
	int index = g_rng->RollRandomIntInRange(0, (int)m_walkableTiles.size() - 1);
	return m_walkableTiles[index];
}

void Map::SetSprinkleChance(float chance1, float chance2)
{
	m_sprinkle1Chance = chance1;
	m_sprinkle2Chance = chance2;
}

void Map::AddVertsForTile(std::vector<Vertex_PCU>& verts, int tileIndex) const
{
	Tile const& tile = m_tiles[tileIndex];
	AABB2 bounds = tile.GetBounds();
	Rgba8 color = tile.GetColor();
	AABB2 spriteSheetUV = tile.m_tileDef->m_spriteSheetUV;
	AddVertsForAABB2D(verts, bounds, color, spriteSheetUV.m_mins, spriteSheetUV.m_maxs);
}

void Map::AddVertsForHeatMapTile(std::vector<Vertex_PCU>& verts, int tileIndex) const
{
	Tile const& tile = m_tiles[tileIndex];
	AABB2 bounds = tile.GetBounds();

	float color = 0.f;
	if (m_tileHeatMap->m_values[tileIndex] < 9999.f)
		color = RangeMap(m_tileHeatMap->m_maxValue - m_tileHeatMap->m_values[tileIndex], 0.f, m_tileHeatMap->m_maxValue, 50.f, 255.f);
	//Rgba8 color = tile.GetColor();
	unsigned char value = (unsigned char)color;
	AABB2 spriteSheetUV = tile.m_tileDef->m_spriteSheetUV;
	AddVertsForAABB2D(verts, bounds, Rgba8(value, value, value), spriteSheetUV.m_mins, spriteSheetUV.m_maxs);

	
}
