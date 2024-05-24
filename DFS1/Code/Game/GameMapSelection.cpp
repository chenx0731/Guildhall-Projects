#define UNUSED(x) (void)(x);
#include "Game/GameMapSelection.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/Actor.hpp"
#include "Game/App.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/OBJ_Loader.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/STLUtils.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"

GameMapSelection::GameMapSelection()
{
	m_clock = new Clock(Clock::GetSystemClock());

	std::vector<Vec2> points0, points1, points2;
	//points.push_back(Vec2(-15.f, -3.f));
	//points.push_back(Vec2(-7.f, 3.f));
	//points.push_back(Vec2(-0.f, -3.f));
	//points.push_back(Vec2(7.f, 1.5f));
	//points.push_back(Vec2(15.f, -2.f));

	points0.push_back(Vec2(0.f, 0.f));
	points0.push_back(Vec2(10.f, 10.f));
	points0.push_back(Vec2(15.f, 5.f));
	points0.push_back(Vec2(20.f, 15.f));

	//points1.push_back(Vec2(10.f, 10.f));
	points1.push_back(Vec2(0.f, 0.f));
	points1.push_back(Vec2(2.f, -10.f));
	points1.push_back(Vec2(-10.f, -15.f));

	//points2.push_back(Vec2(10.f, 10.f));
	points2.push_back(Vec2(0.f, 0.f));
	points2.push_back(Vec2(-15.f, 7.f));
	points2.push_back(Vec2(-18.f, 20.f));

	m_splines[0] = CubicHermiteSpline(points0);
	m_splines[1] = CubicHermiteSpline(points1);
	m_splines[2] = CubicHermiteSpline(points2);

	//m_worldCamera.m_position = Vec3(0.66f, 11.f, 12.2f);
	//m_worldCamera.m_orientation = EulerAngles(275.f, 50.f, 0.f);	
	m_worldCamera.m_position = Vec3(-9.1f, -18.f, 11.25f);
	m_worldCamera.m_orientation = EulerAngles(424.f, 25.5f, 0.f);
	m_worldCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	m_screenCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

	//m_objVerts = g_OBJLoader->LoadedVertices;
	for (int index = 0; index < (int)g_OBJLoader->LoadedVertices.size(); index++) {
		objl::Vertex temp = g_OBJLoader->LoadedVertices[index];
		m_objVerts.emplace_back(Vertex_PCUTBN(Vec3(temp.Position.X, temp.Position.Y, temp.Position.Z),
			Rgba8::WHITE, Vec2(),
			Vec3(temp.Normal.X, temp.Normal.Y, temp.Normal.Z)
		));
	}
	m_objIndexes = g_OBJLoader->LoadedIndices;

	m_vertexBuffer = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN) * m_objVerts.size(), sizeof(Vertex_PCUTBN));
	m_indexBuffer = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int) * m_objIndexes.size());

	m_shader = g_theRenderer->CreateShader("Data/Shaders/SpriteLit");
}

GameMapSelection::~GameMapSelection()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;

	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_shader = nullptr;
}

void GameMapSelection::Startup()
{
	g_textureLibrary[TEXTURE_SHIP] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Starship.png");
	g_textureLibrary[TEXTURE_GALAXY] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Space Background.png");
	g_textureLibrary[TEXTURE_ARROW] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/arrow.png");
		//Space Background.png");
	g_OBJLoader->LoadFile("Data/Models/RobotDog.obj");
	
}

void GameMapSelection::Update()
{
	if (m_state != GameState::MAP_SELECT) {
		g_theGameState = m_state;
		g_theGames[(int)g_theGameState]->Enter();
	}
	SetCamera();
	m_shipOrientation = m_splines[m_nowSplineIndex].GetVelocityAtParametric(GetProcessedTime(), m_isDirtPositive).GetOrientationDegrees();
	m_shipPosition = m_splines[m_nowSplineIndex].EvaluateAtParametric(GetProcessedTime());
	UpdateFromKeyboard();
	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++) {
		Actor*& actor = m_actors[actorIndex];
		if (actor != nullptr) {
			actor->Update(m_clock->GetDeltaSeconds());
		}
	}

}

void GameMapSelection::Render() const
{
	RenderWorld();
	RenderUI();
}

void GameMapSelection::Shutdown()
{
}

void GameMapSelection::Enter()
{
	m_gamePlaying = (GamePlaying*)g_theGames[(int)GameState::PLAYING];
	m_state = GameState::MAP_SELECT;
	Actor* actor = new Actor("Jupiter", Vec3(20.f, 15.f, 0.f));
	m_actors.push_back(actor);
	actor = new Actor("Earth", Vec3(-10.f, -15.f, 0.f));
	m_actors.push_back(actor);
	actor = new Actor("Galaxy", Vec3(0.f, 0.f, -1.f));
	m_actors.push_back(actor);
	actor = new Actor("Skybox", Vec3(0.f, 0.f,-5.f));
	m_actors.push_back(actor);
}

void GameMapSelection::Exit()
{
	PauseMusic(MUSIC_MAINMENU);
	ClearAndDeleteEverything(m_actors);
}


void GameMapSelection::SetCamera()
{
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	m_worldCamera.SetPerspectiveView(2.f, 60.f, 0.1f, 100.f);
	m_worldCamera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	// set camera to ship's position
	m_worldCamera.m_position = m_shipPosition;
	m_worldCamera.m_position += Vec3(-9.1f, -18.f, 11.25f);
	//(0.66f, 11.f, 12.2f);
}

void GameMapSelection::UpdateFromKeyboard()
{
	float deltaSeconds = m_clock->GetDeltaSeconds();
	float m_cameraSpeed = 10.f;
	Vec3 x_forward = Vec3(1.f, 0.f, 0.f);
	Vec3 y_left = Vec3(0.f, 1.f, 0.f);
	Vec3 z_up = Vec3(0.f, 0.f, 1.f);
	if (GetDistanceSquared3D(m_shipPosition, Vec3()) < 0.25f) {
		if (g_theInput->IsKeyDown(KEYCODE_LEFTARROW)) {
			if (m_nowSplineIndex != 2) {
				m_nowSplineIndex = 2;
				m_time = 0.0f;
			}
		}
		if (g_theInput->IsKeyDown(KEYCODE_RIGHTARROW)) {
			if (m_nowSplineIndex != 1) {
				m_nowSplineIndex = 1;
				m_time = 0.0f;
			}
			
		}
		if (g_theInput->IsKeyDown(KEYCODE_UPARROW)) {
			if (m_nowSplineIndex != 0) {
				m_nowSplineIndex = 0;
				m_time = 0.f;
			}
		}
	}
	if (g_theInput->IsKeyDown(KEYCODE_RIGHTARROW) || g_theInput->IsKeyDown(KEYCODE_DOWNARROW)) {
		if (m_nowSplineIndex == 1) {
			m_time += m_clock->GetDeltaSeconds() * 0.5f;
			m_isDirtPositive = false;
		}
		else {
			m_time -= m_clock->GetDeltaSeconds() * 0.5f;
			m_isDirtPositive = true;
		}
		
	}
	if (g_theInput->IsKeyDown(KEYCODE_LEFTARROW) || g_theInput->IsKeyDown(KEYCODE_UPARROW)) {
		if (m_nowSplineIndex == 1) {
			m_time -= m_clock->GetDeltaSeconds() * 0.5f;
			m_isDirtPositive = true;
		}
		else {
			m_time += m_clock->GetDeltaSeconds() * 0.5f;
			m_isDirtPositive = false;
		}
	}


	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC)) {
		if (m_gamePlaying->m_playerNum == 1) {
			for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
				if (m_gamePlaying->m_players[playerIndex]) {
					if (m_gamePlaying->m_players[playerIndex]->m_type == ControllerType::KEYBOARD) {
						m_gamePlaying->DeletePlayerWithSpecController(ControllerType::KEYBOARD);
						//g_theGameState = GameState::ATTRACT;
						m_state = GameState::ATTRACT;
						//m_gamePlaying->m_playerNum--;
						Exit();
						PlayMusic(MUSIC_CLICK);

					}
				}
			}
		}
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ENTER)) {
		Actor* actor = ClosestPlanet();
		if (actor) {
			if (actor->m_actorDef->m_name == "Jupiter")
			{
				PlayMusic(MUSIC_CLICK);
				//m_gamePlaying->m_currentWorld = m_gamePlaying->m_worlds[m_gamePlaying->m_worldNames[0]];
				m_gamePlaying->m_currentMapName = "Saturn";
				m_state = GameState::PLAYING;
				Exit();
			}
			else if (actor->m_actorDef->m_name == "Earth")
			{
				PlayMusic(MUSIC_CLICK);
				//m_gamePlaying->m_currentWorld = m_gamePlaying->m_worlds[m_gamePlaying->m_worldNames[1]];
				m_gamePlaying->m_currentMapName = "Earth";
				m_state = GameState::PLAYING;
				Exit();
			}
		}
		
	}
	if (g_theInput->IsKeyDown('S')) {
		//m_position.x -= m_game->m_clock->GetDeltaSeconds() * 2.f;
		m_worldCamera.m_position -= deltaSeconds * m_cameraSpeed * x_forward;
	}
	if (g_theInput->IsKeyDown('W')) {
		//m_position.x += m_game->m_clock->GetDeltaSeconds() * 2.f;
		m_worldCamera.m_position += deltaSeconds * m_cameraSpeed * x_forward;
	}
	if (g_theInput->IsKeyDown('A')) {
		//m_position.y += m_game->m_clock->GetDeltaSeconds() * 2.f;
		m_worldCamera.m_position += deltaSeconds * m_cameraSpeed * y_left;
	}
	if (g_theInput->IsKeyDown('D')) {
		//m_position.y -= m_game->m_clock->GetDeltaSeconds() * 2.f;
		m_worldCamera.m_position -= deltaSeconds * m_cameraSpeed * y_left;
	}
	if (g_theInput->IsKeyDown('Z')) {
		//m_position.y += m_game->m_clock->GetDeltaSeconds() * 2.f;
		m_worldCamera.m_position += deltaSeconds * m_cameraSpeed * Vec3(0.f, 0.f, 1.f);
	}
	if (g_theInput->IsKeyDown('C')) {

		m_worldCamera.m_position -= deltaSeconds * m_cameraSpeed * Vec3(0.f, 0.f, 1.f);
	}


	m_time = GetClamped(m_time, 0.f, 1.f);
}

float GameMapSelection::GetProcessedTime() const
{
	return SmoothStep3(m_time);
}

Mat44 GameMapSelection::GetShipModelMatrix() const
{
	Mat44 mat;
	//mat.AppendXRotation(90.f);
	EulerAngles orientation;
	orientation.m_yawDegrees = m_shipOrientation;
	orientation.m_pitchDegrees = 0.f;
	orientation.m_rollDegrees = 0.f;
	mat = orientation.GetMatrix_XFwd_YLeft_ZUp();
	mat.AppendXRotation(90.f);
	mat.AppendYRotation(-90.f);
	//mat.Append(orientation.GetMatrix_XFwd_YLeft_ZUp());
	mat.AppendScaleUniform3D(0.2f);
	//Mat44 mat = orientation.GetMatrix_XFwd_YLeft_ZUp();
	
	mat.SetTranslation3D(m_shipPosition);
	return mat;
}

void GameMapSelection::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	Actor* actor = ClosestPlanet();

	std::string ui = "No planet within radar range";
	std::vector<Vertex_PCU> textVerts;

	if (actor != nullptr) {
		if (actor->m_actorDef->m_name == "Jupiter") {
			ui = "Press enter to land Saturn station";
		}
		if (actor->m_actorDef->m_name == "Earth") {
			ui = "Welcome home";
		}
	}

	g_theFont->AddVertsForText2D(textVerts, Vec2(0.f, 750.f), 20.f, ui);

	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)textVerts.size(), textVerts.data());
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->EndCamera(m_screenCamera);
}

void GameMapSelection::RenderWorld() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	std::vector<Vertex_PCU> splineVerts, shipVerts, bgVerts;
	Rgba8 splineColor = Rgba8::WHITE;
	splineColor.a = 200;
	m_splines[m_nowSplineIndex].AddVertsForSpline(splineVerts, 0.05f, splineColor);
	m_splines[1].AddVertsForSpline(splineVerts, 0.05f, splineColor);
	m_splines[2].AddVertsForSpline(splineVerts, 0.05f, splineColor);

	g_theRenderer->DrawVertexArray((int)splineVerts.size(), splineVerts.data());
	g_theRenderer->BindTexture(nullptr);

	Vec2 center = m_splines[m_nowSplineIndex].EvaluateAtParametric(GetProcessedTime());
	
	Vec2 dimension = Vec2(2.5f, 2.5f);
	AABB2 ship;
	ship.SetCenter(Vec2(0.f, 0.f));
	ship.SetDimensions(dimension);

	AddVertsForQuad3D(shipVerts, Vec3(ship.m_mins, 1.f), Vec3(ship.GetBottomRight(), 1.f), Vec3(ship.m_maxs, 1.f), Vec3(ship.GetTopLeft(), 1.f));
	AddVertsForQuad3D(bgVerts, Vec3(-10.f, 10.f, 0.f), Vec3(-10.f, -10.f, 0.f), Vec3(10.f, -10.f, 0.f), Vec3(10.f, 10.f, 0.f));


	g_theRenderer->SetModelConstants(GetShipModelMatrix(), Rgba8::WHITE);
	g_theRenderer->BindShader(m_shader);
	g_theRenderer->SetLightConstants(Vec3(2.f, 1.f, -1.f), 0.8f, Rgba8::BLUE);
	g_theRenderer->DrawVertexIndexArray((int)m_objVerts.size(), (int)m_objIndexes.size(), m_objVerts.data(), m_objIndexes.data(), m_vertexBuffer, m_indexBuffer);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->SetModelConstants();

	RenderArrows();
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_GALAXY]);
	g_theRenderer->DrawVertexArray((int)bgVerts.size(), bgVerts.data());
	g_theRenderer->BindTexture(nullptr);

	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++) {
		Actor* const& actor = m_actors[actorIndex];
		if (actor != nullptr) {
			actor->Render(m_worldCamera);
		}
	}

	g_theRenderer->EndCamera(m_worldCamera);
}

void GameMapSelection::RenderArrows() const
{
	Actor* actor = ClosestPlanet();

	if (actor != nullptr) {
		if (actor->m_actorDef->m_name == "Galaxy") {
			std::vector<Vertex_PCU> arrow0, arrow1, arrow2;
			AABB2 arrow;
			arrow.SetCenter(Vec2(0.f, 0.f));
			arrow.SetDimensions(Vec2(1.f, 1.f));
			Vec3 bl = arrow.m_mins;
			Vec3 br = arrow.GetBottomRight();
			Vec3 tl = arrow.GetTopLeft();
			Vec3 tr = arrow.m_maxs;
			bl.z = 1.f;
			br.z = 1.f;
			tl.z = 1.f;
			tr.z = 1.f;
			Rgba8 color = Rgba8::WHITE;
			float distFactor = 4.f - GetDistanceSquared3D(m_shipPosition, actor->m_position);
			distFactor = RangeMap(distFactor, 0.f, 4.f, 0.f, 1.f);
			color.a = DenormalizeByte(distFactor);
			AddVertsForQuad3D(arrow0, bl, br, tr, tl, color);
			AddVertsForQuad3D(arrow1, bl, br, tr, tl, color);
			AddVertsForQuad3D(arrow2, bl, br, tr, tl, color);
			TransformVertexArrayXY3D((int)arrow0.size(), arrow0.data(), 2.f, 60.f, Vec2(1.f, 1.f));
			TransformVertexArrayXY3D((int)arrow1.size(), arrow1.data(), 2.f, -180.f, Vec2(-1.f, 1.f));
			TransformVertexArrayXY3D((int)arrow2.size(), arrow2.data(), 2.f, 270.f, Vec2(1.f, -1.f));
			g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_ARROW]);
			g_theRenderer->DrawVertexArray((int)arrow0.size(), arrow0.data());
			g_theRenderer->DrawVertexArray((int)arrow1.size(), arrow1.data());
			g_theRenderer->DrawVertexArray((int)arrow2.size(), arrow2.data());
			g_theRenderer->BindTexture(nullptr);
		}
	}
}

Actor* GameMapSelection::ClosestPlanet() const
{
	Vec3 shipPos = m_shipPosition;
	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++) {
		Actor* const& actor = m_actors[actorIndex];
		if (actor != nullptr) {
			if (GetDistanceSquared3D(actor->m_position, shipPos) < 4.f) {
				return actor;
			}
		}
	}
	return nullptr;
}
