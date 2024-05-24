#define UNUSED(x) (void)(x);
#include "Engine/Core/Time.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"

//Game* g_theGame = nullptr;
Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
RandomNumberGenerator* g_rng = nullptr;
//AudioSystem* g_theAudio = nullptr;
Window* g_theWindow = nullptr;
DevConsole* g_theDevConsole = nullptr;
//Game*	g_theGame = nullptr;

Texture* g_textureLibrary[TEXTURE_NUM];
BitmapFont* g_theFont;


App::App()
{
	
}

App::~App()
{

}

bool Command_Quit(EventArgs& args) // Note: STATIC is #define’d to be nothing; just a marker for the reader
{
	UNUSED(args);
	//DebuggerPrintf("Test command received\n");
	g_theApp->HandleQuitRequested();
	//args.DebugPrintContents(); // This method (NamedStrings::DebugPrintContents) is not required; it prints out all key/value pairs
	return true; // Does not consume the event; continue to call other subscribers’ callback functions
}

void App::Startup()
{	
	EventSystemConfig eventSystemConfig;
	g_theEventSystem = new EventSystem(eventSystemConfig);

	InputSystemConfig inputSystemConfig;
	g_theInput = new InputSystem(inputSystemConfig);
	
	WindowConfig windowConfig;
	windowConfig.m_windowTitle = "MathVisualTest";
	windowConfig.m_clientAspect = 2.0f;
	windowConfig.m_inputSystem = g_theInput;
	g_theWindow = new Window(windowConfig);

	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(rendererConfig);



	//AudioSystemConfig audioSystemConfig;
	//g_theAudio = new AudioSystem(audioSystemConfig);
	
	g_rng = new RandomNumberGenerator();

	g_theEventSystem->Startup();
	g_theInput->Startup();
	g_theWindow->Startup();
	g_theRenderer->Startup();
	g_theRenderer->SetModelConstants();
	//g_theAudio->Startup();

	//g_textureLibrary[TEXTURE_FONT] = g_theRenderer->CreateOrGetTextureFromFile("Data/Fonts/MyFixedFont.png");

	g_theFont = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/MyFixedFont");

	m_gameStatus = GAME_STATUS_RAYCAST_CONVEX;
	m_theGame = Game::CreateNewGameOfType(m_gameStatus);

	m_theGame->Startup();

	SubscribeEventCallbackFunction("Quit", &Command_Quit);
}

void App::Shutdown()
{
	m_theGame->Shutdown();

	g_theEventSystem->Shutdown();
	g_theInput->Shutdown();
	g_theWindow->Shutdown();
	g_theRenderer->Shutdown();
	//g_theAudio->Shutdown();

	delete m_theGame;

	delete g_theEventSystem;
	delete g_theInput;
	delete g_theWindow;
	delete g_theRenderer;
	//delete g_theAudio;

	delete g_rng;

	m_theGame = nullptr;

	g_theEventSystem = nullptr;
	g_theInput = nullptr;
	g_theWindow = nullptr;
	g_theRenderer = nullptr;	
	//g_theAudio = nullptr;

	g_rng = nullptr;
}

void App::RunFrame()
{
	static float lastTime = static_cast<float> (GetCurrentTimeSeconds());
	float currentTime = static_cast<float> (GetCurrentTimeSeconds());
	float _deltaSecond = currentTime - lastTime;
	if (_deltaSecond > 0.1f)
	{
		_deltaSecond = 0.1f;
	}
	if (m_isSlowMo)
	{
		_deltaSecond *= 0.1f;
	}
	lastTime = currentTime;
	BeginFrame();
	Update(_deltaSecond);
	Render();
	EndFrame();
}

void App::Run()
{
	while (!m_isQuitting)
	{
		RunFrame();
	}
}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return false;
}

void App::BeginFrame()
{
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	//g_theAudio->BeginFrame();
}

void App::Update(float deltaSeconds)
{	
	m_theGame->Update(deltaSeconds);
	UpdateFromKeyboard(deltaSeconds);
}

void App::Render() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
	m_theGame->Render();
}

void App::EndFrame()
{
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
	g_theRenderer->EndFrame();
	//g_theAudio->EndFrame();
}


void App::UpdateFromKeyboard(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->WasKeyJustPressed(KEYCODE_F6))
	{
		int gameStatus = ((m_gameStatus - 1) % GAME_STATUS_NUM);
		if (gameStatus < 0)
			gameStatus += GAME_STATUS_NUM;
		m_gameStatus = GameStatus(gameStatus);
		m_theGame->Shutdown();
		delete m_theGame;
		m_theGame = Game::CreateNewGameOfType(m_gameStatus);

		m_theGame->Startup();

	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F7))
	{
		int gameStatus = ((m_gameStatus + 1) % GAME_STATUS_NUM);
		m_gameStatus = GameStatus(gameStatus);
		m_theGame->Shutdown();
		delete m_theGame;
		m_theGame = Game::CreateNewGameOfType(m_gameStatus);
		m_theGame->Startup();
	}
	if (g_theInput->WasKeyJustPressed('P'))
	{
		if (m_isPaused)
		{

		}
		else
		{

		}
		m_isPaused = !m_isPaused;
		
	}
	if (g_theInput->IsKeyDown('T'))
	{
		if (!m_isSlowMo) 
		{
			m_isSlowMo = true;
		}	
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC) || g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_BACK))
	{
		if (!m_theGame->m_isAttractMode)
		{
			m_theGame->ReturnAttractModeNormal();
		}
		else HandleQuitRequested();
	}
	if (g_theInput->WasKeyJustReleased('T'))
	{
		if (m_isSlowMo)
		{
			m_isSlowMo = false;
		}
	}
	if (g_theInput->WasKeyJustPressed('B'))
	{

	}
}
