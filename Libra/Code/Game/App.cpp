#define UNUSED(x) (void)(x);
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"


Renderer*				g_theRenderer = nullptr;
InputSystem*			g_theInput = nullptr;
RandomNumberGenerator*	g_rng = nullptr;
AudioSystem*			g_theAudio = nullptr;
Window*					g_theWindow = nullptr;
Game*					g_theGame = nullptr;
DevConsole*				g_theDevConsole = nullptr;


bool					g_debugDraw = false;
bool					g_debugNoclip = false;
bool					g_debugHeatMap = false;

bool					g_fastSpeed = false;
bool					g_slowSpeed = false;

SoundID					g_musicLibrary[MUSIC_NUM];
SoundID					g_lastPlayedMusic[MUSIC_NUM];
Texture*				g_textureLibrary[TEXTURE_NUM];

void ShowAllKeysOnDevConsole();

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

void App::Startup()
{	
	EventSystemConfig eventSystemConfig;
	g_theEventSystem = new EventSystem(eventSystemConfig);
	
	InputSystemConfig inputSystemConfig;
	g_theInput = new InputSystem(inputSystemConfig);
	
	WindowConfig windowConfig;
	windowConfig.m_windowTitle = "C32 SD A7-Libra";
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

	LoadGameConfig("Data/GameConfig.xml");

	m_devCamera->m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

	g_theGame = new Game(this);

	g_theGame->m_worldCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

	g_theGame->m_screenCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

	g_theGame->Startup();

	ShowAllKeysOnDevConsole();

	SubscribeEventCallbackFunction("Quit", &Command_Quit);
}

void App::Shutdown()
{
	g_theGame->Shutdown();

	g_theInput->Shutdown();
	g_theWindow->Shutdown();
	g_theRenderer->Shutdown();
	g_theAudio->Shutdown();

	delete g_theGame;
	delete m_devCamera;

	delete g_theEventSystem;
	delete g_theDevConsole;
	delete g_theInput;
	delete g_theWindow;
	delete g_theRenderer;
	delete g_theAudio;

	delete g_rng;

	g_theGame = nullptr;
	m_devCamera = nullptr;

	g_theEventSystem = nullptr;
	g_theDevConsole = nullptr;
	g_theInput = nullptr;
	g_theWindow = nullptr;
	g_theRenderer = nullptr;
	g_theAudio = nullptr;

	g_rng = nullptr;
}

void App::RunFrame()
{
	Clock::TickSystemClock();
	BeginFrame();
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
	//if (g_fastSpeed)
	//	g_theGame->m_clock->SetTimeScale(4.f);
	//else if (g_slowSpeed)
	//	g_theGame->m_clock->SetTimeScale(0.1f);
	//else
	//	g_theGame->m_clock->SetTimeScale(1.f);
	g_theGame->Update(g_theGame->m_clock->GetDeltaSeconds());
	m_devCamera->SetOrthoView(m_devCamera->m_bottomLeft, m_devCamera->m_topRight);
	UpdateFromKeyboard();
}

void App::Render() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
	g_theGame->Render();
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
			g_slowSpeed = true;
			g_theGame->m_clock->SetTimeScale(0.1f);
			//g_theAudio->SetSoundPlaybackSpeed(g_lastPlayedMusic[MUSIC_BGM], 0.1f);
			SetMusicSpeed(MUSIC_BGM, g_theGame->m_clock->GetTimeScale());
		}	
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
	{
		g_theGame->Shutdown();
		delete g_theGame;
		g_theGame = nullptr;
		g_theGame = new Game(this);
		g_theGame->Startup();
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC) || g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_BACK))
	{
		/*if (!g_theGame->m_isPaused) {
			g_theGame->m_isPaused = true;
		}*/
		if (g_theGame->m_gameStatus == GAME_STATUS_ATTRACT) {
			HandleQuitRequested();
		}
		else if (g_theGame->m_gameStatus == GAME_STATUS_PLAY) {
			PlayMusic(MUSIC_PAUSE);
			PauseMusic(MUSIC_BGM);
			g_theGame->m_gameStatus = GAME_STATUS_PAUSE;
		}
		else if (g_theGame->m_gameStatus == GAME_STATUS_PAUSE 
			|| g_theGame->m_gameStatus == GAME_STATUS_WIN
			|| g_theGame->m_gameStatus == GAME_STATUS_DIE) {
			g_theGame->ReturnAttractModeNormal();
		}

		
	}
	if (g_theInput->WasKeyJustReleased('T'))
	{
		if (m_isSlowMo)
		{
			m_isSlowMo = false;
			g_slowSpeed = false;
			//g_theAudio->SetSoundPlaybackSpeed(g_lastPlayedMusic[MUSIC_BGM], 1.f);
			g_theGame->m_clock->SetTimeScale(1.f);
			//g_theAudio->SetSoundPlaybackSpeed(g_lastPlayedMusic[MUSIC_BGM], 0.1f);
			SetMusicSpeed(MUSIC_BGM, g_theGame->m_clock->GetTimeScale());
		}
	}
	if (g_theInput->IsKeyDown('Y'))
	{
		g_fastSpeed = true;
		//g_theAudio->SetSoundPlaybackSpeed(g_lastPlayedMusic[MUSIC_BGM], 4.f);
		g_theGame->m_clock->SetTimeScale(4.f);
		//g_theAudio->SetSoundPlaybackSpeed(g_lastPlayedMusic[MUSIC_BGM], 0.1f);
		SetMusicSpeed(MUSIC_BGM, g_theGame->m_clock->GetTimeScale());
	}
	if (g_theInput->WasKeyJustReleased('Y'))
	{
		g_fastSpeed = false;
		//g_theAudio->SetSoundPlaybackSpeed(g_lastPlayedMusic[MUSIC_BGM], 1.f);
		g_theGame->m_clock->SetTimeScale(1.f);
		//g_theAudio->SetSoundPlaybackSpeed(g_lastPlayedMusic[MUSIC_BGM], 0.1f);
		SetMusicSpeed(MUSIC_BGM, g_theGame->m_clock->GetTimeScale());
	}

	if (g_theInput->WasKeyJustPressed('B'))
	{

	}
}

void App::LoadGameConfig(const char* docFilePath)
{
	XmlDocument document;
	document.LoadFile(docFilePath);
		//"Data/GameConfig.xml");
	XmlElement* element = document.FirstChildElement("GameConfig");
	while (element) {
		g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*element);
		element = element->NextSiblingElement();
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