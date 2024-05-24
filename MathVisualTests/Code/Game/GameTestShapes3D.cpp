#define UNUSED(x) (void)(x);
#include "Game/GameTestShapes3D.hpp"
#include "Game/ShapeEntity3D.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

GameTestShape3D::GameTestShape3D() : Game()
{
	//int totalNum = g_rng->RollRandomIntInRange(10, 12);
	//int aabbNum = g_rng->RollRandomIntInRange(2, 4);
	//int sphereNum = g_rng->RollRandomIntInRange(2, 4);
	//int cylinderNum = totalNum - aabbNum - sphereNum;
	g_textureLibrary[TEXTURE_TEST3D] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");
	//for (int discIndex = 0; discIndex < discNum; discIndex++) {
	//	ShapeEntityDisc2D* disc = new ShapeEntityDisc2D();
	//	disc->Randomize();
	//	
	//	m_shapes.push_back(disc);
	//}
	for (int entityIndex = 0; entityIndex < 12; entityIndex++) {
		ShapeEntity3D* shape3d = new ShapeEntity3D();

		float x = g_rng->RollRandomFloatInRange(-10.f, 10.f);
		float y = g_rng->RollRandomFloatInRange(-10.f, 10.f);
		float z = g_rng->RollRandomFloatInRange(-10.f, 10.f);
		float radius = g_rng->RollRandomFloatInRange(1.f, 3.f);
		float aabbX = g_rng->RollRandomFloatInRange(1.f, 3.f);
		float aabbY = g_rng->RollRandomFloatInRange(1.f, 3.f);
		float aabbZ = g_rng->RollRandomFloatInRange(1.f, 3.f);
		shape3d->m_position = Vec3(x, y, z);

		if (entityIndex < 2) {
			AABB3 bounds = AABB3(Vec3(0.f, 0.f, 0.f), Vec3(aabbX, aabbY, aabbZ));
			bounds.SetCenter(Vec3(x, y, z));
			AddVertsForAABB3D(shape3d->m_verts, bounds);
			shape3d->m_texture = g_textureLibrary[TEXTURE_TEST3D];
		}
		else if (entityIndex < 4) {
			AABB3 bounds = AABB3(Vec3(0.f, 0.f, 0.f), Vec3(aabbX, aabbY, aabbZ));
			bounds.SetCenter(Vec3(x, y, z));
			AddVertsForAABBWireframe3D(shape3d->m_verts, bounds, 0.1f);
		}
		else if (entityIndex < 6) {
			float numSlice = g_rng->RollRandomFloatInRange(8.f, 16.f);
			AddVertsForCylinderZ3D(shape3d->m_verts, Vec2(x, y), FloatRange(z, z + radius), aabbX, numSlice);
			shape3d->m_texture = g_textureLibrary[TEXTURE_TEST3D];
		}
		else if (entityIndex < 8) {
			float numSlice = g_rng->RollRandomFloatInRange(8.f, 16.f);
			AddVertsForCylinderZWireframe3D(shape3d->m_verts, Vec2(x, y), FloatRange(z, z + radius), aabbX, numSlice, 0.1f);
		}
		else if (entityIndex < 10)
		{
			float numSlice = g_rng->RollRandomFloatInRange(8.f, 16.f);
			float numStack = g_rng->RollRandomFloatInRange(8.f, 16.f);

			AddVertsForUVSphereZ3D(shape3d->m_verts, shape3d->m_position, radius, numSlice, numStack);
			shape3d->m_texture = g_textureLibrary[TEXTURE_TEST3D];
		}
		else
		{
			float numSlice = g_rng->RollRandomFloatInRange(8.f, 16.f);
			float numStack = g_rng->RollRandomFloatInRange(8.f, 16.f);
			AddVertsForUVSphereZWireframe3D(shape3d->m_verts, shape3d->m_position, radius, numSlice, numStack, 0.05f);
		}

		m_shapes.push_back(shape3d);
	}
	m_cursor = Vec2(WORLD_CENTER_X, WORLD_CENTER_Y);
	
}

GameTestShape3D::~GameTestShape3D()
{

}

void GameTestShape3D::Startup()
{
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);
	m_player = new Player(this, Vec3(0.f, 0.f, 0.f));
	m_player->m_camera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	SetCamera();
	g_theRenderer->SetDepthMode(DepthMode::ENABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theInput->SetCursorMode(true, true);
}

void GameTestShape3D::Update(float deltaSeconds)
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

void GameTestShape3D::Render() const
{

	RenderEntities();
	//RenderGrid();

	RenderCoords();

	RenderUI();
	//g_theRenderer->EndCamera(m_player->m_camera);
}

void GameTestShape3D::Shutdown()
{
	g_theInput->SetCursorMode(false, false);
}

void GameTestShape3D::UpdateEntities(float deltaSeconds)
{
	UNUSED(deltaSeconds);
		
	m_player->Update(deltaSeconds);

}

void GameTestShape3D::DeleteGarbageEntities()
{

}

void GameTestShape3D::KillAll()
{

}

void GameTestShape3D::UpdateFromKeyboard(float deltaSeconds)
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

void GameTestShape3D::UpdateFromController(float deltaSeconds)
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

void GameTestShape3D::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	
	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
			(20.f + 5.f * CosDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
				Rgba8(255, 150, 0));

	g_theRenderer->EndCamera(m_screenCamera);
}

void GameTestShape3D::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->SetModelConstants();

	std::vector<Vertex_PCU> fontVerts;
	std::string title = "Mode (F6/F7 for prev/next): Test Shape (3D)";
	std::string controls = "F8 to randomize; WASD = fly horizontal; QE = fly vertical, hold T = slow";
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 720.f, 1600.f, 800.f), 20.f, title.c_str(), Rgba8(200, 200, 0), 0.7f, Vec2(0.01f, 0.7f));
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 616.f, 1600.f, 800.f), 20.f, controls.c_str(), Rgba8(0, 200, 100), 0.7f, Vec2(0.02f, 0.7f));
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	//AddVertsForTextTriangles2D(fontVerts, "Raycast vs. Discs 2D", Vec2(5.f, 92.f), 3.f, Rgba8(255, 255, 0));
	g_theRenderer->DrawVertexArray((int)fontVerts.size(), fontVerts.data());
	g_theRenderer->BindTexture(nullptr);

	g_theRenderer->EndCamera(m_screenCamera);
}


void GameTestShape3D::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_player->m_camera);

	//g_theRenderer->ClearScreen(Rgba8(0, 0, 0));
	for (int i = 0; i < m_shapes.size(); i++) {
		m_shapes[i]->Render();
	}

	g_theRenderer->EndCamera(m_player->m_camera);
}

void GameTestShape3D::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
	g_theRenderer->EndCamera(m_screenCamera);
}

void GameTestShape3D::RenderGrid() const
{
	g_theRenderer->BeginCamera(m_player->m_camera);
	//g_theRenderer->SetModelConstants();
	//std::vector<Vertex_PCU> sphere;
	//AddVertsForSpere3D(sphere, Vec3(10.f, -5.f, 1.f), 1.f);
	//g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_SPHERE]);
	//g_theRenderer->DrawVertexArray((int)sphere.size(), sphere.data());
	g_theRenderer->SetModelConstants();
	g_theRenderer->BindTexture(nullptr);
	std::vector<Vertex_PCU> grids;
	for (int xIndex = -50; xIndex <= 50; xIndex++) {
		float offset = 0.05f;
		Rgba8 color = Rgba8::GREY;
		if (xIndex % 5 == 0) {
			color = Rgba8::GREEN;
			offset *= 2;
		}
		Vec2 mins = Vec2(float(xIndex) - offset, -50.f);
		Vec2 maxs = Vec2(float(xIndex) + offset, 50.f);
		AddVertsForAABB2D(grids, AABB2(mins, maxs), color, Vec2(0.f, 0.f), Vec2(1.f, 1.f));
	}

	for (int yIndex = -50; yIndex <= 50; yIndex++) {
		float offset = 0.05f;
		Rgba8 color = Rgba8::GREY;
		if (yIndex % 5 == 0) {
			color = Rgba8::RED;
			offset *= 2;
		}
		Vec2 mins = Vec2(-50.f, float(yIndex) - offset);
		Vec2 maxs = Vec2(50.f, float(yIndex) + offset);
		AddVertsForAABB2D(grids, AABB2(mins, maxs), color, Vec2(0.f, 0.f), Vec2(1.f, 1.f));
	}
	g_theRenderer->DrawVertexArray((int)grids.size(), grids.data());
	g_theRenderer->EndCamera(m_player->m_camera);
}

void GameTestShape3D::RenderCoords() const
{
	g_theRenderer->BeginCamera(m_player->m_camera);

	g_theRenderer->SetModelConstants();
	Vec3 xBasis = Vec3(1.f, 0.f, 0.f);
	Vec3 yBasis = Vec3(0.f, 1.f, 0.f);
	Vec3 zBasis = Vec3(0.f, 0.f, 1.f);

	Vec3 iBasis;
	Vec3 jBasis;
	Vec3 kBasis;

	m_player->m_orientation.GetVectors_XFwd_YLeft_ZUp(iBasis, jBasis, kBasis);

	std::vector<Vertex_PCU> coordsVerts;
	Vec3 start = Vec3(0.f, 0.f, 0.f);

	AddVertsForLineSegment3D(coordsVerts, start, start + xBasis, 0.1f, Rgba8::RED);
	AddVertsForLineSegment3D(coordsVerts, start, start + yBasis, 0.1f, Rgba8::GREEN);
	AddVertsForLineSegment3D(coordsVerts, start, start + zBasis, 0.1f, Rgba8::BLUE);

	g_theRenderer->BindTexture(nullptr);

	start = m_player->m_position + iBasis * 0.2f;

	AddVertsForLineSegment3D(coordsVerts, start, start + xBasis * 0.01f, 0.001f, Rgba8::RED);
	AddVertsForLineSegment3D(coordsVerts, start, start + yBasis * 0.01f, 0.001f, Rgba8::GREEN);
	AddVertsForLineSegment3D(coordsVerts, start, start + zBasis * 0.01f, 0.001f, Rgba8::BLUE);

	g_theRenderer->BindTexture(nullptr);

	g_theRenderer->DrawVertexArray((int)coordsVerts.size(), coordsVerts.data());

	g_theRenderer->EndCamera(m_player->m_camera);
}

void GameTestShape3D::DebugRenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);
	g_theRenderer->EndCamera(m_worldCamera);
}

void GameTestShape3D::EnterGame()
{
	Shutdown();
	Startup();
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void GameTestShape3D::SetCamera()
{
	m_player->m_camera.SetPerspectiveView(2.f, 60.f, 0.1f, 100.f);
	m_player->m_camera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}

void GameTestShape3D::RandomizeShapes()
{
	m_shapes.clear();
	for (int entityIndex = 0; entityIndex < 12; entityIndex++) {
		ShapeEntity3D* shape3d = new ShapeEntity3D();

		float x = g_rng->RollRandomFloatInRange(-10.f, 10.f);
		float y = g_rng->RollRandomFloatInRange(-10.f, 10.f);
		float z = g_rng->RollRandomFloatInRange(-10.f, 10.f);
		float radius = g_rng->RollRandomFloatInRange(1.f, 3.f);
		float aabbX = g_rng->RollRandomFloatInRange(1.f, 3.f);
		float aabbY = g_rng->RollRandomFloatInRange(1.f, 3.f);
		float aabbZ = g_rng->RollRandomFloatInRange(1.f, 3.f);
		shape3d->m_position = Vec3(x, y, z);

		if (entityIndex < 2) {
			AABB3 bounds = AABB3(Vec3(0.f, 0.f, 0.f), Vec3(aabbX, aabbY, aabbZ));
			bounds.SetCenter(Vec3(x, y, z));
			AddVertsForAABB3D(shape3d->m_verts, bounds);
			shape3d->m_texture = g_textureLibrary[TEXTURE_TEST3D];
		}
		else if (entityIndex < 4) {
			AABB3 bounds = AABB3(Vec3(0.f, 0.f, 0.f), Vec3(aabbX, aabbY, aabbZ));
			bounds.SetCenter(Vec3(x, y, z));
			AddVertsForAABBWireframe3D(shape3d->m_verts, bounds, 0.1f);
		}
		else if (entityIndex < 6) {
			float numSlice = g_rng->RollRandomFloatInRange(8.f, 16.f);
			AddVertsForCylinderZ3D(shape3d->m_verts, Vec2(x, y), FloatRange(z, z + radius), aabbX, numSlice);
			shape3d->m_texture = g_textureLibrary[TEXTURE_TEST3D];
		}
		else if (entityIndex < 8) {
			float numSlice = g_rng->RollRandomFloatInRange(8.f, 16.f);
			AddVertsForCylinderZWireframe3D(shape3d->m_verts, Vec2(x, y), FloatRange(z, z + radius), aabbX, numSlice, 0.1f);
		}
		else if (entityIndex < 10)
		{
			float numSlice = g_rng->RollRandomFloatInRange(8.f, 16.f);
			float numStack = g_rng->RollRandomFloatInRange(8.f, 16.f);

			AddVertsForUVSphereZ3D(shape3d->m_verts, shape3d->m_position, radius, numSlice, numStack);
			shape3d->m_texture = g_textureLibrary[TEXTURE_TEST3D];
		}
		else
		{
			float numSlice = g_rng->RollRandomFloatInRange(8.f, 16.f);
			float numStack = g_rng->RollRandomFloatInRange(8.f, 16.f);
			AddVertsForUVSphereZWireframe3D(shape3d->m_verts, shape3d->m_position, radius, numSlice, numStack, 0.05f);
		}

		m_shapes.push_back(shape3d);
	}
}

void GameTestShape3D::ReturnAttractModeVictory()
{
	ReturnAttractModeNormal();
}

void GameTestShape3D::ReturnAttractModeFailed()
{
	ReturnAttractModeNormal();

}

void GameTestShape3D::ReturnAttractModeNormal()
{

	m_isAttractMode = true;
}

