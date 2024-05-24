#define UNUSED(x) (void)(x);
#include "Game/Game.hpp"
#include "Game/GameAttract.hpp"
#include "Game/GameMain.hpp"
#include "Game/GamePause.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Game/App.hpp"
#include "Game/Map.hpp"
#include "Game/Model.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/UnitDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/UI/Button.hpp"

Game* g_theGames[(int)GameState::COUNT];

Game::Game()
{
	m_clock = new Clock(Clock::GetSystemClock());
	m_worldCamera.m_position = Vec3(3.f, 19.f, 14.f);
	m_worldCamera.m_orientation = EulerAngles(255.f, 60.f, 0.f);
	m_screenCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	m_worldCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

}

Game::~Game()
{
	for (int buttonIndex = 0; buttonIndex < (int)m_buttons.size(); buttonIndex++) {
		delete m_buttons[buttonIndex];
		m_buttons[buttonIndex] = nullptr;
	}
}


void Game::Update()
{


}

void Game::Render() const
{


}

void Game::Shutdown()
{

}

Game* Game::CreateOrGetGameStateOfType(GameState state)
{
	if (g_theGames[(int)state] == nullptr) {
		switch (state)
		{
		case GameState::ATTRACT:
			g_theGames[(int)state] = new GameAttract();
			break;
		case GameState::PLAYING:
			g_theGames[(int)state] = new GamePlaying();
			break;
		case GameState::MAIN:
			g_theGames[(int)state] = new GameMain();
			break;
		case GameState::PAUSE:
			g_theGames[(int)state] = new GamePause();
			break;
		}
	}
	return g_theGames[(int)state];
}

