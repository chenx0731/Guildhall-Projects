#define UNUSED(x) (void)(x);
#include "Game/GameMain.hpp"
#include "Game/GameCommon.hpp"
#include "GamePlaying.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Game/App.hpp"
#include "Game/Map.hpp"
#include "Game/Model.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/UnitDefinition.hpp"
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
#include "Engine/UI/Button.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

GameMain::GameMain() : Game()
{
	AABB2 bound1;
	bound1.SetCenter(Vec2(g_theApp->m_windowWidth * 0.40f, g_theApp->m_windowHeight * 0.53f));
	bound1.SetDimensions(Vec2(g_theApp->m_windowWidth * 0.32f, g_theApp->m_windowHeight * 0.04f));

	AABB2 bound2;
	bound2.SetCenter(Vec2(g_theApp->m_windowWidth * 0.40f, g_theApp->m_windowHeight * 0.47f));
	bound2.SetDimensions(Vec2(g_theApp->m_windowWidth * 0.32f, g_theApp->m_windowHeight * 0.04f));

	Button* button1 = new Button("New Game", bound1, g_uiFont);
	Button* button2 = new Button("Quit", bound2, g_uiFont);


	m_buttons.push_back(button1);
	m_buttons.push_back(button2);

	if (m_buttons.size() > m_focusedIndex)
		m_focusedButton = m_buttons[m_focusedIndex];
}

GameMain::~GameMain()
{

}

void GameMain::Startup()
{
	LoadAssets();

	SetCamera();

}

void GameMain::Update()
{
	if (g_theGameState != GameState::MAIN)
		return;
	Vec2 cursor = g_theWindow->GetNormalizedCursorPos();
	cursor.x *= g_theApp->m_windowWidth;
	cursor.y *= g_theApp->m_windowHeight;
	for (int buttonIndex = 0; buttonIndex < (int)m_buttons.size(); buttonIndex++) {
		m_buttons[buttonIndex]->Update(cursor);
		if (m_buttons[buttonIndex]->m_isHovered) {
			m_focusedButton = m_buttons[buttonIndex];
			m_focusedIndex = buttonIndex;
		}
	}

	
	UpdateFromKeyboard();
	UpdateFromController();

	if (m_focusedButton)
		m_focusedButton->m_isHovered = true;
	SetCamera();

}

void GameMain::Render() const
{
	if (g_theGameState != GameState::MAIN)
		return;
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));


	RenderUI();



	DebugRenderWorld(m_worldCamera);
	DebugRenderScreen(m_screenCamera);
}

void GameMain::Shutdown()
{
	

}

void GameMain::Enter()
{
	g_theInput->EndFrame();
	g_theGame->Shutdown();
	g_theGame->Startup();
}

void GameMain::Exit()
{
	for (int buttonIndex = 0; buttonIndex < (int)m_buttons.size(); buttonIndex++) {
		Button* button = m_buttons[buttonIndex];
		if (button != nullptr) {
			button->m_isHovered = false;
			button->m_isSelected = false;
			button->m_isFocused = false;
		}
	}
}



void GameMain::LoadAssets()
{


}


void GameMain::UpdateFromKeyboard()
{

	if (g_theInput->WasKeyJustPressed(KEYCODE_UPARROW)) {
		m_focusedIndex--;
		GetClamped(m_focusedIndex, 0, (int)m_buttons.size() - 1);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_DOWNARROW)) {
		m_focusedIndex++;
		GetClamped(m_focusedIndex, 0, (int)m_buttons.size() - 1);
	}
	if (m_buttons.size() > m_focusedIndex)
		m_focusedButton = m_buttons[m_focusedIndex];
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE)) {
		if (m_focusedButton) {
			m_focusedButton->m_isSelected = true;
		}
	}
	if (m_buttons[0]->m_isSelected) {
		Exit();
		g_theGameState = GameState::PLAYING;
		g_theGames[(int)g_theGameState]->Enter();
	}
	if (m_buttons[1]->m_isSelected) {
		Exit();
		g_theApp->HandleQuitRequested();
	}


}

void GameMain::UpdateFromController()
{

}

void GameMain::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);


	g_theRenderer->EndCamera(m_screenCamera);
}

void GameMain::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->SetModelConstants();
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));
	
	std::vector<Vertex_PCU> logo, text;
	AABB2 box;
	box.SetCenter(g_theApp->m_windowDimension * 0.5f);
	box.SetDimensions(Vec2(g_theApp->m_windowHeight * 0.5f, g_theApp->m_windowHeight * 0.5f));
	AddVertsForAABB2DWithPaddings(logo, box, Rgba8::WHITE);
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_ATTRACT_LOGO]);
	g_theRenderer->DrawVertexArray((int)logo.size(), logo.data());
	g_theRenderer->BindTexture(nullptr);

	g_theRenderer->SetModelConstants();
	AABB2 textBox;
	textBox.SetCenter(Vec2(0.f, g_theApp->m_windowHeight * 0.5f));
	textBox.SetDimensions(Vec2(g_theApp->m_windowWidth * 0.5f, g_theApp->m_windowHeight * 0.3f));
	//= AABB2(Vec2(0.f, 300.f), Vec2(600.f, 500.f));
	g_uiFont->AddVertsForTextInBox2D(text, textBox, 100.f, "Main Menu");
	TransformVertexArrayXY3D((int)text.size(), text.data(), 1.f, 90.f, Vec2(g_theApp->m_windowWidth * 0.47f, g_theApp->m_windowHeight * 0.5f));
	g_theRenderer->BindTexture(&g_uiFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)text.size(), text.data());
	g_theRenderer->BindTexture(nullptr);

	DrawSquare(Vec2(0.23f * g_theApp->m_windowWidth, 0.f), Vec2(0.24f * g_theApp->m_windowWidth, g_theApp->m_windowHeight), Rgba8::WHITE);

	for (int buttonIndex = 0; buttonIndex < (int)m_buttons.size(); buttonIndex++) {
		m_buttons[buttonIndex]->Render();
	}

	g_theRenderer->EndCamera(m_screenCamera);
}




void GameMain::SetCamera()
{
	//m_worldCamera.SetOrthoView(Vec2(-1.f, -1.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_worldCamera.SetPerspectiveView(m_worldCamera.m_aspect,
		m_worldCamera.m_FOV, m_worldCamera.m_perspectiveNear, m_worldCamera.m_perspectiveFar);
	m_worldCamera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight));
}

