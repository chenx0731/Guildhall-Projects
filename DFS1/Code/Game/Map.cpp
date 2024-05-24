#define UNUSED(x) (void)(x);
#include "Game/Map.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/GameMapSelection.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/AI.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorUID.hpp"
#include "Game/OBJ_Loader.h"
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
#include "Engine/Math/IntVec3.hpp"

Map::Map(Game* owner, const char* name) : m_game(owner)
{
	m_mapDef = MapDefinition::GetMapDef(name);
	//m_dimensions = m_mapDef->m_image.GetDimensions();
	//m_vertexBuffer = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PNCU) * m_dimensions.x * m_dimensions.y * 6 * 4, sizeof(Vertex_PNCU));
	//m_indexBuffer = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int) * m_dimensions.x * m_dimensions.y * 6 * 6);
	m_texture = &m_mapDef->m_spriteSheet->GetTexture();
	PopulateTiles();
	m_level = (float)m_mapDef->m_level;
	m_gravity = m_mapDef->m_gravity;
	for (int actorIndex = 0; actorIndex < m_mapDef->m_spawnInfos.size(); actorIndex++) {
		Actor* const& actor = SpawnActor(m_mapDef->m_spawnInfos[actorIndex]);
		if (m_mapDef->m_spawnInfos[actorIndex].m_actor == "Elevator") {
			m_elevatorDoors.push_back(actor);
		}
		if (m_mapDef->m_spawnInfos[actorIndex].m_actor == "DogBlock") {
			m_dogActor = actor;
		}
		if (m_mapDef->m_spawnInfos[actorIndex].m_actor == "ElevatorBlock") {
			m_elevators.push_back(actor);
		}
		if (m_mapDef->m_spawnInfos[actorIndex].m_actor == "ShipBlock") {
			m_shipActor = actor;
		}
	
	}
	//GamePlaying* game = (GamePlaying*)owner;

	for (int index = 0; index < (int)g_OBJLoader->LoadedVertices.size(); index++) {
		objl::Vertex temp = g_OBJLoader->LoadedVertices[index];
		m_dogVerts.emplace_back(Vertex_PCUTBN(Vec3(temp.Position.X, temp.Position.Y, temp.Position.Z),
			Rgba8::WHITE, Vec2(),
			Vec3(temp.Normal.X, temp.Normal.Y, temp.Normal.Z)
		));
	}
	m_dogIndexes = g_OBJLoader->LoadedIndices;

	m_dogVertexBuffer = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN) * m_dogVerts.size(), sizeof(Vertex_PCUTBN));
	m_dogIndexBuffer = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int) * m_dogIndexes.size());
}

Map::~Map()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;

	delete m_dogVertexBuffer;
	delete m_dogIndexBuffer;
	m_dogVertexBuffer = nullptr;
	m_dogIndexBuffer = nullptr;
}

void Map::Render(const Camera& camera) const
{
	RenderTiles();

	g_theRenderer->SetModelConstants(GetDogModelMatrix(), Rgba8::WHITE);
	g_theRenderer->BindShader(m_mapDef->m_shader);
	//g_theRenderer->SetLightConstants(Vec3(2.f, 1.f, -1.f), 0.8f, Rgba8(50, 123, 123));
	g_theRenderer->SetLightConstants(m_sunDirection, m_sunIntensity,
		Rgba8(0, 75, 75));
	g_theRenderer->DrawVertexIndexArray((int)m_dogVerts.size(), (int)m_dogIndexes.size(), m_dogVerts.data(), m_dogIndexes.data(), m_dogVertexBuffer, m_dogIndexBuffer);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->SetModelConstants();

	RenderStarship();

	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++) {
		if (m_actors[actorIndex] != nullptr)
			m_actors[actorIndex]->Render(camera);
	}
}

void Map::Update(float deltaSeconds)
{
	//UNUSED(deltaSeconds);
	UpdateFromKeyboard();
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
	
	if (m_isRiding) {

		m_dogOrientation = owner->m_players[0]->m_orientation;
		m_dogOrientation.m_pitchDegrees = 0.f;
		m_dogOrientation.m_rollDegrees = 0.f;
	}

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
				if (m_shipActor && GetDistanceSquared2D(actor->m_position.GetVec2XY(), m_shipActor->m_position.GetVec2XY())) {
					//m_gamePlaying->m_currentMapName = "Saturn";
					//GamePlaying* owner = (GamePlaying*)m_game;
					//owner->m_state = GameState::RESULT;
					
				}
				for (int doorIndex = 0; doorIndex < (int)m_elevatorDoors.size(); doorIndex++) {
					Actor*& door = m_elevatorDoors[doorIndex];
					if (door) {
						if (GetDistanceSquared2D(actor->m_position.GetVec2XY(), door->m_position.GetVec2XY()) < 0.5f) {
							if (door->m_nowAnimeName != "Openning" && door->m_nowAnimeName != "Open")
								door->SetNowAnimationByName("Openning");
						}
						else if (door->m_nowAnimeName != "Closing" && door->m_nowAnimeName != "Closed") {
							door->SetNowAnimationByName("Closing");
						}
					}
				}
			}
		}
	}

	MoveElevator(deltaSeconds);
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

		SetElevatorGoalLayer(4.f);
		
	}
	if (g_theInput->WasKeyJustPressed('Q')) {
		SetElevatorGoalLayer(0.f);
	}
	GamePlaying* owner = (GamePlaying*)m_game;
	if (g_theInput->WasKeyJustPressed('R')) {
		if (m_isRiding) {
			m_isRiding = false;
			//owner->m_players[0]->Possess(m_playerActor->m_UID);
			owner->m_players[0]->GetActor()->m_isRiding = false;
			owner->m_players[0]->GetActor()->m_ridingActor = nullptr;
			//m_dogPosition.z = 0.f;
		}
		else if (GetDistanceSquared2D(owner->m_players[0]->m_position.GetVec2XY(), m_dogActor->m_position.GetVec2XY()) < 0.8f) {
			owner->m_players[0]->GetActor()->m_isRiding = true;
			owner->m_players[0]->GetActor()->m_ridingActor = m_dogActor;
			//owner->m_players[0]->Possess(m_dogActor->m_UID);
			
			m_isRiding = true;
		}
	}
}

void Map::RenderTiles() const
{
	g_theRenderer->SetModelConstants();
	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->BindShader(m_mapDef->m_shader);
	//unsigned char a = DenormalizeByte(m_ambientIntensity);
	//Rgba8 color = Rgba8(a, a, a);
	//color.a = a;
	g_theRenderer->SetLightConstants(m_sunDirection, m_sunIntensity,
		m_mapDef->m_lightColor);
			//+ 50.f * CosDegrees(m_game->m_clock->GetTotalSeconds() * 50.f), 200));
	//std::vector<Vertex_PNCU> elevator;
	//AddVertsForElevator(elevator, m_elevatorTile);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	//if (owner->m_currentMap == this)
		//g_theRenderer->DrawVertexArray((int)elevator.size(), elevator.data());
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->DrawVertexIndexArray((int)m_vertexes.size(), (int)m_indexes.size(), m_vertexes.data(), m_indexes.data(), m_vertexBuffer, m_indexBuffer);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->BindShader(nullptr);
	
}

void Map::PopulateTiles()
{

	for (auto it = m_mapDef->m_voxelDefs.begin(); it != m_mapDef->m_voxelDefs.end(); it++) {
		Vec3 offset = it->first;
		std::string def = it->second;
		Strings datas = SplitStringOnDelimiter(def, "data\":[[");
		std::string data = datas[1];
		Strings substrs = SplitStringOnDelimiter(data, "],[");
		for (int subIndex = 0; subIndex < (int)substrs.size(); subIndex++) {
			Strings details = SplitStringOnDelimiter(substrs[subIndex], "\"");
			IntVec3 temp, coord;
			temp.SetFromText(details[0].c_str());
			coord.x = temp.x;
			coord.y = temp.z;
			coord.z = temp.y;
			Rgba8 color;
			color.r = (unsigned char)stoi(details[1].substr(1, 2), nullptr, 16);
			color.g = (unsigned char)stoi(details[1].substr(3, 2), nullptr, 16);
			color.b = (unsigned char)stoi(details[1].substr(5, 2), nullptr, 16);
			color.a = 255;
			Tile tile;
			tile.m_tileDef = TileDefinition::GetTileDefByColor(color);
			AABB3 bounds = AABB3(Vec3((float)coord.x, (float)coord.y, (float)coord.z), Vec3(float(coord.x + 1), float(coord.y + 1), float(coord.z + 1)));
			bounds.Translate(offset);
			tile.m_bounds = bounds;
			m_tiles.push_back(tile);
			//m_blocks.emplace_back(BlockTemplateEntry(BlockDef::GetBlockDefByColor(color), coord));
		}
	}

	for (int index = 0; index < m_tiles.size(); index++) {
		AddVertsForTile(m_vertexes, m_indexes, index);
	}

	m_vertexBuffer = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN) * m_vertexes.size(), sizeof(Vertex_PCUTBN));
	m_indexBuffer = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int) * m_indexes.size());
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

void Map::RenderStarship() const
{
	GameMapSelection* selection =(GameMapSelection*) g_theGames[(int)GameState::MAP_SELECT];
	g_theRenderer->SetModelConstants(GetShipModelMatrix(), Rgba8::WHITE);
	g_theRenderer->BindShader(selection->m_shader);
	g_theRenderer->SetLightConstants(m_sunDirection, m_sunIntensity, Rgba8(0, 0, 50));
	g_theRenderer->DrawVertexIndexArray((int)selection->m_objVerts.size(), (int)selection->m_objIndexes.size(), selection->m_objVerts.data(), selection->m_objIndexes.data(), selection->m_vertexBuffer, selection->m_indexBuffer);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->SetModelConstants();

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

Mat44 Map::GetDogModelMatrix() const
{
	Mat44 mat;
	if (m_dogActor) {
		mat = m_dogOrientation.GetMatrix_XFwd_YLeft_ZUp();
		mat.AppendXRotation(90.f);
		mat.AppendYRotation(90.f);

		mat.AppendScaleUniform3D(0.01f);
		mat.SetTranslation3D(m_dogActor->m_position);
	}
	return mat;
}

Mat44 Map::GetShipModelMatrix() const
{
	Mat44 mat;
	//mat.AppendXRotation(90.f);
	EulerAngles orientation;
	if (m_shipActor)
		orientation.m_yawDegrees = m_shipActor->m_orientation.m_yawDegrees;
	orientation.m_pitchDegrees = 0.f;
	orientation.m_rollDegrees = 0.f;
	mat = orientation.GetMatrix_XFwd_YLeft_ZUp();
	mat.AppendXRotation(90.f);
	mat.AppendYRotation(-90.f);
	//mat.Append(orientation.GetMatrix_XFwd_YLeft_ZUp());
	mat.AppendScaleUniform3D(0.2f);
	//Mat44 mat = orientation.GetMatrix_XFwd_YLeft_ZUp();
	if (m_shipActor)
		mat.SetTranslation3D(m_shipActor->m_position);
	else mat.SetTranslation3D(Vec3(9.f, 13.f, 4.f));
	return mat;
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
	if (actorA->m_actorDef->m_name == "PipeBlock" || actorB->m_actorDef->m_name == "PipeBlock")
		return;
	if (actorA->m_actorDef->m_name == "Skybox" || actorB->m_actorDef->m_name == "Skybox")
		return;
	if (actorA->m_isDead || actorB->m_isDead)
		return;

	if (actorA->m_actorDef->m_rigidbodyType == RigidbodyType::Cylinder && actorB->m_actorDef->m_rigidbodyType == RigidbodyType::Cylinder) {
		// if is overlap
		Vec2 centerA = actorA->m_position.GetVec2XY();
		Vec2 centerB = actorB->m_position.GetVec2XY();
		bool isDiscOverlap = false;
		bool isZOverlap = false;
		FloatRange a = actorA->GetZFloatRange();
		FloatRange b = actorB->GetZFloatRange();
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
	else if (actorA->m_actorDef->m_rigidbodyType == RigidbodyType::Cylinder && actorB->m_actorDef->m_rigidbodyType == RigidbodyType::AABB3) {
		// if is overlap
		AABB3 bounds;
		bounds.SetDimensions(actorB->m_actorDef->m_dimension);
		Vec3 center = actorB->m_position;
		center.z += bounds.GetDimensions().z * 0.5f;
		bounds.SetCenter(center);

		PushCylinder3DOutOfAABB3D(actorA->m_position, actorA->m_actorDef->m_radius, actorA->m_actorDef->m_height,
			bounds, actorA->m_actorDef->m_isCollidesWithActors, actorB->m_actorDef->m_isCollidesWithActors);

	}
	else if (actorA->m_actorDef->m_rigidbodyType == RigidbodyType::AABB3 && actorB->m_actorDef->m_rigidbodyType == RigidbodyType::Cylinder) {
		// if is overlap

		AABB3 bounds;
		bounds.SetDimensions(actorA->m_actorDef->m_dimension);
		Vec3 center = actorA->m_position;
		center.z += bounds.GetDimensions().z * 0.5f;
		bounds.SetCenter(center);

		PushCylinder3DOutOfAABB3D(actorB->m_position, actorB->m_actorDef->m_radius, actorB->m_actorDef->m_height,
			bounds, actorB->m_actorDef->m_isCollidesWithActors, actorA->m_actorDef->m_isCollidesWithActors);

	
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
	if (actor->m_isDead || !actor->m_actorDef->m_isCollidesWithWorld)
		return;

	for (int tileIndex = 0; tileIndex < (int)m_tiles.size(); tileIndex++) {
		if (m_tiles[tileIndex].m_tileDef->m_solidType == SolidType::Null)
			continue;
		AABB3 bounds = m_tiles[tileIndex].GetCollisionBounds();
		Vec3 actorPrePos = actor->m_position;
		if (actor->m_actorDef->m_rigidbodyType == RigidbodyType::Cylinder)
		{
			PushCylinder3DOutOfAABB3D(actor->m_position, actor->m_actorDef->m_radius, actor->m_actorDef->m_height,
				bounds, actor->m_actorDef->m_isCollidesWithWorld);
		}
		else if (actor->m_actorDef->m_rigidbodyType == RigidbodyType::AABB3) {
			AABB3 actorBounds;
			actorBounds.SetDimensions(actorBounds.GetDimensions());
			Vec3 center = actor->m_position;
			center.z += actorBounds.GetDimensions().z * 0.5f;
			actorBounds.SetCenter(center);
			PushAABB3DOutOfFixedAABB3D(actorBounds, bounds);
			actor->m_position = actorBounds.GetCenter();
			actor->m_position.z -= actorBounds.GetDimensions().z * 0.5f;
		}
		float delta = fabsf(actor->m_position.z - actorPrePos.z);
		if (delta > FLT_EPSILON)
			actor->m_velocity.z = 0.f;

		if (actorPrePos != actor->m_position) {
			actor->OnCollide(nullptr);
		}
	}

		
}

void Map::CollideActorWithTile(Actor* actor, int x, int y)
{
	Vec2 center = actor->m_position.GetVec2XY();
	if (AreCoordsInBounds(x, y)) {
		const Tile* tempTile = GetTile(x, y);
		if (tempTile->IsSolid()) {
			
			if (actor->m_actorDef->m_rigidbodyType == RigidbodyType::Cylinder)
				PushDiscOutOfFixedAABB2D(center, actor->m_actorDef->m_radius, tempTile->GetBounds().GetAABB2XY());
			else if (actor->m_actorDef->m_rigidbodyType == RigidbodyType::AABB3) {
				AABB2 bound;
				bound.SetDimensions(actor->m_actorDef->m_dimension.GetVec2XY());
				bound.SetCenter(center);
				PushAABB2DOutOfFixedAABB2D(bound, tempTile->GetBounds().GetAABB2XY());
				center = bound.GetCenter();
			}
				
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
		Actor* player = SpawnActor(spawnInfo);
		m_playerActor = player;
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
	//spawnInfo.m_position.z += m_level;
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
		position.z += actor->m_actorDef->m_eyeHeight;
		RaycastResult3D tempActor = RaycastVsCylinderZ3D(position, (ac->m_position - position).GetNormalized(),
			actor->m_actorDef->m_sightRadius, ac->m_position.GetVec2XY(),
			ac->GetZFloatRange().m_min, ac->GetZFloatRange().m_max, ac->m_actorDef->m_radius);
		if (tempActor.m_didImpact && tempActor.m_impactDis < minDistance) {
			Vec3 eye = actor->m_position;
			eye.z += actor->m_actorDef->m_eyeHeight;
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

void Map::MoveElevator(float deltaSeconds)
{

	if (m_elevatorEnergy != 0.f) {
		float next = 0.f, deltaZ = 0.f;
		if (m_elevatorEnergy > 0.f) {
			next = m_elevatorEnergy - deltaSeconds;
			next = GetClamped(next, 0.f, m_elevatorEnergy);
		}
		else {
			next = m_elevatorEnergy + deltaSeconds;
			next = GetClamped(next, m_elevatorEnergy, 0.f);
			//deltaZ = m_elevatorEnergy - next;
		}
		deltaZ = m_elevatorEnergy - next;
		m_elevatorEnergy = next;
		for (int actorIndex = 0; actorIndex < (int)m_elevators.size(); actorIndex++) {
			Actor*& actor = m_elevators[actorIndex];
			actor->m_position.z += deltaZ;
		}
	}

}

void Map::SetElevatorGoalLayer(float goal)
{
	if (m_elevatorEnergy != 0.f)
	{
		m_elevatorSumEnergy -= m_elevatorEnergy;
	}
	m_elevatorEnergy = goal - m_elevatorSumEnergy;
	m_elevatorSumEnergy = goal;
}

RaycastResult3D Map::RaycastWorldXY(const Vec3& start, const Vec3& direction, float distance) const
{
	RaycastResult3D result;
	float minDistance = distance;
	for (int tileIndex = 0; tileIndex < (int)m_tiles.size(); tileIndex++) {
		Tile const& tile = m_tiles[tileIndex];
		if (tile.m_tileDef->m_solidType != SolidType::Null) {
			RaycastResult3D temp = RaycastVsAABB3D(start, direction, distance, tile.GetCollisionBounds());
			if (temp.m_didImpact && temp.m_impactDis < minDistance) {
				result = temp;
				minDistance = temp.m_impactDis;
			}
		}
	}
	return result;
}

RaycastResult3D Map::RaycastWorldZ(const Vec3& start, const Vec3& direction, float distance) const
{
	RaycastResult3D result;
	UNUSED(start);
	UNUSED(direction);
	UNUSED(distance);
	return result;
}

RaycastResult3D Map::RaycastWorldActors(const Vec3& start, const Vec3& direction, float distance) const
{
	float minDistance = distance;
	RaycastResult3D result;
	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++) {
		const Actor* actor = m_actors[actorIndex];
		
		if (actor != nullptr) {
			if (actor->m_actorDef->m_name == "SpawnPoint" || actor->m_actorDef->m_name == "PipeBlock" || actor->m_actorDef->m_name == "Skybox")
				continue;
			RaycastResult3D temp;
			if (actor->m_actorDef->m_rigidbodyType == RigidbodyType::Cylinder)
			{
				temp = RaycastVsCylinderZ3D(start, direction, distance,
					actor->m_position.GetVec2XY(), actor->GetZFloatRange().m_min, actor->GetZFloatRange().m_max, actor->m_actorDef->m_radius);
			}
			else if (actor->m_actorDef->m_rigidbodyType == RigidbodyType::AABB3) {
				AABB3 aabb;
				Vec3 center = actor->m_position;
				center.z += 0.5f * actor->m_actorDef->m_dimension.z;
				aabb.SetCenter(center);
				aabb.SetDimensions(actor->m_actorDef->m_dimension);
				temp = RaycastVsAABB3D(start, direction, distance, aabb);
				if (temp.m_didImpact && temp.m_impactDis == 0.f) {
					temp.m_impactDis = 0.0001f;
				}
			}
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
			if (actor->m_actorDef->m_name == "SpawnPoint" || actor->m_actorDef->m_name == "PipeBlock" || actor->m_actorDef->m_name == "Skybox" || actor->m_actorDef->m_name == "DogBlock")
				continue;
			RaycastResult3D temp;
			if (actor->m_actorDef->m_rigidbodyType == RigidbodyType::Cylinder) {
				temp = RaycastVsCylinderZ3D(start, direction, distance,
					actor->m_position.GetVec2XY(), actor->GetZFloatRange().m_min, actor->GetZFloatRange().m_max, actor->m_actorDef->m_radius);
			}
			else if (actor->m_actorDef->m_rigidbodyType == RigidbodyType::AABB3) {
				AABB3 aabb;
				Vec3 center = actor->m_position;
				center.z += 0.5f * actor->m_actorDef->m_dimension.z;
				aabb.SetCenter(center);
				aabb.SetDimensions(actor->m_actorDef->m_dimension);
				temp = RaycastVsAABB3D(start, direction, distance, aabb);
				if (temp.m_didImpact && temp.m_impactDis == 0.f) {
					temp.m_impactDis = 0.0001f;
				}
			}
			
			if (temp.m_didImpact && temp.m_impactDis < minDistance && temp.m_impactDis != 0.f) {
				result = temp;
				minDistance = temp.m_impactDis;
				impactActor = actor;
			}
		}
		
	}
	return result;
}
