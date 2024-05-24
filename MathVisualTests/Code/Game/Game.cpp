#define UNUSED(x) (void)(x);
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/App.hpp"
#include "Game/GameNearestPoint.hpp"
#include "Game/GameRaycastVsDisc2D.hpp"
#include "Game/GameRaycastVsAABBs.hpp"
#include "Game/GameRaycastVsLineSegments.hpp"
#include "Game/GameRaycastVsConvex.hpp"
#include "Game/GameBilliards2D.hpp"
#include "Game/GameTestShapes3D.hpp"
#include "Game/GamePachinkoMachine2D.hpp"
#include "Game/GameCurves2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"

Game::Game()
{

}

Game::~Game()
{
}

void Game::UpdateEntities(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void Game::DeleteGarbageEntities()
{

}

void Game::KillAll()
{

}

void Game::UpdateFromKeyboard(float deltaSeconds)
{
	UNUSED(deltaSeconds);

}

void Game::UpdateFromController(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void Game::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	
	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
			(20.f + 5.f * CosDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
				Rgba8(255, 150, 0));

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

	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));

	g_theRenderer->EndCamera(m_worldCamera);
}

void Game::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
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
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void Game::SetCamera()
{
	m_worldCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}

void Game::RandomizeShapes()
{
	for (int entityIndex = 0; entityIndex < m_shapes.size(); entityIndex++)
	{
		ShapeEntity2D*& tempEntity = m_shapes[entityIndex];
		tempEntity->Randomize();
	}
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

	m_isAttractMode = true;
}

Game* Game::CreateNewGameOfType(GameStatus type)
{
	Game* newGame = nullptr;
	switch (type)
	{
	case GAME_STATUS_NULL:
		break;
	case GAME_STATUS_NEAREST_POINT:
		newGame = new GameNearestPoint();
		break;
	case GAME_STATUS_RAYCAST_DISC_2D:
		newGame = new GameRaycastVsDisc2D();
		break;
	case GAME_STATUS_BILLIARDS_2D:
		newGame = new GameBilliards2D();
		break;
	case GAME_STATUS_TEST_SHAPE_3D:
		newGame = new GameTestShape3D();
		break;
	case GAME_STATUS_RAYCAST_LINE_2D:
		newGame = new GameRaycastVsLineSegments();
		break;
	case GAME_STATUS_RAYCAST_AABB_2D:
		newGame = new GameRaycastVsAABBs();
		break;
	case GAME_STATUS_RAYCAST_CONVEX:
		newGame = new GameRaycastVsConvex();
		break;
	case GAME_STATUS_PACHINKO_2D:
		newGame = new GamePachinkoMachine2D();
		break;
	case  GAME_STATUS_CURVES_2D:
		newGame = new GameCurves2D();
		break;
	case GAME_STATUS_NUM:
		break;
	}

	newGame->m_worldCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

	newGame->m_screenCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	return newGame;
}
