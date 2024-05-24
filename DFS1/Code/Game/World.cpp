#define UNUSED(x) (void)(x);
#include "Game/World.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/GameCommon.hpp"
#include "Game/World.hpp"
#include "Game/Player.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Game/App.hpp"
#include "Game/Actor.hpp"
#include "Game/Weapon.hpp"
#include "Game/Map.hpp"
#include "Game/OBJ_Loader.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Vertex_PNCU.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Math/RaycastResult2D.hpp"

World::World(GamePlaying* game, std::string name)
{
	m_game = game;
	m_name = name;

	//m_objVerts = g_OBJLoader->LoadedVertices;
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

	m_shader = g_theRenderer->CreateShader("Data/Shaders/SpriteLit");
}

World::~World()
{
	for (int mapIndex = 0; mapIndex < (int)m_mapNames.size(); mapIndex++) {
		Map* map = m_maps[m_mapNames[mapIndex]];
		delete map;
		m_maps[m_mapNames[mapIndex]] = nullptr;
	}
	delete m_dogVertexBuffer;
	delete m_dogIndexBuffer;
}

void World::Update(float deltaseconds)
{
	UpdateFromKeyboard(deltaseconds);
	
	if (m_isRiding) {
		if (m_dogActor != nullptr) {
			m_dogActor->m_position.x = m_game->m_players[0]->m_position.x;
			m_dogActor->m_position.y = m_game->m_players[0]->m_position.y;
		}
		//m_dogPosition.x = m_game->m_players[0]->m_position.x;
		//m_dogPosition.y = m_game->m_players[0]->m_position.y;
		//m_game->m_players[0]->m_position.z += 0.95f;
		//m_dogPosition.z -= 0.95f;
		m_dogOrientation = m_game->m_players[0]->m_orientation;
		m_dogOrientation.m_pitchDegrees = 0.f;
		m_dogOrientation.m_rollDegrees = 0.f;
	}
	m_currentMap->Update(deltaseconds);
}


void World::Render(const Camera& camera) const
{
	for (auto it = m_maps.begin(); it != m_maps.end(); it++) {
		Map* const& map = it->second;
		if (map != nullptr)
			map->Render(camera);
	}

	g_theRenderer->SetModelConstants(GetDogModelMatrix(), Rgba8::WHITE);
	g_theRenderer->BindShader(m_shader);
	g_theRenderer->SetLightConstants(Vec3(2.f, 1.f, -1.f), 0.8f, Rgba8(50, 123, 123));
	g_theRenderer->DrawVertexIndexArray((int)m_dogVerts.size(), (int)m_dogIndexes.size(), m_dogVerts.data(), m_dogIndexes.data(), m_dogVertexBuffer, m_dogIndexBuffer);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->SetModelConstants();
	//m_currentMap->Render(camera);
}


void World::RemovePlayerFromMap(int playerIndex)
{
	Actor* actor = m_game->m_players[playerIndex]->GetActor();
	int index = actor->m_UID.GetIndex();
	delete m_currentMap->m_actors[index];
	m_currentMap->m_actors[index] = nullptr;
}

void World::AddPlayerToMap(int playerIndex, Map* map)
{
	if (map == nullptr)
	{
		return;
	}
	Actor* actor = m_game->m_players[playerIndex]->GetActor();
	SpawnInfo spawnInfo;
	spawnInfo.m_actor = "Marine";
	spawnInfo.m_orientation = actor->m_orientation;
	spawnInfo.m_position = actor->m_position;
	spawnInfo.m_velocity = actor->m_velocity;
 	Actor* now = map->SpawnActor(spawnInfo);
	now->m_health = actor->m_health;
	now->m_nowAnimeName = actor->m_nowAnimeName;
	now->m_currentWeaponIndex = actor->m_currentWeaponIndex;
	m_game->m_players[playerIndex]->m_map = map;
	m_game->m_players[playerIndex]->m_position = now->m_position;
	m_game->m_players[playerIndex]->Possess(now->m_UID);

	int index = actor->m_UID.GetIndex();
	delete m_currentMap->m_actors[index];
	m_currentMap->m_actors[index] = nullptr;
	m_currentMap = map;
	m_game->m_currentMap = map;
}

Map* World::GetMapByLevel(float level)
{
	for (int mapIndex = 0; mapIndex < m_mapNames.size(); mapIndex++) {
		//Map* map = new Map(this, m_mapNames[mapIndex].c_str());
		Map* const& map = m_maps.at(m_mapNames[mapIndex]);
		if (map->m_level <= level && level < map->m_level + 1.f)
			return map;
	}
	return nullptr;
}

Mat44 World::GetDogModelMatrix() const
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




void World::UpdateFromKeyboard(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->WasKeyJustPressed('R')) {
		if (m_isRiding) {
			m_isRiding = false;
			m_game->m_players[0]->GetActor()->m_isRiding = false;
			m_dogPosition.z = 0.f;
		}
		else if (GetDistanceSquared2D(m_game->m_players[0]->m_position.GetVec2XY(), m_dogActor->m_position.GetVec2XY()) < 0.5f) {
			m_game->m_players[0]->GetActor()->m_isRiding = true;
			m_game->m_players[0]->GetActor()->m_position = m_dogActor->m_position;
			m_game->m_players[0]->GetActor()->m_position.z += m_dogActor->m_actorDef->m_dimension.z;
			m_isRiding = true;
		}
	}
}

