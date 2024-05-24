#define UNUSED(x) (void)(x);
#include "Game/Map.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/AI.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorUID.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/HeatMaps.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Input/InputSystem.hpp"

Map::Map(Game* owner, const char* name) : m_game(owner)
{
	m_mapDef = MapDefinition::GetMapDef(name);
	m_dimensions = m_mapDef->m_image.GetDimensions();
	m_vertexBuffer = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN) * m_dimensions.x * m_dimensions.y * 6 * 4, sizeof(Vertex_PCUTBN));
	m_indexBuffer = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int) * m_dimensions.x * m_dimensions.y * 6 * 6);
	m_texture = &m_mapDef->m_spriteSheet->GetTexture();
	PopulateTiles();
	m_level = (float)m_mapDef->m_level;
	for (int actorIndex = 0; actorIndex < m_mapDef->m_spawnInfos.size(); actorIndex++) {
		Actor* const& actor = SpawnActor(m_mapDef->m_spawnInfos[actorIndex]);
		if (m_mapDef->m_spawnInfos[actorIndex].m_actor == "Elevator") {
			m_elevator = actor;
		}
	}
	//GamePlaying* game = (GamePlaying*)owner;
}

Map::~Map()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
}

void Map::Render(const Camera& camera) const
{
	RenderTiles();

	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++) {
		if (m_actors[actorIndex] != nullptr)
			m_actors[actorIndex]->Render(camera);
	}
}

void Map::Update(float deltaSeconds)
{
	//UNUSED(deltaSeconds);
	for (int actorIndex = 0; actorIndex < m_actors.size(); actorIndex++) {
		Actor* actor = m_actors[actorIndex];
		if (actor != nullptr && actor->m_health > 0) {
			actor->m_isHit = false;
		}
	}
	for (int actorIndex = 0; actorIndex < m_actors.size(); actorIndex++) {
		Actor* actor = m_actors[actorIndex];
		if (actor != nullptr) {
			actor->Update(deltaSeconds);
		}
	}
	CollideActors();
	CollideActorsWithMap();
	DeleteDestroyedActors();
	GamePlaying* owner = (GamePlaying*)m_game;
	
	//if (owner->m_player->GetActor() == nullptr) {
	//	SpawnPlayer();
	//}
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		if (owner->m_players[playerIndex]) {
			if (owner->m_players[playerIndex]->GetActor() == nullptr) {
				owner->m_playerDeath[playerIndex]++;
				SpawnPlayer(playerIndex);
			}
		}
	}
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		if (owner->m_players[playerIndex]) {
			Actor* const& actor = owner->m_players[playerIndex]->GetActor();
			if (actor) {
				//Actor* const& actor = owner->m_players[playerIndex]->GetActor();
				IntVec2 nowCoords = GetCoordsOfActor(actor);
				if (nowCoords == m_elevatorOpen && (m_elevator->m_nowAnimeName != "Openning" && m_elevator->m_nowAnimeName != "Open")) {
					m_elevator->SetNowAnimationByName("Openning");
				}
				else if (nowCoords != m_elevatorOpen && nowCoords != m_elevatorClose && (m_elevator->m_nowAnimeName != "Closing" && m_elevator->m_nowAnimeName != "Closed")) {
					m_elevator->SetNowAnimationByName("Closing");
				}
				//if (m_level < 3.f) {
					//GamePlaying* owner = (GamePlaying*)m_game;
					float goal = owner->m_elevatorGoal;
					if (nowCoords == m_elevatorClose) {
						MoveElevator(GetClamped(goal, m_level + 0.4999f, m_level + 1.5f), deltaSeconds);
					}
					else
					{
						MoveElevator(m_level + 0.5f, deltaSeconds);
					}
				//}
				if (nowCoords == m_elevatorClose) {
					UpdateFromKeyboard();
				}
			}
		}
	}
}

Actor* Map::GetBullet() const
{
	return m_bullet;
}

void Map::UpdateFromKeyboard()
{
	XboxController tempController;
	tempController = g_theInput->GetController(0);
	if (g_theInput->WasKeyJustPressed('E') || tempController.GetButton(XBOX_BUTTON_B).WasKeyJustPressed()) {
		if (m_elevator->m_nowAnimeName != "Openning")
			m_elevator->SetNowAnimationByName("Openning");
		else
		{
			
			GamePlaying* owner = (GamePlaying*)m_game;
			m_elevator->SetNowAnimationByName("Closing");
			if (m_level == 0.f)
			{
				owner->m_elevatorGoal = 3.5f;
			}
			if (m_level == 3.f) {
				owner->m_elevatorGoal = 0.5f;
			}
		}
	}
}

void Map::RenderTiles() const
{
	g_theRenderer->SetModelConstants();
	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->BindShader(m_mapDef->m_shader);
	unsigned char a = DenormalizeByte(m_ambientIntensity);
	Rgba8 color = Rgba8(a, a, a);
	//color.a = a;
	g_theRenderer->SetLightConstants(m_sunDirection, m_sunIntensity,
		color);
			//+ 50.f * CosDegrees(m_game->m_clock->GetTotalSeconds() * 50.f), 200));
	std::vector<Vertex_PCUTBN> elevator;
	AddVertsForElevator(elevator, m_elevatorTile);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	GamePlaying* owner = (GamePlaying*)m_game;
	if (owner->m_currentMap == this)
		g_theRenderer->DrawVertexArray((int)elevator.size(), elevator.data());
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->DrawVertexIndexArray((int)m_vertexes.size(), (int)m_indexes.size(), m_vertexes.data(), m_indexes.data(), m_vertexBuffer, m_indexBuffer);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->BindShader(nullptr);
	
}

void Map::PopulateTiles()
{
	m_tiles.resize(m_dimensions.x * m_dimensions.y);

	for (int xIndex = 0; xIndex < m_dimensions.x; xIndex++) {
		for (int yIndex = 0; yIndex < m_dimensions.y; yIndex++) {
			int tileIndex = xIndex + yIndex * m_dimensions.x;
			// tileDef
			Rgba8 pixelColor = m_mapDef->m_image.GetTexelColor(IntVec2(xIndex, yIndex));
			for (int tileDefIndex = 0; tileDefIndex < TileDefinition::s_tileDefs.size(); tileDefIndex++) {
				if (pixelColor == TileDefinition::s_tileDefs[tileDefIndex]->m_mapImagePixelColor) {
					m_tiles[tileIndex].m_tileDef = TileDefinition::s_tileDefs[tileDefIndex];
					if (m_tiles[tileIndex].m_tileDef->m_name == "ElevatorOpen")
						m_elevatorOpen = IntVec2(xIndex, yIndex);
					if (m_tiles[tileIndex].m_tileDef->m_name == "ElevatorClose" || m_tiles[tileIndex].m_tileDef->m_name == "Pipe") {
						m_elevatorClose = IntVec2(xIndex, yIndex);
						m_elevatorTile = tileIndex;
					}
						//m_elevatorClose = IntVec2(xIndex, yIndex);
					break;
				}
			}
			// bounds
			float level = (float)m_mapDef->m_level;
			AABB3 bounds = AABB3(Vec3((float)xIndex, (float)yIndex, level), Vec3((float)(xIndex + 1), (float)(yIndex + 1), level + 1.f));
			m_tiles[tileIndex].m_bounds = bounds;
		}
	}

	for (int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++) {
		AddVertsForTile(m_vertexes, m_indexes, tileIndex);
	}
}

void Map::AddVertsForTile(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, int tileIndex) const
{
	Tile const& tile = m_tiles[tileIndex];
	if (tile.m_tileDef->m_name == "Null" || tile.m_tileDef->m_name == "ElevatorClose")
		return;
	AABB3 bounds = tile.GetBounds();

	float minX = bounds.m_mins.x;
	float minY = bounds.m_mins.y;
	float minZ = bounds.m_mins.z;
	float maxX = bounds.m_maxs.x;
	float maxY = bounds.m_maxs.y;
	float maxZ = bounds.m_maxs.z;

	if (tile.m_tileDef->m_wallSpriteCoords != IntVec2(-1, -1)) {
		AABB2 wallUV = m_mapDef->m_spriteSheet->GetSpriteUVs(tile.m_tileDef->m_wallSpriteCoords);
		// +x
		AddVertsForQuad3D(verts, indexes, Vec3(maxX, minY, maxZ), Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Rgba8::WHITE, wallUV);
		// -x
		AddVertsForQuad3D(verts, indexes, Vec3(minX, maxY, maxZ), Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Rgba8::WHITE, wallUV);
		// +y
		AddVertsForQuad3D(verts, indexes, Vec3(maxX, maxY, maxZ), Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Rgba8::WHITE, wallUV);
		// -y
		AddVertsForQuad3D(verts, indexes, Vec3(minX, minY, maxZ), Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Rgba8::WHITE, wallUV);
	}
	if (tile.m_tileDef->m_ceilingSpriteCoords != IntVec2(-1, -1)) {
		AABB2 ceilingUV = m_mapDef->m_spriteSheet->GetSpriteUVs(tile.m_tileDef->m_ceilingSpriteCoords);
		// +z
		AddVertsForQuad3D(verts, indexes, Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Rgba8::WHITE, ceilingUV);
	}
	if (tile.m_tileDef->m_floorSpriteCoords != IntVec2(-1, -1)) {
		AABB2 floorUV = m_mapDef->m_spriteSheet->GetSpriteUVs(tile.m_tileDef->m_floorSpriteCoords);
		// -z
		AddVertsForQuad3D(verts, indexes, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Rgba8::WHITE, floorUV);
	}
	//AABB2 spriteSheetUV = tile.m_tileDef->m_spriteSheetUV;
	//AddVertsForAABB2D(verts, bounds, color, spriteSheetUV.m_mins, spriteSheetUV.m_maxs);
}

void Map::AddVertsForElevator(std::vector<Vertex_PCUTBN>& verts, int tileIndex) const
{
	Tile tile = m_tiles[tileIndex];
	tile.m_tileDef = TileDefinition::GetTileDef("ElevatorClose");
	AABB3 bounds = tile.GetBounds();

	float minX = bounds.m_mins.x;
	float minY = bounds.m_mins.y;
	float minZ = bounds.m_mins.z;
	float maxX = bounds.m_maxs.x;
	float maxY = bounds.m_maxs.y;
	float maxZ = bounds.m_maxs.z;

	if (tile.m_tileDef->m_wallSpriteCoords != IntVec2(-1, -1)) {
		AABB2 wallUV = m_mapDef->m_spriteSheet->GetSpriteUVs(tile.m_tileDef->m_wallSpriteCoords);
		//// +x
		//AddVertsForQuad3D_PNCU(verts, Vec3(maxX, minY, maxZ), Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Rgba8::WHITE, wallUV);
		//// -x
		//AddVertsForQuad3D_PNCU(verts, Vec3(minX, maxY, maxZ), Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Rgba8::WHITE, wallUV);
		//// +y
		//AddVertsForQuad3D_PNCU(verts, Vec3(maxX, maxY, maxZ), Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Rgba8::WHITE, wallUV);
		//// -y
		//AddVertsForQuad3D_PNCU(verts, Vec3(minX, minY, maxZ), Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Rgba8::WHITE, wallUV);		
		// +x
		AddVertsForQuad3D_PNCU(verts, Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ),  Vec3(maxX, maxY, maxZ), Vec3(maxX, maxY, minZ), Rgba8::WHITE, wallUV);
		// -x
		AddVertsForQuad3D_PNCU(verts, Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Vec3(minX, minY, maxZ), Vec3(minX, minY, minZ), Rgba8::WHITE, wallUV);
		// +y
		AddVertsForQuad3D_PNCU(verts, Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), Vec3(minX, maxY, minZ), Rgba8::WHITE, wallUV);
		// -y
		AddVertsForQuad3D_PNCU(verts, Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, minY, minZ), Rgba8::WHITE, wallUV);
	}
	if (tile.m_tileDef->m_ceilingSpriteCoords != IntVec2(-1, -1)) {
		AABB2 ceilingUV = m_mapDef->m_spriteSheet->GetSpriteUVs(tile.m_tileDef->m_ceilingSpriteCoords);
		// +z
		AddVertsForQuad3D_PNCU(verts, Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Rgba8::WHITE, ceilingUV);
	}
	if (tile.m_tileDef->m_floorSpriteCoords != IntVec2(-1, -1)) {
		AABB2 floorUV = m_mapDef->m_spriteSheet->GetSpriteUVs(tile.m_tileDef->m_floorSpriteCoords);
		// -z
		AddVertsForQuad3D_PNCU(verts, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Rgba8::WHITE, floorUV);
	}
}

bool Map::IsPositionInBounds(Vec3 position, const float tolerance) const
{
	int x = RoundDownToInt(position.x);
	int y = RoundDownToInt(position.y);
	if (AreCoordsInBounds(x, y) && position.z > -tolerance && position.z < 1.f + tolerance)
		return true;
	return false;
}

bool Map::AreCoordsInBounds(int x, int y) const
{
	if (x < 0 || x >= m_dimensions.x)
		return false;
	if (y < 0 || y >= m_dimensions.y)
		return false;
	return true;
}

const Tile* Map::GetTile(int x, int y) const
{
	int index = x + y * m_dimensions.x;
	return &m_tiles[index];
}

IntVec2 Map::GetCoordsOfActor(Actor* actor)
{
	int x = RoundDownToInt(actor->m_position.x);
	int y = RoundDownToInt(actor->m_position.y);
	return IntVec2(x, y);
}

IntVec2 Map::GetCoordsOfPoint(Vec3 point) const
{
	int x = RoundDownToInt(point.x);
	int y = RoundDownToInt(point.y);
	return IntVec2(x, y);
}

void Map::CollideActors()
{
	for (int i = 0; i < (int)m_actors.size(); i++) {
		for (int j = i + 1; j < (int)m_actors.size(); j++) {
			if (m_actors[i] != nullptr && m_actors[j] != nullptr)
				CollideActors(m_actors[i], m_actors[j]);
		}
	}
}

void Map::CollideActors(Actor* actorA, Actor* actorB)
{
	if (actorA == actorB->m_owner || actorB == actorA->m_owner)
		return;
	if (!actorA->m_actorDef->m_isCollidesWithActors && !actorB->m_actorDef->m_isCollidesWithActors)
		return;
	if (actorA->m_actorDef->m_name == "SpawnPoint" || actorB->m_actorDef->m_name == "SpawnPoint")
		return;
	if (actorA->m_isDead || actorB->m_isDead)
		return;

	// if is overlap
	Vec2 centerA = actorA->m_position.GetVec2XY();
	Vec2 centerB = actorB->m_position.GetVec2XY();
	bool isDiscOverlap = false;
	bool isZOverlap = false;

	if (DoDiscsOverlap(centerA, actorA->m_actorDef->m_radius, centerB, actorB->m_actorDef->m_radius)) {
		isDiscOverlap = true;
	}
	if (actorA->GetZFloatRange().IsOverlappingWith(actorB->GetZFloatRange())) {
		isZOverlap = true;
	}
	if (!isDiscOverlap || !isZOverlap)
		return;

	if (isDiscOverlap) {
		if (!actorA->m_actorDef->m_isCollidesWithActors) {
			PushDiscOutOfFixedDisc2D(centerB, actorB->m_actorDef->m_radius, centerA, actorA->m_actorDef->m_radius);
		}
			
		else if (!actorB->m_actorDef->m_isCollidesWithActors) {
			PushDiscOutOfFixedDisc2D(centerA, actorA->m_actorDef->m_radius, centerB, actorB->m_actorDef->m_radius);
		}
			
		else {
			PushDiscsOutOfEachOther2D(centerA, actorA->m_actorDef->m_radius, centerB, actorB->m_actorDef->m_radius);
		}
		actorA->OnCollide(actorB);
		actorB->OnCollide(actorA);
		actorA->m_position.x = centerA.x;
		actorA->m_position.y = centerA.y;
		actorB->m_position.x = centerB.x;
		actorB->m_position.y = centerB.y;
		return;
	}
}

void Map::CollideActorsWithMap()
{
	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++) {
		if (m_actors[actorIndex] != nullptr && m_actors[actorIndex]->m_actorDef->m_isCollidesWithWorld)
			CollideActorWithMap(m_actors[actorIndex]);
	}
}

void Map::CollideActorWithMap(Actor* actor)
{
	if (actor->m_isDead)
		return;
	IntVec2 coords = GetCoordsOfActor(actor);
	
	CollideActorWithTile(actor, coords.x, coords.y);

	CollideActorWithTile(actor, coords.x - 1, coords.y);
	CollideActorWithTile(actor, coords.x, coords.y - 1);
	CollideActorWithTile(actor, coords.x + 1, coords.y);
	CollideActorWithTile(actor, coords.x, coords.y + 1);

	CollideActorWithTile(actor, coords.x - 1, coords.y - 1);
	CollideActorWithTile(actor, coords.x - 1, coords.y + 1);
	CollideActorWithTile(actor, coords.x + 1, coords.y - 1);
	CollideActorWithTile(actor, coords.x + 1, coords.y + 1);

	//float temp = actor->m_position.z;
	if (GetCoordsOfActor(actor) != m_elevatorClose) {
		float level = m_level;
		actor->m_position.z = GetClamped(actor->m_position.z, level, level + 1.f - actor->m_actorDef->m_height);
		if (!actor->m_actorDef->m_isFlying)
			actor->m_position.z = level;
		if (actor->m_position.z == level || actor->m_position.z == level + 1.f - actor->m_actorDef->m_height)
		{
			actor->OnCollide(nullptr);
		}
	}
	else
	{
		//const Tile* tile = GetTile(m_elevatorClose.x, m_elevatorClose.y);
		Tile tile = m_tiles[m_elevatorTile];
		//float level = m_level;
		actor->m_position.z = tile.m_bounds.m_mins.z;
			//GetClamped(actor->m_position.z, tile.m_bounds.m_mins.z, level + 1.f - actor->m_actorDef->m_height);
	}
		
}

void Map::CollideActorWithTile(Actor* actor, int x, int y)
{
	Vec2 center = actor->m_position.GetVec2XY();
	if (AreCoordsInBounds(x, y)) {
		const Tile* tempTile = GetTile(x, y);
		if (tempTile->IsSolid()) {
			
			PushDiscOutOfFixedAABB2D(center, actor->m_actorDef->m_radius, tempTile->GetBounds().GetAABB2XY());
			Vec2 preXY = actor->m_position.GetVec2XY();
			actor->m_position.x = center.x;
			actor->m_position.y = center.y;
			if (preXY != center)
			{
				if (actor->m_actorDef->m_name == "PlasmaProjectile")
				{
					;
				}
				actor->OnCollide(nullptr);
			}
				
		}
	}
}

void Map::DeleteDestroyedActors()
{
	for (int actorIndex = 0; actorIndex < m_actors.size(); actorIndex++) {
		Actor* actor = m_actors[actorIndex];
		if (actor != nullptr && actor->m_isGarbage) {
			delete actor;
			m_actors[actorIndex] = nullptr;
		}
	}
}

RaycastResult3D Map::RaycastAll(const Vec3& start, const Vec3& direction, float distance) const
{
	RaycastResult3D results[3], result;
	results[0] = RaycastWorldXY(start, direction, distance);
	results[1] = RaycastWorldZ(start, direction, distance);
	results[2] = RaycastWorldActors(start, direction, distance);
	float minDist = distance;
	for (int resultIndex = 0; resultIndex < 3; resultIndex++) {
		if (!results[resultIndex].m_didImpact)
			continue;
		if (results[resultIndex].m_impactDis < minDist) {
			result = results[resultIndex];
			minDist = result.m_impactDis;
		}
	}
	return result;
}

RaycastResult3D Map::RaycastAll(const Vec3& start, const Vec3& direction, float distance, Actor*& actor) const
{
	RaycastResult3D results[3], result;
	results[0] = RaycastWorldXY(start, direction, distance);
	results[1] = RaycastWorldZ(start, direction, distance);
	results[2] = RaycastWorldActors(start, direction, distance, actor);
	float minDist = distance;
	for (int resultIndex = 0; resultIndex < 3; resultIndex++) {
		if (!results[resultIndex].m_didImpact)
			continue;
		if (results[resultIndex].m_impactDis < minDist) {
			result = results[resultIndex];
			minDist = result.m_impactDis;
		}
	}
	if (result.m_impactDis != results[2].m_impactDis)
		actor = nullptr;
	return result;
}


void Map::SpawnPlayer(int playerIndex)
{
	std::vector<Actor*> spawnPoints;
	for (int actorIndex = 0; actorIndex < m_actors.size(); actorIndex++) {
		if (m_actors[actorIndex] != nullptr) {
			if (m_actors[actorIndex]->m_actorDef->m_name == "SpawnPoint") {
				spawnPoints.push_back(m_actors[actorIndex]);
			}
		}
	}
	if (spawnPoints.size() > 0) {
		int index = g_rng->RollRandomIntInRange(0, (int)spawnPoints.size() - 1);
		SpawnInfo spawnInfo;
		spawnInfo.m_actor = "Marine";
		spawnInfo.m_position = spawnPoints[index]->m_position;
		spawnInfo.m_orientation = spawnPoints[index]->m_orientation;
		spawnInfo.m_position.z = 0.f;
		Actor* player = SpawnActor(spawnInfo);
		GamePlaying* owner = (GamePlaying*)m_game;
		owner->m_players[playerIndex]->m_map = this;
		owner->m_players[playerIndex]->m_position = player->m_position;
		owner->m_players[playerIndex]->Possess(player->m_UID);
	}
	
}

Actor* Map::SpawnActor(SpawnInfo spawnInfo)
{
	int index = -1;
	for (int actorIndex = 0; actorIndex < m_actors.size(); actorIndex++) {
		if (m_actors[actorIndex] == nullptr) {
			index = actorIndex;
			break;
		}
	}
	if (index == -1) {
		index = (int)m_actors.size();
		m_actors.push_back(nullptr);
	}
	Actor* actor = new Actor(this, spawnInfo.m_actor.c_str(), ActorUID(m_currentSalt, index));
	m_currentSalt++;
	spawnInfo.m_position.z += m_level;
	actor->m_position = spawnInfo.m_position;
	actor->m_orientation = spawnInfo.m_orientation;
	actor->m_velocity = spawnInfo.m_velocity;
	m_actors[index] = actor;
	if (actor->m_AIController != nullptr) {
		actor->m_AIController->Possess(actor->m_UID);
	}
	return actor;
}

Actor* Map::GetActorByUID(ActorUID uid)
{
	int index = uid.GetIndex();
	if (index >= (int)m_actors.size() || index < 0)
		return nullptr;
	if (m_actors[index] != nullptr && m_actors[index]->m_UID == uid) {
		return m_actors[index];
	}
	return nullptr;
}

Actor* Map::GetClosetVisibleEnemy(Actor const* actor)
{
	std::vector<Actor*> insideSight;
	for (int actorIndex = 0; actorIndex < m_actors.size(); actorIndex++) {
		if (m_actors[actorIndex] != nullptr) {
			Actor* temp = m_actors[actorIndex];
			// if not an enemy
			if (temp->m_actorDef->m_faction == actor->m_actorDef->m_faction || !temp->m_actorDef->m_isVisible || temp->m_actorDef->m_faction == "")
				continue;
			else {
				// if inside the sector
				if (IsPointInsideOrientedSector2D(temp->m_position.GetVec2XY(), actor->m_position.GetVec2XY(), 
					actor->m_orientation.m_yawDegrees, actor->m_actorDef->m_sightAngle, actor->m_actorDef->m_sightRadius)) {
					insideSight.push_back(temp);
				}
			}
		}
	}
	// if visible
	float minDistance = actor->m_actorDef->m_sightRadius;
	Actor* closetActor = nullptr;
	RaycastResult3D result;
	for (int actorIndex = 0; actorIndex < (int)insideSight.size(); actorIndex++) {
		Actor* ac = insideSight[actorIndex];
		Vec3 position = actor->m_position;
		position.z = actor->m_actorDef->m_eyeHeight + m_level;
		RaycastResult3D tempActor = RaycastVsCylinderZ3D(position, (ac->m_position - position).GetNormalized(),
			actor->m_actorDef->m_sightRadius, ac->m_position.GetVec2XY(),
			ac->GetZFloatRange().m_min, ac->GetZFloatRange().m_max, ac->m_actorDef->m_radius);
		if (tempActor.m_didImpact && tempActor.m_impactDis < minDistance) {
			Vec3 eye = actor->m_position;
			eye.z = actor->m_actorDef->m_eyeHeight + m_level;
			RaycastResult3D tempAll = RaycastAll(eye, (ac->m_position - eye).GetNormalized(), actor->m_actorDef->m_sightRadius);
			//DebugAddWorldLine(eye, eye + 2.f * (ac->m_position - actor->m_position).GetNormalized(), 0.01f, 100.f, Rgba8::WHITE, Rgba8::WHITE, DebugRenderMode::X_RAY);
			if (!tempAll.m_didImpact || (tempActor.m_impactDis <= tempAll.m_impactDis && tempActor.m_impactDis < minDistance)) {
				result = tempActor;
				minDistance = tempActor.m_impactDis;
				closetActor = ac;
			}
				//continue;
		}
	}

	return closetActor;
}

void Map::DebugPossessNext()
{
	GamePlaying* game = (GamePlaying*)m_game;
	int index = game->m_players[0]->GetActor()->m_UID.GetIndex();
	int start = index;
	while (1) {
		index = (index + 1) % m_actors.size();
		if (m_actors[index] != nullptr && m_actors[index]->m_actorDef->m_canBePossessed) {
			game->m_players[0]->Possess(m_actors[index]->m_UID);
			break;
		}
		if (index == start)
			break;
	}
}

void Map::MoveElevator(float goalCenter, float deltaSeconds)
{
	float nowCenter = m_tiles[m_elevatorTile].m_bounds.GetCenter().z;
	if (goalCenter < nowCenter) {
		nowCenter -= deltaSeconds;
		nowCenter = GetClamped(nowCenter, goalCenter, nowCenter);
	}
	else
	{
		nowCenter += deltaSeconds;
		nowCenter = GetClamped(nowCenter, nowCenter, goalCenter);
	}
	Vec3 center = m_tiles[m_elevatorTile].m_bounds.GetCenter();
	center.z = nowCenter;
	m_tiles[m_elevatorTile].m_bounds.SetCenter(center);
}

RaycastResult3D Map::RaycastWorldXY(const Vec3& start, const Vec3& direction, float distance) const
{
	RaycastResult3D result;

	IntVec2 startCoords = GetCoordsOfPoint(start);
	int tileX = startCoords.x;
	int tileY = startCoords.y;
	if (GetTile(startCoords.x, startCoords.y)->IsSolid() && (start.z > m_level && start.z < 1.f + m_level)) {
		result.m_didImpact = true;
		result.m_impactPos = start;
		result.m_impactNormal = -direction;
		return result;
	}
	float fwdDistPerXCrossing = 0.f;
	int tileStepDirX = 0;
	float xAtFirstXCrossing = (float)tileX;
	float xDistToFirstXCrossing = 0.f;
	float fwdDistAtNextXCrossing = 99999.f;
	if (direction.x != 0.f) {
		fwdDistPerXCrossing = 1.f / abs(direction.x);
		if (direction.x < 0.f)
			tileStepDirX = -1;
		else tileStepDirX = 1;
		xAtFirstXCrossing += (tileStepDirX + 1) / 2;
		xDistToFirstXCrossing = xAtFirstXCrossing - start.x;
		fwdDistAtNextXCrossing = fabsf(xDistToFirstXCrossing) * fwdDistPerXCrossing;
	}

	float fwdDistPerYCrossing = 0.f;
	int tileStepDirY = 0;
	float yAtFirstYCrossing = (float)tileY;
	float yDistToFirstYCrossing = 0.f;
	float fwdDistAtNextYCrossing = 99999.f;
	if (direction.y != 0.f) {
		fwdDistPerYCrossing = 1.f / abs(direction.y);
		if (direction.y < 0.f)
			tileStepDirY = -1;
		else tileStepDirY = 1;
		yAtFirstYCrossing += (tileStepDirY + 1) / 2;
		yDistToFirstYCrossing = yAtFirstYCrossing - start.y;
		fwdDistAtNextYCrossing = fabsf(yDistToFirstYCrossing) * fwdDistPerYCrossing;
	}
	while (1) {
		if (fwdDistAtNextXCrossing < fwdDistAtNextYCrossing) {
			if (fwdDistAtNextXCrossing > distance) {
				return result;
			}
			tileX += tileStepDirX;
			if (!AreCoordsInBounds(tileX, tileY)) {
				return result;
			}
			float nowZ = start.z + direction.z * fwdDistAtNextXCrossing;
			if (GetTile(tileX, tileY)->IsSolid() && nowZ > m_level && nowZ < 1.f + m_level) {
				result.m_didImpact = true;
				result.m_impactDis = fwdDistAtNextXCrossing;
				result.m_impactPos = start + direction * fwdDistAtNextXCrossing;
				if (tileStepDirX > 0)
					result.m_impactNormal = Vec3(-1.f, 0.f, 0.f);
				else
					result.m_impactNormal = Vec3(1.f, 0.f, 0.f);
				return result;
			}
			fwdDistAtNextXCrossing += fwdDistPerXCrossing;
		}
		else {
			if (fwdDistAtNextYCrossing > distance) {
				return result;
			}
			tileY += tileStepDirY;
			if (!AreCoordsInBounds(tileX, tileY)) {
				return result;
			}
			float nowZ = start.z + direction.z * fwdDistAtNextYCrossing;
			if (GetTile(tileX, tileY)->IsSolid() && nowZ > m_level && nowZ < 1.f + m_level) {
				result.m_didImpact = true;
				result.m_impactDis = fwdDistAtNextYCrossing;
				result.m_impactPos = start + direction * fwdDistAtNextYCrossing;
				if (tileStepDirY > 0)
					result.m_impactNormal = Vec3(0.f, -1.f, 0.f);
				else
					result.m_impactNormal = Vec3(0.f, 1.f, 0.f);
				return result;
			}
			fwdDistAtNextYCrossing += fwdDistPerYCrossing;
		}
	}
}

RaycastResult3D Map::RaycastWorldZ(const Vec3& start, const Vec3& direction, float distance) const
{
	RaycastResult3D result;
	float level = (float)m_mapDef->m_level;
	if (direction.z == level)
		return result;
	if (direction.z > level) {
		float t = (1.f + level - start.z) / direction.z;
		if (t > 0.f && t < distance) {
			result.m_didImpact = true;
			result.m_impactNormal = Vec3(0.f, 0.f, -1.f);
			result.m_impactPos = start + t * direction;
			result.m_impactDis = GetDistance3D(result.m_impactPos, start);
		}
	}
	else {
		float t = -start.z / direction.z;
		if (t > 0.f && t < distance) {
			result.m_didImpact = true;
			result.m_impactNormal = Vec3(0.f, 0.f, 1.f);
			result.m_impactPos = start + t * direction;
			result.m_impactDis = GetDistance3D(result.m_impactPos, start);
		}
	}
	return result;
}

RaycastResult3D Map::RaycastWorldActors(const Vec3& start, const Vec3& direction, float distance) const
{
	float minDistance = distance;
	RaycastResult3D result;
	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++) {
		const Actor* actor = m_actors[actorIndex];
		if (actor != nullptr) {
			RaycastResult3D temp = RaycastVsCylinderZ3D(start, direction, distance,
			actor->m_position.GetVec2XY(), actor->GetZFloatRange().m_min, actor->GetZFloatRange().m_max, actor->m_actorDef->m_radius);
			if (temp.m_didImpact && temp.m_impactDis < minDistance && temp.m_impactDis != 0.f) {
				result = temp;
				minDistance = temp.m_impactDis;
			}
		}
		
	}
	return result;
}


RaycastResult3D Map::RaycastWorldActors(const Vec3& start, const Vec3& direction, float distance, Actor*& impactActor) const
{
	float minDistance = distance;
	RaycastResult3D result;
	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++) {
		Actor* actor = m_actors[actorIndex];
		if (actor != nullptr) {
			RaycastResult3D temp = RaycastVsCylinderZ3D(start, direction, distance,
			actor->m_position.GetVec2XY(), actor->GetZFloatRange().m_min, actor->GetZFloatRange().m_max, actor->m_actorDef->m_radius);
			if (temp.m_didImpact && temp.m_impactDis < minDistance && temp.m_impactDis != 0.f) {
				result = temp;
				minDistance = temp.m_impactDis;
				impactActor = actor;
			}
		}
		
	}
	return result;
}
