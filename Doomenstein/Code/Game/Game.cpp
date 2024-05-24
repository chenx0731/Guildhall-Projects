#define UNUSED(x) (void)(x);
#include "Game/Game.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/GameAttract.hpp"
#include "Game/GameLobby.hpp"
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

Game* g_theGames[(int)GameState::COUNT];

Vertex_PCU vertices[] = {
	Vertex_PCU(Vec2(800.f, 600.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(400.f, 200.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(1200.f, 200.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
};


Game::Game()
{
	m_clock = new Clock(Clock::GetSystemClock());

	
}

Game::~Game()
{

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
		case GameState::LOBBY:
			g_theGames[(int)state] = new GameLobby();
			break;
		}
	}
	return g_theGames[(int)state];
}

