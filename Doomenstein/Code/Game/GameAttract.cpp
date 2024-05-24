#define UNUSED(x) (void)(x);
#include "Game/GameAttract.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/Player.hpp"
#include "Game/App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
GameAttract::GameAttract()
{
	m_clock = new Clock(Clock::GetSystemClock());

	
}

GameAttract::~GameAttract()
{

}

void GameAttract::Startup()
{
	
}

void GameAttract::Update()
{
	SetCamera();
	UpdateFromKeyboard();
	
}

void GameAttract::Render() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->SetModelConstants();

	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * m_clock->GetTotalSeconds())),
		(20.f + 5.f * CosDegrees(100.f * m_clock->GetTotalSeconds())),
		Rgba8(255, 150, 0));
	std::vector<Vertex_PCU> texts;
	std::string string = Stringf("Press SPACE to join with mouse and keyboard\nPress START to join with controller\nPress ESCAPE or BACK to exit");
	g_theFont->AddVertsForTextInBox2D(texts, AABB2(0.f, 0.f, 1600.f, 300.f), 20.f, string);
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)texts.size(), texts.data());
	g_theRenderer->BindTexture(nullptr);

	g_theRenderer->EndCamera(m_screenCamera);
	
}

void GameAttract::Shutdown()
{
}

void GameAttract::Enter()
{
	SetCamera();
	//SetVolume(MUSIC_MAINMENU, )
	PlayMusicLoop(MUSIC_MAINMENU);
}

void GameAttract::Exit()
{
	
}


void GameAttract::SetCamera()
{
	m_screenCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}

void GameAttract::UpdateFromKeyboard()
{
	XboxController tempController;
	tempController = g_theInput->GetController(0);
	if (g_theInput->WasKeyJustPressed(' ')) {
		PlayMusic(MUSIC_CLICK);
		g_theGameState = GameState::LOBBY;
		GamePlaying* playing = (GamePlaying*)g_theGames[(int)GameState::PLAYING];
		//playing->m_players[0] = new Player(nullptr);
		//playing->m_players[0]->m_type = ControllerType::KEYBOARD;
		playing->AddPlayerWithSpecController(ControllerType::KEYBOARD, 0);
		Exit();
		g_theGames[(int)g_theGameState]->Enter();
	}
	if (tempController.GetButton(XBOX_BUTTON_START).WasKeyJustPressed()) {
		PlayMusic(MUSIC_CLICK);
		g_theGameState = GameState::LOBBY;
		GamePlaying* playing = (GamePlaying*)g_theGames[(int)GameState::PLAYING];
		//playing->m_players[0] = new Player(nullptr);
		//playing->m_players[0]->m_type = ControllerType::CONTROLLER0;
		playing->AddPlayerWithSpecController(ControllerType::CONTROLLER0, 0);
		Exit();
		g_theGames[(int)g_theGameState]->Enter();
	}
}
