#define UNUSED(x) (void)(x);
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Game/App.hpp"
#include "Game/Model.hpp"
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
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/AABB2.hpp"

Vertex_PCU vertices[] = {
	Vertex_PCU(Vec2(800.f, 600.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(400.f, 200.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(1200.f, 200.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
};

bool LoadModel(EventArgs& args)
{
	std::string filename = args.GetValue("File", "");
	if (!IsFileExist(filename))
	{
		g_theDevConsole->AddLine(DevConsole::ERROR, "Invalid path: " + filename);
		return false;
	}
		
	if (!g_theGame)
		return false;
	if (g_theGame->m_model != nullptr) {
		delete g_theGame->m_model;
		g_theGame->m_model = nullptr;
	}
	g_theGame->m_model = new Model(filename.c_str());
	return true;
}

Game::Game(App* owner)
{
	UNUSED(owner);

	m_clock = new Clock(Clock::GetSystemClock());

	g_theEventSystem->SubscribeEventCallbackFunction("LoadModel", &LoadModel);
}

Game::~Game()
{
	delete m_cube;
	m_cube = nullptr;
	delete m_sphere;
	m_sphere = nullptr;
}

void Game::Startup()
{
	LoadAssets();
	
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);

	m_player = new Player(this, Vec3(3.f, 0.f, 3.f));
	m_player->m_orientation.m_yawDegrees = 180.f;
	m_player->m_orientation.m_rollDegrees = 0.f;
	m_player->m_orientation.m_pitchDegrees = 45.f;
	m_player->m_camera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	//std::string filename = g_gameConfigBlackboard.GetValue("defaultModel", "");
	m_model = new Model(g_gameConfigBlackboard.GetValue("defaultModel", "Data/Models/Tutorial_Box.xml").c_str());
	m_model->m_angularVelocity.m_yawDegrees = 45.f;
		//"Data/Models/Teapot.xml");
	std::vector<Vertex_PCUTBN> cubeVerts;
	std::vector<unsigned int> cubeIndexes;

	AABB3 cubeBox;
	cubeBox.SetCenter(Vec3());
	cubeBox.SetDimensions(Vec3(2.f, 2.f, 2.f));
	AddVertsForCube3D(cubeVerts, cubeIndexes, cubeBox);
	CalculateTangentSpaceBasisVectors(cubeVerts, cubeIndexes);
	CPUMesh* cube = new CPUMesh(cubeVerts, cubeIndexes);
	Material* material = new Material("Data/Materials/Grass.xml");
	m_cube = new Prop(cube, material);
	m_cube->m_angularVelocity.m_yawDegrees = 45.f;
	//m_cube = new Prop();
	m_sunOrientation.m_pitchDegrees = 90.f;

	SetCamera();

}

void Game::Update()
{
	UpdateFromKeyboard();
	UpdateFromController();

	SetCamera();


	m_player->Update();
	if (!m_isPaused)
	{
		UpdateEntities();

		DeleteGarbageEntities();
	}
}

void Game::Render() const
{
	//if (m_isAttractMode)
	//{
	//	RenderAttractMode();
	//	return;
	//}
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));
	RenderGrid();
	if (m_model != nullptr) {
		g_theRenderer->SetLightConstants(m_sunDirection, m_sunIntensity, Rgba8(DenormalizeByte(m_ambientIntensity)
			, 127, 255, 0), m_player->m_position, m_normalMode, m_specularMode, m_specularIntensity, m_specularPower);
		//m_model->Render();
		RenderEntities();
	}

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

void Game::Shutdown()
{
	delete m_model;
	m_model = nullptr;
}

void Game::UpdateEntities()
{

	Mat44 camMat = m_player->GetModelMatrix();

	if (m_cube) {
		m_cube->Update();
	}
	else if (m_sphere) {
		m_sphere->Update();
	}
	else if (m_model) {
		m_model->Update(m_clock->GetDeltaSeconds());
	}
	m_sunDirection = m_sunOrientation.GetForwardVector();

}

void Game::LoadAssets()
{
	//g_musicLibrary[MUSIC_WELCOME] = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");

	g_textureLibrary[TEXTURE_SPHERE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	g_textureLibrary[TEXTURE_CUBE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");

}

void Game::DeleteGarbageEntities()
{

}

void Game::KillAll()
{

}

void Game::UpdateFromKeyboard()
{
	if (g_theInput->WasKeyJustPressed('1')) {
		if (m_cube)
		{
			delete m_cube;
			m_cube = nullptr;
		}
		if (m_sphere)
		{
			delete m_sphere;
			m_sphere = nullptr;
		}
		std::vector<Vertex_PCUTBN> cubeVerts;
		std::vector<unsigned int> cubeIndexes;

		AABB3 cubeBox;
		cubeBox.SetCenter(Vec3());
		cubeBox.SetDimensions(Vec3(2.f, 2.f, 2.f));
		AddVertsForCube3D(cubeVerts, cubeIndexes, cubeBox);
		CalculateTangentSpaceBasisVectors(cubeVerts, cubeIndexes);
		CPUMesh* cube = new CPUMesh(cubeVerts, cubeIndexes);
		Material* material = new Material("Data/Materials/Grass.xml");
		m_cube = new Prop(cube, material);
		m_cube->m_angularVelocity.m_yawDegrees = 45.f;

	}
	if (g_theInput->WasKeyJustPressed('2')) {
		if (m_sphere)
		{
			delete m_sphere;
			m_sphere = nullptr;
		}
		if (m_cube)
		{
			delete m_cube;
			m_cube = nullptr;
		}
		std::vector<Vertex_PCUTBN> sphereVerts;
		std::vector<unsigned int> sphereIndexes;

		AABB3 sphereBox;
		sphereBox.SetCenter(Vec3());
		sphereBox.SetDimensions(Vec3(2.f, 2.f, 2.f));
		
		AddVertsForUVSphereZ3D(sphereVerts, sphereIndexes, Vec3(), 1.f, 32.f, 16.f);
		CalculateTangentSpaceBasisVectors(sphereVerts, sphereIndexes);
		CPUMesh* sphere = new CPUMesh(sphereVerts, sphereIndexes);
		Material* material = new Material("Data/Materials/Grass.xml");
		m_sphere = new Prop(sphere, material);
		m_sphere->m_angularVelocity.m_yawDegrees = 45.f;
	}
	if (g_theInput->WasKeyJustPressed('3')) {
		if (m_cube)
		{
			delete m_cube;
			m_cube = nullptr;
		}
		if (m_sphere)
		{
			delete m_sphere;
			m_sphere = nullptr;
		}
		std::vector<Vertex_PCUTBN> cubeVerts;
		std::vector<unsigned int> cubeIndexes;

		AABB3 cubeBox;
		cubeBox.SetCenter(Vec3());
		cubeBox.SetDimensions(Vec3(2.f, 2.f, 2.f));
		AddVertsForCube3D(cubeVerts, cubeIndexes, cubeBox);
		CalculateTangentSpaceBasisVectors(cubeVerts, cubeIndexes);
		CPUMesh* cube = new CPUMesh(cubeVerts, cubeIndexes);
		Material* material = new Material("Data/Materials/Brick.xml");
		m_cube = new Prop(cube, material);
		m_cube->m_angularVelocity.m_yawDegrees = 45.f;
	}
	if (g_theInput->WasKeyJustPressed('4')) {
		if (m_sphere)
		{
			delete m_sphere;
			m_sphere = nullptr;
		}
		if (m_cube)
		{
			delete m_cube;
			m_cube = nullptr;
		}
		std::vector<Vertex_PCUTBN> sphereVerts;
		std::vector<unsigned int> sphereIndexes;

		AABB3 sphereBox;
		sphereBox.SetCenter(Vec3());
		sphereBox.SetDimensions(Vec3(2.f, 2.f, 2.f));

		AddVertsForUVSphereZ3D(sphereVerts, sphereIndexes, Vec3(), 1.f, 32.f, 16.f);
		CalculateTangentSpaceBasisVectors(sphereVerts, sphereIndexes);
		CPUMesh* sphere = new CPUMesh(sphereVerts, sphereIndexes);
		Material* material = new Material("Data/Materials/Brick.xml");
		m_sphere = new Prop(sphere, material);
		m_sphere->m_angularVelocity.m_yawDegrees = 45.f;
	}
	if (g_theInput->WasKeyJustPressed('5')) {
		if (m_cube)
		{
			delete m_cube;
			m_cube = nullptr;
		}
		if (m_sphere)
		{
			delete m_sphere;
			m_sphere = nullptr;
		}
		if (!m_model) {
			m_model->m_orientation = EulerAngles();
		}
	}
	if (g_theInput->IsKeyDown(KEYCODE_UPARROW)) {
		m_sunOrientation.m_pitchDegrees += 45.f * m_clock->GetDeltaSeconds();
	}
	if (g_theInput->IsKeyDown(KEYCODE_DOWNARROW)) {
		m_sunOrientation.m_pitchDegrees -= 45.f * m_clock->GetDeltaSeconds();
	}
	if (g_theInput->IsKeyDown(KEYCODE_RIGHTARROW)) {
		m_sunOrientation.m_yawDegrees -= 45.f * m_clock->GetDeltaSeconds();
	}
	if (g_theInput->IsKeyDown(KEYCODE_LEFTARROW)) {
		m_sunOrientation.m_yawDegrees += 45.f * m_clock->GetDeltaSeconds();
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_COMMA)) {
		m_sunIntensity += 0.1f;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_PERIOD)) {
		m_sunIntensity -= 0.1f;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_SEMICOLON)) {
		m_specularIntensity += 0.1f;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_SINGLEQUOTE)) {
		m_specularIntensity -= 0.1f;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHTBRACKET)) {
		m_specularPower -= 0.1f;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFTBRACKET)) {
		m_specularPower += 0.1f;
	}
	if (g_theInput->WasKeyJustPressed('B')) {
		m_isDebug = !m_isDebug;
	}
	if (g_theInput->WasKeyJustPressed('N')) {
		m_normalMode = m_normalMode == 1 ? 0 : 1;
	}
	if (g_theInput->WasKeyJustPressed('M')) {
		m_specularMode = m_specularMode == 1 ? 0 : 1;
	}
	
	if (g_theInput->WasKeyJustPressed('P'))
	{
		m_isPaused = !m_isPaused;
	}
	if (g_theInput->WasKeyJustPressed('O'))
	{

		m_player->m_position = Vec3(0.f, -5.f, 1.f);
		m_player->m_orientation.m_yawDegrees = 90.f;
		m_player->m_orientation.m_rollDegrees = 0.f;
		m_player->m_orientation.m_pitchDegrees = 0.f;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
	{
		m_isDebug = !m_isDebug;
	}

	std::string msg = Stringf("Player position: %.2f, %.2f, %.2f                                        time = %.2f, fps = %.2f, scale = %.1f", 
		m_player->m_position.x, m_player->m_position.y, m_player->m_position.z, 
		m_clock->GetTotalSeconds(), 1.f / m_clock->GetDeltaSeconds(), m_clock->GetTimeScale());
	//DebugAddMessage(msg, 0.f);
	AABB2 box0 = AABB2(Vec2(0.f, 770.f), Vec2(1600.f, 800.f));
	AABB2 box1 = AABB2(Vec2(0.f, 740.f), Vec2(1600.f, 770.f));
	AABB2 box2 = AABB2(Vec2(0.f, 710.f), Vec2(1600.f, 740.f));
	AABB2 box3 = AABB2(Vec2(0.f, 680.f), Vec2(1600.f, 710.f));
	AABB2 box4 = AABB2(Vec2(0.f, 650.f), Vec2(1600.f, 680.f));
	AABB2 box5 = AABB2(Vec2(0.f, 620.f), Vec2(1600.f, 650.f));
	AABB2 box6 = AABB2(Vec2(0.f, 590.f), Vec2(1600.f, 620.f));
	AABB2 box7 = AABB2(Vec2(0.f, 560.f), Vec2(1600.f, 590.f));
	AABB2 box8 = AABB2(Vec2(0.f, 530.f), Vec2(1600.f, 560.f));
	DebugAddScreenText(Stringf("FPS: %.1f", 1.f / m_clock->GetDeltaSeconds()), box0, 20.f, Vec2(1.f, 0.5f), 0.f);
	DebugAddScreenText(Stringf("Sun Orientation: (%.1f, %.1f, %.1f)", m_sunOrientation.m_yawDegrees, m_sunOrientation.m_pitchDegrees, m_sunOrientation.m_rollDegrees), box1, 20.f, Vec2(1.f, 0.5f), 0.f);
	DebugAddScreenText(Stringf("Sun Direction: (%.1f, %.1f, %.1f)", m_sunDirection.x, m_sunDirection.y, m_sunDirection.z), box2, 20.f, Vec2(1.f, 0.5f), 0.f);
	DebugAddScreenText(Stringf("Sun Intensity: %.1f", m_sunIntensity), box3, 20.f, Vec2(1.f, 0.5f), 0.f);
	DebugAddScreenText(Stringf("Ambient Intensity: %.1f", m_ambientIntensity), box4, 20.f, Vec2(1.f, 0.5f), 0.f);
	if (m_normalMode == 0)
		DebugAddScreenText(Stringf("Normals: Texture"), box5, 20.f, Vec2(1.f, 0.5f), 0.f);
	else DebugAddScreenText(Stringf("Normals: Vertex"), box5, 20.f, Vec2(1.f, 0.5f), 0.f);
	if (m_specularMode == 0)
		DebugAddScreenText(Stringf("Specular: Texture"), box6, 20.f, Vec2(1.f, 0.5f), 0.f);
	else DebugAddScreenText(Stringf("Specular: Constant"), box6, 20.f, Vec2(1.f, 0.5f), 0.f);
	DebugAddScreenText(Stringf("Specular Intensity: %.1f", m_specularIntensity), box7, 20.f, Vec2(1.f, 0.5f), 0.f);
	DebugAddScreenText(Stringf("Specular Power: %.1f", m_specularPower), box8, 20.f, Vec2(1.f, 0.5f), 0.f);
	DebugAddWorldArrow(Vec3(0.f, 0.f, 2.f), Vec3(0.f, 0.f, 2.f) + m_sunDirection * 0.5f, 0.05f, 0.f, Rgba8::YELLOW, Rgba8::YELLOW);
	m_sunOrientation.m_yawDegrees = GetAngleDegreesWithin360(m_sunOrientation.m_yawDegrees);
	m_sunOrientation.m_pitchDegrees = GetAngleDegreesWithin360(m_sunOrientation.m_pitchDegrees);
}

void Game::UpdateFromController()
{

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

	g_theRenderer->EndCamera(m_screenCamera);
}


void Game::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_player->m_camera);
	if (m_cube) {
		m_cube->Render();
	}
	else if (m_sphere) {
		m_sphere->Render();
	}
	else if (m_model) {
		m_model->Render();
	}
	g_theRenderer->EndCamera(m_player->m_camera);
}

void Game::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	//DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
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
	m_player->m_camera.SetPerspectiveView(2.f, 60.f, 0.1f, 100.f);
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
