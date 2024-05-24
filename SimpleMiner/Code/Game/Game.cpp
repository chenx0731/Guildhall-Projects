#define UNUSED(x) (void)(x);
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Game/App.hpp"
#include "Game/World.hpp"
#include "Game/BlockDef.hpp"
#include "Game/GameCommon.hpp"
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

Vertex_PCU vertices[] = {
	Vertex_PCU(Vec2(800.f, 600.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(400.f, 200.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(1200.f, 200.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
};


Game::Game(App* owner)
{
	UNUSED(owner);

	m_clock = new Clock(Clock::GetSystemClock());

}

Game::~Game()
{

}

void Game::Startup()
{
	LoadAssets();
	
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);

	m_player = new Player(this, Vec3(8.f, 8.f, 85.f));
	m_player->m_camera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	m_prop1 = new Prop(this, Vec3(2.f, 2.f, 0.0f));
	m_world = new World(this);
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
	m_sphere->m_texture = g_textureLibrary[TEXTURE_SPHERE];
	
	
	SetCamera();



}

void Game::Update()
{
	UpdateFromKeyboard();
	UpdateFromController();

	SetCamera();

	if (m_isAttractMode)
	{
		return;
	}


	if (m_isOver)
	{
		m_hangingTime += m_clock->GetDeltaSeconds();
		if (m_hangingTime >= 3.f)
		{
			ReturnAttractModeVictory();
		}
	}
	
	if (!m_isPaused)
	{
		//UpdateEntities();
		m_world->Update(m_clock->GetDeltaSeconds());
		m_player->Update();
		DeleteGarbageEntities();
	}

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

void Game::Render() const
{
	if (m_isAttractMode)
	{
		RenderAttractMode();
		return;
	}
	//g_theRenderer->ClearScreen(Rgba8(0, 0, 0));
	//RenderGrid();
	//RenderEntities();
	if (m_isDebug)
	{
		DebugRenderEntities();
	}

	g_theRenderer->BeginCamera(m_player->m_camera);
	m_world->Render();
	g_theRenderer->EndCamera(m_player->m_camera);

	RenderCoords();

	RenderUI();

	if (m_isPaused)
	{
		RenderPaused();
	}
	
	DebugRenderWorld(m_player->m_camera);
	DebugRenderScreen(m_screenCamera);
}

void Game::Shutdown()
{
	delete m_world;
	m_world = nullptr;
}

void Game::UpdateEntities()
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

void Game::LoadAssets()
{
	//g_musicLibrary[MUSIC_WELCOME] = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");

	g_textureLibrary[TEXTURE_SPHERE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	g_textureLibrary[TEXTURE_CUBE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");
	g_textureLibrary[TEXTURE_BASIC_SPRITE_64_64] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/BasicSprites_64x64.png");

	BlockDef::InitializeBlockDefs();

}

void Game::DeleteGarbageEntities()
{

}

void Game::KillAll()
{

}

void Game::UpdateFromKeyboard()
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
	std::string msg0 = Stringf("WASD=Horizontal, EQ=Vertical, Space(fast), F8=Regenerate, F1=Debug");
	std::string msg = Stringf("Player position: %.2f, %.2f, %.2f                                        time = %.2f, fps = %.2f, scale = %.1f", 
		m_player->m_position.x, m_player->m_position.y, m_player->m_position.z, 
		m_clock->GetTotalSeconds(), 1.f / m_clock->GetDeltaSeconds(), m_clock->GetTimeScale());
	//DebugAddMessage(msg0, 0.f, Rgba8::YELLOW, Rgba8::YELLOW);
	//DebugAddMessage(msg, 0.f);
}

void Game::UpdateFromController()
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

void Game::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	
	g_theRenderer->SetModelConstants();
	g_theRenderer->ClearScreen(Rgba8(100, 255, 255));
	g_theRenderer->DrawVertexArray(3, vertices);
	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * m_clock->GetTotalSeconds())),
			(20.f + 5.f * CosDegrees(100.f * m_clock->GetTotalSeconds())),
				Rgba8(255, 150, 0));
	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	std::string msg0 = Stringf("WASD=Horizontal, EQ=Vertical, Space(fast), F8=Regenerate, F1=Debug");
	std::string msg1 = Stringf("Chunk=%d/%d, Verts=%d; xyz=(%.0f,%.0f,%.0f), ypr=(%.0f,%.0f,%.0f); frameMS=%.0f(%.0fFPS)", (int)m_world->m_activeChunks.size(), m_world->m_maxChunks, m_world->GetTotalVertexes(), 
		m_player->m_position.x, m_player->m_position.y, m_player->m_position.z,
		m_player->m_orientation.m_yawDegrees, m_player->m_orientation.m_pitchDegrees, m_player->m_orientation.m_rollDegrees,
		m_clock->GetDeltaSeconds() * 1000.f, 1.f / m_clock->GetDeltaSeconds());
	std::string msg2 = Stringf("Press 1 to Change block placement type, now is ");
	std::string msg3 = "Cobble Stone";
	std::string msg4 = "Glow Stone";
	if (m_world->m_isPlacingCobbleStone)
		msg2.append(msg3);
	else msg2.append(msg4);
	std::vector<Vertex_PCU> opVerts, infoVerts, blockVerts;
	g_theFont->AddVertsForText2D(opVerts, 8.f * Vec2(3.f, 95.f), 20.f, msg0, Rgba8::YELLOW, 0.7f);
	g_theFont->AddVertsForText2D(infoVerts, 8.f * Vec2(3.f, 92.f), 20.f, msg1, Rgba8(0, 200, 200), 0.7f);
	g_theFont->AddVertsForText2D(blockVerts, 8.f * Vec2(3.f, 89.f), 20.f, msg2, Rgba8(200, 0, 200), 0.7f);
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)opVerts.size(), opVerts.data());
	g_theRenderer->DrawVertexArray((int)infoVerts.size(), infoVerts.data());
	g_theRenderer->DrawVertexArray((int)blockVerts.size(), blockVerts.data());
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::RenderCoords() const
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

	AddVertsForLineSegment3D(coordsVerts, start, start + xBasis, 0.05f, Rgba8::RED);
	AddVertsForLineSegment3D(coordsVerts, start, start + yBasis, 0.05f, Rgba8::GREEN);
	AddVertsForLineSegment3D(coordsVerts, start, start + zBasis, 0.05f, Rgba8::BLUE);

	g_theRenderer->BindTexture(nullptr);

	start = m_player->m_position + iBasis * 0.2f;

	AddVertsForLineSegment3D(coordsVerts, start, start + xBasis * 0.01f, 0.001f, Rgba8::RED);
	AddVertsForLineSegment3D(coordsVerts, start, start + yBasis * 0.01f, 0.001f, Rgba8::GREEN);
	AddVertsForLineSegment3D(coordsVerts, start, start + zBasis * 0.01f, 0.001f, Rgba8::BLUE);

	g_theRenderer->BindTexture(nullptr);

	g_theRenderer->DrawVertexArray((int)coordsVerts.size(), coordsVerts.data());

	g_theRenderer->EndCamera(m_player->m_camera);
}


void Game::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_player->m_camera);

	m_prop1->Render();

	m_prop2->Render();

	m_sphere->Render();

	g_theRenderer->EndCamera(m_player->m_camera);
}

void Game::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::RenderGrid() const
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

void Game::DebugRenderEntities() const
{
	g_theRenderer->BeginCamera(m_player->m_camera);

	g_theRenderer->EndCamera(m_player->m_camera);
}

void Game::EnterGame()
{
	//SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	//PlayMusic(MUSIC_WELCOME);
	//g_theAudio->StartSound(testSound);
	Shutdown();
	Startup();
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void Game::SetCamera()
{
	//m_worldCamera.SetOrthoView(Vec2(-1.f, -1.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_player->m_camera.SetPerspectiveView(2.f, 60.f, 0.1f, 1000.f);
	m_player->m_camera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
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
	//PlayMusic(MUSIC_WELCOME);
	m_isAttractMode = true;
}
