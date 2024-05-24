#define UNUSED(x) (void)(x);
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/GamePlaying.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Net/NetSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

//Game* g_theGame = nullptr;
Renderer*				g_theRenderer = nullptr;
InputSystem*			g_theInput = nullptr;
RandomNumberGenerator*	g_rng = nullptr;
AudioSystem*			g_theAudio = nullptr;
Window*					g_theWindow = nullptr;
GamePlaying*			g_theGame = nullptr;
DevConsole*				g_theDevConsole = nullptr;
NetSystem*				g_theNet = nullptr;
BitmapFont*				g_uiFont = nullptr;

GameState				g_theGameState;

SoundID					g_musicLibrary[MUSIC_NUM];
SoundID					g_lastPlayedMusic[MUSIC_NUM];
Texture*				g_textureLibrary[TEXTURE_NUM];

void ShowAllKeysOnDevConsole();

App::App()
{
	m_devCamera = new Camera();
	m_devCamera->m_bottomLeft = Vec2(0.f, 0.f);
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

bool Command_LoadGameConfig(EventArgs& args)
{
	std::string file = args.GetValue("File", "Data/GameConfig.xml");
	g_theApp->LoadGameConfig(file.c_str());
	return true;
}

void App::Startup()
{	

	//LoadGameConfig("Data/GameConfig.xml");
	std::string s = "RemoteCommand Command=\"Help\"";
	Strings sr = SplitStringWithQuotes(s, " ");

	EventSystemConfig eventSystemConfig;
	g_theEventSystem = new EventSystem(eventSystemConfig);
	
	InputSystemConfig inputSystemConfig;
	g_theInput = new InputSystem(inputSystemConfig);
	std::string initialCommand = g_gameConfigBlackboard.GetValue("CommandLineString", "");
	SubscribeEventCallbackFunction("LoadGameConfig", &Command_LoadGameConfig);
	//g_theDevConsole->Execute(initialCommand);
	//FireEvent("LoadGameConfig");
	LoadGameConfig("Data/GameConfig.xml");

	if (initialCommand != "")
		LoadGameConfig(initialCommand.c_str());
	WindowConfig windowConfig;
	windowConfig.m_windowTitle = g_gameConfigBlackboard.GetValue("windowTitle", "Vaporum");
	windowConfig.m_clientAspect = 2.0f;
	windowConfig.m_isFullScreen = g_gameConfigBlackboard.GetValue("windowFullscreen", false);
	windowConfig.m_windowSize = g_gameConfigBlackboard.GetValue("windowSize", IntVec2(-1, -1));
	windowConfig.m_windowPosition = g_gameConfigBlackboard.GetValue("windowPosition", IntVec2(-1, -1));
	windowConfig.m_inputSystem = g_theInput;
	g_theWindow = new Window(windowConfig);

	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(rendererConfig);

	DevConsoleConfig devConsoleConfig;
	devConsoleConfig.m_camera = m_devCamera;
	devConsoleConfig.m_renderer = g_theRenderer;
	g_theDevConsole = new DevConsole(devConsoleConfig);
	// open dev console by default
	//g_theDevConsole->ToggleOpen();


	AudioSystemConfig audioSystemConfig;
	g_theAudio = new AudioSystem(audioSystemConfig);

	DebugRenderConfig debugRenderConfig;
	debugRenderConfig.m_renderer = g_theRenderer;

	NetSystemConfig netSystemConfig;
	std::string netMode = g_gameConfigBlackboard.GetValue("netMode", "None");
	if (netMode == "Node") {
		netSystemConfig.m_mode = NetSystemMode::NONE;
	}
	else if (netMode == "Client") {
		netSystemConfig.m_mode = NetSystemMode::CLIENT;
	}
	else if (netMode == "Server") {
		netSystemConfig.m_mode = NetSystemMode::SERVER;
	}
	netSystemConfig.m_receiveBufferSize = g_gameConfigBlackboard.GetValue("netRecvBufferSize", 2048);
	netSystemConfig.m_sendBufferSize = g_gameConfigBlackboard.GetValue("netSendBufferSize", 2048);
	netSystemConfig.m_host = g_gameConfigBlackboard.GetValue("netHostAddress", "127.0.0.1:27015");
	g_theNet = new NetSystem(netSystemConfig);
	g_rng = new RandomNumberGenerator();


	//g_theInput->Startup();
	g_theEventSystem->Startup();
	g_theWindow->Startup();
	g_theRenderer->Startup();
	g_theDevConsole->Startup();
	g_theInput->Startup();
	g_theAudio->Startup();
	g_theNet->Startup();
	DebugRenderSystemStartup(debugRenderConfig);

	m_windowWidth = (float)g_theWindow->GetClientDimensions().x;
	m_windowHeight = (float)g_theWindow->GetClientDimensions().y;
	m_devCamera->m_topRight = Vec2(m_windowWidth, m_windowHeight);
	m_windowDimension = Vec2(m_windowWidth, m_windowHeight);

	g_uiFont = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/MyFixedFont");

	m_devCamera->m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

	g_theGameState = GameState::ATTRACT;

	for (int i = 0; i < (int)GameState::COUNT; i++) {
		Game::CreateOrGetGameStateOfType((GameState)i);
		if (g_theGames[i] != nullptr)
			g_theGames[i]->Startup();
		if (GameState(i) == GameState::PLAYING) {
			g_theGame = (GamePlaying*)g_theGames[i];
		}
	}


	SubscribeEventCallbackFunction("Quit", &Command_Quit);

	ShowAllKeysOnDevConsole();
}

void App::Shutdown()
{
	for (int i = 0; i < (int)GameState::COUNT; i++) {
		if (g_theGames[i] != nullptr)
			g_theGames[i]->Shutdown();
	}
	for (int i = 0; i < (int)GameState::COUNT; i++) {
		if (g_theGames[i] != nullptr) {

			delete g_theGames[i];
		}
		g_theGames[i] = nullptr;
		//Game::CreateOrGetGameStateOfType((GameState)i);
	}
	g_theGame = nullptr;

	g_theEventSystem->Shutdown();
	g_theInput->Shutdown();
	g_theWindow->Shutdown();
	g_theRenderer->Shutdown();
	g_theAudio->Shutdown();
	g_theNet->Shutdown();
	g_theDevConsole->Shutdown();
	DebugRenderSystemShutdown();

	delete g_theGame;
	delete m_devCamera;

	delete g_theEventSystem;
	delete g_theDevConsole;
	delete g_theInput;
	delete g_theWindow;
	delete g_theRenderer;
	delete g_theAudio;
	delete g_theNet;

	delete g_rng;

	g_theGame = nullptr;
	m_devCamera = nullptr;

	g_theEventSystem = nullptr;
	g_theDevConsole = nullptr;
	g_theInput = nullptr;
	g_theWindow = nullptr;
	g_theRenderer = nullptr;	
	g_theAudio = nullptr;
	g_theNet = nullptr;

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
	g_theNet->BeginFrame();
	DebugRenderBeginFrame();
}

void App::Update()
{	
	m_devCamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(m_windowWidth, m_windowHeight));
	g_theGames[(int)g_theGameState]->Update();
	bool mouse_show = true;
		//g_theWindow->IsFocused();
	g_theInput->SetCursorMode(!mouse_show, !mouse_show);
	UpdateFromKeyboard();
}

void App::Render() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
	g_theGames[(int)g_theGameState]->Render();
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
	g_theNet->EndFrame();
	DebugRenderEndFrame();
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
		}	
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC) || g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_BACK))
	{
		if (g_theGameState == GameState::PAUSE)
		{
			g_theGames[(int)g_theGameState]->Exit();
			g_theGameState = GameState::MAIN;
			g_theGames[(int)g_theGameState]->Enter();
		}
		else if (g_theGameState == GameState::ATTRACT)
		{
			g_theGames[(int)g_theGameState]->Exit();
			HandleQuitRequested();
		}
		else if (g_theGameState == GameState::MAIN) {
			g_theGames[(int)g_theGameState]->Exit();
			HandleQuitRequested();
		}
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

void ShowAllKeysOnDevConsole()
{
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Type Help for a list of commands");
	g_theDevConsole->AddLine(DevConsole::INFO_MAJOR, "Keys");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "W				- Forward");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "S				- Back");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "A				- Left");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "D				- Right");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Q				- Down");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "E				- Up");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "L				- Load Model");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "UP Arrow		- Pitch up");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Down Arrow	- Pitch down");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Left Arrow	- Yaw left");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Right Arrow	- Yaw right");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Right Arrow	- Yaw right");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "LoadModel path=Data/Models/Cow.xml");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "LoadMap name=Grid32x32");

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