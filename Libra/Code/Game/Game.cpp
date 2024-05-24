#define UNUSED(x) (void)(x);
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/App.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/Explosion.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/NamedStrings.hpp"

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

SpriteSheet*			g_terrainSprites = nullptr;
SpriteAnimDefinition*	g_explosionAnim = nullptr;
int				TERRAIN_SPRITE_WIDTH;
int				TERRAIN_SPRITE_HEIGHT;

Game::Game(App* owner)
{
	UNUSED(owner);

	m_screenSize.x = g_gameConfigBlackboard.GetValue("screenSizeX", 1600.f);
	m_screenSize.y = g_gameConfigBlackboard.GetValue("screenSizeY", 800.f);
	m_worldSize.x  = g_gameConfigBlackboard.GetValue("worldSizeX", 16.f);
	m_worldSize.y  = g_gameConfigBlackboard.GetValue("worldSizeY", 8.f);

	m_clock = new Clock(Clock::GetSystemClock());

}

Game::~Game()
{

}

bool Command_Test(EventArgs& args) // Note: STATIC is #define’d to be nothing; just a marker for the reader
{
	UNUSED(args);
	DebuggerPrintf("Test command received\n");
	//args.DebugPrintContents(); // This method (NamedStrings::DebugPrintContents) is not required; it prints out all key/value pairs
	return false; // Does not consume the event; continue to call other subscribers’ callback functions
}

void Game::Startup()
{
	LoadAssets();
	
	PlayMusicLoop(MUSIC_WELCOME);
	if (m_currentMap == nullptr) {
		m_currentMapIndex = 0;
		std::string maps = g_gameConfigBlackboard.GetValue("maps", "");
		m_mapNames = SplitStringOnDelimiter(maps, ",");
		for (int mapIndex = 0; mapIndex < m_mapNames.size(); mapIndex++) {
			Map* map = new Map(this, m_mapNames[mapIndex].c_str());
			m_maps[m_mapNames[mapIndex]] = map;
			//m_maps.push_back(map);
		}
		m_currentMap = m_maps[m_mapNames[m_currentMapIndex]];
		//m_currentMap = m_maps[m_currentMapIndex];
	}
	if (m_player == nullptr) {
		Entity* player = m_currentMap->SpawnNewEntity(ENTITY_TYPE_GOOD_PLAYER, Vec2(1.5f, 1.5f), 45.f);
		m_currentMap->AddEntityToMap(player);
		m_player = player;
	}
	SetCamera();
	
	SubscribeEventCallbackFunction("Test", &Command_Test);
	FireEvent("Test");

	//g_theEventSystem->FireEvent("Test");
}

void Game::Update(float deltaSeconds)
{
	UpdateFromKeyboard(deltaSeconds);
	UpdateFromController(deltaSeconds);

	SetCamera();

	if (m_gameStatus == GAME_STATUS_ATTRACT)
		return;

	if (m_player == nullptr) {
		m_gameStatus = GAME_STATUS_HANGING;
	}
	else {
		if (IsPointInsideDisc2D(m_currentMap->GetExitPosition(), m_player->m_position, m_player->m_physicsRadius)) {
			m_currentMapIndex++;
			if (m_currentMapIndex >= m_mapNames.size()) {
				m_gameStatus = GAME_STATUS_WIN;
			}
			else {
				m_currentMap->RemoveEntityFromMap(m_player);
				m_player->m_position = Vec2(1.5f, 1.5f);
				m_currentMap = m_maps[m_mapNames[m_currentMapIndex]];
				//m_currentMap = m_maps[m_currentMapIndex];
				m_player->m_map = m_currentMap;
				m_currentMap->AddEntityToMap(m_player);
			}
		}
	}
	if (m_gameStatus == GAME_STATUS_HANGING)
	{
		m_hangingTime += deltaSeconds;
		if (m_hangingTime >= 3.f)
		{
			m_gameStatus = GAME_STATUS_DIE;
		}
	}
	
	//if (!m_isPaused)
	if (m_gameStatus == GAME_STATUS_PLAY || m_gameStatus == GAME_STATUS_HANGING)
	{
		UpdateEntities(deltaSeconds);
		m_currentMap->Update(deltaSeconds);
		DeleteGarbageEntities();
	}

	if (m_isShaking)
	{
		float theta = g_rng->RollRandomFloatInRange(0.f, 360.f);
		float r = g_rng->RollRandomFloatInRange(0.f, 5.f * (1.f - m_shakingTime));
		m_worldCamera.Translate2D(Vec2::MakeFromPolarDegrees(theta, r));
		m_shakingTime += deltaSeconds;
		if (m_shakingTime >= 1.f)
		{
			m_shakingTime = 0.f;
			m_isShaking = false;
		}
	}
	
}

void Game::Render() const
{
	g_theRenderer->SetModelConstants();
	//if (m_isAttractMode)
	if (m_gameStatus == GAME_STATUS_ATTRACT) {
		RenderAttractMode();
		return;
	}

	if (m_gameStatus == GAME_STATUS_WIN) {
		g_theRenderer->BeginCamera(m_screenCamera);
		std::vector<Vertex_PCU> attractVerts;
		AABB2 localBounds = AABB2(Vec2(0.f, 0.f), Vec2(m_screenSize.x, m_screenSize.y));
		AddVertsForAABB2D(attractVerts, localBounds, Rgba8(255, 255, 255));
		g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_VICTORY_SCREEN]);
		g_theRenderer->DrawVertexArray((int)attractVerts.size(), attractVerts.data());
		g_theRenderer->BindTexture(nullptr);

		g_theRenderer->EndCamera(m_screenCamera);
		return;
	}

	if (m_gameStatus == GAME_STATUS_DIE) {
		g_theRenderer->BeginCamera(m_screenCamera);
		std::vector<Vertex_PCU> attractVerts;
		AABB2 localBounds = AABB2(Vec2(0.f, 0.f), Vec2(m_screenSize.x, m_screenSize.y));
		AddVertsForAABB2D(attractVerts, localBounds, Rgba8(255, 255, 255));
		g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_YOUDIED_SCREEN]);
		g_theRenderer->DrawVertexArray((int)attractVerts.size(), attractVerts.data());
		g_theRenderer->BindTexture(nullptr);

		g_theRenderer->EndCamera(m_screenCamera);
		return;
	}

	if (m_isDebug) {
		DebugRenderEntities();
	}
	
	if (m_currentMap) {
		g_theRenderer->BeginCamera(m_worldCamera);
		m_currentMap->Render();
		g_theRenderer->EndCamera(m_worldCamera);
	}

	

	RenderUI();

	//if (m_isPaused)
	if (m_gameStatus == GAME_STATUS_PAUSE)
	{
		RenderPaused();
	}
}

void Game::Shutdown()
{
	for (int musicIndex = 0; musicIndex < MUSIC_NUM; musicIndex++) {
		StopMusic((Music)musicIndex);
	}
	delete m_player;
	m_player = nullptr;

	for (int mapIndex = 0; mapIndex < m_mapNames.size(); mapIndex++) {
		
		delete m_maps[m_mapNames[mapIndex]];
		m_maps[m_mapNames[mapIndex]] = nullptr;
		//delete m_maps[mapIndex];
		//m_maps[mapIndex] = nullptr;
	}

	m_currentMap = nullptr;
}

void Game::UpdateEntities(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void Game::LoadAssets()
{
	 // music
	 g_musicLibrary[MUSIC_WELCOME]			= g_theAudio->CreateOrGetSound("Data/Audio/AttractMusic.mp3");
	 g_musicLibrary[MUSIC_BGM]				= g_theAudio->CreateOrGetSound("Data/Audio/GameplayMusic.mp3");
	 g_musicLibrary[MUSIC_PAUSE]			= g_theAudio->CreateOrGetSound("Data/Audio/Pause.mp3");
	 g_musicLibrary[MUSIC_UNPAUSE]			= g_theAudio->CreateOrGetSound("Data/Audio/Unpause.mp3");
	 g_musicLibrary[MUSIC_START]			= g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	 g_musicLibrary[MUSIC_QUIT]				= g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	 g_musicLibrary[MUSIC_PLAYER_SHOOT]		= g_theAudio->CreateOrGetSound("Data/Audio/PlayerShootNormal.ogg");
	 g_musicLibrary[MUSIC_ENEMY_SHOOT]		= g_theAudio->CreateOrGetSound("Data/Audio/EnemyShoot.wav");
	 g_musicLibrary[MUSIC_PLAYER_HIT]		= g_theAudio->CreateOrGetSound("Data/Audio/PlayerHit.wav");
	 g_musicLibrary[MUSIC_ENEMY_HIT]		= g_theAudio->CreateOrGetSound("Data/Audio/EnemyHit.wav");
	 g_musicLibrary[MUSIC_PLAYER_DIE]		= g_theAudio->CreateOrGetSound("Data/Audio/GameOver.mp3");
	 g_musicLibrary[MUSIC_ENEMY_DIE]		= g_theAudio->CreateOrGetSound("Data/Audio/EnemyDied.wav");
	 g_musicLibrary[MUSIC_VICTORY]			= g_theAudio->CreateOrGetSound("Data/Audio/Victory.mp3");
	
	 // texture
	 g_textureLibrary[TEXTURE_TANKBASE]				= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
	 g_textureLibrary[TEXTURE_TANKTOP]				= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankTop.png");
	 g_textureLibrary[TEXTURE_TERRAIN]				= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Terrain_8x8.png");
	 g_textureLibrary[TEXTURE_ENEMY_ARIES]			= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/EnemyAries.png");
	 g_textureLibrary[TEXTURE_ENEMY_LEO]			= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/EnemyTank4.png");
	 g_textureLibrary[TEXTURE_ENEMY_CAPRICORN]		= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/EnemyTank3.png");
	 g_textureLibrary[TEXTURE_ENEMY_SCORPIO_BASE]	= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/EnemyTurretBase.png");
	 g_textureLibrary[TEXTURE_ENEMY_SCORPIO_TOP]	= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/EnemyCannon.png");
	 g_textureLibrary[TEXTURE_PLAYER_BOLT]			= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/FriendlyBolt.png");
	 g_textureLibrary[TEXTURE_ENEMY_BOLT]			= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/EnemyBolt.png");
	 g_textureLibrary[TEXTURE_PLAYER_BULLET]		= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/FriendlyBullet.png");
	 g_textureLibrary[TEXTURE_ENEMY_BULLET]			= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/EnemyBullet.png");
	 g_textureLibrary[TEXTURE_ATTRACT_SCREEN]		= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/AttractScreen.png");
	 g_textureLibrary[TEXTURE_VICTORY_SCREEN]		= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/VictoryScreen.jpg");
	 g_textureLibrary[TEXTURE_YOUDIED_SCREEN]		= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/YouDiedScreen.png");
	 g_textureLibrary[TEXTURE_TEST_ANIM]			= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestSpriteSheet_4x4.png");
	 g_textureLibrary[TEXTURE_EXPLOSION]			= g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Explosion_5x5.png");
	 
	 TERRAIN_SPRITE_WIDTH = g_gameConfigBlackboard.GetValue("terrainSpriteWidth", 8);
	 TERRAIN_SPRITE_HEIGHT = g_gameConfigBlackboard.GetValue("terrainSpriteHeight", 8);
	 g_terrainSprites = new SpriteSheet(*g_textureLibrary[TEXTURE_TERRAIN], IntVec2(TERRAIN_SPRITE_WIDTH, TERRAIN_SPRITE_HEIGHT));

	 SpriteSheet* sp = new SpriteSheet(*g_textureLibrary[TEXTURE_EXPLOSION], IntVec2(5, 5));
	 g_explosionAnim = new SpriteAnimDefinition(*sp, 0, 24);
	 //xml
	 TileDefinition::InitializeTileDefs("Data/Definitions/TileDefinitions.xml");
	 MapDefinition::InitializeMapDefs("Data/Definitions/MapDefinitions.xml");
}

void Game::DeleteGarbageEntities()
{
	m_currentMap->DeleteGarbageEntities();
}

void Game::KillAll()
{

}

void Game::UpdateFromKeyboard(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->WasKeyJustPressed('P')) {
		//if (m_isAttractMode) {
		if (m_gameStatus == GAME_STATUS_ATTRACT) {
			EnterGame();
		}
		else {
			if (m_gameStatus == GAME_STATUS_PAUSE)
				m_gameStatus = GAME_STATUS_PLAY;
			else if (m_gameStatus == GAME_STATUS_PLAY)
				m_gameStatus = GAME_STATUS_PAUSE;
			else if (m_gameStatus == GAME_STATUS_DIE || m_gameStatus == GAME_STATUS_WIN)
				m_gameStatus = GAME_STATUS_ATTRACT;

			//if (m_isPaused)
			if (m_gameStatus == GAME_STATUS_PAUSE)
			{
				PauseMusic(MUSIC_BGM);
				PlayMusic(MUSIC_PAUSE);
			}
			else if (m_gameStatus == GAME_STATUS_PLAY)
			{
				ResumeMusic(MUSIC_BGM);
				PlayMusic(MUSIC_UNPAUSE);
			}
		}
		

	}
	if (g_theInput->WasKeyJustPressed(' ') || g_theInput->WasKeyJustPressed('N')) {
		//if (m_isAttractMode)
		if (m_gameStatus == GAME_STATUS_ATTRACT) {
			EnterGame();
		}
		if (m_gameStatus == GAME_STATUS_DIE) {
			Entity* player = m_currentMap->SpawnNewEntity(ENTITY_TYPE_GOOD_PLAYER, m_playerRespawnPoint, 45.f);
			m_currentMap->AddEntityToMap(player);
			m_player = player;
			m_gameStatus = GAME_STATUS_PLAY;
			m_hangingTime = 0.f;
		}
		if (m_gameStatus == GAME_STATUS_WIN)
			m_gameStatus = GAME_STATUS_ATTRACT;
	}
	if (g_theInput->WasKeyJustPressed('O')) {

	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1)) {
		m_isDebug = !m_isDebug;
		g_debugDraw = !g_debugDraw;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F3)) {
		g_debugNoclip = !g_debugNoclip;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F4)) {
		m_isDebugCamera = !m_isDebugCamera;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F5)) {
		g_debugHeatMap = !g_debugHeatMap;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F9)) {
		m_currentMapIndex++;
		if (m_currentMapIndex >= m_mapNames.size()) {
			PlayMusic(MUSIC_VICTORY);
			m_gameStatus = GAME_STATUS_WIN;
		}
		else {
			m_currentMap->RemoveEntityFromMap(m_player);
			m_player->m_position = Vec2(1.5f, 1.5f);
			m_currentMap = m_maps[m_mapNames[m_currentMapIndex]];
			//m_currentMap = m_maps[m_currentMapIndex];
			m_player->m_map = m_currentMap;
			m_currentMap->AddEntityToMap(m_player);
		}
	}
	
	if (g_theInput->WasKeyJustPressed('K'))
	{
		KillAll();
	}
}

void Game::UpdateFromController(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_START) 
		|| g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_A))
	{
		if (m_gameStatus == GAME_STATUS_ATTRACT) {
			EnterGame();
		}
		if (m_gameStatus == GAME_STATUS_DIE) {
			Entity* player = m_currentMap->SpawnNewEntity(ENTITY_TYPE_GOOD_PLAYER, Vec2(1.5f, 1.5f), 45.f);
			m_currentMap->AddEntityToMap(player);
			m_player = player;
			m_gameStatus = GAME_STATUS_PLAY;
			m_hangingTime = 0.f;
		}
		if (m_gameStatus == GAME_STATUS_WIN)
			m_gameStatus = GAME_STATUS_ATTRACT;
	}
}

void Game::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->SetModelConstants();

	std::vector<Vertex_PCU> attractVerts;
	AABB2 localBounds = AABB2(Vec2(0.f, 0.f), Vec2(m_screenSize.x, m_screenSize.y));
	AddVertsForAABB2D(attractVerts, localBounds, Rgba8(255, 255, 255));
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_ATTRACT_SCREEN]);
	g_theRenderer->DrawVertexArray((int)attractVerts.size(), attractVerts.data());
	g_theRenderer->BindTexture(nullptr);
	DebugDrawRing(Vec2(m_screenSize.x * 0.5f, m_screenSize.y * 0.5f),
		(150.f + 50.f * SinDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
			(20.f + 5.f * CosDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
				Rgba8(255, 150, 0));

	RenderTextInBox();
	
	RenderAnimation();

	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->EndCamera(m_screenCamera);
}


void Game::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	g_theRenderer->ClearScreen(Rgba8(0, 0, 255));

	g_theRenderer->EndCamera(m_worldCamera);
}

void Game::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	g_theRenderer->BindTexture(nullptr);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::RenderTextInBox() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	BitmapFont* g_testFont = nullptr;

	// ...and then, each frame; draw two text strings on screen
	g_testFont = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/MyFixedFont"); // DO NOT SPECIFY FILE .EXTENSION!!  (Important later on.)

	std::vector<Vertex_PCU> textVerts;
	Vec2 aabbmins1 = Vec2(100.f, 200.f);
	Vec2 aabbmaxs1 = Vec2(400.f + 150.f * SinDegrees(50.f * static_cast<float>(GetCurrentTimeSeconds())), 400.f + 150.f * SinDegrees(50.f * static_cast<float>(GetCurrentTimeSeconds())));

	Vec2 aabbmins2 = Vec2(800.f, 200.f);
	Vec2 aabbmaxs2 = Vec2(1100.f + 150.f * SinDegrees(50.f * static_cast<float>(GetCurrentTimeSeconds())), 400.f + 150.f * SinDegrees(50.f * static_cast<float>(GetCurrentTimeSeconds())));

	Vec2 alignment = Vec2(0.5f + 0.5f * SinDegrees(50.f * static_cast<float>(GetCurrentTimeSeconds())), 0.5f + 0.5f * SinDegrees(50.f * static_cast<float>(GetCurrentTimeSeconds())));
	std::string text = "Hello, world\nIt's nice to have options!\nTest if the function works well";
	int maxGlyphsToDraw = static_cast<int>(GetCurrentTimeSeconds() * 10) % text.size();
	DrawSquare(aabbmins1, aabbmaxs1, Rgba8::BLACK);
	DrawSquare(aabbmins2, aabbmaxs2, Rgba8::BLACK);

	g_testFont->AddVertsForTextInBox2D(textVerts, AABB2(aabbmins1, aabbmaxs1), 15.f, text, Rgba8::GREY, 0.6f, alignment, SHRINK_TO_FIT);
	g_testFont->AddVertsForTextInBox2D(textVerts, AABB2(aabbmins2, aabbmaxs2), 15.f, text, Rgba8::GREY, 0.6f, alignment, OVERRUN);

	g_testFont->AddVertsForTextInBox2D(textVerts, AABB2(aabbmins1, aabbmaxs1), 15.f, text, Rgba8::RED, 0.6f, alignment, SHRINK_TO_FIT, maxGlyphsToDraw);
	g_testFont->AddVertsForTextInBox2D(textVerts, AABB2(aabbmins2, aabbmaxs2), 15.f, text, Rgba8::RED, 0.6f, alignment, OVERRUN, maxGlyphsToDraw);
	g_theRenderer->BindTexture(&g_testFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)textVerts.size(), textVerts.data());

	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::RenderAnimation() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	static float start = static_cast<float> (GetCurrentTimeSeconds());
	float seconds = static_cast<float> (GetCurrentTimeSeconds()) - start;

	std::vector<Vertex_PCU> animVerts;
	SpriteAnimDefinition* g_spriteAnimation1 = nullptr;
	SpriteAnimDefinition* g_spriteAnimation2 = nullptr;
	SpriteAnimDefinition* g_spriteAnimation3 = nullptr;
	
	SpriteSheet* spritesheet = new SpriteSheet(*g_textureLibrary[TEXTURE_TEST_ANIM], IntVec2(4, 4));

	g_spriteAnimation1 = new SpriteAnimDefinition(*spritesheet, 0, 4, 2.f, SpriteAnimPlaybackType::ONCE);
	g_spriteAnimation2 = new SpriteAnimDefinition(*spritesheet, 0, 4, 2.f, SpriteAnimPlaybackType::LOOP);
	g_spriteAnimation3 = new SpriteAnimDefinition(*spritesheet, 0, 4, 2.f, SpriteAnimPlaybackType::PINGPONG);

	AddVertsForAABB2D(animVerts, AABB2(200.f, 500.f, 300.f, 600.f), Rgba8::WHITE, g_spriteAnimation1->GetSpriteDefAtTime(seconds).GetUVs().m_mins, g_spriteAnimation1->GetSpriteDefAtTime(seconds).GetUVs().m_maxs);
	AddVertsForAABB2D(animVerts, AABB2(400.f, 500.f, 500.f, 600.f), Rgba8::WHITE, g_spriteAnimation2->GetSpriteDefAtTime(seconds).GetUVs().m_mins, g_spriteAnimation2->GetSpriteDefAtTime(seconds).GetUVs().m_maxs);
	AddVertsForAABB2D(animVerts, AABB2(600.f, 500.f, 700.f, 600.f), Rgba8::WHITE, g_spriteAnimation3->GetSpriteDefAtTime(seconds).GetUVs().m_mins, g_spriteAnimation3->GetSpriteDefAtTime(seconds).GetUVs().m_maxs);

	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_TEST_ANIM]);
	g_theRenderer->DrawVertexArray((int)animVerts.size(), animVerts.data());

	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::DebugRenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	g_theRenderer->EndCamera(m_worldCamera);
}

void Game::EnterGame()
{

	Shutdown();
	Startup();

	m_gameStatus = GAME_STATUS_PLAY;

	StopMusic(MUSIC_WELCOME);
	PlayMusicLoop(MUSIC_BGM);
	PlayMusic(MUSIC_START);

	g_theInput->EndFrame();
}

void Game::SetCamera()
{
	float world_size_x = static_cast<float> (m_currentMap->GetMapDimension().x);
	float world_size_y = static_cast<float> (m_currentMap->GetMapDimension().y);
	if (m_isDebugCamera) {

		m_worldCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(m_worldSize.x * 4, m_worldSize.y * 4));
	}
	else {
		if (m_player) {
			float worldCamera_minX = m_player->m_position.x - m_worldSize.x * 0.5f;
			float worldCamera_maxX = m_player->m_position.x + m_worldSize.x * 0.5f;
			float worldCamera_minY = m_player->m_position.y - m_worldSize.y * 0.5f;
			float worldCamera_maxY = m_player->m_position.y + m_worldSize.y * 0.5f;

			if (worldCamera_minX < 0.f) {
				worldCamera_minX = 0.f;
				worldCamera_maxX = worldCamera_minX + m_worldSize.x;
			}
			if (worldCamera_maxX > world_size_x) {
				worldCamera_maxX = world_size_x;
				worldCamera_minX = world_size_x - m_worldSize.x;
			}
			if (worldCamera_minY < 0.f) {
				worldCamera_minY = 0.f;
				worldCamera_maxY = worldCamera_minY + m_worldSize.y;
			}
			if (worldCamera_maxY > world_size_y) {
				worldCamera_maxY = world_size_y;
				worldCamera_minY = world_size_y - m_worldSize.y;
			}
			m_worldCamera.SetOrthoView(Vec2(worldCamera_minX, worldCamera_minY), Vec2(worldCamera_maxX, worldCamera_maxY));
		}
	}
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(m_screenSize.x, m_screenSize.y));
}

void Game::ReturnAttractModeVictory()
{
	ReturnAttractModeNormal();
}

void Game::ReturnAttractModeFailed()
{
	ReturnAttractModeNormal();

}

void Game::ReturnAttractModeNormal()
{
	StopMusic(MUSIC_BGM);
	PlayMusic(MUSIC_QUIT);
	PlayMusicLoop(MUSIC_WELCOME);
	m_gameStatus = GAME_STATUS_ATTRACT;
}

