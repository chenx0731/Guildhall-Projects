#define UNUSED(x) (void)(x);
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Renderer/ShaderManager.hpp"

//Game* g_theGame = nullptr;
Renderer*				g_theRenderer = nullptr;
InputSystem*			g_theInput = nullptr;
RandomNumberGenerator*	g_rng = nullptr;
AudioSystem*			g_theAudio = nullptr;
Window*					g_theWindow = nullptr;
Game*					g_theGame = nullptr;
DevConsole*				g_theDevConsole = nullptr;
ShaderManager*			g_shaderManager = nullptr;

GameState				g_theGameState;

SoundID					g_musicLibrary[MUSIC_NUM];
SoundID					g_lastPlayedMusic[MUSIC_NUM];
Texture*				g_textureLibrary[TEXTURE_NUM];

void ShowAllKeysOnDevConsole();

App::App()
{
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
	LoadGameConfig("Data/GameConfig.xml");
	
	EventSystemConfig eventSystemConfig;
	g_theEventSystem = new EventSystem(eventSystemConfig);
	
	InputSystemConfig inputSystemConfig;
	g_theInput = new InputSystem(inputSystemConfig);
	
	WindowConfig windowConfig;
	windowConfig.m_windowTitle = "Doomenstein";
	windowConfig.m_clientAspect = g_gameConfigBlackboard.GetValue("windowAspect", 2.0f);
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

	//g_theInput->Startup();
	g_theEventSystem->Startup();
	g_theWindow->Startup();
	g_theRenderer->Startup();
	g_theDevConsole->Startup();
	g_theInput->Startup();
	g_theAudio->Startup();

	ShaderManagerConfig shaderManagerConfig;
	shaderManagerConfig.m_defFilepath = "Data/Shaders/Engine/ShaderDefinitions";
	shaderManagerConfig.m_dimension = g_theRenderer->GetConfig().m_window->GetClientDimensions();
	shaderManagerConfig.m_renderer = g_theRenderer;

	g_shaderManager = new ShaderManager(shaderManagerConfig);
	g_shaderManager->Startup();

	DebugRenderConfig debugRenderConfig;
	debugRenderConfig.m_renderer = g_theRenderer;
	DebugRenderSystemStartup(debugRenderConfig);

	g_theGameState = GameState::ATTRACT;

	for (int i = 0; i < (int)GameState::COUNT; i++) {
	  	Game::CreateOrGetGameStateOfType((GameState)i);
		if (g_theGames[i] != nullptr)
		g_theGames[i]->Startup();
	}
	//g_theGames[(int)g_theGameState]->Startup();

	
	std::string mainmenu = g_gameConfigBlackboard.GetValue("mainMenuMusic", "");
	std::string game = g_gameConfigBlackboard.GetValue("gameMusic", "");
	std::string click = g_gameConfigBlackboard.GetValue("buttonClickSound", "");
	g_musicLibrary[MUSIC_MAINMENU] = g_theAudio->CreateOrGetSound(mainmenu.c_str(), true);
	g_musicLibrary[MUSIC_GAME] = g_theAudio->CreateOrGetSound(game.c_str(), true);
	g_musicLibrary[MUSIC_CLICK] = g_theAudio->CreateOrGetSound(click.c_str(), true);

	g_theGames[(int)g_theGameState]->Enter();

	m_devCamera->m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

	SubscribeEventCallbackFunction("Quit", &Command_Quit);

	ShowAllKeysOnDevConsole();
}

void App::Shutdown()
{
	//g_theGame->Shutdown();
	g_shaderManager->Shutdown();
	delete g_shaderManager;
	g_shaderManager = nullptr;

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

	DebugRenderSystemShutdown();
	g_theEventSystem->Shutdown();
	g_theInput->Shutdown();
	g_theWindow->Shutdown();
	g_theRenderer->Shutdown();
	g_theAudio->Shutdown();
	g_theDevConsole->Shutdown();

	//delete g_theGame;
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
	DebugRenderBeginFrame();
	g_shaderManager->BeginFrame();
}

void App::Update()
{	
	UpdateFromKeyboard();
	m_devCamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	g_theGames[(int)g_theGameState]->Update();
	bool mouse_show = g_theGameState == GameState::ATTRACT || g_theDevConsole->IsOpen() || !g_theWindow->IsFocused();
	g_theInput->SetCursorMode(!mouse_show, !mouse_show);
	
}

void App::Render() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
	g_theGames[(int)g_theGameState]->Render();
	g_theDevConsole->Render(AABB2(0.f, 0.f, 1400.f, 700.f));
}

void App::EndFrame()
{
	g_shaderManager->EndFrame();
	g_theEventSystem->EndFrame();
	g_theDevConsole->EndFrame();
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
	g_theRenderer->EndFrame();
	g_theAudio->EndFrame();
	DebugRenderEndFrame();
}


void App::UpdateFromKeyboard()
{
	if (g_theInput->WasKeyJustPressed('P'))
	{
		if (!g_theGames[(int)GameState::PLAYING]->m_clock->IsPaused())
			g_theGames[(int)GameState::PLAYING]->m_clock->Pause();
		else g_theGames[(int)GameState::PLAYING]->m_clock->Unpause();
		
	}
	if (g_theInput->WasKeyJustPressed('O'))
	{
		g_theGames[(int)GameState::PLAYING]->m_clock->StepSingleFrame();
	}
	if (g_theInput->IsKeyDown('T'))
	{
		if (!m_isSlowMo) 
		{
			m_isSlowMo = true;
		}	
	}
	//if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
	//{
	//	g_theGames[(int)GameState::PLAYING]->Shutdown();
	//	delete g_theGames[(int)GameState::PLAYING];
	//	g_theGames[(int)GameState::PLAYING] = nullptr;
	//	Game::CreateOrGetGameStateOfType(GameState::PLAYING);
	//	g_theGames[(int)GameState::PLAYING]->Startup();
	//}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC) || g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_BACK))
	{
		if (g_theGameState != GameState::ATTRACT && g_theGameState != GameState::LOBBY)
		{
			g_theGames[(int)g_theGameState]->Exit();
			g_theGameState = GameState::ATTRACT;
			g_theGames[(int)g_theGameState]->Enter();
			//g_theGame->ReturnAttractModeNormal();
		}
		else if(g_theGameState == GameState::ATTRACT) {
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
	g_theDevConsole->AddLine(DevConsole::INFO_MAJOR, "Controls");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Mouse	 - Aim");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "W / A	 - Move");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "S / D	 - Strafe");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Z / C	 - Elevate");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "E / Controller B	 - Close Elevator");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Shift	 - Sprint");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "1				 	- Previous Weapon");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "2				 	- Next Weapon");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "P				 	- Pause");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "O				 	- Step Frame");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "F				 	- Toggle Free Camera");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "N				 	- Possess Next Actor");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "~					 - Open Dev Console");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Escape - Exit Game");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Space	 - Start Game");

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