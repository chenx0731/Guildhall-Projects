#define UNUSED(x) (void)(x);
#include "Game/GamePachinkoMachine2D.hpp"
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

GamePachinkoMachine2D::GamePachinkoMachine2D() : Game()
{
	int discNum = 10;
	for (int discIndex = 0; discIndex < discNum; discIndex++) {
		ShapeEntityDisc2D* disc = new ShapeEntityDisc2D();
		//Billiard* disc = new Billiard();
		disc->Randomize();
		float elasticity = g_rng->RollRandomFloatZeroToOne();
		unsigned char g = DenormalizeByte(elasticity);
		disc->m_color = Rgba8(255 - g, g, 0, 150);
		disc->m_elasticity = elasticity;
		m_discs.push_back(disc);
	}
	for (int discIndex = 0; discIndex < discNum; discIndex++) {
		ShapeEntityOBB2D* disc = new ShapeEntityOBB2D();
		//Billiard* disc = new Billiard();
		disc->Randomize();
		float elasticity = g_rng->RollRandomFloatZeroToOne();
		unsigned char g = DenormalizeByte(elasticity);
		disc->m_color = Rgba8(255 - g, g, 0, 150);
		disc->m_elasticity = elasticity;
		m_obbs.push_back(disc);
	}
	for (int discIndex = 0; discIndex < discNum; discIndex++) {
		ShapeEntityCapsule2D* disc = new ShapeEntityCapsule2D();
		//Billiard* disc = new Billiard();
		disc->Randomize();
		float elasticity = g_rng->RollRandomFloatZeroToOne();
		unsigned char g = DenormalizeByte(elasticity);
		disc->m_color = Rgba8(255 - g, g, 0, 150);
		disc->m_elasticity = elasticity;
		m_capsules.push_back(disc);
	}
	
	m_cursor = Vec2(WORLD_CENTER_X, WORLD_CENTER_Y);
}

GamePachinkoMachine2D::~GamePachinkoMachine2D()
{

}

void GamePachinkoMachine2D::Startup()
{
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);

	SetCamera();

	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
}

void GamePachinkoMachine2D::Update(float deltaSeconds)
{
	UpdateFromKeyboard(deltaSeconds);
	UpdateFromController(deltaSeconds);

	m_deltaSeconds = deltaSeconds;
	if (m_isFixed) {
		m_remainTime += deltaSeconds;
		while (m_remainTime > m_fixedTime) {
			UpdateEntities(m_fixedTime);
			m_remainTime -= m_fixedTime;
		}
	}
	else {
		UpdateEntities(deltaSeconds);
	}

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

void GamePachinkoMachine2D::Render() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	for (int entityIndex = 0; entityIndex < m_billiards.size(); entityIndex++)
	{
		ShapeEntity2D* const tempEntity = m_billiards[entityIndex];
		tempEntity->Render();
	}
	for (int entityIndex = 0; entityIndex < m_discs.size(); entityIndex++)
	{
		ShapeEntity2D* const tempEntity = m_discs[entityIndex];
		tempEntity->Render();
	}
	for (int entityIndex = 0; entityIndex < m_obbs.size(); entityIndex++)
	{
		ShapeEntity2D* const tempEntity = m_obbs[entityIndex];
		tempEntity->Render();
	}
	for (int entityIndex = 0; entityIndex < m_capsules.size(); entityIndex++)
	{
		ShapeEntity2D* const tempEntity = m_capsules[entityIndex];
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
	DebugDrawLine(Vec2(0.f, 0.f), Vec2(0.f, WORLD_SIZE_Y), 2.f, Rgba8(100, 255, 255));
	DebugDrawLine(Vec2(WORLD_SIZE_X, 0.f), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y), 2.f, Rgba8(100, 255, 255));
	if (m_isBounce)
		DebugDrawLine(Vec2(0.f, 0.f), Vec2(WORLD_SIZE_X, 0.f), 2.f, Rgba8(100, 255, 255));
	std::vector<Vertex_PCU> fontVerts;
	std::string title = "Mode (F6/F7 for prev/next): Pachinko Machine (2D)";
	std::string controls = "F8 resets; LMB/RMB/ESDF/IJKL move spawn; T = slow, space(N) = ball(s), B=bounce";
	std::string fixedTimestep = Stringf(", timestep=%.2fms (V,[,]), dt=%.1fms", m_fixedTime * 1000.f, m_deltaSeconds * 1000.f);
	std::string notfixedTimestep = Stringf(", variable Timestep(V), dt=%.1fms", m_deltaSeconds * 1000.f);
	if (m_isFixed)
		controls.append(fixedTimestep);
	else
		controls.append(notfixedTimestep);
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 90.f, 200.f, 100.f), 2.5f, title.c_str(), Rgba8(200, 200, 0), 0.7f, Vec2(0.01f, 0.7f));
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 77.f, 200.f, 100.f), 2.2f, controls.c_str(), Rgba8(0, 200, 100), 0.7f, Vec2(0.02f, 0.7f));
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	//AddVertsForTextTriangles2D(fontVerts, "Raycast vs. Discs 2D", Vec2(5.f, 92.f), 3.f, Rgba8(255, 255, 0));
	g_theRenderer->DrawVertexArray((int)fontVerts.size(), fontVerts.data());
	g_theRenderer->BindTexture(nullptr);
	DrawCircle(m_cursor, 0.5f, Rgba8(255, 255, 255));
	DebugDrawRing(m_arrowStart, 1.f, 0.2f, Rgba8(100, 100, 255));
	DebugDrawRing(m_arrowStart, 3.f, 0.2f, Rgba8(100, 100, 255));
	g_theRenderer->EndCamera(m_worldCamera);
}

void GamePachinkoMachine2D::Shutdown()
{

}

void GamePachinkoMachine2D::UpdateEntities(float deltaSeconds)
{
	for (int billiardIndex = 0; billiardIndex < m_billiards.size(); billiardIndex++) {
		ShapeEntityDisc2D* temp = (ShapeEntityDisc2D*)m_billiards[billiardIndex];
		temp->m_velocity += deltaSeconds * temp->m_acceleration;
		temp->m_center += deltaSeconds * temp->m_velocity;
		CheckIfOffScreen(temp->m_center, temp->m_radius, temp->m_velocity);
	}

	for (int i = 0; i < m_billiards.size(); i++) {
		ShapeEntityDisc2D* tempA = (ShapeEntityDisc2D*)m_billiards[i];
		for (int j = i + 1; j < m_billiards.size(); j++) {
			//Billiard* tempB = (Billiard*)m_billiards[j];
			ShapeEntityDisc2D* tempB = (ShapeEntityDisc2D*)m_billiards[j];
			BounceDiscOffEachOther2D(tempA->m_center, tempA->m_radius, tempA->m_velocity, tempB->m_center, tempB->m_radius, tempB->m_velocity, tempA->m_elasticity, tempB->m_elasticity);
		}
	}

	for (int i = 0; i < m_billiards.size(); i++) {
		ShapeEntityDisc2D* tempA = (ShapeEntityDisc2D*)m_billiards[i];
		for (int j = 0; j < m_discs.size(); j++) {
			ShapeEntityDisc2D* tempB = (ShapeEntityDisc2D*)m_discs[j];
			BounceDiscOffFixedDisc2D(tempA->m_center, tempA->m_radius, tempA->m_velocity, tempB->m_center, tempB->m_radius, tempA->m_elasticity, tempB->m_elasticity);
		}
	}

	for (int i = 0; i < m_billiards.size(); i++) {
		ShapeEntityDisc2D* tempA = (ShapeEntityDisc2D*)m_billiards[i];
		for (int j = 0; j < m_obbs.size(); j++) {
			ShapeEntityOBB2D* tempB = (ShapeEntityOBB2D*)m_obbs[j];
			BounceDiscOffFixedOBB2D(tempA->m_center, tempA->m_radius, tempA->m_velocity, tempB->m_box, tempA->m_elasticity, tempB->m_elasticity);
		}
	}
	
	for (int i = 0; i < m_billiards.size(); i++) {
		ShapeEntityDisc2D* tempA = (ShapeEntityDisc2D*)m_billiards[i];
		for (int j = 0; j < m_capsules.size(); j++) {
			ShapeEntityCapsule2D* tempB = (ShapeEntityCapsule2D*)m_capsules[j];
			BounceDiscOffFixedCapsule2D(tempA->m_center, tempA->m_radius, tempA->m_velocity, tempB->m_capsule, tempA->m_elasticity, tempB->m_elasticity);
		}
	}

}

void GamePachinkoMachine2D::DeleteGarbageEntities()
{

}

void GamePachinkoMachine2D::KillAll()
{

}

void GamePachinkoMachine2D::UpdateFromKeyboard(float deltaSeconds)
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
		//m_isFixed = !m_isFixed; 
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
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFTBRACKET)) {
		m_fixedTime *= 0.9f;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHTBRACKET)) {
		m_fixedTime *= (1.f / 0.9f);
	}
	if (g_theInput->IsKeyDown('N') || g_theInput->WasKeyJustPressed(' '))
	{	
		ShapeEntityDisc2D* billiard = new ShapeEntityDisc2D();
		billiard->m_radius = g_rng->RollRandomFloatInRange(1.f, 3.f);
		//billiard->m_velocity = (m_cursor - m_arrowStart) * 5.f;
		if (billiard->m_velocity.GetLengthSquared() == 0.f)
			
		billiard->m_acceleration = Vec2(0.f, -9.8f * 2);
		billiard->SetPositionAndVelocity(m_arrowStart, (m_arrowEnd - m_arrowStart) * 2.f);
		billiard->m_elasticity = 0.9f;
		unsigned char g = DenormalizeByte(billiard->m_radius / 3.f);
		billiard->m_color = Rgba8(g, g, 255, 150);
		//billiard->m_color = Rgba8(100, 150, 255, 200)

		m_billiards.push_back(billiard);

	}
	if (g_theInput->WasKeyJustPressed('B')) {
		m_isBounce = !m_isBounce;
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
	if (g_theInput->WasKeyJustPressed('V')) {
		m_isFixed = !m_isFixed;
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

void GamePachinkoMachine2D::UpdateFromController(float deltaSeconds)
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

void GamePachinkoMachine2D::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	
	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
			(20.f + 5.f * CosDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
				Rgba8(255, 150, 0));

	g_theRenderer->EndCamera(m_screenCamera);
}

void GamePachinkoMachine2D::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->EndCamera(m_screenCamera);
}


void GamePachinkoMachine2D::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));

	g_theRenderer->EndCamera(m_worldCamera);
}

void GamePachinkoMachine2D::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
	g_theRenderer->EndCamera(m_screenCamera);
}

void GamePachinkoMachine2D::DebugRenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);
	g_theRenderer->EndCamera(m_worldCamera);
}

void GamePachinkoMachine2D::EnterGame()
{
	Shutdown();
	Startup();
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void GamePachinkoMachine2D::SetCamera()
{
	m_worldCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}

void GamePachinkoMachine2D::RandomizeShapes()
{
	m_billiards.clear();
	for (int entityIndex = 0; entityIndex < m_discs.size(); entityIndex++)
	{
		ShapeEntity2D*& tempEntity = m_discs[entityIndex];
		tempEntity->Randomize();
	}
	for (int entityIndex = 0; entityIndex < m_obbs.size(); entityIndex++)
	{
		ShapeEntity2D*& tempEntity = m_obbs[entityIndex];
		tempEntity->Randomize();
	}
	for (int entityIndex = 0; entityIndex < m_capsules.size(); entityIndex++)
	{
		ShapeEntity2D*& tempEntity = m_capsules[entityIndex];
		tempEntity->Randomize();
	}
}

bool GamePachinkoMachine2D::CheckIfOffScreen(Vec2& position, float radius, Vec2& velocity)
{
	if (position.x + radius >= WORLD_SIZE_X || position.x - radius <=  0.f) {
		velocity.x = -velocity.x * 0.81f;

		position.x = GetClamped(position.x, radius, WORLD_SIZE_X - radius);
		position.y = GetClamped(position.y, radius, WORLD_SIZE_Y - radius);

		return true;
	}
	if (position.y - radius < 0.f) {
		if (m_isBounce) {
			velocity.y = -velocity.y * 0.81f;

			position.x = GetClamped(position.x, radius, WORLD_SIZE_X - radius);
			position.y = GetClamped(position.y, radius, WORLD_SIZE_Y - radius);
		}
		else {
			position.y = WORLD_SIZE_Y + 10.f;
		}
		

		return true;
	}
	return false;
}

void GamePachinkoMachine2D::ReturnAttractModeVictory()
{
	ReturnAttractModeNormal();
}

void GamePachinkoMachine2D::ReturnAttractModeFailed()
{
	ReturnAttractModeNormal();

}

void GamePachinkoMachine2D::ReturnAttractModeNormal()
{

	m_isAttractMode = true;
}

