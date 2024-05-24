#define UNUSED(x) (void)(x);
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"

//Game* g_theGame = nullptr;
Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
RandomNumberGenerator* g_rng = nullptr;
AudioSystem* g_theAudio = nullptr;
Window* g_theWindow = nullptr;
DevConsole* g_theDevConsole = nullptr;


App::App()
{
	//m_theGame = new Game(this);
	m_devCamera = new Camera();
	m_devCamera->m_bottomLeft = Vec2(0.f, 0.f);
	m_devCamera->m_topRight = Vec2(1600.f, 800.f);
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

bool Command_SetTimeScale(EventArgs& args)
{
	float scale = args.GetValue("scale", -99.f);
	if (scale == -99.f) {
		g_theDevConsole->AddLine(DevConsole::ERROR, "Invalid format: SetGameTimeScale. Example:");
		g_theDevConsole->AddLine(DevConsole::WARNING, "SetGameTimeScale scale=1.0");
		FireEvent("FailToFire");
		return true;
	}
	g_theApp->SetGameTimeScale(scale);
	return true;
}

void App::Startup()
{	
	EventSystemConfig eventSystemConfig;
	g_theEventSystem = new EventSystem(eventSystemConfig);
	
	InputSystemConfig inputSystemConfig;
	g_theInput = new InputSystem(inputSystemConfig);
	
	WindowConfig windowConfig;
	windowConfig.m_windowTitle = "C32 Starship Gold (refactored)";
	windowConfig.m_clientAspect = 2.0f;
	windowConfig.m_inputSystem = g_theInput;
	g_theWindow = new Window(windowConfig);

	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(rendererConfig);

	DevConsoleConfig devConsoleConfig;
	devConsoleConfig.m_camera = m_devCamera;
	devConsoleConfig.m_renderer = g_theRenderer;
	g_theDevConsole = new DevConsole(devConsoleConfig);

	AudioSystemConfig audioSystemConfig;
	g_theAudio = new AudioSystem(audioSystemConfig);
	
	g_rng = new RandomNumberGenerator();

	g_theEventSystem->Startup();
	g_theWindow->Startup();
	g_theRenderer->Startup();
	g_theDevConsole->Startup();
	g_theInput->Startup();
	g_theAudio->Startup();

	g_theRenderer->SetModelConstants();
	m_theGame = new Game(this);
	m_theGame->Startup();
	SoundID test = g_theAudio->CreateOrGetSound("Data/Audio/Welcome.mp3");
	g_theAudio->StartSound(test);

	m_devCamera->m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	
	m_theGame->m_worldCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

	m_theGame->m_screenCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

	SubscribeEventCallbackFunction("Quit", &Command_Quit);
	SubscribeEventCallbackFunction("SetGameTimeScale", &Command_SetTimeScale);
	ShowAllKeysOnDevConsole();
}

void App::Shutdown()
{
	m_theGame->Shutdown();

	g_theEventSystem->Shutdown();
	g_theInput->Shutdown();
	g_theWindow->Shutdown();
	g_theRenderer->Shutdown();
	g_theAudio->Shutdown();
	g_theDevConsole->Shutdown();

	delete m_theGame;
	delete m_devCamera;

	delete g_theEventSystem;
	delete g_theDevConsole;
	delete g_theInput;
	delete g_theWindow;
	delete g_theRenderer;
	delete g_theAudio;

	delete g_rng;

	m_theGame = nullptr;
	m_devCamera = nullptr;

	g_theDevConsole = nullptr;
	g_theInput = nullptr;
	g_theWindow = nullptr;
	g_theRenderer = nullptr;	
	g_theAudio = nullptr;

	g_rng = nullptr;
}

void App::RunFrame()
{
	BeginFrame();
	Clock::TickSystemClock();
	Update();
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

void App::SetGameTimeScale(float scale)
{
	m_theGame->m_clock->SetTimeScale(scale);
}

void App::BeginFrame()
{
	g_theEventSystem->BeginFrame();
	g_theDevConsole->BeginFrame();
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudio->BeginFrame();
}

void App::Update()
{	
	m_theGame->Update();
	m_devCamera->SetOrthoView(m_devCamera->m_bottomLeft, m_devCamera->m_topRight);
	UpdateFromKeyboard();
}

void App::Render() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
	m_theGame->Render();
	g_theDevConsole->Render(AABB2(0.f, 0.f, 1400.f, 700.f));
}

void App::EndFrame()
{
	g_theEventSystem->EndFrame();
	g_theDevConsole->EndFrame();
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
	g_theRenderer->EndFrame();
	g_theAudio->EndFrame();
}


void App::UpdateFromKeyboard()
{
	if (g_theInput->WasKeyJustPressed('P'))
	{
		if (m_isPaused)
		{
			SoundID unpause = g_theAudio->CreateOrGetSound("Data/Audio/Unpause.mp3");
			g_theAudio->StartSound(unpause);
		}
		else
		{
			SoundID pause = g_theAudio->CreateOrGetSound("Data/Audio/Pause.mp3");
			g_theAudio->StartSound(pause);
		}
		m_isPaused = !m_isPaused;
		
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
	{
		m_theGame->Shutdown();
		delete m_theGame;
		m_theGame = nullptr;
		m_theGame = new Game(this);
		SoundID test = g_theAudio->CreateOrGetSound("Data/Audio/Welcome.mp3");
		g_theAudio->StartSound(test);
		m_theGame->Startup();
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
		SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/TestSound.mp3");
		g_theAudio->StartSound(testSound);
	}
}

void ShowAllKeysOnDevConsole()
{
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Type Help for a list of commands");
	g_theDevConsole->AddLine(DevConsole::INFO_MAJOR, "Keys");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "P		- Pause");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "O		- Step Frame");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "T		- Slow Time");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "S		- Turn Left");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "F		- Turn Right");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "E		- Thrust");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "N		- Respawn");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Space - Fire");

}
