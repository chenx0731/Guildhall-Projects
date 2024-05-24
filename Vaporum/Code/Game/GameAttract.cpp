#define UNUSED(x) (void)(x);
#include "Game/GameAttract.hpp"
#include "Game/GameCommon.hpp"
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


GameAttract::GameAttract() : Game()
{
	//m_clock = new Clock(Clock::GetSystemClock());

}

GameAttract::~GameAttract()
{

}

void GameAttract::Startup()
{
	LoadAssets();

	SetCamera();

}

void GameAttract::Update()
{
	UpdateFromKeyboard();
	UpdateFromController();


	SetCamera();

}

void GameAttract::Render() const
{

	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));


	RenderUI();


	
	DebugRenderWorld(m_worldCamera);
	DebugRenderScreen(m_screenCamera);
}

void GameAttract::Shutdown()
{


}

void GameAttract::Enter()
{
	g_theInput->EndFrame();
}

void GameAttract::Exit()
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



void GameAttract::LoadAssets()
{
	g_textureLibrary[TEXTURE_ATTRACT_LOGO] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Logo.png");
}


void GameAttract::UpdateFromKeyboard()
{

	if (g_theInput->WasKeyJustPressed('O'))
	{

	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ENTER) || g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE)) {
		g_theGameState = GameState::MAIN;
	}

}

void GameAttract::UpdateFromController()
{

}

void GameAttract::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);


	g_theRenderer->EndCamera(m_screenCamera);
}

void GameAttract::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->SetModelConstants();
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));

	std::vector<Vertex_PCU> title, text;

	AABB2 titleBox;
	titleBox.SetCenter(Vec2(g_theApp->m_windowWidth * 0.5f, g_theApp->m_windowHeight * 0.85f));
	titleBox.SetDimensions(Vec2(g_theApp->m_windowWidth * 0.6f, g_theApp->m_windowHeight * 0.2f));
		//= AABB2(Vec2(600.f, 600.f), Vec2(1000.f, 800.f));
	AABB2 textBox;
	textBox.SetCenter(Vec2(g_theApp->m_windowWidth * 0.5f, g_theApp->m_windowHeight * 0.2f));
	textBox.SetDimensions(Vec2(g_theApp->m_windowWidth * 0.6f, g_theApp->m_windowHeight * 0.1f));
		//= AABB2(Vec2(500.f, 100.f), Vec2(1100.f, 200.f));

	g_uiFont->AddVertsForTextInBox2D(title, titleBox, 80.f, "Vaporum", Rgba8::WHITE, 0.7f);
	g_uiFont->AddVertsForTextInBox2D(text, textBox, 40.f, "Press Enter or Click to Start Game", Rgba8::WHITE, 0.7f);

	g_theRenderer->BindTexture(&g_uiFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)title.size(), title.data());
	g_theRenderer->DrawVertexArray((int)text.size(), text.data());
	g_theRenderer->BindTexture(nullptr);

	std::vector<Vertex_PCU> logo;
	AABB2 box;
	box.SetCenter(g_theApp->m_windowDimension * 0.5f);
	box.SetDimensions(Vec2(g_theApp->m_windowHeight * 0.5f, g_theApp->m_windowHeight * 0.5f));

	AddVertsForAABB2DWithPaddings(logo, box, Rgba8::WHITE);
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_ATTRACT_LOGO]);
	g_theRenderer->DrawVertexArray((int)logo.size(), logo.data());
	g_theRenderer->BindTexture(nullptr);

	g_theRenderer->EndCamera(m_screenCamera);
}




void GameAttract::SetCamera()
{
	//m_worldCamera.SetOrthoView(Vec2(-1.f, -1.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_worldCamera.SetPerspectiveView(m_worldCamera.m_aspect,
		m_worldCamera.m_FOV, m_worldCamera.m_perspectiveNear, m_worldCamera.m_perspectiveFar);
	m_worldCamera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight));
}

