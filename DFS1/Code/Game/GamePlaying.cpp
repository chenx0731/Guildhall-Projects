#define UNUSED(x) (void)(x);
#include "Game/Game.hpp"
#include "Game/GamePlaying.hpp"
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

BitmapFont* g_theFont = nullptr;
GamePlaying::GamePlaying()
{
	m_clock = new Clock(Clock::GetSystemClock());
}

GamePlaying::~GamePlaying()
{

}

void GamePlaying::Startup()
{
	LoadAssets();

}

void GamePlaying::Update()
{
	UpdateFromKeyboard();
	SetCamera();
	m_currentMap->Update(m_clock->GetDeltaSeconds());
	//m_currentWorld->Update(m_clock->GetDeltaSeconds());
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		if (m_players[playerIndex]) {
			m_players[playerIndex]->Update(m_clock->GetDeltaSeconds());
			Vec3 iBasis, jBasis, kBasis;
			m_players[playerIndex]->m_worldCamera.m_orientation.GetVectors_XFwd_YLeft_ZUp(iBasis, jBasis, kBasis);
			g_theAudio->UpdateListener(0, m_players[playerIndex]->m_position, iBasis, kBasis);
			//Actor* actor = m_players[playerIndex]->GetActor();
			//Map* map = m_currentWorld->GetMapByLevel(actor->m_position.z);
			//if (map != m_currentMap) {
			//	m_currentWorld->AddPlayerToMap(playerIndex, map);
			//	//RemovePlayerFromMap(playerIndex);
			//}
		}
	}
}

void GamePlaying::Render() const
{
	RenderWorld();
	RenderScreen();
}

void GamePlaying::Shutdown()
{
	MapDefinition::ClearDefinitions();
	TileDefinition::ClearDefinitions();
}

void GamePlaying::Enter()
{
	std::string maps = g_gameConfigBlackboard.GetValue("maps", "");
	m_mapNames = SplitStringOnDelimiter(maps, ",");
	for (int mapIndex = 0; mapIndex < m_mapNames.size(); mapIndex++) {
		Map* map = new Map(this, m_mapNames[mapIndex].c_str());
		m_maps[m_mapNames[mapIndex]] = map;
	}
	m_currentMap = m_maps[m_currentMapName];
	if (m_playerNum == 0)
		AddPlayerWithSpecController(ControllerType::KEYBOARD, 0);
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		if (m_players[playerIndex])
			m_currentMap->SpawnPlayer(playerIndex);
	}
	g_theAudio->SetNumListeners(m_playerNum);
	m_screenCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	SetCamera();
	PlayMusicLoop(MUSIC_GAME);
}

void GamePlaying::Exit()
{
	//m_currentWorld->RemovePlayerFromMap(0);
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		if (m_players[playerIndex])
		{
			delete m_players[playerIndex];
			m_players[playerIndex] = nullptr;
		}
	}
	
	m_playerNum = 0;
	for (int mapIndex = 0; mapIndex < m_mapNames.size(); mapIndex++) {
		delete m_maps[m_mapNames[mapIndex]];
		m_maps[m_mapNames[mapIndex]] = nullptr;
	}
	PauseMusic(MUSIC_GAME);
}

void GamePlaying::AddPlayerWithSpecController(ControllerType type, int playerIndex)
{
	m_players[playerIndex] = new Player(nullptr);
	m_playerNum++;
	m_players[playerIndex]->m_type = type;
	m_players[playerIndex]->m_playerIndex = playerIndex;
	SetCameraByPlayerNum();
}

void GamePlaying::DeletePlayerWithSpecController(ControllerType type)
{
	bool isFound = false;
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		if (m_players[playerIndex]) {
			if (m_players[playerIndex]->m_type == type) {
				delete m_players[playerIndex];
				m_players[playerIndex] = nullptr;
				isFound = true;
			}
		}
	}
	if (isFound)
		m_playerNum--;
	SetCameraByPlayerNum();
}

void GamePlaying::RemovePlayerFromMap(int playerIndex)
{
	Actor* actor = m_players[playerIndex]->GetActor();
	int index = actor->m_UID.GetIndex();
	delete m_currentMap->m_actors[index];
	m_currentMap->m_actors[index] = nullptr;
}

void GamePlaying::AddPlayerToMap(int playerIndex, Map* map)
{
	Actor* actor = m_players[playerIndex]->GetActor();
	SpawnInfo spawnInfo;
	spawnInfo.m_actor = "Marine";
	spawnInfo.m_orientation = actor->m_orientation;
	spawnInfo.m_position = actor->m_position;
	spawnInfo.m_velocity = actor->m_velocity;
 	Actor* now = map->SpawnActor(spawnInfo);
	now->m_health = actor->m_health;
	now->m_nowAnimeName = actor->m_nowAnimeName;
	now->m_currentWeaponIndex = actor->m_currentWeaponIndex;
	m_players[playerIndex]->m_map = map;
	m_players[playerIndex]->m_position = now->m_position;
	 m_players[playerIndex]->Possess(now->m_UID);

	int index = actor->m_UID.GetIndex();
	delete m_currentMap->m_actors[index];
	m_currentMap->m_actors[index] = nullptr;
	m_currentMap = map;
}

void GamePlaying::SetCameraByPlayerNum()
{
	if (m_playerNum == 1) {
		for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
			if (m_players[playerIndex]) {
				m_players[playerIndex]->m_worldCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
					Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
						(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
				m_players[playerIndex]->m_screenCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
					Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
						(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
				m_players[playerIndex]->m_screenCamera.m_bottomLeft = Vec2(0.f, 0.f);
				m_players[playerIndex]->m_screenCamera.m_topRight = Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y);
			}
		}

	}
	if (m_playerNum == 2) {
		AABB2 newView0 = AABB2(Vec2(0.f, 0.f),
			Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
				0.5f * (float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
		AABB2 newView1 = AABB2(Vec2(0.f, 0.5f * (float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y),
			Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
				(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
		
		m_players[0]->m_worldCamera.m_viewport = newView0;
		m_players[0]->m_screenCamera.m_viewport = newView0;
		m_players[0]->m_screenCamera.m_bottomLeft = Vec2(0.f, 0.f);
		m_players[0]->m_screenCamera.m_topRight = Vec2(SCREEN_CAMERA_SIZE_X, 0.5f * SCREEN_CAMERA_SIZE_Y);

		m_players[1]->m_worldCamera.m_viewport = newView1;
		m_players[1]->m_screenCamera.m_viewport = newView1;
		m_players[1]->m_screenCamera.m_bottomLeft = Vec2(0.f, 0.f);
		m_players[1]->m_screenCamera.m_topRight = Vec2(SCREEN_CAMERA_SIZE_X, 0.5f * SCREEN_CAMERA_SIZE_Y);
	}
}


void GamePlaying::LoadAssets()
{
	TileDefinition::InitializeTileDefs("Data/Definitions/TileDefinitions.xml");
	MapDefinition::InitializeMapDefs("Data/Definitions/MapDefinitions.xml");
	ActorDefinition::InitializeActorDefs("Data/Definitions/ProjectileActorDefinitions.xml");
	ActorDefinition::InitializeActorDefs("Data/Definitions/ActorDefinitions.xml");
	WeaponDefinition::InitializeWeaponDefs("Data/Definitions/WeaponDefinitions.xml");
	g_textureLibrary[TEXTURE_HUD] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Hud_Base.png");
	g_textureLibrary[TEXTURE_RETICLE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Reticle.png");
}

void GamePlaying::SetCamera()
{
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		if (m_players[playerIndex]) {
			m_players[playerIndex]->m_screenCamera.
				//SetSelfOrthoView();
				SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
		}
	}
}

void GamePlaying::UpdateFromKeyboard()
{
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1)) {
		DebugAddMessage(Stringf("Sun Direction X: %.2f (F2 / F3 to change)", m_currentMap->m_sunDirection.x), 2.f);
		DebugAddMessage(Stringf("Sun Direction Y: %.2f (F4 / F5 to change)", m_currentMap->m_sunDirection.y), 2.f);
		DebugAddMessage(Stringf("Sun Intensity: %.2f (F6 / F7 to change)", m_currentMap->m_sunIntensity), 2.f);
		DebugAddMessage(Stringf("Ambient Intensity: %.2f (F8 / F9 to change)", m_currentMap->m_ambientIntensity), 2.f);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F2)) {
		m_currentMap->m_sunDirection.x -= 1.f;
		DebugAddMessage(Stringf("Sun Direction X: %.2f", m_currentMap->m_sunDirection.x), 2.f);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F3)) {
		m_currentMap->m_sunDirection.x += 1.f;
		DebugAddMessage(Stringf("Sun Direction X: %.2f", m_currentMap->m_sunDirection.x), 2.f);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F4)) {
		m_currentMap->m_sunDirection.y -= 1.f;
		DebugAddMessage(Stringf("Sun Direction Y: %.2f", m_currentMap->m_sunDirection.y), 2.f);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F5)) {
		m_currentMap->m_sunDirection.y += 1.f;
		DebugAddMessage(Stringf("Sun Direction Y: %.2f", m_currentMap->m_sunDirection.y), 2.f);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F6)) {
		m_currentMap->m_sunIntensity -= 0.05f;
		DebugAddMessage(Stringf("Sun Intensity: %.2f", m_currentMap->m_sunIntensity), 2.f);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F7)) {
		m_currentMap->m_sunIntensity += 0.05f;
		DebugAddMessage(Stringf("Sun Intensity: %.2f", m_currentMap->m_sunIntensity), 2.f);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F8)) {
		m_currentMap->m_ambientIntensity -= 0.05f;
		DebugAddMessage(Stringf("Ambient Intensity: %.2f", m_currentMap->m_ambientIntensity), 2.f);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F9)) {
		m_currentMap->m_ambientIntensity += 0.05f;
		DebugAddMessage(Stringf("Ambient Intensity: %.2f", m_currentMap->m_ambientIntensity), 2.f);
	}
}

void GamePlaying::RenderScreen() const
{
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		if (m_players[playerIndex]) {
			g_theRenderer->SetModelConstants();
			const Camera& temp = m_players[playerIndex]->m_screenCamera;
			g_theRenderer->BeginCamera(m_players[playerIndex]->m_screenCamera);
			//RenderUI();
			Actor* playerActor = m_players[playerIndex]->GetActor();
			if (playerActor->m_actorDef->m_name != "Marine") {
				continue;
			}
			playerActor->m_weapons[playerActor->m_currentWeaponIndex]->RenderHUD(m_players[playerIndex]->m_screenCamera);

			std::vector<Vertex_PCU> playerHealth, kill, sumDeath;
			std::string health = Stringf("%d", m_players[playerIndex]->GetActor()->m_health);
			std::string killNum;
			if (playerIndex == 0)
				killNum = Stringf("%d", m_playerDeath[1]);
			else
				killNum = Stringf("%d", m_playerDeath[0]);
			std::string sum = Stringf("%d", m_playerDeath[playerIndex]);
			g_theFont->AddVertsForTextInBox2D(playerHealth, AABB2(temp.m_bottomLeft, temp.m_topRight), 60.f, health, Rgba8::WHITE, 0.7f, Vec2(0.29f, 0.03f));
			g_theFont->AddVertsForTextInBox2D(kill, AABB2(temp.m_bottomLeft, temp.m_topRight), 60.f, killNum, Rgba8::WHITE, 0.7f, Vec2(0.06f, 0.03f));
			g_theFont->AddVertsForTextInBox2D(sumDeath, AABB2(temp.m_bottomLeft, temp.m_topRight), 60.f, sum, Rgba8::WHITE, 0.7f, Vec2(0.95f, 0.03f));
			g_theRenderer->BindTexture(&g_theFont->GetTexture());
			g_theRenderer->DrawVertexArray((int)playerHealth.size(), playerHealth.data());
			g_theRenderer->DrawVertexArray((int)kill.size(), kill.data());
			g_theRenderer->DrawVertexArray((int)sumDeath.size(), sumDeath.data());
			g_theRenderer->BindTexture(nullptr);

			g_theRenderer->EndCamera(m_players[playerIndex]->m_screenCamera);
		}
		
		//DebugRenderWorld(m_players[playerIndex]->m_worldCamera);
	}
	g_theRenderer->BeginCamera(m_screenCamera);

	RenderUI();
	DebugRenderScreen(m_screenCamera);

	g_theRenderer->EndCamera(m_screenCamera);
}

void GamePlaying::RenderWorld() const
{
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		if (m_players[playerIndex]) {
			g_theRenderer->BeginCamera(m_players[playerIndex]->m_worldCamera);
			m_currentMap->Render(m_players[playerIndex]->m_worldCamera);
			g_theRenderer->EndCamera(m_players[playerIndex]->m_worldCamera);
			DebugRenderWorld(m_players[playerIndex]->m_worldCamera);
		}
	}
}

void GamePlaying::RenderUI() const
{
	g_theRenderer->SetModelConstants();
	std::vector<Vertex_PCU> gameClock, playerHealth, cameraMode;
	std::string clock = Stringf("[Game Clock] Time = %.2f, FPS = %.2f, Scale = %.1f",
		m_clock->GetTotalSeconds(), 1.f / m_clock->GetDeltaSeconds(), m_clock->GetTimeScale());
	//std::string health = Stringf("%d", m_player->GetActor()->m_health);
	//std::vector<Vertex_PCU> hudVerts, reticleVerts;
	//AABB2 hudAABB, reticleAABB;
	//hudAABB = AABB2(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, 100.f));
	//reticleAABB = AABB2(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f - 10.f, SCREEN_CAMERA_SIZE_Y * 0.5f - 10.f), 
	//	Vec2(SCREEN_CAMERA_SIZE_X * 0.5f + 10.f, SCREEN_CAMERA_SIZE_Y * 0.5f + 10.f));

	//AddVertsForAABB2D(hudVerts, hudAABB, Rgba8::WHITE);
	//AddVertsForAABB2D(reticleVerts, reticleAABB, Rgba8::WHITE);

	//g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_HUD]);
	//g_theRenderer->DrawVertexArray((int)hudVerts.size(), hudVerts.data());
	//g_theRenderer->BindTexture(nullptr);

	//g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_RETICLE]);
	//g_theRenderer->DrawVertexArray((int)reticleVerts.size(), reticleVerts.data());
	//g_theRenderer->BindTexture(nullptr);

	g_theFont->AddVertsForTextInBox2D(gameClock, AABB2(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y)), 20.f, clock, Rgba8::WHITE, 0.7f, Vec2(1.f, 1.f));
	//g_theFont->AddVertsForTextInBox2D(playerHealth, AABB2(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y)), 80.f, health, Rgba8::WHITE, 0.7f, Vec2(0.5f, 0.0f));
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)gameClock.size(), gameClock.data());
	g_theRenderer->DrawVertexArray((int)cameraMode.size(), cameraMode.data());
	//g_theRenderer->DrawVertexArray((int)playerHealth.size(), playerHealth.data());
	g_theRenderer->BindTexture(nullptr);
	
}

