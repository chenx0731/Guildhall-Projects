#define UNUSED(x) (void)(x);
#include "Game/GameRaycastVsAABBs.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
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

GameRaycastVsAABBs::GameRaycastVsAABBs() : Game()
{
	int discNum = g_rng->RollRandomIntInRange(10, 12);
	for (int discIndex = 0; discIndex < discNum; discIndex++) {
		ShapeEntityAABB2D* aabb = new ShapeEntityAABB2D();
		aabb->Randomize();
		
		m_shapes.push_back(aabb);
	}
	m_cursor = Vec2(WORLD_CENTER_X, WORLD_CENTER_Y);
}

GameRaycastVsAABBs::~GameRaycastVsAABBs()
{

}

void GameRaycastVsAABBs::Startup()
{
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);

	SetCamera();

	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
}

void GameRaycastVsAABBs::Update(float deltaSeconds)
{
	UpdateFromKeyboard(deltaSeconds);
	UpdateFromController(deltaSeconds);
	UpdateEntities(deltaSeconds);
	SetCamera();

	m_cursor = g_theWindow->GetNormalizedCursorPos();
	m_cursor.x *= WORLD_SIZE_X;
	m_cursor.y *= WORLD_SIZE_Y;
	
	if (!m_setStart)
		m_arrowStart = m_cursor;
	if (!m_setEnd)
		m_arrowEnd = m_cursor;

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

void GameRaycastVsAABBs::Render() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	std::vector<Vertex_PCU> fontVerts;
	std::string title = "Mode (F6/F7 for prev/next): Raycast Vs AABB (2D)";
	std::string controls = "F8 to randomize; LMB/RMB set ray start/end; ESDF move start, IJKL move end, hold T = slow";
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 90.f, 200.f, 100.f), 2.5f, title.c_str(), Rgba8(200, 200, 0), 0.7f, Vec2(0.01f, 0.7f));
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 77.f, 200.f, 100.f), 2.2f, controls.c_str(), Rgba8(0, 200, 100), 0.7f, Vec2(0.02f, 0.7f));
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	//AddVertsForTextTriangles2D(fontVerts, "Raycast vs. Discs 2D", Vec2(5.f, 92.f), 3.f, Rgba8(255, 255, 0));
	g_theRenderer->DrawVertexArray((int)fontVerts.size(), fontVerts.data());
	g_theRenderer->BindTexture(nullptr);
	for (int entityIndex = 0; entityIndex < m_shapes.size(); entityIndex++)
	{
		ShapeEntity2D* const tempEntity = m_shapes[entityIndex];
		if (entityIndex != m_shapes.size() - 1)
			m_shapes[entityIndex]->m_rayCastResult.m_didImpact = false;
		if (!m_didImpact)
		{
			m_shapes[entityIndex]->m_rayCastResult.m_didImpact = false;
		}
		tempEntity->Render();
	}

	std::vector<Vertex_PCU> arrowVerts;
	if (!m_didImpact) {
		AddVertsForArrow2D(arrowVerts, m_arrowStart, m_arrowEnd, 1.f, 0.2f, Rgba8(0, 255, 0));
		g_theRenderer->DrawVertexArray((int)arrowVerts.size(), arrowVerts.data());
	}
	else {
		AddVertsForArrow2D(arrowVerts, m_arrowStart, m_arrowEnd, 1.f, 0.2f, Rgba8(127, 127, 127));
		g_theRenderer->DrawVertexArray((int)arrowVerts.size(), arrowVerts.data());
		std::vector<Vertex_PCU> impactVerts;
		AddVertsForArrow2D(impactVerts, m_arrowStart, m_impactPos, 1.f, 0.2f, Rgba8(255, 0, 0));
		g_theRenderer->DrawVertexArray((int)impactVerts.size(), impactVerts.data());
		std::vector<Vertex_PCU> impactNormalVerts;
		AddVertsForArrow2D(impactNormalVerts, m_impactPos, m_impactPos + m_impactNormal * 10.f, 1.f, 0.2f, Rgba8(200, 200, 0));
		g_theRenderer->DrawVertexArray((int)impactNormalVerts.size(), impactNormalVerts.data());
		std::vector<Vertex_PCU> impactPosVerts;
		AddVertsForDisc2D(impactPosVerts, m_impactPos, 0.5f, Rgba8(255, 255, 255));
		g_theRenderer->DrawVertexArray((int)impactPosVerts.size(), impactPosVerts.data());
	}
	
	DrawCircle(m_cursor, 0.5f, Rgba8(255, 255, 255));
	g_theRenderer->EndCamera(m_worldCamera);
}

void GameRaycastVsAABBs::Shutdown()
{

}

void GameRaycastVsAABBs::UpdateEntities(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	m_didImpact = false;
	ShapeEntity2D* firstImpact = nullptr;
	float minImpactDis = 9999.f;
	int impactIndex = 0;
	for (int entityIndex = (int)m_shapes.size() - 1; entityIndex >= 0; entityIndex--) {
		ShapeEntity2D*& tempEntity = m_shapes[entityIndex];
		tempEntity->UpdateForRaycast(m_arrowStart, m_arrowEnd);
		RaycastResult2D result = tempEntity->IsImpactedByRaycast();
		if (result.m_didImpact && result.m_impactDis < minImpactDis) {
			firstImpact = tempEntity;
			minImpactDis = result.m_impactDis;
			impactIndex = entityIndex;
			m_impactPos = result.m_impactPos;
			m_impactNormal = result.m_impactNormal;
		}
	}

	if (firstImpact != nullptr) {
		m_didImpact = true;
		m_shapes[impactIndex] = m_shapes[m_shapes.size() - 1];
		m_shapes[m_shapes.size() - 1] = firstImpact;
	}
		
	
}

void GameRaycastVsAABBs::DeleteGarbageEntities()
{

}

void GameRaycastVsAABBs::KillAll()
{

}

void GameRaycastVsAABBs::UpdateFromKeyboard(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->IsKeyDown(KEYCODE_UPARROW))
	{
		m_arrowStart.y += deltaSeconds * 20.f;
		m_arrowEnd.y += deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_DOWNARROW))
	{
		m_arrowStart.y -= deltaSeconds * 20.f;
		m_arrowEnd.y -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_LEFTARROW))
	{
		m_arrowStart.x -= deltaSeconds * 20.f;
		m_arrowEnd.x -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_RIGHTARROW))
	{
		m_arrowStart.x += deltaSeconds * 20.f;
		m_arrowEnd.x += deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown('E'))
	{
		m_arrowStart.y += deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown('D'))
	{
		m_arrowStart.y -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown('S'))
	{
		m_arrowStart.x -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown('F'))
	{
		m_arrowStart.x += deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown('I'))
	{
		m_arrowEnd.y += deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown('K'))
	{
		m_arrowEnd.y -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown('J'))
	{
		m_arrowEnd.x -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown('L'))
	{
		m_arrowEnd.x += deltaSeconds * 20.f;
	}
	if (g_theInput->WasKeyJustPressed('P'))
	{
		m_isPaused = !m_isPaused;
	}
	if (g_theInput->WasKeyJustPressed(' ') || g_theInput->WasKeyJustPressed('N'))
	{
		if (m_isAttractMode)
		{
			EnterGame();
		}
			
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
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE))
	{
		m_arrowStart = g_theWindow->GetNormalizedCursorPos();
		m_arrowStart.x *= WORLD_SIZE_X;
		m_arrowStart.y *= WORLD_SIZE_Y;
		m_setStart = !m_setStart;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHT_MOUSE))
	{
		m_arrowEnd = g_theWindow->GetNormalizedCursorPos();
		m_arrowEnd.x *= WORLD_SIZE_X;
		m_arrowEnd.y *= WORLD_SIZE_Y;
		m_setEnd = !m_setEnd;
	}
}

void GameRaycastVsAABBs::UpdateFromController(float deltaSeconds)
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

void GameRaycastVsAABBs::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	
	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
			(20.f + 5.f * CosDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
				Rgba8(255, 150, 0));

	g_theRenderer->EndCamera(m_screenCamera);
}

void GameRaycastVsAABBs::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->EndCamera(m_screenCamera);
}


void GameRaycastVsAABBs::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));

	g_theRenderer->EndCamera(m_worldCamera);
}

void GameRaycastVsAABBs::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
	g_theRenderer->EndCamera(m_screenCamera);
}

void GameRaycastVsAABBs::DebugRenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);
	g_theRenderer->EndCamera(m_worldCamera);
}

void GameRaycastVsAABBs::EnterGame()
{
	Shutdown();
	Startup();
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void GameRaycastVsAABBs::SetCamera()
{
	m_worldCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}

void GameRaycastVsAABBs::RandomizeShapes()
{
	for (int entityIndex = 0; entityIndex < m_shapes.size(); entityIndex++)
	{
		ShapeEntity2D*& tempEntity = m_shapes[entityIndex];
		tempEntity->Randomize();
	}
}

void GameRaycastVsAABBs::ReturnAttractModeVictory()
{
	ReturnAttractModeNormal();
}

void GameRaycastVsAABBs::ReturnAttractModeFailed()
{
	ReturnAttractModeNormal();

}

void GameRaycastVsAABBs::ReturnAttractModeNormal()
{

	m_isAttractMode = true;
}

