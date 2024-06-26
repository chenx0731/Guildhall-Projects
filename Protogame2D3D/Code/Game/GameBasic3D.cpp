#define UNUSED(x) (void)(x);
#include "Game/GameBasic3D.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
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
#include "Engine/Renderer/Shader.hpp"


GameBasic3D::GameBasic3D() : Game()
{
	
	m_clock = new Clock(Clock::GetSystemClock());
	AABB2 viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	m_screenCamera.m_viewport = viewport;
	m_worldCamera.m_viewport = viewport;
	m_status = GameStatus::BASIC_3D;
	
}

GameBasic3D::~GameBasic3D()
{

}

void GameBasic3D::Startup()
{
	LoadAssets();
	
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);

	m_player = new Player(this, Vec3(0.f, 0.f, 0.f));
	m_player->m_camera.m_viewport = m_screenCamera.m_viewport;
	m_prop1 = new Prop(this, Vec3(2.f, 2.f, 0.0f));
	//AddVertsForAABB3D(m_prop1->m_vertexes, AABB3(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, 0.5f)));
	//m_prop1->m_texture = g_textureLibrary[TEXTURE_CUBE];
	float maxX = 0.5f;
	float maxY = 0.5f;
	float maxZ = 0.5f;
	float minX = -0.5f;
	float minY = -0.5f;
	float minZ = -0.5f;

	// +x
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), Rgba8(255, 0, 0));
	// -x					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 255, 255));
	// +y					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), Rgba8(0, 255, 0));
	// -y					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), Rgba8(255, 0, 255));
	// +z					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 0, 255));
	// -z					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ), Rgba8(255, 255, 0));

	m_prop2 = new Prop(this, Vec3(-2.f, -2.f, 0.f));
	
	// +x
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), Rgba8(255, 0, 0));
	// -x					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 255, 255));
	// +y					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), Rgba8(0, 255, 0));
	// -y					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), Rgba8(255, 0, 255));
	// +z					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 0, 255));
	// -z					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ), Rgba8(255, 255, 0));

	m_sphere = new Prop(this, Vec3(10.f, -5.f, 1.f));
	AddVertsForSphere3D(m_sphere->m_vertexes, Vec3(0.f, 0.f, 0.f), 1.f);
	//AddVertsForCylinder3D(m_sphere->m_vertexes, Vec3(1.f, 1.f, 1.f), Vec3(2.f, 2.f, 2.f), 0.5f);
	//AddVertsForCone3D(m_sphere->m_vertexes, Vec3(-1.f, -1.f, -1.f), Vec3(-2.f, -2.f, -2.f), 0.5f);
	//AddVertsForQuad3D(m_sphere->m_vertexes, Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, 1.f, 1.f));
	m_sphere->m_texture = g_textureLibrary[TEXTURE_SPHERE];
	
	
	SetCamera();

}

void GameBasic3D::Update()
{
	if (g_theGameStatus != m_status) {
		Exit();
		g_theGames[(int)g_theGameStatus]->Enter();
	}
	UpdateFromKeyboard();
	UpdateFromController();

	SetCamera();

	
	if (!m_isPaused)
	{
		UpdateEntities();

		DeleteGarbageEntities();
	}
	
}


void GameBasic3D::Render() const
{
	if (m_isAttractMode)
	{
		RenderAttractMode();
		return;
	}
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));
	RenderGrid();
	RenderEntities();

	if (m_isDebug)
	{
		DebugRenderEntities();
	}
	
	RenderUI();

	if (m_isPaused)
	{
		RenderPaused();
	}
	
	DebugRenderWorld(m_player->m_camera);
	DebugRenderScreen(m_screenCamera);
}

void GameBasic3D::Shutdown()
{
	
}

void GameBasic3D::Enter()
{

}

void GameBasic3D::Exit()
{

}

void GameBasic3D::UpdateEntities()
{
	m_prop1->m_orientation.m_yawDegrees += 30.f * m_clock->GetDeltaSeconds();
	m_prop1->m_orientation.m_rollDegrees += 30.f * m_clock->GetDeltaSeconds();
	m_prop1->Update();
	//if (m_clock->GetFrameCount() % 120 == 0) {
	//	if (m_prop2->m_color.r == 255)
	//		m_prop2->m_color = Rgba8::BLACK;
	//	else m_prop2->m_color = Rgba8::WHITE;
	//}
	unsigned char rgb = unsigned char(127 + RoundDownToInt(50.f * CosDegrees(50.f * m_clock->GetTotalSeconds())));
	m_prop2->m_color = Rgba8(rgb, rgb, rgb);
	m_prop2->Update();
	m_sphere->m_orientation.m_yawDegrees += 45.f * m_clock->GetDeltaSeconds();
	Mat44 camMat = m_player->GetModelMatrix();
	//GetMatrix_YFwd_ZLeft_XUp();
	//camMat.SetTranslation3D(camera.m_position);
	m_sphere->Update();
	m_player->Update();
}

void GameBasic3D::LoadAssets()
{
	//g_musicLibrary[MUSIC_WELCOME] = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");

	g_textureLibrary[TEXTURE_SPHERE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	g_textureLibrary[TEXTURE_CUBE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");

}

void GameBasic3D::DeleteGarbageEntities()
{

}

void GameBasic3D::KillAll()
{

}

void GameBasic3D::UpdateFromKeyboard()
{
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

		UpdateEntities();

		DeleteGarbageEntities();

		if (m_isShaking)
		{
			float theta = g_rng->RollRandomFloatInRange(0.f, 360.f);
			float r = g_rng->RollRandomFloatInRange(0.f, 5.f * (1.f - m_shakingTime));
			m_worldCamera.Translate2D(Vec2::MakeFromPolarDegrees(theta, r));
			m_shakingTime += m_clock->GetDeltaSeconds();
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
	if (g_theInput->WasKeyJustPressed('K'))
	{
		KillAll();
	}
	if (g_theInput->WasKeyJustPressed('1')) {
		Vec3 pos = m_player->m_position + m_player->m_orientation.GetMatrix_XFwd_YLeft_ZUp().GetIBasis3D() * 2.f;
		//DebugAddWorldPoint(pos, 1.f, 5.f, Rgba8::GREEN, Rgba8::RED, DebugRenderMode::X_RAY);
		DebugAddWorldWireSphere(pos, 1.f, 5.f, Rgba8::GREEN, Rgba8::RED);
	}
	if (g_theInput->WasKeyJustPressed('2')) {
		Vec3 end = m_player->m_position + m_player->m_orientation.GetMatrix_XFwd_YLeft_ZUp().GetIBasis3D() * 20.f;
		//DebugAddWorldPoint(pos, 1.f, 5.f, Rgba8::GREEN, Rgba8::RED, DebugRenderMode::X_RAY);
		DebugAddWorldLine(m_player->m_position, end, 0.1f, 10.f, Rgba8::BLUE, Rgba8::BLUE, DebugRenderMode::X_RAY);
	}
	if (g_theInput->WasKeyJustPressed('3')) {
		Vec3 xEnd = m_player->m_position + m_player->m_orientation.GetMatrix_XFwd_YLeft_ZUp().GetIBasis3D();
		Vec3 yEnd = m_player->m_position + m_player->m_orientation.GetMatrix_XFwd_YLeft_ZUp().GetJBasis3D();
		Vec3 zEnd = m_player->m_position + m_player->m_orientation.GetMatrix_XFwd_YLeft_ZUp().GetKBasis3D();
		DebugAddWorldArrow(m_player->m_position, xEnd, 0.1f, 20.f, Rgba8::RED, Rgba8::RED);
		DebugAddWorldArrow(m_player->m_position, yEnd, 0.1f, 20.f, Rgba8::GREEN, Rgba8::GREEN);
		DebugAddWorldArrow(m_player->m_position, zEnd, 0.1f, 20.f, Rgba8::BLUE, Rgba8::BLUE);
		//DebugAddWorldPoint(pos, 1.f, 5.f, Rgba8::GREEN, Rgba8::RED, DebugRenderMode::X_RAY);
	}
	if (g_theInput->WasKeyJustPressed('4')) {
		std::string text = Stringf("Player position: %.2f, %.2f, %.2f;  Player orientation: yaw = %.2f, pitch = %.2f, roll = %.2f",
			m_player->m_position.x, m_player->m_position.y, m_player->m_position.z, 
			m_player->m_orientation.m_yawDegrees, m_player->m_orientation.m_pitchDegrees, m_player->m_orientation.m_rollDegrees);
		DebugAddWorldBillboardText(text, m_player->m_position + m_player->m_orientation.GetMatrix_XFwd_YLeft_ZUp().GetIBasis3D(), 0.3f, Vec2(0.5f, 0.5f), 10.f, Rgba8::WHITE, Rgba8::RED);
	}
	if (g_theInput->WasKeyJustPressed('5')) {
		Vec3 pos = m_player->m_position;
		//DebugAddWorldPoint(pos, 1.f, 5.f, Rgba8::GREEN, Rgba8::RED, DebugRenderMode::X_RAY);
		DebugAddWorldWireCylinder(pos - Vec3(0.f, 0.f, 0.5f), pos + Vec3(0.f, 0.f, 0.5f), 0.5f, 10.f, Rgba8::WHITE, Rgba8::RED);
	}
	if (g_theInput->IsKeyDown('6')) {
		Vec3 pos = m_player->m_position;
		pos.z = 0.f;
		DebugAddWorldPoint(pos, 1.f, 60.f, Rgba8(150, 75, 0), Rgba8(150, 75, 0));
	}
	if (g_theInput->WasKeyJustPressed('7')) {
		std::string msg = Stringf("Player orientation: yaw = %.2f, pitch = %.2f, roll = %.2f",
			m_player->m_orientation.m_yawDegrees, m_player->m_orientation.m_pitchDegrees, m_player->m_orientation.m_rollDegrees
			);

		DebugAddMessage(msg, 5.f);
	}
	std::string msg = Stringf("Player position: %.2f, %.2f, %.2f                                        time = %.2f, fps = %.2f, scale = %.1f", 
		m_player->m_position.x, m_player->m_position.y, m_player->m_position.z, 
		m_clock->GetTotalSeconds(), 1.f / m_clock->GetDeltaSeconds(), m_clock->GetTimeScale());
	DebugAddMessage(msg, 0.f);
}

void GameBasic3D::UpdateFromController()
{
	if (g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_START) 
		|| g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_A))
	{
		if (m_isAttractMode)
		{
			EnterGame();
		}
	}
}

void GameBasic3D::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	
	g_theRenderer->SetModelConstants();
	g_theRenderer->ClearScreen(Rgba8(100, 255, 255));

	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * m_clock->GetTotalSeconds())),
			(20.f + 5.f * CosDegrees(100.f * m_clock->GetTotalSeconds())),
				Rgba8(255, 150, 0));

	g_theRenderer->EndCamera(m_screenCamera);
}

void GameBasic3D::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	g_theRenderer->SetModelConstants();
	g_theRenderer->UnbindDepthStencil();
	g_theRenderer->BindTexture(g_theRenderer->m_depthStencilTexture);
	//g_theRenderer->BindShader(m_shader);
	DrawSquare(Vec2(0.f, 0.f), Vec2(400.f, 200.f), Rgba8::WHITE);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->BindDepthStencil();
	g_theRenderer->EndCamera(m_screenCamera);
}


void GameBasic3D::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_player->m_camera);

	m_prop1->Render();

	m_prop2->Render();

	m_sphere->Render();

	g_theRenderer->EndCamera(m_player->m_camera);
}

void GameBasic3D::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
	g_theRenderer->EndCamera(m_screenCamera);
}

void GameBasic3D::RenderGrid() const
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

void GameBasic3D::DebugRenderEntities() const
{
	g_theRenderer->BeginCamera(m_player->m_camera);

	g_theRenderer->EndCamera(m_player->m_camera);
}

void GameBasic3D::EnterGame()
{
	//SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	//PlayMusic(MUSIC_WELCOME);
	//g_theAudio->StartSound(testSound);
	Shutdown();
	Startup();
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void GameBasic3D::SetCamera()
{
	//m_worldCamera.SetOrthoView(Vec2(-1.f, -1.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_player->m_camera.SetPerspectiveView(2.f, 60.f, 0.1f, 100.f);
	m_player->m_camera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}
