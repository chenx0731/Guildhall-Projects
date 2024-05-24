#define UNUSED(x) (void)(x);
#include "Game/GameNearestPoint.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

GameNearestPoint::GameNearestPoint() : Game()
{

	ShapeEntityDisc2D* disc = new ShapeEntityDisc2D();
	disc->Randomize();
	m_shapes.push_back(disc);
	ShapeEntityAABB2D* aabb = new ShapeEntityAABB2D();
	aabb->Randomize();
	m_shapes.push_back(aabb);
	ShapeEntityCapsule2D* capsule = new ShapeEntityCapsule2D();
	capsule->Randomize();
	m_shapes.push_back(capsule);
	ShapeEntityInfiniteLine2D* infiniteLine = new ShapeEntityInfiniteLine2D();
	infiniteLine->Randomize();
	m_shapes.push_back(infiniteLine);
	ShapeEntityLineSegment2D* line = new ShapeEntityLineSegment2D();
	line->Randomize();
	m_shapes.push_back(line);
	ShapeEntityOBB2D* obb = new ShapeEntityOBB2D();
	obb->Randomize();
	m_shapes.push_back(obb);
	m_cursor = Vec2(WORLD_CENTER_X, WORLD_CENTER_Y);
}

GameNearestPoint::~GameNearestPoint()
{

}

void GameNearestPoint::Startup()
{
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);

	SetCamera();

	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
}

void GameNearestPoint::Update(float deltaSeconds)
{
	UpdateFromKeyboard(deltaSeconds);
	UpdateFromController(deltaSeconds);
	UpdateEntities(deltaSeconds);
	SetCamera();

	if (m_isAttractMode)
	{
		return;
	}


	if (m_isOver)
	{
		m_hangingTime += deltaSeconds;
		if (m_hangingTime >= 3.f)
		{
			ReturnAttractModeVictory();
		}
	}
	
	if (!m_isPaused)
	{
		UpdateEntities(deltaSeconds);

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

void GameNearestPoint::Render() const
{
	g_theRenderer->BeginCamera(m_worldCamera);
	std::vector<Vertex_PCU> fontVerts;
	std::string title = "Mode (F6/F7 for prev/next): Get Nearest Point (2D)";
	std::string controls = "F8 to randomize; WASD move the dot";
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 90.f, 200.f, 100.f), 2.5f, title.c_str(), Rgba8(200, 200, 0), 0.7f, Vec2(0.01f, 0.7f));
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 77.f, 200.f, 100.f), 2.2f, controls.c_str(), Rgba8(0, 200, 100), 0.7f, Vec2(0.02f, 0.7f));
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	//AddVertsForTextTriangles2D(fontVerts, "Raycast vs. Discs 2D", Vec2(5.f, 92.f), 3.f, Rgba8(255, 255, 0));
	g_theRenderer->DrawVertexArray((int)fontVerts.size(), fontVerts.data());
	g_theRenderer->BindTexture(nullptr);
	for (int entityIndex = 0; entityIndex < m_shapes.size(); entityIndex++)
	{
		ShapeEntity2D* const tempEntity = m_shapes[entityIndex];
		tempEntity->Render();
	}
	DrawCircle(m_cursor, 0.5f, Rgba8(255, 255, 255));
	g_theRenderer->EndCamera(m_worldCamera);
}

void GameNearestPoint::Shutdown()
{
	for (int entityIndex = 0; entityIndex < m_shapes.size(); entityIndex++) {
		if (m_shapes[entityIndex]) {
			delete m_shapes[entityIndex];
			m_shapes[entityIndex] = nullptr;
		}
	}
}

void GameNearestPoint::UpdateEntities(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	for (int entityIndex = 0; entityIndex < m_shapes.size(); entityIndex++)
	{
		ShapeEntity2D*& tempEntity = m_shapes[entityIndex];
		tempEntity->UpdateForCursorPos(m_cursor);
	}
}

void GameNearestPoint::DeleteGarbageEntities()
{

}

void GameNearestPoint::KillAll()
{

}

void GameNearestPoint::UpdateFromKeyboard(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->IsKeyDown(KEYCODE_UPARROW))
	{
		m_cursor.y += deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_DOWNARROW))
	{
		m_cursor.y -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_LEFTARROW))
	{
		m_cursor.x -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_RIGHTARROW))
	{
		m_cursor.x += deltaSeconds * 20.f;
	}
	if (g_theInput->WasKeyJustPressed('P'))
	{
		m_isPaused = !m_isPaused;
	}
	if (g_theInput->WasKeyJustPressed(' ') || g_theInput->WasKeyJustPressed('N'))
	{
			
	}
	if (g_theInput->WasKeyJustPressed('O'))
	{
		m_isPaused = true;
		SetCamera();

		if (m_isAttractMode)
		{
			return;
		}

		UpdateEntities(deltaSeconds);

		DeleteGarbageEntities();

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
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
	{
		m_isDebug = !m_isDebug;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
	{
		RandomizeShapes();
	}
	if (g_theInput->WasKeyJustPressed('K'))
	{
		KillAll();
	}
}

void GameNearestPoint::UpdateFromController(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_START) 
		|| g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_A))
	{
		if (m_isAttractMode)
		{
			EnterGame();
		}
	}
}

void GameNearestPoint::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	
	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
			(20.f + 5.f * CosDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
				Rgba8(255, 150, 0));

	g_theRenderer->EndCamera(m_screenCamera);
}

void GameNearestPoint::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->EndCamera(m_screenCamera);
}


void GameNearestPoint::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));

	g_theRenderer->EndCamera(m_worldCamera);
}

void GameNearestPoint::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
	g_theRenderer->EndCamera(m_screenCamera);
}

void GameNearestPoint::DebugRenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);
	g_theRenderer->EndCamera(m_worldCamera);
}

void GameNearestPoint::EnterGame()
{
	Shutdown();
	Startup();
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void GameNearestPoint::SetCamera()
{
	m_worldCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}

void GameNearestPoint::RandomizeShapes()
{
	for (int entityIndex = 0; entityIndex < m_shapes.size(); entityIndex++)
	{
		ShapeEntity2D*& tempEntity = m_shapes[entityIndex];
		tempEntity->Randomize();
	}
}


