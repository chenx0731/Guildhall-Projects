#define UNUSED(x) (void)(x);
#include "Game/GameResult.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/Player.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
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
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/UI/Button.hpp"
#include "Engine/Core/STLUtils.hpp"
GameResult::GameResult()
{
	m_clock = new Clock(Clock::GetSystemClock());
	m_worldCamera.m_position = Vec3(3.f, 19.f, 14.f);
	m_worldCamera.m_orientation = EulerAngles(255.f, 60.f, 0.f);
	m_screenCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	m_worldCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
}

GameResult::~GameResult()
{

}

void GameResult::Startup()
{

	
	AABB2 button1 = AABB2(Vec2(700.f, 450.f), Vec2(900.f, 500.f));
	AABB2 button2 = AABB2(Vec2(700.f, 380.f), Vec2(900.f, 430.f));
	AABB2 button3 = AABB2(Vec2(700.f, 310.f), Vec2(900.f, 360.f));

	Button* button = new Button("Restart", button1, g_theFont);
	m_buttons.push_back(button);
	button = new Button("Option", button2, g_theFont);
	m_buttons.push_back(button);
	button = new Button("Exit", button3, g_theFont);
	m_buttons.push_back(button);
}

void GameResult::Update()
{
	SetCamera();
	//UpdateFromKeyboard();
	if (m_state != GameState::RESULT) {
		g_theGameState = m_state;
		g_theGames[(int)g_theGameState]->Enter();
	}
	Vec2 cursor = g_theWindow->GetNormalizedCursorPos();
	cursor.x *= SCREEN_CAMERA_SIZE_X;
	cursor.y *= SCREEN_CAMERA_SIZE_Y;

	for (int buttonIndex = 0; buttonIndex < (int)m_buttons.size(); buttonIndex++) {
		Button*& button = m_buttons[buttonIndex];
		if (button != nullptr) {
			button->m_isHovered = false;
			//button->m_isSelected = false;
			if (button->IsPositionInside(cursor)) {
				button->m_isHovered = true;
			}
		}
	}

	UpdateFromKeyboard();

	m_jupiterDegree += m_clock->GetDeltaSeconds() * 10.f;
	m_earthDegree += m_clock->GetDeltaSeconds() * 20.f;

	if (m_buttons[2]->m_isSelected) {
		Exit();
		m_state = GameState::ATTRACT;
	}
	if (m_buttons[0]->m_isSelected) {

	}
	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++) {
		Actor*& actor = m_actors[actorIndex];
		if (actor != nullptr) {
			if (actor->m_animeClock.GetElapsedTime() == 0.f) {
				actor->m_animeClock.Start();
			}
			if (actor->m_actorDef->m_name == "Jupiter") {
				actor->m_position = Vec2::MakeFromPolarDegrees(m_jupiterDegree, 15.f);
			}
			if (actor->m_actorDef->m_name == "Earth") {
				actor->m_position = Vec2::MakeFromPolarDegrees(m_earthDegree, 7.5f);
			}
			actor->Update(m_clock->GetDeltaSeconds());
		}
	}
}

void GameResult::Render() const
{
	RenderWorld();
	RenderUI();
}

void GameResult::Shutdown()
{
	for (int buttonIndex = 0; buttonIndex < (int)m_buttons.size(); buttonIndex++) {
		Button* button = m_buttons[buttonIndex];
		if (button != nullptr) {
			delete button;
		}
	}
}

void GameResult::Enter()
{
	SetCamera();
	//SetVolume(MUSIC_MAINMENU, )
	PlayMusicLoop(MUSIC_MAINMENU);

	Actor* actor = new Actor("Sun", Vec3(0.f, 0.f, 0.f));
	m_actors.push_back(actor);
	actor = new Actor("Jupiter", Vec3(15.f, 0.f, 0.f));
	m_actors.push_back(actor);
	actor = new Actor("Earth", Vec3(0.f, 7.5f, 0.f));
	m_actors.push_back(actor);
}

void GameResult::Exit()
{
	for (int buttonIndex = 0; buttonIndex < (int)m_buttons.size(); buttonIndex++) {
		Button* button = m_buttons[buttonIndex];
		if (button != nullptr) {
			button->m_isHovered = false;
			button->m_isSelected = false;
			button->m_isFocused = false;
		}
	}
	ClearAndDeleteEverything(m_actors);
}


void GameResult::RenderWorld() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	DebugDrawRing(Vec2(0.f, 0.f), 7.5f, 0.02f, Rgba8::WHITE);
	DebugDrawRing(Vec2(0.f, 0.f), 15.f, 0.02f, Rgba8::WHITE);
	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++) {
		Actor* const& actor = m_actors[actorIndex];
		if (actor != nullptr) {
			actor->Render(m_worldCamera);
		}
	}

	g_theRenderer->EndCamera(m_worldCamera);
}

void GameResult::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->SetModelConstants();

	//DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
	//	(150.f + 50.f * SinDegrees(100.f * m_clock->GetTotalSeconds())),
	//	(20.f + 5.f * CosDegrees(100.f * m_clock->GetTotalSeconds())),
	//	Rgba8(255, 150, 0));
	std::vector<Vertex_PCU> texts;

	for (int buttonIndex = 0; buttonIndex < (int)m_buttons.size(); buttonIndex++) {
		Button* const& button = m_buttons[buttonIndex];
		if (button != nullptr) {
			std::vector<Vertex_PCU> bg, text;
			button->AddVertsForButtonBackground(bg);
			button->AddVertsForButtonText(text);

			g_theRenderer->DrawVertexArray(int(bg.size()), bg.data());
			g_theRenderer->BindTexture(&button->m_font->GetTexture());
			g_theRenderer->DrawVertexArray(int(text.size()), text.data());
			g_theRenderer->BindTexture(nullptr);
		}
	}

	std::string string = Stringf("Result Screen");
	g_theFont->AddVertsForTextInBox2D(texts, AABB2(0.f, 550.f, 1600.f, 800.f), 20.f, string);
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)texts.size(), texts.data());
	g_theRenderer->BindTexture(nullptr);

	g_theRenderer->EndCamera(m_screenCamera);
}

void GameResult::SetCamera()
{
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	m_worldCamera.SetPerspectiveView(2.f, 60.f, 0.1f, 100.f);
	m_worldCamera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
}

void GameResult::UpdateFromKeyboard()
{
	XboxController tempController;
	tempController = g_theInput->GetController(0);
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC)) {
		Exit();
		m_state = GameState::ATTRACT;
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE)) {
		Vec2 cursor = g_theWindow->GetNormalizedCursorPos();
		cursor.x *= SCREEN_CAMERA_SIZE_X;
		cursor.y *= SCREEN_CAMERA_SIZE_Y;

		for (int buttonIndex = 0; buttonIndex < (int)m_buttons.size(); buttonIndex++) {
			Button*& button = m_buttons[buttonIndex];
			if (button != nullptr) {
				if (button->IsPositionInside(cursor)) {
					button->m_isSelected = true;
				}
			}
		}
	}
}

void GameResult::EnterLobby()
{

}
